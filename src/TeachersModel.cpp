#include "TeachersModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QList>
#include <QStyle>
#include <QApplication>
#include <QDebug>

namespace Absencoid {

/* Konstruktor */
TeachersModel::TeachersModel(QObject* parent): QAbstractTableModel(parent) {
    /* Výběr všech učitelů */
    QSqlQuery query("SELECT id, name, flags FROM teachers ORDER BY name;");

    /* Procházení a umisťování do listu */
    while(query.next()) {
        Teacher t;
        t.id = query.value(0).toInt();
        t.name = query.value(1).toString();
        t.flags = query.value(2).toInt();
        teachers.append(t);
    }
}

/* Počet řádků */
int TeachersModel::columnCount(const QModelIndex& parent) const {
    return 3;
}

/* Počet řádků */
int TeachersModel::rowCount(const QModelIndex& parent) const {
    return teachers.count();
}

/* Hlavičky */
QVariant TeachersModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole) {

        /* Horizontální hlavičky */
        if(orientation == Qt::Horizontal) {
            switch(section) {
                case 0:     return QVariant(tr("Jméno"));
                case 1:     return QVariant(tr("Absence"));
                case 2:     return QVariant(tr("Školní akce"));
            }
        }

        /* Vertikální hlavičky = ID učitele */
        if(orientation == Qt::Vertical) {
            if(section >= 0 && section < teachers.count())
                return QVariant(teachers[section].id);
        }
    }

    return QVariant();
}

/* Přístup k datům */
QVariant TeachersModel::data(const QModelIndex& index, int role) const {
    /* Jestli index patří k modelu a je platný */
    if(!index.isValid() ||
        index.row() > teachers.size() ||
        index.column() > 2) return QVariant();

    /* Jméno */
    else if(index.column() == 0 && role == Qt::DisplayRole)
        return teachers[index.row()].name;

    /* Zda zapisuje absence */
    else if(index.column() == 1) {
        /* Text */
        if(role == Qt::DisplayRole)
            return teachers[index.row()].flags & 0x01 ? tr("Zapisuje") : QString();

        /* Dekorace */
        else if(role == Qt::DecorationRole)
            return QApplication::style()->standardIcon(teachers[index.row()].flags & 0x01 ?
                QStyle::SP_DialogNoButton : QStyle::SP_DialogYesButton);

    /* Zda uznává školní akce */
    } else if(index.column() == 2) {
        /* Text */
        if(role == Qt::DisplayRole)
            return teachers[index.row()].flags & 0x02 ? QString() : tr("Neuznává");

        /* Dekorace */
        else if(role == Qt::DecorationRole)
            return QApplication::style()->standardIcon(teachers[index.row()].flags & 0x02 ?
            QStyle::SP_DialogYesButton : QStyle::SP_DialogNoButton);
    }

    /* Nejspíše se nehodila role */
    return QVariant();
}

/* Flags */
Qt::ItemFlags TeachersModel::flags(const QModelIndex& index) const {
    if(!index.isValid() ||
        index.row() > teachers.size() ||
        index.column() > 2) return Qt::ItemIsEnabled;

    /* Slupec jména učitele */
    if(index.column() == 0)
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;

    /* Boolean sloupce */
    return Qt::ItemIsUserCheckable;
}

/* Zápisový přístup k datům */
bool TeachersModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid() ||
        index.row() > teachers.size() ||
        index.column() > 2) return false;

    /* Editujeme ... */
    if(role == Qt::EditRole) {

        /* Jméno učitele */
        if(index.column() == 0) {
            if(value.toString().isEmpty()) return false;

            /* Aktualizace dat */
            teachers[index.row()].name = value.toString();

            /* Aktualizace DB */
            QSqlQuery query;
            query.prepare("UPDATE teachers SET name = :name WHERE id = :id;");
            query.bindValue(":name", teachers[index.row()].name);
            query.bindValue(":id", teachers[index.row()].id);

            if(!query.exec()) {
                qDebug() << tr("Nepovedlo se aktualizovat!") << query.lastError().text()
                         << query.lastQuery();
                return false;
            }

            emit dataChanged(index, index);

            return true;
        }
    }

    return false;
}

}
