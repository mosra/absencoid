#ifndef ABSENCOID_CLASSESMODEL_H
#define ABSENCOID_CLASSESMODEL_H

#include <QAbstractTableModel>

namespace Absencoid {
class TeachersModel;

/**
 * @brief Model předmětů
 *
 * Model poskytující přístup k předmětům ve formě tabulky. Jsou zde tyto
 * sloupce:
 * <ul>
 *  <li>Souhrn - ve formátu "název předmětu (učitel)"</li>
 *  <li>Název předmětu</li>
 *  <li>Učitel (vzaný z TeachersModel)</li>
 * </ul>
 *
 * První dva řádky modelu jsou speciální. První řádek zastupuje volnou hodinu
 * (jeho <tt>classId</tt> je rovno nule, tedy jako v ještě nevybraném
 * comboboxu), druhý řádek zastupuje jakoukoli hodinu (použitelné ve změněných
 * hodinách, pokud změna platí plošně pro všechny hodiny / rozvrhy). Jeho
 * <tt>classId</tt> je rovno konstantě Absencoid::ClassesModel::WHATEVER.
 */
class ClassesModel: public QAbstractTableModel {
    public:

        /**
        * @brief Konstanta značící jakoukoli hodinu
        *
        * Použitelné ve změnách, pokud se mění z jakékoli hodiny. Index
        * odpovídající této hodině = 1
        */
        static const int WHATEVER;

        /**
         * @brief Konstruktor
         *
         * @param   _teachersModel  Ukazatel na model učitelů (pro zjištění dat)
         * @param   parent          Rodičovský widget
         */
        ClassesModel(TeachersModel* _teachersModel, QObject* parent = 0);

        /**
         * @brief Počet sloupců
         *
         * Dva sloupce - název předmětu a učitel
         */
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * @brief Počet řádků
         */
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * @brief Čtecí přístup k datům
         */
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

        /**
         * @brief Data hlaviček
         */
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        /**
         * @brief Flags
         *
         * Zda je položka editovatelná
         */
        virtual Qt::ItemFlags flags(const QModelIndex& index) const;

        /**
         * @brief Zápisový přístup k datům
         */
        virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

        /**
         * @brief Přidání dalšího předmětu
         *
         * Předmět se přidá do lokálních dat, až po napsání jména a vybrání
         * učitele se uloží do DB. Dokud není předmět uložen v DB, je v hlavičce
         * řádku hvězdička místo ID.
         */
        virtual bool insertRow(int row, const QModelIndex& parent = QModelIndex());

        /**
         * @brief Odebrání předmětů
         */
        virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

        /**
         * @brief Získání indexu z ID předmětu
         */
        int indexFromId(int id);

        /**
         * @brief Získání ID předmětu z indexu
         */
        int idFromIndex(int index);

    private:
        /** @brief Struktura pro předmět */
        struct Class {
            int id;             /** @brief ID předmětu */
            int teacherId;      /** @brief ID učitele */
            QString name;       /** @brief Jméno učitele */
        };

        /** @brief Data tříd */
        QList<Absencoid::ClassesModel::Class> classes;

        /** @brief Model učitelů */
        Absencoid::TeachersModel* teachersModel;

        /**
         * @brief Uložení nového předmětu do databáze
         *
         * Uloží nový předmět (označený hvězdičkou) do databáze a aktualizuje
         * hlavičku na ID přidaného řádku. Voláno z
         * Absencoid::ClassesModel::insertRow, když jsou všechna data zapsána.
         * @param   row     Číslo řádku, který se má uložit (číslo v classes, ne
         *                  v tabulce)
         * @return  Zda se povedlo uložit
         */
        bool saveRow(int index);
};

}

#endif
