#include "ClassesModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

namespace Absencoid {

/* Konstruktor */
ClassesModel::ClassesModel(QObject* parent): QAbstractTableModel(parent) {
    /* Dotaz */
    QSqlQuery query("SELECT classes.id AS classesId, classes.name AS classesName, teachers.name AS teachersName "
                    "FROM classes LEFT JOIN teachers "
                    "ON classes.teacherId = teachers.id "
                    "ORDER BY classesName;");

    /** @todo Ošetřit errory */

    /* Procházení a umisťování do listu */
    while(query.next()) {
        Class c;
        c.id = query.value(0).toInt();
        c.name = query.value(1).toString();
        c.teacher = query.value(2).toString();
        classes.append(c);
    }
}

/* Počet sloupců */
int ClassesModel::columnCount(const QModelIndex& parent) const {
    return 2;
}

/* Počet řádků */
int ClassesModel::rowCount(const QModelIndex& parent) const {
    return classes.count();
}

/* Data hlaviček */
QVariant ClassesModel::headerData(int section, Qt::Orientation orientation, int role) const {

    /* Horizontální hlavičky */
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case 0: return tr("Předmět");
            case 1: return tr("Učitel");
        }
    }

    /* Vertikální hlavičky */
    else if(orientation == Qt::Vertical && section < classes.count() && role == Qt::DisplayRole)
        return classes[section].id;

    return QAbstractItemModel::headerData(section, orientation, role);
}


/* Čtecí přístup k datům */
QVariant ClassesModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid() ||
        index.column() > 1 ||
        index.row() >= classes.count()) return false;

    /* Jméno */
    if(index.column() == 0 && role == Qt::DisplayRole)
        return classes[index.row()].name;

    /* Učitel */
    if(index.column() == 1 && role == Qt::DisplayRole)
        return classes[index.row()].teacher;

    /* Něco jiného */
    return QVariant();
}

}
