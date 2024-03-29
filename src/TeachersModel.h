#ifndef ABSENCOID_TEACHERSMODEL_H
#define ABSENCOID_TEACHERSMODEL_H

#include <QAbstractTableModel>

namespace Absencoid {

/**
 * @brief Datový model pro učitele
 *
 * Poskytuje třísloupcovou tabulku:
 * <ul>
 *  <li>Jméno</li>
 *  <li>Zda zapisuje absence</li>
 *  <li>Zda uznává školní akce</li>
 * </ul>
 * @todo Jiné chování učitele u různých předmětů
 */
class TeachersModel: public QAbstractTableModel {
    Q_OBJECT

    public:
        /** @brief Sloupce */
        enum Columns {
            NAME = 0,                   /**< @brief Jméno učitele */
            COUNTS = 1,                 /** @brief Zda učitel počítá absence */
            ACCEPTS = 2                 /** @brief Zda učitel uznává školní akce */
        };

        /**
         * @brief Konstruktor
         *
         * Načte data z tabulky <tt>teachers</tt> databáze.
         * @param   parent     Rodičovský widget
         */
        TeachersModel(QObject* parent = 0);

        /**
         * @brief Počet sloupců
         *
         * Model má tři sloupce (jméno, zda zapisuje, zda uznává školní akce).
         */
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * @brief Počet řádků
         */
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * @brief Čtecí přístup k datům
         *
         * Vlastnosti učitelů jsou zobrazovány s ikonkami (Ano/Ne)
         */
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

        /**
         * @brief Přístup k datům hlaviček
         *
         * Vertikální hlavička zobrazuje ID učitelů, horizontální klasické popisky
         */
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        /**
         * @brief Flags
         *
         * Například pro zjištění, zda je položka daného indexu editovatelná.
         */
        virtual Qt::ItemFlags flags(const QModelIndex& index) const;

        /**
         * @brief Zápisový přístup k datům
         */
        virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

        /**
         * @brief Přidání nového učitele
         */
        virtual bool insertRow(int row, const QModelIndex& parent = QModelIndex());

        /**
         * @brief Odebrání učitelů
         */
        virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

        /**
         * @brief Získání indexu odpovídajícího předanému ID učitele
         *
         * Index ukazuje na první položku odpovídajícího řádku (tj. jméno)
         * @todo Vracet jen int
         */
        QModelIndex indexFromId(int id) const;

        /**
         * @brief Získání ID učitele z předaného indexu či čísla řádku
         */
        int idFromIndex(int row) const;

    public slots:
        /**
         * @brief (Znovu)načtení dat z databáze
         *
         * Pokud nejde o první načtení, vyšle také resetovací signál.
         */
        void reload();

    private:
        /** @brief Struktura pro data učitele */
        struct Teacher {
            int id;         /** @brief ID učitele */
            int flags;      /** @brief Vlastnosti učitele */
            QString name;   /** @brief Jméno učitele */
        };

        /** @brief Data učitelů */
        QList<Absencoid::TeachersModel::Teacher> teachers;

        /**
         * @brief Uložení nového učitele do databáze
         *
         * Uloží nového učitele (označeného hvězdičkou) do databáze, aktualizuje
         * hlavičku na ID přidaného řádku. Voláno z
         * Absencoid::TeachersModel::insertRow, když jsou všechna data zapsána.
         * @param   row     Řádek, který se má uložit
         * @return  Zda se povedlo uložit
         */
        bool saveRow(int row);
};

}

#endif
