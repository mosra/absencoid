#ifndef TIMETABLETAB_H
#define TIMETABLETAB_H

#include <QWidget>

class QLineEdit;
class QDateEdit;
class QComboBox;

namespace Absencoid {

class TimetableListModel;
class ClassesModel;

/**
 * @brief Panel s rozvrhem hodin
 */
class TimetableTab: public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Konstruktor
         *
         * @param   classesModel    Ukazatel na model předmětů
         * @param   parent          Rodičovský widget
         */
        TimetableTab(ClassesModel* classesModel, QWidget* parent = 0);

    private:
        TimetableListModel* timetableListModel; /** @brief Model seznamu rozvrhů */
        QComboBox* timetableCombo;  /** @brief Políčko pro výběr rozvrhu */
        QLineEdit* description;     /** @brief Popisek rozvrhu */
        QDateEdit *validFrom,       /** @brief Počátek platnosti aktuálního rozvrhu */
                  *validTo;         /** @brief Konec platnosti aktuálního rozvrhu */
        QComboBox* followedBy;      /** @brief Výběr rozvrhu, který bude tento následovat */

    private slots:

        /**
         * @brief Načtení rozvrhu
         *
         * @param   index           Index aktuální položky
         */
        void loadTimetable(int index);

        /**
         * @brief Změna tooltipu u comboboxu pro následující rozvrh
         *
         * @param   index           Index aktuální položky
         */
        void changeFollowedByTooltip(int index);
};

}

#endif
