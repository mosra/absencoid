#ifndef ABSENCOID_SUMMARYTAB_H
#define ABSENCOID_SUMMARYTAB_H

#include <QWidget>
#include <QDate>

class QGroupBox;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QDateEdit;
class QLabel;

namespace Absencoid {

class ClassesModel;
class TeachersModel;
class HottestModel;
class AbsencesModel;
class ChangesModel;
class TimetableTab;

class SummaryTab: public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Konstruktor
         */
        SummaryTab(TeachersModel* teachersModel, ClassesModel* classesModel, TimetableTab* _timetableTab, ChangesModel* _changesModel, AbsencesModel* _absencesModel, QWidget* parent = 0);

    signals:
        /** @brief Databáze byla aktualizována */
        void updated();

    private slots:
        /** @brief Vytvoření zálohy */
        void createDump();

        /** @brief Vytvoření aktualizačního souboru */
        void createUpdate();

        /**
         * @brief Ověření správnosti URL v editovacím políčku
         *
         * Pokud má URL nesprávný formát, je políčko zobrazeno s červeným pozadím
         */
        void validateUrlEdit();

        /** @brief Načtení dat do políček */
        void reload();

        /** @brief Nastavení data začátku pololetí */
        void setBeginDate();

        /** @brief Nastavení data konce pololetí */
        void setEndDate();

        /** @brief Nastavení aktuálního rozvrhu */
        void setActiveTimetable();

        /** @brief Nastavení URL pro aktualizaci z internetu */
        void setWebUpdateUrl();

        /** @brief Nastavení aktualizace po startu / zálohování při ukončení */
        void setBooleans();

        /** @brief Aktualizace z internetu */
        void updateFromWeb();

        /** @brief Aktualizace z internetu, pokud nenajde novější aktualizaci, neaktualizuje */
        void updateFromWebSilent();

        /** @brief Aktualizace ze souboru */
        void updateFromFile();

        /** @brief Obnovení zálohy */
        void loadDump();

        /** @brief Statistika */
        void reloadStatistics();

    private:
        QDateEdit* beginDate;       /** @brief Začátek pololetí */
        QDateEdit* endDate;         /** @brief Konec pololetí */
        QComboBox* activeTimetable; /** @brief Aktuální rozvrh */
        QGroupBox* updateGroup;     /** @brief Groupbox s aktualizací */
        QDate      lastUpdate;      /** @brief Datum poslední aktualizace */
        QCheckBox* updateOnStart;   /** @brief Aktualizovat po startu */
        QCheckBox* dumpOnExit;      /** @brief Zálohovat při ukončení */
        QLineEdit* webUpdateUrl;    /** @brief Políčko s URL pro aktualizaci z internetu */
        QAction* updateFromWebAction; /** @brief Akce menu pro aktualizaci z internetu */

        QLabel* statsAllAbsences;   /** @brief Počet absencí */
        QLabel* statsAllAbsencesPercent;    /** @brief Absence v procentech */
        QLabel* statsAllHours;      /** @brief Počet hodin */
        QLabel* statsAllHoursForecast;  /** @brief Počet hodin do konce pololetí */
        QLabel* statsAddedHours;    /** @brief Počet přidaných hodin */
        QLabel* statsAddedHoursForecast;    /** @brief Počet přidaných hodin do konce pololetí */
        QLabel* statsRemovedHours;  /** @brief Počet ubraných hodin */
        QLabel* statsRemovedHoursForecast;  /** @brief Počet odebraných hodin do konce pololetí */
        QLabel* statsDeltaHours;    /** @brief Počet přidaných / odebraných hodin */
        QLabel* statsDeltaHoursForecast;    /** @brief Počet přidaných / odebraných hodin do konce pololetí */

        TimetableTab* timetableTab; /** @brief Tab s rozvrhy */
        ChangesModel* changesModel; /** @brief Model změn */
        AbsencesModel* absencesModel;   /** @brief Model absencí */
        HottestModel* hottestModel;     /** @brief Model nejžhavějších absencí */
};

}

#endif
