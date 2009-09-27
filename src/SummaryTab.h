#ifndef ABSENCOID_SUMMARYTAB_H
#define ABSENCOID_SUMMARYTAB_H

#include <QWidget>

#include "Dump.h"

class QLineEdit;

namespace Absencoid {

class TimetableModel;
class ConfigurationModel;

class SummaryTab: public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Konstruktor
         */
        SummaryTab(TimetableModel* timetableModel, QWidget* parent = 0);

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

    private:
        Dump dump;                  /** @brief Třída pro vytváření a načítání XML dumpů */
        QLineEdit* webUpdateUrl;    /** @brief Políčko s URL pro aktualizaci z internetu */
        QAction* updateFromWeb;     /** @brief Akce menu pro aktualizaci z internetu */

        /** @brief Model pro konfiguraci */
        ConfigurationModel* configurationModel;
};

}

#endif
