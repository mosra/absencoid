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
        static const int FIXED;    /** @brief Označení zamknuté položky */

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
        * @brief Zda lze načíst další podpoložky daného indexu
        *
        * @return Vrací true, pokud je <tt>parent</tt> položka v seznamu
        * rozvrhů a data příslušného rozvrhu nebyla ještě načtena.
        */
        virtual bool canFetchMore(const QModelIndex& parent) const;

        /**
        * @brief Načtení dalších podpoložek daného indexu
        *
        * Tímto způsobem lze načíst data rozvrhu, který je potomkem indexu
        * <tt>parent</tt> a to všechna najednou.
        */
        virtual void fetchMore(const QModelIndex& parent);

        /**
         * @brief Zjištění ID rozvrhu z indexu
         *
         * @param   index       Index rozvrhu (číslo řádku)
         * @return  ID rozvrhu
         * @note Komplexnost funkce: konstatní
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
         * @param   day         Číslo dne (0-4, 0 = pondělí)
         * @param   hour        Hodina (0-9, 0 = nultá hodina, -1 = všechny hodiny)
         * @return  Den/hodina
         */
        inline int dayHour(int day, int hour) const  {
            if(hour == -1) hour = 0x0F; return day << 4 | hour;
        }

        /**
         * @brief Zjištění, kolik rozvrhů obsahuje tento předmět v daný den/hodinu
         *
         * @todo Sloučit s hasLesson
         * @todo Ověřovat datum a zda rozvrh v dané datum platí
         * @param   dayHour     Den/hodina
         * @param   classId     Id předmětu
         * @return  Počet rozvrhů, který odpovídá dotazu.
         */
        int timetablesWithThisClass(int dayHour, int classId);

        /**
         * @brief Zjištění indexu aktuálního (tj. vybraného a platného) rozvrhu
         */
        int timetableForDate(QDate date);

        /**
         * @brief Zjištění, zda rozvrh obsahuje danou hodinu
         *
         * Vrací true, pokud rozvrh (platný v dané datum) obsahuje v danou hodinu
         * předaný předmět. Pokud hodina není uvedena, vrací true, pokud rozvrh
         * kdekoliv obsahuje předaný předmět.
         */
        bool hasLesson(QDate date, int classId, int hour = -1);

    public slots:
        /**
         * @brief Slot pro nastavení aktualáního rozvrhu
         */
        void setActualTimetable(int index);

    private slots:
        /**
         * @brief Zjištění změn v modelu předmětů
         *
         * Zjistí, zda se změny v modelu předmětů projeví zde, pokud ani, vyšle
         * signál dataChanged()
         */
        void checkClassChanges(const QModelIndex& topLeft, const QModelIndex& bottomRight);

    private:
        /**
         * @brief Index nemá žádnou nadřazenou položku
         *
         * Konstanta použitá při specifikaci <tt>internalId</tt> indexu.
         * Označuje index jako kořenový (nemá žádnou nadřazenou položku).
         */
        static const quint32 NO_PARENT;

        /** @brief Flags pro rozvrh */
        enum Flags {
            LOADED = 0x01,
            ACTIVE = 0x02
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
        ClassesModel* classesModel;     /** @brief Ukazatel na model předmětů */
};

}

#endif
