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
    Q_OBJECT

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

    private:
        /** @brief Struktura pro data učitele */
        struct Teacher {
            int id;         /** @brief ID učitele */
            int flags;      /** @brief Vlastnosti učitele */
            QString name;   /** @brief Jméno učitele */
        };

        /** @brief Data učitelů */
        QList<Absencoid::TeachersModel::Teacher> teachers;

    public slots:
        void addTeacher();
};

}

#endif
