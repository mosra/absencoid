#include "ConfigurationModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "TimetableModel.h"

namespace Absencoid {

/* Konstruktor */
ConfigurationModel::ConfigurationModel(TimetableModel* _timetableModel, QObject* parent):
QAbstractTableModel(parent), timetableModel(_timetableModel), activeTimetableId(0), flags(0) {
    QSqlQuery query(
        "SELECT beginDate, endDate, activeTimetableId, webUpdateUrl, lastUpdate, flags "
        "FROM configuration LIMIT 1;");

    /* Pokud není dostupný řádek, chyba */
    if(!query.next()) {
        qDebug() << tr("Nelze načíst konfiguraci!") << query.lastError()
                 << query.lastQuery();
        return;
    }

    /* Dosazení do proměnných */
    beginDate = query.value(0).toDate();
    endDate = query.value(1).toDate();
    activeTimetableId = query.value(2).toInt();
    webUpdateUrl = query.value(3).toString();
    lastUpdate = query.value(4).toDate();
    flags = query.value(5).toInt();
}

/* Počet sloupců */
int ConfigurationModel::columnCount(const QModelIndex& parent) const {
    return 7;
}

/* Počet řádků */
int ConfigurationModel::rowCount(const QModelIndex& parent) const {
    return 1;
}

/* Hlavičky */
QVariant ConfigurationModel::headerData(int section, Qt::Orientation orientation, int role) const {
    /* Vertikální hlavičky */
    if(orientation == Qt::Vertical && role == Qt::DisplayRole) return QVariant();

    /* Horizonátlní hlavičky */
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case 0: return tr("Začátek pololetí");
            case 1: return tr("Konec pololetí");
            case 2: return tr("Použitý rozvrh");
            case 3: return tr("Adresa pro aktualizace z internetu");
            case 4: return tr("Poslední aktualizace");
            case 5: return tr("Zjišťovat aktualizace při startu");
            case 6: return tr("Automaticky zálohovat při ukončení programu");
        }
    }

    /* Něco jiného */
    return QAbstractTableModel::headerData(section, orientation, role);
}

/* Data */
QVariant ConfigurationModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid()) return QVariant();

    /* Začátek pololetí */
    if(index.column() == 0) {
        if(role == Qt::DisplayRole) return beginDate.toString("ddd dd.MM.yyyy");
        if(role == Qt::EditRole)    return beginDate;

    /* Konec pololetí */
    } else if(index.column() == 1) {
        if(role == Qt::DisplayRole) return endDate.toString("ddd dd.MM.yyyy");
        if(role == Qt::EditRole)    return endDate;

    /* Použitý rozvrh */
    } else if(index.column() == 2) {
        if(role == Qt::DisplayRole)
            return timetableModel->index(timetableModel->indexFromId(activeTimetableId), 0).data();
        if(role == Qt::EditRole)
            return timetableModel->indexFromId(activeTimetableId);

    /* URL pro aktualizace */
    } else if(index.column() == 3 && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        return webUpdateUrl;

    /* Datum poslední aktualizace */
    } else if(index.column() == 4) {
        if(role == Qt::DisplayRole) return lastUpdate.toString("ddd dd.MM.yyyy");
        if(role == Qt::EditRole)    return lastUpdate;

    /* Zda zjišťovat aktualizace po startu */
    } else if(index.column() == 5) {
        if(role == Qt::DisplayRole) return flags & 0x01 ? "Ano" : "Ne";
        if(role == Qt::EditRole)    return (bool) (flags & 0x01);
        if(role == Qt::CheckStateRole)
            return flags & 0x01 ? Qt::Checked : Qt::Unchecked;

    /* Zda vytvářet zálohy při ukončení */
    } else if(index.column() == 6) {
        if(role == Qt::DisplayRole) return flags & 0x02 ? "Ano" : "Ne";
        if(role == Qt::EditRole)    return (bool) (flags & 0x02);
        if(role == Qt::CheckStateRole)
            return flags & 0x02 ? Qt::Checked : Qt::Unchecked;
    }

    /* Něco jiného */
    return QVariant();
}

}
