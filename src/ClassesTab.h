#ifndef CLASSESTAB_H
#define CLASSESTAB_H

#include <QWidget>

namespace Absencoid {
class ClassesModel;
class TeachersModel;

/**
 * @brief Tab s předměty
 */
class ClassesTab: public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Konstruktor
         *
         * @param   teachersModel   Ukazatel na model učitelů
         * @param   parent          Rodičovský widget
         */
        ClassesTab(TeachersModel* teachersModel, QWidget* parent = 0);

    private:
        ClassesModel* classesModel; /** @brief Model předmětů */

    private slots:
        /**
         * @brief Přidání předmětu
         */
        void addClass();
};

}

#endif
