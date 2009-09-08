#ifndef TEACHERSTAB_H
#define TEACHERSTAB_H

#include <QWidget>

class QPushButton;
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
        TeachersModel* teachersModel;       /** @brief Model s daty učitelů */
        QTableView* teachersView;           /** @brief Zobrazení učitelů */
        QPushButton* removeTeachersButton;  /** @brief Tlačítko pro odstranění učitelů */

    private slots:

        /**
         * @brief Přidání učitele
         */
        void addTeacher();

        /**
         * @brief Odebrání vybraných učitelů
         */
        void removeTeachers();

        /**
         * @brief Zašedne / povolí tlačítko pro odebírání vybraných učitelů
         *
         * Volané při změně výběru. Pokud není nic vybrané, tlačítko je
         * zašedlé.
         */
        void updateRemoveButton();
};

}

#endif
