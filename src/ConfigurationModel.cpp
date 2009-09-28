#include "ConfigurationModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "TimetableModel.h"

namespace Absencoid {

/* Konstruktor */
ConfigurationModel::ConfigurationModel(TimetableModel* _timetableModel, QObject* parent):
QAbstractTableModel(parent), timetableModel(_timetableModel), activeTimetableId(0), booleans(0) {
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
    booleans = query.value(5).toInt();
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
        if(role == Qt::DisplayRole) return booleans & 0x01 ? "Ano" : "Ne";
        if(role == Qt::EditRole)    return (bool) (booleans & 0x01);
        if(role == Qt::CheckStateRole)
            return booleans & 0x01 ? Qt::Checked : Qt::Unchecked;

    /* Zda vytvářet zálohy při ukončení */
    } else if(index.column() == 6) {
        if(role == Qt::DisplayRole) return booleans & 0x02 ? "Ano" : "Ne";
        if(role == Qt::EditRole)    return (bool) (booleans & 0x02);
        if(role == Qt::CheckStateRole)
            return booleans & 0x02 ? Qt::Checked : Qt::Unchecked;
    }

    /* Něco jiného */
    return QVariant();
}

/* Flags */
Qt::ItemFlags ConfigurationModel::flags(const QModelIndex& index) const {
    if(!index.isValid()) return Qt::ItemIsEnabled;

    /* Editovatelné položky */
    if(index.column() < 5) return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;

    /* Zaškrtávatelné položky */
    return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
}

/* Zápisový přístup k datům */
bool ConfigurationModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid()) return false;

    QSqlQuery query;

    /* Začátek pololetí */
    if(index.column() == 0 && role == Qt::EditRole) {
        beginDate = value.toDate();

        query.prepare("UPDATE configuration SET beginDate = :beginDate;");
        query.bindValue(":beginDate", beginDate.toString(Qt::ISODate));

    /* Konec pololetí */
    } else if(index.column() == 1 && role == Qt::EditRole) {
        endDate = value.toDate();

        query.prepare("UPDATE configuration SET endDate = :endDate;");
        query.bindValue(":endDate", endDate.toString(Qt::ISODate));

    /* Aktuální použitý rozvrh (konverze z indexu) */
    } else if(index.column() == 2 && role == Qt::EditRole) {
        activeTimetableId = timetableModel->idFromIndex(value.toInt());

        query.prepare("UPDATE configuration SET activeTimetableId = :activeTimetableId;");
        query.bindValue(":activeTimetableId", activeTimetableId);

    /* URL pro aktualizace */
    } else if(index.column() == 3 && role == Qt::EditRole) {
        webUpdateUrl = value.toString();

        query.prepare("UPDATE configuration SET webUpdateUrl = :webUpdateUrl;");
        query.bindValue(":webUpdateUrl", webUpdateUrl);

    /* Datum poslední aktualizace */
    } else if(index.column() == 4 && role == Qt::EditRole) {
        lastUpdate = value.toDate();

        query.prepare("UPDATE configuration SET lastUpdate = :lastUpdate;");
        query.bindValue(":lastUpdate", lastUpdate);

    /* Zda zjišťovat aktualizace po startu, zda zálohovat před ukončením */
    } else if(index.column() < 7 && role == Qt::CheckStateRole) {
        /* Zaškrtnuto, přiORujeme */
        if(value.toBool())  booleans |= 1 << (index.column()-5);

        /* Odškrtnuto, AND s doplňkem */
        else                booleans &= ~(1 << (index.column()-5));

        query.prepare("UPDATE configuration SET flags = :flags;");
        query.bindValue(":flags", booleans);

    /* Něco jiného */
    } else return false;

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se aktualizovat konfiguraci!")
                 << query.lastError() << query.lastQuery();
        return false;
    }

    emit dataChanged(index, index);
    return true;
}

}
