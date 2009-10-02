#ifndef ABSENCOID_SUMMARYTAB_H
#define ABSENCOID_SUMMARYTAB_H

#include <QWidget>

class QGroupBox;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QDateEdit;

namespace Absencoid {

class TimetableModel;
class TimetableTab;
class ConfigurationModel;

class SummaryTab: public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Konstruktor
         */
        SummaryTab(TimetableTab* _timetableTab, QWidget* parent = 0);

    private slots:
        /**
         * @brief Vytvoření zálohy
         */
        void createDump();

        /**
         * @brief Vytvoření aktualizačního souboru
         */
        void createUpdate();

        /**
         * @brief Ověření správnosti URL
         */
        void validateUrlEdit();

        /**
         * @brief Nastavení data začátku pololetí
         */
        void setBeginDate();

        /**
         * @brief Nastavení data konce pololetí
         */
        void setEndDate();

        /**
         * @brief Nastavení aktuálního rozvrhu
         */
        void setActiveTimetable();

        /**
         * @brief Nastavení URL pro aktualizaci z internetu
         */
        void setWebUpdateUrl();

        /**
         * @brief Nastavení aktualizace po startu
         */
        void setUpdateOnStart();

        /**
         * @brief Nastavení zálohování při ukončení
         */
        void setDumpOnExit();

    private:
        QDateEdit* beginDate;       /** @brief Začátek pololetí */
        QDateEdit* endDate;         /** @brief Konec pololetí */
        QComboBox* activeTimetable; /** @brief Aktuální rozvrh */
        QGroupBox* updateGroup;     /** @brief Groupbox s aktualizací */
        QCheckBox* updateOnStart;   /** @brief Aktualizovat po startu */
        QCheckBox* dumpOnExit;      /** @brief Zálohovat při ukončení */
        QLineEdit* webUpdateUrl;    /** @brief Políčko s URL pro aktualizaci z internetu */
        QAction* updateFromWeb;     /** @brief Akce menu pro aktualizaci z internetu */

        /** @brief Model pro konfiguraci */
        ConfigurationModel* configurationModel;

        /** @brief Tab s rozvrhy */
        TimetableTab* timetableTab;
};

}

#endif
