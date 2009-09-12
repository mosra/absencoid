#ifndef CLASSESMODEL_H
#define CLASSESMODEL_H

#include <QAbstractTableModel>

namespace Absencoid {
class TeachersModel;

class ClassesModel: public QAbstractTableModel {
    Q_OBJECT

    public:
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

    private slots:
        /**
         * @brief Zjištění změn v modelu učitelů
         *
         * Zjistí, zda se změny v modelu očitelů projeví na tomto modelu, pokud
         * ano, vyšle signál dataChanged.
         */
        void checkTeacherChanges(const QModelIndex& topLeft, const QModelIndex& bottomRight);
};

}

#endif
