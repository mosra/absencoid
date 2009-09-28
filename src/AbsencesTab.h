#ifndef ABSENCOID_ABSENCESTAB_H
#define ABSENCOID_ABSENCESTAB_H

#include <QWidget>

namespace Absencoid {

class ClassesModel;
class TimetableModel;
class ChangesModel;

class AbsencesTab: public QWidget {
    public:
        /**
         * @brief Konstruktor
         */
        AbsencesTab(ClassesModel* classesModel, TimetableModel* timetablesModel, ChangesModel* changesModel, QWidget* parent = 0);
};

}

#endif
