#ifndef TEACHERSMODEL_H
#define TEACHERSMODEL_H

#include <QAbstractTableModel>

namespace Absencoid {

/**
 * @brief Datový model pro učitele
 *
 * Obousměrný přístup k datům učitelů.
 */
class TeachersModel: public QAbstractTableModel {
    public:
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
         * @brief Přístup k datům
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

    private:
        /** @brief Struktura pro data učitele */
        struct Teacher {
            int id;         /** @brief ID učitele */
            int flags;      /** @brief Vlastnosti učitele */
            QString name;   /** @brief Jméno učitele */
        };

        /** @brief Data učitelů */
        QList<Absencoid::TeachersModel::Teacher> teachers;
};

}

#endif
