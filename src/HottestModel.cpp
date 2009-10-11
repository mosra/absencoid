#include "HottestModel.h"

#include <QDebug>

#include "TeachersModel.h"
#include "ClassesModel.h"
#include "TimetableModel.h"
#include "ChangesModel.h"
#include "AbsencesModel.h"
#include <QFont>

namespace Absencoid {

/* Konstruktor */
HottestModel::HottestModel(TeachersModel* _teachersModel, ClassesModel* _classesModel, TimetableModel* _timetableModel, ChangesModel* _changesModel, AbsencesModel* _absencesModel, QObject* parent): QAbstractTableModel(parent), teachersModel(_teachersModel), classesModel(_classesModel), timetableModel(_timetableModel), changesModel(_changesModel), absencesModel(_absencesModel) {
    reload();
}

/* (Znovu)načtení dat modelu */
void HottestModel::reload() {
    classes.clear();

    /* Zjištění nejbližšího staršího data všedního dne */
    QDate date = QDate::currentDate();
    if(date.dayOfWeek() > 5) date = date.addDays(5-date.dayOfWeek());

    /* Shrnutí předmětů bez absence do jednoho */
    Class other = {0, 0, 0, 0};

    /* Naplnění dat */
    for(int classIndex = 0; classIndex != classesModel->rowCount(); ++classIndex) {
        int id = classesModel->idFromIndex(classIndex);

        /* Pokud tento předmět je speciální či neexistuje v aktuálním rozvrhu, nepočítat */
        if(id == 0 || id == ClassesModel::WHATEVER || timetableModel->timetablesWithThisLesson(date, 0xFF, id, true) != 1)
            continue;

        int absences = absencesModel->absencesCount(id);
        int hours = timetableModel->lessonCount(id, true)+changesModel->deltaHours(id, true);
        int hoursForecast = timetableModel->lessonCount(id)+changesModel->deltaHours(id);

        /* Nulový počet absencí => přidáváme do "ostatních předmětů" */
        if(absences == 0) {
            other.hours += hours;
            other.hoursForecast += hoursForecast;

        /* Normálka ... */
        } else {
            Class c;
            c.id = id;
            c.hours = hours;
            c.hoursForecast = hoursForecast;
            c.absences = absences;
            classes.append(c);
        }
    }

    /* Přidání ostatních předmětů na konec */
    classes.append(other);

    /* Seřazení od největších absencí k nejmenším */
    qSort(classes.begin(), classes.end(), qGreater<Class>());

    reset();
}

/* Počet sloupců */
int HottestModel::columnCount(const QModelIndex& parent) const {
    return 3;
}

/* Počet řádků */
int HottestModel::rowCount(const QModelIndex& parent) const {
    return classes.count();
}

/* Hlavičky */
QVariant HottestModel::headerData(int section, Qt::Orientation orientation, int role) const {
    /* Horizontální hlavičky */
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case 0: return tr("Předmět");
            case 1: return tr("Absence dosud");
            case 2: return tr("Odhad celkem");
        }

    /* Vertikální hlavičky - nevracíme nic */
    } else if(orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return "";
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

/* Čtecí přístup k datům */
QVariant HottestModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid()) return QVariant();

    /* Název předmětu (bez jména učitele) */
    if(index.column() == 0 && role == Qt::DisplayRole) {
        /* ID = 0 => sourhn ostatních předmětů bez absence */
        if(classes[index.row()].id == 0) return tr("Ostatní");

        return classesModel->index(classesModel->indexFromId(classes[index.row()].id), 1).data();

    /* Počet absencí dosud */
    } else if(index.column() == 1) {
        int absences = classes[index.row()].absences;
        int hours = classes[index.row()].hours;

        int percent;
        if(hours == 0) percent = 0;
        else percent = absences*100/hours;

        if(role == Qt::DisplayRole)
            return tr("%1/%2 (%3%)").arg(absences).arg(hours).arg(percent);
        if(role == Qt::FontRole && percent >= 25) {
            QFont font;
            font.setBold(true);
            return font;
        }

    /* Optimistický odhad */
    } else if(index.column() == 2) {
        int absences = classes[index.row()].absences;
        int hoursForecast = classes[index.row()].hoursForecast;

        int percent;
        if(hoursForecast == 0) percent = 0;
        else percent = absences*100/hoursForecast;

        if(role == Qt::DisplayRole)
            return tr("%1/%2 (%3%)").arg(absences).arg(hoursForecast).arg(percent);
        if(role == Qt::FontRole && percent >= 25) {
            QFont font;
            font.setBold(true);
            return font;
        }
    }

    return QVariant();
}

/* Porovnávací operátor pro absence */
bool HottestModel::Class::operator<(const Absencoid::HottestModel::Class& c) const {
    int percent1, percent2;
    if(hours == 0)  percent1 = 0;
    else            percent1 = absences*100/hours;

    if(c.hours == 0)    percent2 = 0;
    else                percent2 = c.absences*100/c.hours;

    return percent1 < percent2;
}


}
