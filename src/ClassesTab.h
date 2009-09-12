#ifndef CLASSESTAB_H
#define CLASSESTAB_H

#include <QWidget>

namespace Absencoid {
class TeachersModel;

/**
 * @brief Tab s předměty
 */
class ClassesTab: public QWidget {
    public:
        /**
         * @brief Konstruktor
         *
         * @param   teachersModel   Ukazatel na model učitelů
         * @param   parent          Rodičovský widget
         */
        ClassesTab(TeachersModel* teachersModel, QWidget* parent = 0);
};

}

#endif
