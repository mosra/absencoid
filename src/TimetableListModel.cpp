#include "TimetableListModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

namespace Absencoid {

/* Konstruktor */
TimetableListModel::TimetableListModel(QObject* parent): QAbstractTableModel(parent) {
    QSqlQuery query("SELECT id, description, validFrom, validTo, followedBy FROM timetables;");

    /* Ukládání výsledků dotazu */
    while(query.next()) {
        Timetable t;
        t.id = query.value(0).toInt();
        t.description = query.value(1).toString();
        t.validFrom = query.value(2).toDate();
        t.validTo = query.value(3).toDate();
        t.followedBy = query.value(4).toInt();
        timetableList.append(t);
    }
}

/* Počet sloupců */
int TimetableListModel::columnCount(const QModelIndex& parent) const {
    return 4;
}

/* Počet řádků */
int TimetableListModel::rowCount(const QModelIndex& parent) const {
    return timetableList.count();
}

/* Čtecí přístup k datům */
QVariant TimetableListModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid() ||
        index.row() >= timetableList.count() ||
        index.column() > 3) return QVariant();

    /* Popisek: Rozvrh (platný od 01.09.2009 do 30.06.2010) */
    if((index.column() == 0 && role == Qt::DisplayRole) ||
       (index.column() == 1 && role == Qt::ToolTipRole))
        return timetableList[index.row()].description + tr(" (platný od ")
            + timetableList[index.row()].validFrom.toString("dd.MM.yyyy") + " do "
            + timetableList[index.row()].validTo.toString("dd.MM.yyyy") + ")";

    /* Popisek samotný */
    if(index.column() == 1 && role == Qt::DisplayRole)
        return timetableList[index.row()].description;

    /* Platný od */
    if(index.column() == 2 && role == Qt::DisplayRole)
        return timetableList[index.row()].validFrom;

    /* Platný do */
    if(index.column() == 3 && role == Qt::DisplayRole)
        return timetableList[index.row()].validTo;

    /* Něco jiného */
    return QVariant();
}

/* Flags */
Qt::ItemFlags TimetableListModel::flags(const QModelIndex& index) const {
    if(!index.isValid() ||
        index.column() > 3 ||
        index.row() >= timetableList.count()) return Qt::ItemIsEnabled;

    /* Dlouhý popisek není editovatelný */
    if(index.column() == 0) return QAbstractItemModel::flags(index);

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

/* Nastavení dat */
bool TimetableListModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid() ||
        index.column() > 3 ||
        index.row() >= timetableList.count()) return false;

    QSqlQuery query;

    /* Popisek */
    if(index.column() == 1 && role == Qt::EditRole) {
        if(value.toString().isEmpty()) return false;

        timetableList[index.row()].description = value.toString();

        query.prepare("UPDATE timetables SET description = :description WHERE id = :id;");
        query.bindValue(":description", timetableList[index.row()].description);
        query.bindValue(":id", timetableList[index.row()].id);

    /* Začátek platnosti */
    } else if(index.column() == 2 && role == Qt::EditRole) {
        timetableList[index.row()].validFrom = value.toDate();

        query.prepare("UPDATE timetables SET validFrom = :validFrom WHERE id = :id;");
        query.bindValue(":validFrom", timetableList[index.row()].validFrom.toString(Qt::ISODate));
        query.bindValue(":id", timetableList[index.row()].id);

    /* Konec platnosti */
    } else if(index.column() == 3 && role == Qt::EditRole) {
        timetableList[index.row()].validTo = value.toDate();

        query.prepare("UPDATE timetables SET validTo = :validTo WHERE id = :id;");
        query.bindValue(":validTo", timetableList[index.row()].validTo.toString(Qt::ISODate));
        query.bindValue(":id", timetableList[index.row()].id);

    /* Něco jiného */
    } else return false;

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se aktualizovat rozvrh!") << query.lastError()
                 << query.lastQuery();
        return false;
    }

    /* Změny v popisku a datech se projeví i v "souhrnném" sloupci */
    if(index.column() != 4)
        emit dataChanged(index.sibling(index.row(), 0), index.sibling(index.row(), 0));

    emit dataChanged(index, index);
    return true;
}

/* Zjištění indexu z ID rozvrhu */
int TimetableListModel::indexFromId(int id) const {
    for(int i = 0; i != timetableList.count(); ++i) {
        if(timetableList[i].id == id) return i;
    }

    /* Nic nenalezeno, vracíme neplatný index */
    return -1;
}

}
