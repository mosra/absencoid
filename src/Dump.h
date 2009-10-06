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
            DUMP = 0x01,                        /** @brief Vytvořit zálohu */
            UPDATE = 0x02,                      /** @brief Vytvořit aktualizaci */
            INDENT = 0x04                       /** @brief Odsazovat XML */
        };

    public:
        /**
         * @brief Flags pro konfiguraci
         * @todo Najít pro to lepší místo!
         */
        enum ConfigurationFlags {
            UPDATE_ON_START = 1,                /** @brief Aktualizovat při startu */
            DUMP_ON_EXIT = 2                    /** @brief Zálohovat při ukončení */
        };

        static const int DUMP_VERSION = 1;      /** @brief Verze dumpů */
        static const int UPDATE_VERSION = 1;    /** @brief Verze aktualizací */

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
        * @brief Univerzální funkce pro vytvoření dumpu
        */
        QString create(int flags, const QString& note = QString());

};

}

#endif
