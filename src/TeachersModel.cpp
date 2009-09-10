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

    /* Horizontální hlavičky */
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case 0:     return tr("Jméno");
            case 1:     return tr("Absence");
            case 2:     return tr("Školní akce");
        }
    }

    /* Vertikální hlavičky = ID učitele */
    if(orientation == Qt::Vertical && role == Qt::DisplayRole && section >= 0 && section < teachers.count())
            return teachers[section].id;

    return QAbstractItemModel::headerData(section, orientation, role);
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
            return teachers[index.row()].flags & 0x01 ? tr("Zapisuje") : tr("Nezapisuje");

        /* Dekorace */
        else if(role == Qt::DecorationRole)
            return QApplication::style()->standardIcon(teachers[index.row()].flags & 0x01 ?
                QStyle::SP_DialogNoButton : QStyle::SP_DialogYesButton);

        /* Checkovatelnost položky */
        else if(role == Qt::CheckStateRole)
            return teachers[index.row()].flags & 0x01 ? Qt::Checked : Qt::Unchecked;

    /* Zda uznává školní akce */
    } else if(index.column() == 2) {
        /* Text */
        if(role == Qt::DisplayRole)
            return teachers[index.row()].flags & 0x02 ? tr("Uznává") : tr("Neuznává");

        /* Dekorace */
        else if(role == Qt::DecorationRole)
            return QApplication::style()->standardIcon(teachers[index.row()].flags & 0x02 ?
            QStyle::SP_DialogYesButton : QStyle::SP_DialogNoButton);

        /* Checkovatelnost položky */
        else if(role == Qt::CheckStateRole)
            return teachers[index.row()].flags & 0x02 ? Qt::Checked : Qt::Unchecked;
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
    return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
}

/* Zápisový přístup k datům */
bool TeachersModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid() ||
        index.row() > teachers.size() ||
        index.column() > 2) return false;

    /* Jméno učitele */
    if(index.column() == 0 && role == Qt::EditRole) {
        if(value.toString().isEmpty()) return false;

        /* Aktualizace dat */
        teachers[index.row()].name = value.toString();

        /* Aktualizace DB */
        QSqlQuery query;
        query.prepare("UPDATE teachers SET name = :name WHERE id = :id;");
        query.bindValue(":name", teachers[index.row()].name);
        query.bindValue(":id", teachers[index.row()].id);

        /** @todo Ověřit duplicitu jmen */
        /** @todo Spojit query.exec() do jednoho */
        /** @todo Zavolat sort() po změně jména */

        if(!query.exec()) {
            qDebug() << tr("Nepovedlo se aktualizovat!") << query.lastError().text()
                     << query.lastQuery();
            return false;
        }

        emit dataChanged(index, index);

        return true;

    /* Flags */
    } else if(index.column() >= 1 && index.column() <= 2 && role == Qt::CheckStateRole) {

        /* Trochu moc hackoidní přístup, ale budiž. ID sloupce zrovna
            odpovídá použitým flagům, takže to lze takto zkrátit. */
        if(value.toBool())
            teachers[index.row()].flags |= index.column();
        else
            teachers[index.row()].flags &= ~index.column();

        /* Aktualizace DB */
        QSqlQuery query;
        query.prepare("UPDATE teachers SET flags = :flags WHERE id = :id;");
        query.bindValue(":flags", teachers[index.row()].flags);
        query.bindValue(":id", teachers[index.row()].id);

        if(!query.exec()) {
            qDebug() << tr("Nepovedlo se aktualizovat!") << query.lastError().text()
                     << query.lastQuery();
            return false;
        }

        emit dataChanged(index, index);

        return true;
    }

    return false;
}

/* Přidání nového učitele */
bool TeachersModel::insertRow(int row, const QModelIndex& parent) {
    beginInsertRows(parent, row, row);

    /* Otestujeme, jestli zde nejsou dva nepojmenovaní učitelé */
    Teacher test; foreach(test, teachers) {
        if(test.name.isEmpty()) {
            /** @todo Vyhodit hlášku! */
            qDebug() << tr("Nelze přidat dva nepojmenované učitele!");
            return false;
        }
    }

    /* Přidání do DB (prvně, protože se musí zjistit ID nového učitele) */
    QSqlQuery query;
    if(!query.exec("INSERT INTO teachers (gradeId, name, flags) VALUES (1, \"\", 2)")) {
        qDebug() << tr("Nepovedlo se přidat učitele!") << query.lastError().text()
        << query.lastQuery();
        return false;
    }

    int id = query.lastInsertId().toInt();

    qDebug() << id;

    Teacher t;
    t.id = id;
    t.name = "";
    t.flags = 2;

    /* Přidání na danou pozici */
    teachers.insert(row, t);

    endInsertRows();
}

/* Odstranění učitelů */
bool TeachersModel::removeRows(int row, int count, const QModelIndex& parent) {
    beginRemoveRows(parent, row, row+count-1);

    QSqlQuery query;
    query.prepare("DELETE FROM teachers WHERE id = :id;");
    for(int i = 0; i != count; i++) {

        /* Smazání z DB (připravený dotaz, aby to nebylo tak příšerně pomalý). */
        query.bindValue(":id", teachers[row+i].id);

        /* Chybička se vloudila... */
        if(!query.exec()) {
            qDebug() << tr("Nelze odstranit učitele!") << query.lastError().text()
                     << query.lastQuery();
            return false;

        /* Pokud smazání proběhlo OK, smažeme i z listu */
        } else teachers.removeAt(row+i);
    }

    endRemoveRows();
    return true;
}

}
