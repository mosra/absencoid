#ifndef TIMETABLETAB_H
#define TIMETABLETAB_H

#include <QWidget>

class QTableView;
class QLabel;
class QLineEdit;
class QDateEdit;
class QComboBox;
class QPushButton;

namespace Absencoid {

class TimetableModel;
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
        TimetableModel* timetableModel;     /** @brief Model dat rozvrhu */
        QTableView* timetableView;  /** @brief Tabulka rozvrhu */
        QComboBox* timetableCombo;  /** @brief Políčko pro výběr rozvrhu */
        QPushButton* removeTimetableButton; /** @brief Smazání rozvrhu */
        QPushButton* switchDirectionButton; /** @brief Změna směru rozvrhu */
        QLabel* descriptionLabel;   /** @brief Label k popisku rozvrhu */
        QLineEdit* description;     /** @brief Popisek rozvrhu */
        QLabel* validFromLabel;     /** @brief Label k počátku platnosti */
        QDateEdit *validFrom;       /** @brief Počátek platnosti aktuálního rozvrhu */
        QLabel* followedByLabel;    /** @brief Label k následujícímu rozvrhu */
        QComboBox* followedBy;      /** @brief Výběr rozvrhu, který bude tento následovat */
        QPushButton* removeLessonsButton;   /** @brief Odstranění vybraných hodin */

    private slots:

        /**
         * @brief Načtení rozvrhu
         *
         * @param   index           Index aktuální položky
         */
        void loadTimetable(int index);

        /**
         * @brief Přidání rozvrhu
         *
         * Přidá nový rozvrh s default údaji a načte jej
         */
        void addTimetable();

        /**
         * @brief Odebrání rozvrhu
         *
         * Po potvrzení odebere rozvrh i s daty
         */
        void removeTimetable();

        /**
         * @brief Nastavení popisku aktuálního rozvrhu
         */
        void setDescription();

        /**
         * @brief Nastavení začátku platnosti aktuálního rozvrhu
         */
        void setValidFrom();

        /**
         * @brief Nastavení následujícího rozvrhu
         */
        void setFollowedBy();

        /**
         * @brief Odstranění vybraných hodin z rozvrhu
         */
        void removeLessons();

        /**
         * @brief Zašednutí / povolení mazacího tlačítka
         *
         * Tlačítko pro mazání hodin je aktivní, jen pokud jsou nějaké vybrané
         */
        void updateRemoveButton();
};

}

#endif
