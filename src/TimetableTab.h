#ifndef TIMETABLETAB_H
#define TIMETABLETAB_H

#include <QWidget>

namespace Absencoid {
class ClassesModel;

/**
 * @brief Panel s rozvrhem hodin
 */
class TimetableTab: public QWidget {
    public:
        /**
         * @brief Konstruktor
         *
         * @param   classesModel   Ukazatel na model předmětů
         * @param   parent          Rodičovský widget
         */
        TimetableTab(ClassesModel* classesModel, QWidget* parent = 0);
};

}

#endif
