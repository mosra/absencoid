#include "ChangedLessonsModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFont>

#include "configure.h"
#include "ClassesModel.h"
#include "TimetableModel.h"

namespace Absencoid {

/* Konstruktor */
ChangedLessonsModel::ChangedLessonsModel(ClassesModel* _classesModel, TimetableModel* _timetableModel, QObject* parent): QAbstractTableModel(parent), classesModel(_classesModel), timetableModel(_timetableModel) {
    QSqlQuery query("SELECT id, date, hour, fromClassId, toClassId FROM changedLessons ORDER BY date;");

    /* Procházení vüsledků dotazu */
    while(query.next()) {
        ChangedLesson c;
        c.id = query.value(0).toInt();
        c.date = query.value(1).toDate();
        c.hour = query.value(2).toInt();
        c.fromClassId = query.value(3).toInt();
        c.toClassId = query.value(4).toInt();
        changedLessons.append(c);
    }
}

/* Počet sloupců */
int ChangedLessonsModel::columnCount(const QModelIndex& parent) const {
    return 5;
}

/* Počet řádků */
int ChangedLessonsModel::rowCount(const QModelIndex& parent) const {
    return changedLessons.count();
}

/* Hlavičky */
QVariant ChangedLessonsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    /* Horizontální hlavičky */
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case 0: return tr("Datum");
            case 1: return tr("Hodina");
            case 2: return tr("Z předmětu");
            case 3: return tr("Na předmět");
            case 4: return tr("Ovliv. rozvrhů");
        }

    /* Vertikální hlavičky */
    } else if(orientation == Qt::Vertical) {

        /* U nových záznamů ukazujeme hvězdičku */
        if(role == Qt::DisplayRole)
            return changedLessons[section].id == 0 ? QVariant("*") : QVariant(changedLessons[section].id);

        /* Hvězdička je zobrazena tučně */
        if(role == Qt::FontRole && changedLessons[section].id == 0) {
            QFont font;
            font.setBold(true);
            return font;
        }
    }

    /* Ostatní */
    return QAbstractTableModel::headerData(section, orientation, role);
}

/* Data */
QVariant ChangedLessonsModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid()) return QVariant();

    /* Datum */
    if(index.column() == 0) {
        if(role == Qt::DisplayRole)
            return changedLessons[index.row()].date.toString("ddd dd.MM.yyyy");
        if(role == Qt::EditRole)
            return changedLessons[index.row()].date;

    /* Hodina */
    } else if(index.column() == 1 && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        /* Číslo hodiny -1 značí všechny hodiny */
        if(changedLessons[index.row()].hour == -1 && role == Qt::DisplayRole)
            return tr("Všechny");

        return changedLessons[index.row()].hour;

    /* Předměty */
    } else if(index.column() == 2 || index.column() == 3) {
        /* ID předmětu */
        int id;

        /* Předmět, ze kterého se mění */
        if(index.column() == 2) id = changedLessons[index.row()].fromClassId;

        /* Předmět, na který se mění */
        else                    id = changedLessons[index.row()].toClassId;

        /* Index předmětu */
        int index = classesModel->indexFromId(id);

        /* Pro zobrazení odešleme popis */
        if(role == Qt::DisplayRole)
            return classesModel->index(index, 0).data(Qt::DisplayRole);

        /* Pro editaci index */
        if(role == Qt::EditRole) return index;

    /* Počet ovlivněných rozvrhů */
    } else if(index.column() == 4 && role == Qt::DisplayRole) {
        /* Pondělí v Qt == 1, pondělí v Absencoid == 0 */
        return timetableModel->timetablesWithThisClass(
            timetableModel->dayHour(changedLessons[index.row()].date.dayOfWeek()-1,
                                    changedLessons[index.row()].hour),
            changedLessons[index.row()].fromClassId);
    }

    /* Cokoliv jiného */
    return QVariant();
}

/* Flags */
Qt::ItemFlags ChangedLessonsModel::flags(const QModelIndex& index) const {
    #ifndef ADMIN_VERSION
    return QAbstractItemModel::flags(index);
    #endif

    if(!index.isValid()) return Qt::ItemIsEnabled;

    /* Počet ovlivněných rozvrhů je "disabled" */
    if(index.column() == 4) return 0;

    /* Vše ostatní editovatelné je */
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

/* Zápisový přístup k datům */
bool ChangedLessonsModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid() || role != Qt::EditRole) return false;

    QSqlQuery query;

    /* Datum */
    if(index.column() == 0) {
        /* Ověření jedinečnosti */
        if(!checkUnique(value.toDate(), changedLessons[index.row()].hour, changedLessons[index.row()].fromClassId))
            return false;

        /* Uložení dat */
        changedLessons[index.row()].date = value.toDate();

        /* Nový záznam, místo UPDATE děláme INSERT, emitujeme signál
            a pokusíme se řádek uložit do DB */
        if(changedLessons[index.row()].id == 0) {
            /* Kromě data se mění také počet ovlivněných rozvrhů */
            emit dataChanged(index.sibling(index.row(), 4), index.sibling(index.row(), 4));

            emit dataChanged(index, index);
            return saveRow(index.row());
        }

        query.prepare("UPDATE changedLessons SET date = :date WHERE id = :id;");
        query.bindValue(":date", changedLessons[index.row()].date.toString(Qt::ISODate));

    /* Hodina */
    } else if(index.column() == 1) {
        /* Ověření jedinečnosti */
        if(!checkUnique(changedLessons[index.row()].date, value.toInt(), changedLessons[index.row()].fromClassId))
            return false;

        /* Uložení dat */
        changedLessons[index.row()].hour = value.toInt();

        /* Nový záznam, místo UPDATE děláme INSERT, emitujeme signál
            a pokusíme se řádek uložit do DB */
        if(changedLessons[index.row()].id == 0) {
            /* Kromě data se mění také počet ovlivněných rozvrhů */
            emit dataChanged(index.sibling(index.row(), 4), index.sibling(index.row(), 4));

            emit dataChanged(index, index);
            return saveRow(index.row());
        }

        query.prepare("UPDATE changedLessons SET hour = :hour WHERE id = :id;");
        query.bindValue(":hour", changedLessons[index.row()].hour);

    /* Předmět, ze kterého se mění */
    } else if(index.column() == 2) {
        /* Ověření jedinečnosti */
        if(!checkUnique(changedLessons[index.row()].date, changedLessons[index.row()].hour, classesModel->idFromIndex(value.toInt())))
            return false;

        /* Uložení dat */
        changedLessons[index.row()].fromClassId = classesModel->idFromIndex(value.toInt());

        /* Nový záznam, místo UPDATE děláme INSERT, emitujeme signál
            a pokusíme se řádek uložit do DB */
        if(changedLessons[index.row()].id == 0) {
            /* Kromě data se mění také počet ovlivněných rozvrhů */
            emit dataChanged(index.sibling(index.row(), 4), index.sibling(index.row(), 4));

            emit dataChanged(index, index);
            return saveRow(index.row());
        }

        query.prepare("UPDATE changedLessons SET fromClassId = :fromClassId WHERE id = :id;");
        query.bindValue(":fromClassId", changedLessons[index.row()].fromClassId);

    /* Předmět, na který se mění */
    } else if(index.column() == 3) {
        /* Uložení dat */
        changedLessons[index.row()].toClassId = classesModel->idFromIndex(value.toInt());

        /* Nový záznam, místo UPDATE děláme INSERT, emitujeme signál
            a pokusíme se řádek uložit do DB */
        if(changedLessons[index.row()].id == 0) {
            emit dataChanged(index, index);
            return saveRow(index.row());
        }

        query.prepare("UPDATE changedLessons SET toClassId = :toClassId WHERE id = :id;");
        query.bindValue(":toClassId", changedLessons[index.row()].toClassId);

    /* Něco jiného */
    } else return false;

    /* Uložení do DB */
    query.bindValue(":id", changedLessons[index.row()].id);
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se změnit změnu!") << query.lastError()
                 << query.lastQuery();
    }

    /* U prvních třech sloupců se kromě data se mění také počet ovlivněných rozvrhů */
    if(index.column() < 3)
        emit dataChanged(index.sibling(index.row(), 4), index.sibling(index.row(), 4));

    emit dataChanged(index, index);
    return true;
}

/* Přidání nového řádku (do lokálních dat) */
bool ChangedLessonsModel::insertRow(int row, const QModelIndex& parent) {
    beginInsertRows(parent, row, row);

    ChangedLesson c;
    c.date = QDate::currentDate();
    c.id = 0;
    c.hour = 1;
    c.fromClassId = 0;
    c.toClassId = 0;

    changedLessons.insert(row, c);

    endInsertRows();

    return true;
}

/* Odebrání řádků */
bool ChangedLessonsModel::removeRows(int row, int count, const QModelIndex& parent) {
    QSqlQuery query;
    query.prepare("DELETE FROM changedLessons WHERE id = :id;");

    beginRemoveRows(parent, row, row+count-1);

    /* Mazání z DB */
    for(int i = row; i != row+count; ++i) {
        query.bindValue(":id", changedLessons[i].id);

        /* Mazání z DB */
        if(!query.exec()) {
            qDebug() << tr("Nepodařilo se smazat změnu!") << query.lastError()
                     << query.lastQuery();
            break;
        }

        /* Mazání z lokálních dat */
        changedLessons.removeAt(i);
    }

    endRemoveRows();

    return true;
}

/* Zjištění unikátnosti záznamu */
bool ChangedLessonsModel::checkUnique(QDate date, int hour, int fromClassId) {
    /* Špatná hodina */
    if(hour < -1 || hour > 9) return false;

    /* Procházení a hledání záznamu */
    for(int i = 0; i != changedLessons.count(); ++i) {
        /* Datum z hlediska výkonnosti testujeme až na konci */
        if(changedLessons[i].id != 0 &&
           changedLessons[i].hour == hour &&
           changedLessons[i].fromClassId == fromClassId &&
           changedLessons[i].date == date)
            return false;
    }

    return true;
}

/* Uložení nového záznamu do DB */
bool ChangedLessonsModel::saveRow(int row) {
    /* Špatný index */
    if(row < 0 || row >= changedLessons.count()) return false;

    /* Záznam byl již uložen. Vracíme true, protože nedošlo k žádné chybě. */
    if(changedLessons[row].id != 0) return true;

    /* SQL dotaz */
    QSqlQuery query;
    query.prepare("INSERT INTO changedLessons (gradeId, date, hour, fromClassId, toClassId) "
                  "VALUES (1, :date, :hour, :fromClassId, :toClassId);");
    query.bindValue(":date", changedLessons[row].date.toString(Qt::ISODate));
    query.bindValue(":hour", changedLessons[row].hour);
    query.bindValue(":fromClassId", changedLessons[row].fromClassId);
    query.bindValue(":toClassId", changedLessons[row].toClassId);

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se přidat změnu!") << query.lastError()
                 << query.lastQuery();
        return false;
    }

    /* Aktualizace ID */
    changedLessons[row].id = query.lastInsertId().toInt();

    /* Signál o změně hlavičky */
    emit headerDataChanged(Qt::Vertical, row, row);

    /* Signál o změně rozbrazení (aby se hlavička přizpůsobila dlouhým ID) */
    emit layoutChanged();

    return true;
}

}
