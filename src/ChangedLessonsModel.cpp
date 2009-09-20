#include "ChangedLessonsModel.h"

#include <QSqlQuery>

#include "ClassesModel.h"
#include "TimetableModel.h"

namespace Absencoid {

/* Konstruktor */
ChangedLessonsModel::ChangedLessonsModel(ClassesModel* _classesModel, TimetableModel* _timetableModel, QObject* parent): QAbstractTableModel(parent), classesModel(_classesModel), timetableModel(_timetableModel) {
    QSqlQuery query("SELECT date, hour, fromClassId, toClassId FROM changedLessons;");

    /* Procházení vüsledků dotazu */
    while(query.next()) {
        ChangedLesson c;
        c.date = query.value(0).toDate();
        c.hour = query.value(1).toInt();
        c.fromClassId = query.value(2).toInt();
        c.toClassId = query.value(3).toInt();
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
    }

    /* Ostatní */
    return QAbstractTableModel::headerData(section, orientation, role);
}

/* Data */
QVariant ChangedLessonsModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid()) return QVariant();

    /* Datum */
    if(index.column() == 0 && (role == Qt::DisplayRole || role == Qt::EditRole)) {
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

}
