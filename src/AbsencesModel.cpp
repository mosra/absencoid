#include "AbsencesModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFont>

#include "ClassesModel.h"
#include "TimetableModel.h"
#include "ChangesModel.h"

namespace Absencoid {

const int AbsencesModel::SCHOOL_ACTION = 0x8000;

/* Konstruktor */
AbsencesModel::AbsencesModel(ClassesModel* _classesModel, TimetableModel* _timetableModel, ChangesModel* _changesModel, QObject* parent): QAbstractTableModel(parent), classesModel(_classesModel), timetableModel(_timetableModel), changesModel(_changesModel) {
    QSqlQuery query;

    if(!query.exec("SELECT id, date, hours FROM absences ORDER BY date;")) {
        qDebug() << tr("Nepodařilo se získat sezman absencí!")
                 << query.lastError() << query.lastQuery();
        return;
    }

    /* Naplňování dat */
    while(query.next()) {
        /* Inicializace známých hodnot */
        Absence a;
        a.id = query.value(0).toInt();
        a.date = query.value(1).toDate();
        a.hours = query.value(2).toInt();
        absences.append(a);

        /* Zjištění názvů předmětů pro jednotlivé hodiny */
        loadClassIds(absences.count()-1);
    }
}

/* Počet sloupců */
int AbsencesModel::columnCount(const QModelIndex& parent) const {
    return 12;
}

/* Počet řádků */
int AbsencesModel::rowCount(const QModelIndex& parent) const {
    return absences.count();
}

/* Data hlaviček */
QVariant AbsencesModel::headerData(int section, Qt::Orientation orientation, int role) const {
    /* Horizontální hlavičky */
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case 0:     return tr("Datum");
            case 1:     return tr("Školní");
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
                return QString::number(section-2) + tr(". h");
        }
    }

    /* Vertikální hlavičky */
    if(orientation == Qt::Vertical && section < absences.count()) {
        /* Nová položka, zobrazujeme hvězdičku */
        if(absences[section].id == 0) {
            /* Hvězdička zobrazena tučně */
            if(role == Qt::FontRole) {
                QFont font; font.setBold(true);
                return font;
            }

            if(role == Qt::DisplayRole) return "*";
        }

        else if(role == Qt::DisplayRole) return absences[section].id;
    }

    /* Něco jiného */
    return QAbstractTableModel::headerData(section, orientation, role);
}

/* Datová funkce */
QVariant AbsencesModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid()) return QVariant();

    /* Datum */
    if(index.column() == 0) {
        if(role == Qt::DisplayRole)
            return absences[index.row()].date.toString("ddd dd.MM.yyyy");
        if(role == Qt::EditRole)
            return absences[index.row()].date;

    /* Zda je to školní akce */
    } else if(index.column() == 1) {
        if(role == Qt::DisplayRole)
            return absences[index.row()].hours & SCHOOL_ACTION ? tr("Ano") : tr("Ne");

        if(role == Qt::CheckStateRole)
            return absences[index.row()].hours & SCHOOL_ACTION ? Qt::Checked : Qt::Unchecked;

    /* Zameškané hodiny */
    } else if(index.column() < 12) {
        if(role == Qt::DisplayRole)
            return classesModel->index(absences[index.row()].classIndexes[index.column()-2], 1).data();
        if(role == Qt::CheckStateRole)
            return (absences[index.row()].hours >> (index.column()-2)) & 0x01 ? Qt::Checked : Qt::Unchecked;
    }

    return QVariant();
}

/* Zjištění předmětů v jednotlivých hodinách pro daný index */
void AbsencesModel::loadClassIds(int index) {
    /* Pročištění listu */
    absences[index].classIndexes.clear();

    /* Rozvrh platný daný den */
    int timetableIndex = timetableModel->timetableForDate(absences[index].date);

    /* Den v týdnu pro toto datum */
    int day = absences[index].date.dayOfWeek()-1;

    /* Žádný rozvrh ten den neplatil, nebo ten den je víkend
        => naplnění prázdnými hodinami */
    if(timetableIndex == -1 || day > 4) {
        for(int hour = 0; hour != 10; ++hour)
            absences[index].classIndexes.append(0);
        return;
    }

    /* Zjištění indexů všech souvisejících změn daný den */
    QList<int> changeIndexesList = changesModel->relatedChanges(absences[index].date);

    /* Zjištění jednotlivých hodin z rozvrhu */
    for(int hour = 0; hour != 10; ++hour) {
        absences[index].classIndexes.append(
            timetableModel->index(timetableIndex, 0).child(hour, day).data(Qt::EditRole).toInt());
    }

    /* Aplikování suplů */
    foreach(int changeIndex, changeIndexesList) {
        /* Číslo hodiny absence */
        int hourNumber = changesModel->index(changeIndex, 1).data(Qt::EditRole).toInt();

        /* Index hodiny, ze které měníme */
        int fromHour = changesModel->index(changeIndex, 2).data(Qt::EditRole).toInt();

        /* Index hodiny, na kterou měníme */
        int toHour = changesModel->index(changeIndex, 3).data(Qt::EditRole).toInt();

        /* Změna, který mění všechny hodiny */
        if(hourNumber == -1) {
            /* Změna všech hodin */
            for(int hour = 0; hour != 10; ++hour) {
                /* Pokud měníme z jakéhokoli předmětu a nebo předmět odpovídá, změníme jej */
                if(fromHour == classesModel->indexFromId(ClassesModel::WHATEVER) ||
                   absences[index].classIndexes[hour] == fromHour)
                    absences[index].classIndexes[hour] = toHour;
            }

            /* Další změny už neaplikujeme */
            return;

        /* Změna, která mění jen jednu hodinu */
        } else {
            /* Pokud měníme z jakéhokoli předmětu a nebo předmět odpovídá, změníme jej */
            if(fromHour == classesModel->indexFromId(ClassesModel::WHATEVER) ||
               absences[index].classIndexes[hourNumber] == fromHour)
                absences[index].classIndexes[hourNumber] = toHour;
        }
    }
}

}
