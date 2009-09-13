#include "TimetableListModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

namespace Absencoid {

/* Konstruktor */
TimetableListModel::TimetableListModel(QObject* parent): QAbstractTableModel(parent) {
    QSqlQuery query("SELECT id, description, validFrom, followedBy FROM timetables;");

    /* Ukládání výsledků dotazu */
    while(query.next()) {
        Timetable t;
        t.id = query.value(0).toInt();
        t.description = query.value(1).toString();
        t.validFrom = query.value(2).toDate();
        /* Pokud není zadáno, čím je následován, je následován sám sebou */
        t.followedBy = query.value(3).toInt() == 0 ? t.id : query.value(3).toInt();
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

    /* Popisek: Rozvrh (platný od 01.09.2009) */
    if((index.column() == 0 && role == Qt::DisplayRole) ||
       (index.column() == 1 && role == Qt::ToolTipRole))
        return timetableList[index.row()].description + tr(" (platný od ")
            + timetableList[index.row()].validFrom.toString("dd.MM.yyyy") + ")";

    /* Popisek samotný */
    if(index.column() == 1 && role == Qt::DisplayRole)
        return timetableList[index.row()].description;

    /* Platný od */
    if(index.column() == 2 && role == Qt::DisplayRole)
        return timetableList[index.row()].validFrom;

    /* Následující rozvrh */
    if(index.column() == 3 && role == Qt::DisplayRole)
        return timetableList[index.row()].followedBy;

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

    /* Následující rozvrh */
    } else if(index.column() == 3 && role == Qt::EditRole) {
        timetableList[index.row()].followedBy = value.toInt();

        query.prepare("UPDATE timetables SET followedBy = :followedBy WHERE id = :id;");
        query.bindValue(":validTo", timetableList[index.row()].followedBy);
        query.bindValue(":id", timetableList[index.row()].id);

    /* Něco jiného */
    } else return false;

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se aktualizovat rozvrh!") << query.lastError()
                 << query.lastQuery();
        return false;
    }

    /* Změny v popisku a datumu se projeví i v "souhrnném" sloupci */
    if(index.column() != 3)
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

/* Zjištění ID z indexu */
int TimetableListModel::idFromIndex(int index) const {
    /* Pokud je index špatný, vracíme nulu (a takové ID žádný rozvrh mít nemůže) */
    if(index < 0 || index >= timetableList.count()) return 0;

    return timetableList[index].id;
}

}
