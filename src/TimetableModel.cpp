#include "TimetableModel.h"

namespace Absencoid {

/* Konstruktor */
TimetableModel::TimetableModel(ClassesModel* classesModel, QObject* parent):
QAbstractTableModel(parent), horizontalLessons(true) {}

/* Počet sloupců */
int TimetableModel::columnCount(const QModelIndex& parent) const {
    /* Na horizontální ose jsou zobrazeny hodiny */
    if(horizontalLessons) return 10;

    /* Na horizontální ose jsou zobrazeny dny */
    else return 5;
}

/* Počet řádků */
int TimetableModel::rowCount(const QModelIndex& parent) const {
    /* Na vertikální ose jsou zobrazeny dny */
    if(horizontalLessons) return 5;

    /* Na vertikální ose jsou zobrazeny hodiny */
    else return 10;
}

/* Hlavičky */
QVariant TimetableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole) return QVariant();

    /* Dlouhé názvy hodin pro horizontální hlavičky */
    if(orientation == Qt::Horizontal && horizontalLessons)
        return QString::number(section) + tr(". hodina");

    /* Krátké názvy hodin pro vertikální hlavičky */
    else if(orientation == Qt::Vertical && !horizontalLessons)
        return QString::number(section) + tr(".");

    /* Dlouhé názvy dnů pro horizontální hlavičky */
    else if(orientation == Qt::Horizontal && !horizontalLessons) {
        switch(section) {
            case 0: return tr("Pondělí");
            case 1: return tr("Úterý");
            case 2: return tr("Středa");
            case 3: return tr("Čtvrtek");
            case 4: return tr("Pátek");
        }
    }

    /* Krátké názvy dnů pro vertikální hlavičky */
    else if(orientation == Qt::Vertical && horizontalLessons) {
        switch(section) {
            case 0: return tr("Po");
            case 1: return tr("Út");
            case 2: return tr("St");
            case 3: return tr("Čt");
            case 4: return tr("Pá");
        }
    }

    /* Něco jinýho */
    return QVariant();
}

/* Data rozvrhu */
QVariant TimetableModel::data(const QModelIndex& index, int role) const {
    return QVariant();
}

/* Přehození směru */
void TimetableModel::switchDirection() {
    horizontalLessons = horizontalLessons ? false : true;

    /* Kompletně se změnila struktura, takže je lepší zavolat reset, než
        milionkrát vysílat dataChanged() a headerDataChanged() */
    reset();
}

}
