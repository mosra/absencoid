#ifndef ABSENCOID_CHANGESMODEL_H
#define ABSENCOID_CHANGESMODEL_H

#include <QAbstractTableModel>
#include <QDate>

namespace Absencoid {

class ClassesModel;
class TimetableModel;

/**
 * @brief Model změněných hodin
 *
 * Tabulka zobrazující změněné či odpadnuté hodiny. Každý řádek obsahuje tato
 * data:
 * <ul>
 *  <li>Datum</li>
 *  <li>Číslo hodiny (viz ChangesModel::Change::hour)</li>
 *  <li>Předmět, ze kterého se mění (viz ChangesModel::Change::fromClassId)</li>
 *  <li>Předmět, na který se mění</li>
 *  <li>Počet rozvrhů, které tato změna ovlivní (needitovatelné)</li>
 * </ul>
 */
class ChangesModel: public QAbstractTableModel {
    public:
        /** @brief Flags */
        enum Flags {
            ALL_HOURS = -1      /** @brief Změna platí pro všechny hodiny */
        };

        /**
         * @brief Konstruktor
         */
        ChangesModel(ClassesModel* _classesModel, TimetableModel* _timetableModel, QObject* parent = 0);

        /**
         * @brief Počet sloupců
         */
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * @brief Počet řádků
         */
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * @brief Hlavičky
         */
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        /**
         * @brief Čtecí přístup k datům
         */
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

        /**
         * @brief Flags
         */
        virtual Qt::ItemFlags flags(const QModelIndex& index) const;

        /**
         * @brief Zápisový přístup k datům
         * @todo Ověření, zda se někde nekryjí datum, hodina a fromClass
         * @todo Ověření, zda není z stejné jako do
         */
        virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

        /**
         * @brief Přidání řádku
         *
         * Řádek se přidá do lokálních dat, do DB je zapsaný až při vyplnění
         * povinných polí (datum, hodina a jeden z předmětů)
         */
        virtual bool insertRow(int row, const QModelIndex& parent = QModelIndex());

        /**
         * @brief Odstranění řádků
         */
        virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

        /**
         * @brief Změny, které souvisí s daným datem a rozvrhem, který tou dobou platil
         *
         * Vrací indexy změn, které souvisí s rozvrhem platným v daný datum:
         * Pokud hodina, ze které se mění, je v něm na stejné pozici, nebo
         * rozvrh kdekoli obsahuje hodinu, na kterou se mění (pokud se mění z
         * prázdné hodiny).
         * @param   date        Datum
         * @return  Seznam indexů změn
         */
        QList<int> relatedChanges(QDate date) const;

    private:
        /** @brief Struktura změněné hodiny */
        struct Change {
            QDate date;         /** @brief Datum */
            int id;             /** @brief ID změny */

            /**
             * @brief Hodina, ze které se mění
             * 0 - 9 značí nultou až devátou hodinu, hodnota -1 znamená
             * všechny hodiny v daném dni.
             */
            int hour;

            /**
             * @brief ID předmětu, ze kterého se mění
             * Pokud je uvedena nula, znamená to prázdnou hodinu, hodnota
             * ClassesModel::WHATEVER značí jakoukoli hodinu.
             */
            int fromClassId;

            int toClassId;      /** @brief ID předmětu, na který se mění */
        };

        ClassesModel* classesModel;     /** @brief Model předmětů */
        TimetableModel* timetableModel; /** @brief Model rozvrhů */
        /** @todo Předělat změny na multihash, abychom měli rychlejší hledání podle data! */
        QList<Change> changes;          /** @brief List se změnami */

        /**
         * @brief Ověření unikátnosti záznamů
         *
         * Ověří, zda je předaný záznam unikátní (ještě takový neexistuje v
         * databázi). Tj. ověřuje jen jestli takový záznam existuje mezi
         * uloženými záznamy (s nenulovým ID).
         * @param   date        Datum
         * @param   hour        Číslo hodiny
         * @param   fromClassId Předmět, ze kterého se mění
         * @return  Vrací false, pokud již takovýto záznam existuje či pokud je
         *          nějaký parametr neplatný (např. číslo hodiny mimo rozsah).
         */
        bool checkUnique(QDate date, int hour, int fromClassId);

        /**
         * @brief Uložení řádku do DB
         *
         * Nově přidané řádky se do DB ukládají až při vyplnění unikátních dat.
         * Ověření unikátnosti záznamu je na volajícím, tato funkce to nedělá.
         * @param   row         Číslo řádku, který se má uložit
         */
        bool saveRow(int row);
};

}

#endif
