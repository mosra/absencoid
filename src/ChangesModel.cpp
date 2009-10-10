#include "ChangesModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFont>

#include "configure.h"
#include "ClassesModel.h"
#include "TimetableModel.h"

namespace Absencoid {

/* Konstruktor */
ChangesModel::ChangesModel(ClassesModel* _classesModel, TimetableModel* _timetableModel, QObject* parent): QAbstractTableModel(parent), classesModel(_classesModel), timetableModel(_timetableModel) {
    reload();
}

/* (Znovu)načtení dat z databáze */
void ChangesModel::reload() {
    changes.clear();

    /* Načtení seznamu změn */
    QSqlQuery query("SELECT id, date, hour, fromClassId, toClassId FROM changes ORDER BY date;");

    /* Procházení vüsledků dotazu */
    while(query.next()) {
        Change c;
        c.id = query.value(0).toInt();
        c.date = query.value(1).toDate();
        c.hour = query.value(2).toInt();
        c.fromClassId = query.value(3).toInt();
        c.toClassId = query.value(4).toInt();
        changes.append(c);
    }

    reset();
}

/* Počet sloupců */
int ChangesModel::columnCount(const QModelIndex& parent) const {
    return 5;
}

/* Počet řádků */
int ChangesModel::rowCount(const QModelIndex& parent) const {
    return changes.count();
}

/* Hlavičky */
QVariant ChangesModel::headerData(int section, Qt::Orientation orientation, int role) const {
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
            return changes[section].id == 0 ? QVariant("*") : QVariant(changes[section].id);

        /* Hvězdička je zobrazena tučně */
        if(role == Qt::FontRole && changes[section].id == 0) {
            QFont font;
            font.setBold(true);
            return font;
        }
    }

    /* Ostatní */
    return QAbstractTableModel::headerData(section, orientation, role);
}

/* Data */
QVariant ChangesModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid()) return QVariant();

    /* Datum */
    if(index.column() == 0) {
        if(role == Qt::DisplayRole)
            return changes[index.row()].date.toString("ddd dd.MM.yyyy");
        if(role == Qt::EditRole)
            return changes[index.row()].date;

    /* Hodina */
    } else if(index.column() == 1 && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        /* Číslo hodiny -1 značí všechny hodiny */
        if(changes[index.row()].hour == ALL_HOURS && role == Qt::DisplayRole)
            return tr("Všechny");

        return changes[index.row()].hour;

    /* Předměty */
    } else if(index.column() == 2 || index.column() == 3) {
        /* ID předmětu */
        int id;

        /* Předmět, ze kterého se mění */
        if(index.column() == 2) id = changes[index.row()].fromClassId;

        /* Předmět, na který se mění */
        else                    id = changes[index.row()].toClassId;

        /* Index předmětu */
        int index = classesModel->indexFromId(id);

        /* Pro zobrazení odešleme popis */
        if(role == Qt::DisplayRole)
            return classesModel->index(index, 0).data(Qt::DisplayRole);

        /* Pro editaci index */
        if(role == Qt::EditRole) return index;

    /* Počet ovlivněných rozvrhů */
    } else if(index.column() == 4 && role == Qt::DisplayRole) {
        /* Pokud měníme z nějaké určité hodiny, nalezení všech rozvrhů, které ji
            tento den a hodinu obsahují */
        if(changes[index.row()].fromClassId != 0) {
            return timetableModel->timetablesWithThisLesson(
                changes[index.row()].date,
                changes[index.row()].hour,
                changes[index.row()].fromClassId);

        /* Pokud měníme z prázdné hodiny na jinou, nalezení všech rozvrhů, které
            mají tu "jinou" (kdekoli). */
        } else {
            return timetableModel->timetablesWithThisLesson(
                changes[index.row()].date,
                0x0F,
                changes[index.row()].toClassId);
        }
    }

    /* Cokoliv jiného */
    return QVariant();
}

/* Flags */
Qt::ItemFlags ChangesModel::flags(const QModelIndex& index) const {
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
bool ChangesModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid() || role != Qt::EditRole) return false;

    QSqlQuery query;

    /* Datum */
    if(index.column() == 0) {
        /* Ověření jedinečnosti */
        if(!checkUnique(value.toDate(), changes[index.row()].hour, changes[index.row()].fromClassId))
            return false;

        /* Uložení dat */
        changes[index.row()].date = value.toDate();

        /* Nový záznam, místo UPDATE děláme INSERT, emitujeme signál
            a pokusíme se řádek uložit do DB */
        if(changes[index.row()].id == 0) {
            /* Kromě data se mění také počet ovlivněných rozvrhů */
            emit dataChanged(index.sibling(index.row(), 4), index.sibling(index.row(), 4));

            emit dataChanged(index, index);
            return saveRow(index.row());
        }

        query.prepare("UPDATE changes SET date = :date WHERE id = :id;");
        query.bindValue(":date", changes[index.row()].date.toString(Qt::ISODate));

    /* Hodina */
    } else if(index.column() == 1) {
        /* Ověření jedinečnosti */
        if(!checkUnique(changes[index.row()].date, value.toInt(), changes[index.row()].fromClassId))
            return false;

        /* Uložení dat */
        changes[index.row()].hour = value.toInt();

        /* Nový záznam, místo UPDATE děláme INSERT, emitujeme signál
            a pokusíme se řádek uložit do DB */
        if(changes[index.row()].id == 0) {
            /* Kromě data se mění také počet ovlivněných rozvrhů */
            emit dataChanged(index.sibling(index.row(), 4), index.sibling(index.row(), 4));

            emit dataChanged(index, index);
            return saveRow(index.row());
        }

        query.prepare("UPDATE changes SET hour = :hour WHERE id = :id;");
        query.bindValue(":hour", changes[index.row()].hour);

    /* Předmět, ze kterého se mění */
    } else if(index.column() == 2) {
        /* Ověření jedinečnosti */
        if(!checkUnique(changes[index.row()].date, changes[index.row()].hour, classesModel->idFromIndex(value.toInt())))
            return false;

        /* Uložení dat */
        changes[index.row()].fromClassId = classesModel->idFromIndex(value.toInt());

        /* Nový záznam, místo UPDATE děláme INSERT, emitujeme signál
            a pokusíme se řádek uložit do DB */
        if(changes[index.row()].id == 0) {
            /* Kromě data se mění také počet ovlivněných rozvrhů */
            emit dataChanged(index.sibling(index.row(), 4), index.sibling(index.row(), 4));

            emit dataChanged(index, index);
            return saveRow(index.row());
        }

        query.prepare("UPDATE changes SET fromClassId = :fromClassId WHERE id = :id;");
        query.bindValue(":fromClassId", changes[index.row()].fromClassId);

    /* Předmět, na který se mění */
    } else if(index.column() == 3) {
        /* Uložení dat */
        changes[index.row()].toClassId = classesModel->idFromIndex(value.toInt());

        /* Nový záznam, místo UPDATE děláme INSERT, emitujeme signál
            a pokusíme se řádek uložit do DB */
        if(changes[index.row()].id == 0) {
            emit dataChanged(index, index);
            return saveRow(index.row());
        }

        query.prepare("UPDATE changes SET toClassId = :toClassId WHERE id = :id;");
        query.bindValue(":toClassId", changes[index.row()].toClassId);

    /* Něco jiného */
    } else return false;

    /* Uložení do DB */
    query.bindValue(":id", changes[index.row()].id);
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
bool ChangesModel::insertRow(int row, const QModelIndex& parent) {
    beginInsertRows(parent, row, row);

    Change c;
    c.date = QDate::currentDate();
    c.id = 0;
    c.hour = 1;
    c.fromClassId = 0;
    c.toClassId = 0;

    changes.insert(row, c);

    endInsertRows();

    return true;
}

/* Odebrání řádků */
bool ChangesModel::removeRows(int row, int count, const QModelIndex& parent) {
    QSqlQuery query;
    query.prepare("DELETE FROM changes WHERE id = :id;");

    beginRemoveRows(parent, row, row+count-1);

    /* Mazání z DB */
    for(int i = row; i != row+count; ++i) {
        query.bindValue(":id", changes[i].id);

        /* Mazání z DB */
        if(!query.exec()) {
            qDebug() << tr("Nepodařilo se smazat změnu!") << query.lastError()
                     << query.lastQuery();
            break;
        }

        /* Mazání z lokálních dat */
        changes.removeAt(i);
    }

    endRemoveRows();

    return true;
}

/* Zjištění unikátnosti záznamu */
bool ChangesModel::checkUnique(QDate date, int hour, int fromClassId) {
    /* Špatná hodina */
    if(hour < -1 || hour > 9) return false;

    /* Procházení a hledání záznamu */
    for(int i = 0; i != changes.count(); ++i) {
        /* Datum z hlediska výkonnosti testujeme až na konci */
        if(changes[i].id != 0 &&
           changes[i].hour == hour &&
           changes[i].fromClassId == fromClassId &&
           changes[i].date == date)
            return false;
    }

    return true;
}

/* Změny, které souvisí s daným datem a rozvrhem, který tou dobou platil */
QList<int> ChangesModel::relatedChanges(QDate date) const {
    QList<int> list;

    /* Hledání změn v příslušný datum */
    for(int i = 0; i != changes.size(); ++i) if(changes[i].date == date) {

        /* Měníme z prázdné hodiny */
        if(changes[i].fromClassId == 0) {

            /* Pokud aktivní rozvrh obsahuje _kdekoli_ předmět, na který měníme, přidáme */
            if(timetableModel->timetablesWithThisLesson(date, 0x0F, changes[i].toClassId, true) != 0)
                list.append(i);

        /* Měníme z hodiny na hodinu */
        } else {

            /* Pokud aktivní rozvrh obsahuje v daný den/hodinu předmět, ze které měníme, přidáme */
            if(timetableModel->timetablesWithThisLesson(date, changes[i].hour, changes[i].fromClassId, true) != 0)
                list.append(i);
        }
    }

    return list;
}

/* Počet přidaných / odebraných hodin daného předmětu v pololetí */
int ChangesModel::deltaHours(int classId, bool tillNow, int direction) {
    int delta = 0;

    QDate endDate = tillNow ? QDate::currentDate() : timetableModel->endDate();

    /* Procházení jednotlivých změn */
    for(int i = 0; i != changes.size(); ++i) {
        /* Datum mimo rozsah */
        if(changes[i].date < timetableModel->beginDate() || changes[i].date > endDate) continue;

        /* Mění se z prázdné hodiny, tj. přibývá hodina */
        if(changes[i].fromClassId == 0 && direction >= 0) {

            /* Měníme na nějaký jiný předmět, než který hledáme */
            if(classId != 0 && changes[i].toClassId != classId) continue;

            /* Pokud aktivní rozvrh obsahuje _kdekoli_ předmět, na který měníme */
            if(timetableModel->timetablesWithThisLesson(changes[i].date, 0x0F, changes[i].toClassId, true) != 0)
                delta++;

        /* Měníme z hodiny na hodinu, možná přibývá, možná ubývá hodina */
        } else if(changes[i].fromClassId != 0 && direction <= 0) {

            /* Měníme z jakéhokoli předmětu */
            if(changes[i].fromClassId == ClassesModel::WHATEVER) {
                /* Měníme ze všech hodin */
                if(changes[i].hour == ALL_HOURS) {

                    /* Hledáme konkrétní předmět - musíme prohledat každou hodinu */
                    if(classId != 0) for(int hour = 0; hour != 10; ++hour) {
                        /* Pokud se mění z hledaného předmětu na jiný, odečteme */
                        if(timetableModel->timetablesWithThisLesson(changes[i].date, hour, classId, true) == 1 && classId != changes[i].toClassId)
                            delta--;

                        /* Pokud v danou dobu je nějaký předmět a měníme na hledaný, přičteme */
                        else if(changes[i].toClassId == classId && timetableModel->timetablesWithThisLesson(changes[i].date, changes[i].hour, ClassesModel::WHATEVER, true) == 1)
                            delta++;

                    /* Nehledáme konkrétní předmět, pokud měníme na prázdnou
                        hodinu, jen spočítáme kolik hodin odpadlo */
                    } else if(changes[i].toClassId == 0)
                        delta -= timetableModel->lessonCount(changes[i].date);

                /* Měníme z konkrétní hodiny */
                } else {
                    /* Pokud se mění z hledaného předmětu na jiný, odečteme */
                    if(timetableModel->timetablesWithThisLesson(changes[i].date, changes[i].hour, classId, true) == 1 && classId != changes[i].toClassId)
                        delta--;

                    /* Pokud v danou dobu je nějaký předmět a měníme na hledaný, přičteme */
                    else if(changes[i].toClassId == classId && timetableModel->timetablesWithThisLesson(changes[i].date, changes[i].hour, ClassesModel::WHATEVER, true) == 1)
                        delta++;
                }


            /* Měníme z konkrétního předmětu na jiný, ověříme zda jej rozvrh obsahuje */
            } else if(changes[i].toClassId != changes[i].fromClassId && timetableModel->timetablesWithThisLesson(changes[i].date, changes[i].hour, changes[i].fromClassId, true) == 1) {
                /* Hledáme konkrétní předmět */
                if(classId != 0) {
                    /* Měníme ze všech hodin */
                    if(changes[i].hour == ALL_HOURS) {
                        /* Pokud z něj měníme na jiný, odečteme všechny hodiny,
                            ve kterých předmět byl */
                        if(changes[i].fromClassId == classId)
                            delta -= timetableModel->lessonCount(changes[i].date, changes[i].fromClassId);

                        /* Pokud měníme z jiného na hledaný, přičteme všechny hodiny
                            daný den (ve kterých již není hledaný předmět) */
                        else if(changes[i].toClassId == classId)
                            delta += timetableModel->lessonCount(changes[i].date)
                                -timetableModel->lessonCount(changes[i].date, changes[i].toClassId);

                    /* Měníme z konkrétní hodiny */
                    } else {
                        /* Pokud z něj měníme, odečteme */
                        if(changes[i].fromClassId == classId) delta--;

                        /* Pokud měníme z jiného na hledaný, přičteme */
                        else if(changes[i].toClassId == classId)   delta++;
                    }

                /* Hledáme jakýkoli předmět, odečítáme jen pokud se mění na prázdnou hodinu */
                } else if(changes[i].toClassId == 0) {
                    /* Měníme z jakékoli hodiny, odečteme všechny hodiny, které ten den jsou */
                    if(changes[i].hour == ALL_HOURS)
                        delta -= timetableModel->lessonCount(changes[i].date);

                    /* Měníme z určené hodiny, odečteme jen jednu */
                    else delta--;
                }
            }
        }
    }

    return delta;
}

/* Uložení nového záznamu do DB */
bool ChangesModel::saveRow(int row) {
    /* Špatný index */
    if(row < 0 || row >= changes.count()) return false;

    /* Záznam byl již uložen. Vracíme true, protože nedošlo k žádné chybě. */
    if(changes[row].id != 0) return true;

    /* SQL dotaz */
    QSqlQuery query;
    query.prepare("INSERT INTO changes (gradeId, date, hour, fromClassId, toClassId) "
                  "VALUES (1, :date, :hour, :fromClassId, :toClassId);");
    query.bindValue(":date", changes[row].date.toString(Qt::ISODate));
    query.bindValue(":hour", changes[row].hour);
    query.bindValue(":fromClassId", changes[row].fromClassId);
    query.bindValue(":toClassId", changes[row].toClassId);

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se přidat změnu!") << query.lastError()
                 << query.lastQuery();
        return false;
    }

    /* Aktualizace ID */
    changes[row].id = query.lastInsertId().toInt();

    /* Signál o změně hlavičky */
    emit headerDataChanged(Qt::Vertical, row, row);

    /* Signál o změně rozbrazení (aby se hlavička přizpůsobila dlouhým ID) */
    emit layoutChanged();

    return true;
}

}
