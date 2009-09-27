#ifndef ABSENCOID_TIMETABLETAB_H
#define ABSENCOID_TIMETABLETAB_H

#include <QWidget>

class QTableView;
class QLabel;
class QLineEdit;
class QDateEdit;
class QComboBox;
class QPushButton;

namespace Absencoid {

class TimetableModel;
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

        /**
         * @brief Vrácení ukazatele na model rozvrhu
         */
        inline TimetableModel* getTimetableModel() const { return timetableModel; }

    private:
        TimetableModel* timetableModel;     /** @brief Model dat rozvrhu */
        QTableView* timetableView;  /** @brief Tabulka rozvrhu */
        QComboBox* timetableCombo;  /** @brief Políčko pro výběr rozvrhu */
        QPushButton* removeTimetableButton; /** @brief Smazání rozvrhu */
        QLabel* descriptionLabel;   /** @brief Label k popisku rozvrhu */
        QLineEdit* description;     /** @brief Popisek rozvrhu */
        QLabel* validFromLabel;     /** @brief Label k počátku platnosti */
        QDateEdit *validFrom;       /** @brief Počátek platnosti aktuálního rozvrhu */
        QLabel* followedByLabel;    /** @brief Label k následujícímu rozvrhu */
        QComboBox* followedBy;      /** @brief Výběr rozvrhu, který bude tento následovat */
        QPushButton* removeLessonsButton;   /** @brief Odstranění vybraných hodin */
        QPushButton* fixLessonsButton;  /** @brief Zamknutí vybraných hodin */

    public slots:
        /**
         * @brief Načtení rozvrhu
         *
         * @param   index           Index aktuální položky
         */
        void loadTimetable(int index);

    private slots:
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
         * @brief Zamknutí / odemknutí vybraných hodin
         */
        void toggleFixedLessons(bool setFixed);

        /**
         * @brief Zašednutí / povolení mazacího a zamykacího tlačítka
         *
         * Tlačítka jsou aktivní, jen pokud jsou nějaké vybrané hodiny. Pokud
         * jsou vybrané samé zamknuté hodiny, je zamykací tlačítko nastaveno
         * na zamáčknutý stav.
         */
        void updateRemoveFixedButtons();
};

}

#endif
