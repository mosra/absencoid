#include "ClassesModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "TeachersModel.h"

namespace Absencoid {

/* Konstruktor */
ClassesModel::ClassesModel(TeachersModel* _teachersModel, QObject* parent):
QAbstractTableModel(parent), teachersModel(_teachersModel) {
    /* Dotaz */
    QSqlQuery query("SELECT id, name, teacherId FROM classes ORDER by name;");

    /** @todo Ošetřit errory */

    /* Procházení a umisťování do listu */
    while(query.next()) {
        Class c;
        c.id = query.value(0).toInt();
        c.name = query.value(1).toString();
        c.teacherId = query.value(2).toInt();
        classes.append(c);
    }

    /* Propojení změn v TeachersModel se změnami zde */
    connect(teachersModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(checkTeacherChanges(QModelIndex,QModelIndex)));
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
    if(index.column() == 0 && (role == Qt::DisplayRole || role == Qt::EditRole))
        return classes[index.row()].name;

    /* Učitel */
    if(index.column() == 1 && role == Qt::DisplayRole) {
        /* Vrácení dat z TeachersModel, aby byly aktuální */
        return teachersModel->indexFromId(classes[index.row()].teacherId).data();
    }

    /* Něco jiného */
    return QVariant();
}

/* Flags */
Qt::ItemFlags ClassesModel::flags(const QModelIndex& index) const {
    if(!index.isValid() ||
        index.column() > 1 ||
        index.row() >= classes.count()) return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

/* Zápisový přístup k datům */
bool ClassesModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid() ||
        index.column() > 1 ||
        index.row() >= classes.count()) return false;

    /* Jméno předmětu */
    if(index.column() == 0 && role == Qt::EditRole) {
        if(value.toString().isEmpty()) return false;

        /* Aktualizace dat */
        classes[index.row()].name = value.toString();

        /* Aktualizace DB */
        QSqlQuery query;
        query.prepare("UPDATE classes SET name = :name WHERE id = :id;");
        query.bindValue(":name", classes[index.row()].name);
        query.bindValue(":id", classes[index.row()].id);

        if(!query.exec()) {
            qDebug() << tr("Nepovedlo se aktualizovat předmět!") << query.lastError()
                     << query.lastQuery();
            return false;
        }

        emit dataChanged(index, index);

        return true;
    }

    return false;
}

/* Zjištění změn v modelu učitelů */
void ClassesModel::checkTeacherChanges(const QModelIndex& topLeft, const QModelIndex& bottomRight) {
    /* Pokud nejsou změny ve sloupci se jmény, nás se netýkají */
    if(topLeft.column() != 0) return;

    /* Z hlediska výkonnosti je jedno, jestli procházíme změněné řádky a až v nich
        třídy, nebo naopak - není zde žádné break pro předčasné ukončení cyklu,
        takže vyjde oboje úplně nastejno */

    /* Procházení změněných řádků a zjišťování, jestli jsme změnami ovlivněni */
    for(int row = topLeft.row(); row <= bottomRight.row(); ++row) {
        /* Zjištění ID učitele */
        int teacherId = teachersModel->idFromIndex(row);

        /* Procházení tříd a hledání tohoto učitele */
        for(int i = 0; i != classes.count(); ++i) {
            /* Vyslání signálu, že se změnila data ve druhém sloupci daného řádku,
               do nadřazeného cyklu se nevracíme, protože víc předmětů může mít
               stejného učitele. */
            if(classes[i].teacherId == teacherId)
                /* Data se změnila ve druhém sloupci (učitel)   daného řádku */
                emit dataChanged(index(i, 1), index(i, 1));
        }
    }
}

}
