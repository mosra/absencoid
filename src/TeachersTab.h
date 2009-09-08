#ifndef TEACHERSTAB_H
#define TEACHERSTAB_H

#include <QWidget>

class QTableView;

namespace Absencoid {

class TeachersModel;

/**
 * @brief Tab s daty učitelů
 */
class TeachersTab: public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Konstruktor
         */
        TeachersTab(QWidget* parent = 0);

    private:
        TeachersModel* teachersModel;   /** @brief Model s daty učitelů */
        QTableView* teachersView;       /** @brief Zobrazení učitelů */

    private slots:

        /**
         * @brief Přidání učitele
         */
        void addTeacher();

        /**
         * @brief Odebrání vybraných učitelů
         */
        void removeTeachers();
};

}

#endif
