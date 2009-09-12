#include "TimetableListModel.h"

#include <QSqlQuery>

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

    if(role == Qt::DisplayRole) {
        /* Popisek: Rozvrh (platný od 01.09.2009 do 30.06.2010) */
        if(index.column() == 0)
            return timetableList[index.row()].description + tr(" (platný od ")
                + timetableList[index.row()].validFrom.toString("dd.MM.yyyy") + " do "
                + timetableList[index.row()].validTo.toString("dd.MM.yyyy") + ")";

        /* Popisek samotný */
        if(index.column() == 1)
            return timetableList[index.row()].description;

        /* Platný od */
        if(index.column() == 2)
            return timetableList[index.row()].validFrom;

        /* Platný do */
        if(index.column() == 3)
            return timetableList[index.row()].validTo;
    }

    /* Něco jiného */
    return QVariant();
}

}
