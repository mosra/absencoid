#ifndef ABSENCOID_DUMP_H
#define ABSENCOID_DUMP_H

#include <QObject>
#include <QDomDocument>

namespace Absencoid {

/**
 * @brief Třída pro vytváření a obnovování záloh a aktualizačních souborů v XML
 *
 * Po provedení jakékoli funkce je možné získat statistiku o provedených změnách
 * pomocí delta- funkcí.
 */
class Dump: public QObject {
    private:
        /** @brief Flags pro vytváření dumpu */
        enum Flags {
            DUMP = 0x01,
            UPDATE = 0x02,
            INDENT = 0x04
        };

    public:
        static const int
            DUMP_VERSION,       /** @brief Verze dumpů */
            UPDATE_VERSION;     /** @brief Verze aktualizací */

        /**
         * @brief Konstruktor
         *
         * @param   indent  Zda odsazovat vytvořené XML soubory
         */
        Dump(bool indent = true);

        /**
         * @brief Vytvoření kompletní zálohy
         *
         * Vytvoří XML soubor se zálohou všech dat i uživatelského nastavení.
         */
        inline QString createDump(void) { return create(DUMP|flags); }

        /**
         * @brief Vytvoření aktualizačního souboru
         *
         * Vytvoří XML soubor se zálohou  učitelů, předmětů, zamknutých dat
         * rozvrhů a změn. Z nastavení uloží jen začátek a konec pololetí a
         * adresu pro aktualizace z internetu.
         * @param   note    Poznámka k aktualizaci
         * @return  Vytvořený XML soubor
         */
        inline QString createUpdate(const QString& note) { return create(UPDATE|flags, note); }

        /**
         * @brief Aktualizace / obnovení ze zálohy
         *
         * Načte obsah XML souboru zpět do databáze.
         */
        bool loadDump(const QString& dump);

        /**
         * @brief Počet ovlivněných učitelů po poslední akci
         *
         * Vrací počet učitelů, kteří byli ovlivněni poslední akcí. Při exportu
         * vrací počet exportovaných učitelů, pokud je např. při aktualizaci
         * 5 učitelů smazáno, vrací -5.
         */
        inline int deltaTeachers() const { return _deltaTeachers; }

        /** @brief Počet ovlivněných předmětů po poslední akci */
        inline int deltaClasses() const { return _deltaClasses; }

        /** @brief Počet ovlivněných rozvrhů po poslední akci */
        inline int deltaTimetables() const { return _deltaTimetables; }

        /** @brief Počet ovlivněných dat rozvrhů po poslední akci */
        inline int deltaTimetableData() const { return _deltaTimetableData; }

        /** @brief Počet ovlivněných změn po poslední akci */
        inline int deltaChanges() const { return _deltaChanges; }

        /** @brief Počet ovlivněných absencí po poslední akci */
        inline int deltaAbsences() const { return _deltaAbsences; }

    private:
        int flags;                  /** @brief Flags */

        int _deltaTeachers;         /** @brief Počet ovlivněných učitelů po poslední akci */
        int _deltaClasses;          /** @brief Počet ovlivněných předmětů po poslední akci */
        int _deltaTimetables;       /** @brief Počet ovlivněných rozvrhů po poslední akci */
        int _deltaTimetableData;    /** @brief Počet ovlivněných dat rozvrhů po poslední akci */
        int _deltaChanges;          /** @brief Počet ovlivněných změn po poslední akci */
        int _deltaAbsences;         /** @brief Počet ovlivněných absencí po poslední akci */

        QDomDocumentType doctype;   /** @brief Doctype používané pro vytváření souborů */

        /**
         * @brief Pročištění statistik
         *
         * Nastaví všechny _delta- proměnné na nulu.
         */
        void cleanStats();

        /**
        * @brief Univerzální funkce pro vytvoření dumpu
        */
        QString create(int flags, const QString& note = QString());

};

}

#endif
