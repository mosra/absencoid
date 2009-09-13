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
         * @param   row     Řádek, který se má uložit
         * @return  Zda se povedlo uložit
         */
        bool saveRow(int index);

    private slots:
        /**
         * @brief Zjištění změn v modelu učitelů
         *
         * Zjistí, zda se změny v modelu učitelů projeví na tomto modelu, pokud
         * ano, vyšle signál dataChanged.
         */
        void checkTeacherChanges(const QModelIndex& topLeft, const QModelIndex& bottomRight);
};

}

#endif
