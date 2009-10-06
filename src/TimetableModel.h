#ifndef ABSENCOID_TIMETABLEMODEL_H
#define ABSENCOID_TIMETABLEMODEL_H

#include <QAbstractItemModel>
#include <QDate>

namespace Absencoid {

class ClassesModel;

/**
 * @brief Model rozvrhu
 *
 * Poskytuje data všech rozvrhů v hierarchickém uspořádání. Kořenové položky
 * jsou <strong>jednotlivé rozvrhy</strong>, jejich potomci potom <strong>data
 * příslušného rozvrhu</strong>.
 *
 * Řádek v seznamu rozvrhů obsahuje tato data:
 * <ul>
 *  <li>Sourhnný popis, např. <tt>Nový rozvrh (platný od 07.09.2009)</tt>
 *  (needitovatelná).</li>
 *  <li>Název rozvrhu, tedy <tt>Nový rozvrh</tt></li>
 *  <li>Datum začátku platnosti, tedy <tt>2009-09-07</tt></li>
 *  <li>Následující rozvrh:
 *   <ul>
 *    <li><tt>Qt::DisplayRole</tt>: název</li>
 *    <li><tt>Qt::EditRole</tt>: index (číslo řádku)</li>
 *   </ul>
 *  </li>
 * </ul>
 *
 * Data příslušného rozvrhu jsou v tabulce, kde jsou na horizontální ose dny
 * (pondělí - pátek) a na vertikální ose hodiny (nultá až devátá). Každá položka
 * obsahuje data předmětu (převzatá z Absencoid::ClassesModel):
 * <ul>
 *  <li><tt>Qt::DisplayRole</tt>: popisek, tj. "název předmětu (učitel)"</li>
 *  <li><tt>Qt::EditRole</tt>: index (číslo řádku)</li>
 * </ul>
 */
class TimetableModel: public QAbstractItemModel {
    Q_OBJECT

    public:
        /**
         * @brief Označení zamknuté položky
         * @todo Raději nějakou menší + jednobitovou hodnotu!
         */
        static const int FIXED = 0x70000000;

        /**
         * @brief Konstruktor
         *
         * @param   _classesModel   Ukazatel na model předmětů
         * @param   parent          Rodičovský widget
         */
        TimetableModel(ClassesModel* _classesModel, QObject* parent = 0);

        /**
         * @brief Vytvoření indexu příslušné položky
         *
         * Pro rozlišení, jestli patří index položce v seznamu rozvrhů nebo
         * datům rozvrhu je použito <tt>internalId</tt>. Hodnota NO_PARENT značí,
         * že index náleží položce v seznamu rozvrhů, jakékoliv jiné číslo určuje
         * číslo řádku nadřazeného rozvrhu, tj. index náleží datům rozvrhu.
         */
        virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

        /**
         * @brief Zjištění rodičovské položky daného indexu
         *
         * Pokud je <tt>internalId</tt> předaného indexu rovno NO_PARENT, vrací
         * QModelIndex(). Jakékoliv jiné <tt>internalId</tt> specifikuje číslo
         * řádku nadřazeného rozvrhu a funkce tedy vrací index první položky
         * řádku s nadřazeným rozvrhem.
         */
        virtual QModelIndex parent(const QModelIndex& child) const;

        /**
         * @brief Počet sloupců
         *
         * Položky v seznamu rozvrhů mají 4 sloupce (popis, název, začátek
         * platnosti, další rozvrh), data rozvrhu mají 5 sloupců (pondělí -
         * pátek).
         */
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * @brief Počet řádků
         *
         * Počet řádků v seznamu rozvrhů zásií na počtu rozvrhů, kdežto data
         * rozvrhu mají 10 řádků (nultá až devátá hodina).
         */
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * @brief Hlavičky
         *
         * Protože seznam rozvrhů není aplikačně zobrazován ve formě tabulky,
         * jsou vraceny právě a jen hlavičky pro tabulku rozvrhu, tj. den
         * horizontálně a číslo hodiny vertikálně.
         */
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        /**
         * @brief Čtecí přístup k datům
         *
         * U dat rozvrhu lze použít Qt::UserRole pro zjištění, zda je položka
         * zamknutá (když je vrácená hodnota nenulová).
         */
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

        /**
         * @brief Flags
         */
        virtual Qt::ItemFlags flags(const QModelIndex& index) const;

        /**
         * @brief Zápisový přístup k datům
         *
         * Kromě Qt::EditRole lze na položky rozvrhu použít Qt::UserRole, při
         * předání nulové hodnoty se položka označí jako odemknutá, při ostatních
         * se označí jako zamknutá (a bude editovatelná jen správcem).
         */
        virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

        /**
         * @brief Přidání řádku
         *
         * Pokud <tt>parent</tt> odpovídá kořenovému seznamu rozvrhů (tj. je
         * neplatný), přidá nový rozvrh. Jinak nedělá nic.
         * @todo Implementovat raději insertRows(int, int, const QModelIndex&);
         */
        virtual bool insertRow(int row, const QModelIndex& parent = QModelIndex());

        /**
         * @brief Odebrání řádku
         *
         * Pokud <tt>parent</tt> odpovídá kořenovému seznamu rozvrhů (tj. je
         * neplatný), odebere daný rozvrh. Jinak nedělá nic.
         * @todo Implementovat raději removeRows(int, int, const QModelIndex&);
         */
        virtual bool removeRow(int row, const QModelIndex& parent = QModelIndex());

        /**
         * @brief Zjištění ID rozvrhu z indexu
         *
         * @note Komplexnost funkce: konstatní
         * @param   index       Index rozvrhu (číslo řádku)
         * @return  ID rozvrhu
         */
        int idFromIndex(int index) const;

        /**
         * @brief Zjištění indexu z ID
         *
         * @note Komplexnost funkce: lineární podle počtu rozvrhů
         * @param   id          ID rozvrhu
         * @return  Index rozvrhu
         */
        int indexFromId(int id) const;

        /**
         * @brief Spočítání dne/hodiny
         *
         * Pokud nejsou parametry v povoleném rozsahu, vrací nulu (tj.
         * nultá hodina v pondělí).
         * @param   day         Číslo dne (0-4, 0 = pondělí)
         * @param   hour        Hodina (0-9, 0 = nultá hodina)
         * @return  Den/hodina
         */
        static inline int dayHour(int day, int hour) {
            if(day < 0 || day > 4 || hour < 0 || hour > 9) return 0;
            return day << 4 | hour;
        }

        /**
         * @brief Zjištění dne z dne/hodiny
         *
         * Pokud je předaný den/hodina neplatný, vrací 0 (pondělí).
         */
        static inline int day(int dayHour) {
            int day = (dayHour & 0xF0) >> 4;
            if(day < 0 || day > 4) return 0;
            return day;
        }

        /**
         * @brief Zjištění hodiny z dne/hodiny
         *
         * Pokud je předaný den/hodina neplatný, vrací 0 (nultá hodina).
         */
        static inline int hour(int dayHour) {
            int hour = dayHour & 0x0F;
            if(hour < 0 || hour > 9) return 0;
            return hour;
        }

        /**
         * @brief Seznam rozvrhů platných v dané datum
         *
         * @param   date        Datum
         * @param   activeOnly  Hledat jen aktivní rozvrh
         * @return  Seznam indexů platných rozvrhů, pokud je nastaveno hledání
         *          jen aktivního rozvrhu, vrací jednoprvkové nebo prázdné pole.
         */
        QList<int> validTimetables(QDate date, bool activeOnly = false);

        /**
         * @brief Počet platných rozvrhů, které obsahují daný předmět
         *
         * Vrátí počet platných rozvrhů, které obsahují daný předmět. V
         * závislosti na parametrech se předmět hledá v předaném dni a příslušné
         * hodině, v celém předaném dni nebo v celém rozvrhu.
         * @param   date        Datum
         * @param   hour        Číslo hodiny (0 - 9), pokud je uvedeno
         *          ChangesModel::ALL_HOURS, hledá se ve všech hodinách daného
         *          dne, pokud je uvedeno ještě něco jiného, hledá se v celém
         *          rozvrhu.
         * @param   classId     ID předmětu, pokud je uvedeno
         *          ClassesModel::WHATEVER, jen se zjišťuje, zda na daném místě
         *          je nějaká hodina.
         * @param   activeOnly  Hledat jen v aktivním rozvrhu
         * @return  Počet rozvrhů, které odpovídají dotazu, pokud je nastaveno
         *          hledání jen v aktivním rozvrhu, vrací buď 1 (nalezeno) nebo
         *          0 (nenalezeno).
         */
        int timetablesWithThisLesson(QDate date, int hour, int classId, bool activeOnly = false);

        /**
         * @brief Zjištění, zda je rozvrh aktivní
         *
         * Tj. s flagem ACTIVE
         */
        bool isActive(int index) {
            if(index < 0 || index > timetables.count()) return false;
            return timetables[index].flags & ACTIVE;
        }

        /**
         * @brief Nalezení předka rozvrhu
         *
         * Nalezne rozvrh, který má tento rozvrh jako následovníka
         * @return  Index nalezeného rozvrhu, nebo index tohoto rozvrhu, pokud
         *          není žádný předchozí nalezen.
         */
        int previousTimetable(int index);

        /** @brief Datum začátku pololetí */
        inline QDate beginDate() const { return _beginDate; }

        /** @brief Datum konce pololetí */
        inline QDate endDate() const { return _endDate; }

    signals:
        /**
         * @brief Signál o změně aktuálního rozvrhu
         */
        void actualTimetableChanged();

        /**
         * @brief Signál o změně data začátku a konce pololetí
         */
        void dateRangeChanged();

    public slots:
        /**
         * @brief (Znovu)načtení dat z databáze
         *
         * Pokud nejde o první načtení, vyšle také resetovací signál.
         */
        void reload();

        /** @brief Nastavení aktivního rozvrhu */
        void setActualTimetable(int index);

        /** @brief Nastavení začátku pololetí */
        void setBeginDate(QDate date);

        /** @brief Nastavení konce pololetí */
        void setEndDate(QDate date);

    private:
        /**
         * @brief Index nemá žádnou nadřazenou položku
         *
         * Konstanta použitá při specifikaci <tt>internalId</tt> indexu.
         * Označuje index jako kořenový (nemá žádnou nadřazenou položku).
         */
        static const quint32 NO_PARENT = 0xFFFF;

        /** @brief Flags pro rozvrh */
        enum Flags {
            ACTIVE = 0x02           /** @brief Zda je rozvrh aktivní */
        };

        /** @brief Struktura rozvrhu */
        struct Timetable {
            int id;                 /** @brief ID rozvrhu */
            int flags;              /** @brief Flags */
            int followedBy;         /** @brief ID následujícího rozvrhu */
            QString description;    /** @brief Popisek */
            QDate validFrom;        /** @brief Datum začátku platnosti */

            /**
            * @brief Data aktuálního rozvrhu
            *
            * Párové hodnoty den/hodina - ID předmětu. Den/hodina je uložena v tomto
            * tvaru:
            * <tt>0xF0</tt> - dny (<tt>0x00</tt> = pondělí, <tt>0x40</tt> = pátek)
            * <tt>0x0F</tt> - hodiny (<tt>0x00</tt> = nultá hodina, <tt>0x09</tt> = devátá hodina)
            * Tedy <tt>0x23</tt> znamená třetí hodinu ve středu.
            */
            QHash<int, int> data;
        };

        QList<Timetable> timetables;    /** @brief Pole s rozvrhy */
        QDate _beginDate;               /** @brief Datum začátku pololetí */
        QDate _endDate;                 /** @brief Datum konce pololetí */
        ClassesModel* classesModel;     /** @brief Ukazatel na model předmětů */
};

}

#endif
