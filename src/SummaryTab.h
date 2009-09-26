#ifndef ABSENCOID_SUMMARYTAB_H
#define ABSENCOID_SUMMARYTAB_H

#include <QWidget>

#include "Dump.h"

namespace Absencoid {

class SummaryTab: public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Konstruktor
         */
        SummaryTab(QWidget* parent = 0);

    private slots:
        /**
         * @brief Vytvoření zálohy
         */
        void createDump();

        /**
         * @brief Vytvoření aktualizačního souboru
         */
        void createUpdate();

    private:
        Dump dump;  /** @brief Třída pro vytváření a načítání XML dumpů */
};

}

#endif
