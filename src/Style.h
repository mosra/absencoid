#ifndef ABSENCOID_STYLE_H
#define ABSENCOID_STYLE_H

#include <QIcon>
#include <QHash>
#include <QPixmap>

namespace Absencoid {

/**
 * @brief Třída pro globální přístup k ikonám
 */
class Style: public QObject {
    public:
        /**
         * @brief Ikonky tabů, tlačítek, statusy
         *
         * Složka 0xF0 hodnoty znamená číslo řádku, 0x0F znamená pozici na řádku.
         */
        enum Icon {
            SummaryTab          = 0x00, /**< @brief Sourhn */
            TeachersTab         = 0x01, /**< @brief Učitelé */
            ClassesTab          = 0x02, /**< @brief Předměty */
            TimetableTab        = 0x03, /**< @brief Rozvrhy */
            ChangesTab          = 0x04, /**< @brief Změny */
            AbsencesTab         = 0x05,  /**< @brief Absence */

            UpdateIcon          = 0x10, /**< @brief Aktualizovat */
            CreateUpdateIcon    = 0x11, /**< @brief Vytvořit aktualizaci */
            UpdateFromWebIcon   = 0x12, /**< @brief Aktualizovat z internetu */
            UpdateFromFileIcon  = 0x13, /**< @brief Aktualizovat ze souboru */
            DumpIcon            = 0x14, /**< @brief Zálohovat */
            LoadDumpIcon        = 0x15, /**< @brief Obnovit ze zálohy */

            MinusIcon           = 0x20, /**< @brief Mínus */
            PlusIcon            = 0x21, /**< @brief Plus */
            LockIcon            = 0x22, /**< @brief Zamknout */
            UnlockIcon          = 0x23, /**< @brief Odemknout */
            ExitIcon            = 0x24, /**< @brief Ukončit */
            HelpIcon            = 0x25,  /**< @brief Nápověda */

            Action              = 0x30, /**< @brief Spuštění akce */
            Information         = 0x31, /**< @brief Poznámka */
            Done                = 0x32, /**< @brief Akce dokončena */
            Error               = 0x33, /**< @brief Nastala chyba */

            ValidTimetable      = 0x32, /**< @brief Právě platný rozvrh */
            ActiveTimetable     = 0x31, /**< @brief Aktivní rozvrh */

            Blank               = 0xFF  /**< @brief Prázdná ikonka */
        };

        /**
         * @brief Konstruktor
         *
         * Konstruktor je třeba spustit před tím, než začneme načítat nějaké
         * ikony.
         *
         * @param   file        Soubor s ikonami
         * @param   parent      Rodičovský objekt
         */
        Style(const QString& file, QObject* parent = 0);

        /**
         * @brief Ukazatel na inicializovaný objekt
         */
        static inline Style* style() { return object; }

        /**
         * @brief Získání ikony
         */
        const QIcon& icon(Icon icon);

    private:
        static Style* object;       /**< @brief Ukazatel na inicializovaný objekt */
        QHash<int, QIcon> icons;    /**< @brief Ikony */
        QPixmap iconFile;           /**< @brief Soubor s ikonami */
};

}

#endif
