#ifndef CLASSESMODEL_H
#define CLASSESMODEL_H

#include <QAbstractTableModel>

namespace Absencoid {

class ClassesModel: public QAbstractTableModel {
    public:
        /**
         * @brief Konstruktor
         */
        ClassesModel(QObject* parent = 0);

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

    private:
        /** @brief Struktura pro předmět */
        struct Class {
            int id;             /** @brief ID předmětu */
            QString name;       /** @brief Jméno učitele */
            QString teacher;    /** @brief Jméno učitele */
        };

        /** @brief Data tříd */
        QList<Absencoid::ClassesModel::Class> classes;
};

}

#endif
