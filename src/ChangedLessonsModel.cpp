#include "ChangedLessonsModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "ClassesModel.h"
#include "TimetableModel.h"

namespace Absencoid {

/* Konstruktor */
ChangedLessonsModel::ChangedLessonsModel(ClassesModel* _classesModel, TimetableModel* _timetableModel, QObject* parent): QAbstractTableModel(parent), classesModel(_classesModel), timetableModel(_timetableModel) {
    QSqlQuery query("SELECT id, date, hour, fromClassId, toClassId FROM changedLessons;");

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
            case 4: return tr("Ovlivněno rozvrhů");
        }

    /* Vertikální hlavičky */
    } else if(orientation == Qt::Vertical && role == Qt::EditRole) {
        return changedLessons[section].id;
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
            return changedLessons[index.row()].date.toString("dd.MM.yyyy");
        if(role == Qt::EditRole)
            return changedLessons[index.row()].date;

    /* Hodina */
    } else if(index.column() == 1 && (role == Qt::DisplayRole || role == Qt::EditRole)) {
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
    }

    /* Cokoliv jiného */
    return QVariant();
}

/* Flags */
Qt::ItemFlags ChangedLessonsModel::flags(const QModelIndex& index) const {
    if(!index.isValid()) return Qt::ItemIsEnabled;

    /* Počet ovlivněných rozvrhů není editovatelný */
    if(index.column() == 4) return QAbstractTableModel::flags(index);

    /* Vše ostatní editovatelné je */
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

/* Zápisový přístup k datům */
bool ChangedLessonsModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid() || role != Qt::EditRole) return false;

    QSqlQuery query;

    /* Datum */
    if(index.column() == 0) {
        /* Uložení dat */
        changedLessons[index.row()].date = value.toDate();

        query.prepare("UPDATE changedLessons SET date = :date WHERE id = :id;");
        query.bindValue(":date", changedLessons[index.row()].date.toString(Qt::ISODate));

    /* Hodina */
    } else if(index.column() == 1) {
        /* Uložení dat */
        changedLessons[index.row()].hour = value.toInt();

        query.prepare("UPDATE changedLessons SET hour = :hour WHERE id = :id;");
        query.bindValue(":hour", changedLessons[index.row()].hour);

    /* Předmět, ze kterého se mění */
    } else if(index.column() == 2) {
        /* Uložení dat */
        changedLessons[index.row()].fromClassId = classesModel->idFromIndex(value.toInt());

        query.prepare("UPDATE changedLessons SET fromClassId = :fromClassId WHERE id = :id;");
        query.bindValue(":fromClassId", changedLessons[index.row()].fromClassId);

    /* Předmět, na který se mění */
    } else if(index.column() == 3) {
        /* Uložení dat */
        changedLessons[index.row()].toClassId = classesModel->idFromIndex(value.toInt());

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

    emit dataChanged(index, index);
    return true;
}

}
