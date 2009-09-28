#ifndef ABSENCOID_ABSENCESMODEL_H
#define ABSENCOID_ABSENCESMODEL_H

#include <QAbstractTableModel>
#include <QDate>

namespace Absencoid {

class ClassesModel;
class TimetableModel;
class ChangesModel;

/**
 * @brief Model absencí
 *
 * Poskytuje tabulku absencí s 12 sloupci. Pro jednodušší vybírání zobrazuje
 * předměty, které byly daný den v rozvrhu (včetně aplikovaných změn).
 * <ul>
 *  <li>Datum</li>
 *  <li>Zda je to školní akce (zaškrtávací políčko)</li>
 *  <li>Absence 0. hodinu (zaškrtávací políčko)</li>
 *  <li>Absence 1. hodinu (zaškrtávací políčko)</li>
 *  <li>...</li>
 * </ul>
 * @todo Zredukovat šířku ... jak?
 */
class AbsencesModel: public QAbstractTableModel {
    public:
        /**
         * @brief Konstruktor
         */
        AbsencesModel(ClassesModel* _classesModel, TimetableModel* _timetableModel, ChangesModel* _changesModel, QObject* parent = 0);

        /**
         * @brief Počet sloupců
         */
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * @brief Počet řádků
         */
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * @brief Data hlaviček
         */
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        /**
         * @brief Čtecí přístup k datům
         */
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    private:
        /**
         * @brief Hodnota značící, že se jednalo o školní akci
         *
         * Spojuje se binárním ORem s AbsencesModel::Absence::hours.
         */
        static const int SCHOOL_ACTION;

        /** @brief Struktura pro absenci */
        struct Absence {
            QDate date;                 /** @brief Datum absence */
            int id;                     /** @brief ID absence */

            /**
             * @brief Zameškané hodiny
             *
             * Tzv. binární checkbox, např. 0b0001110 znamená, že jsou zameškané
             * první tři hodiny. Může obsahovat také AbsencesModel::SCHOOL_ACTION.
             */
            int hours;

            /**
             * @brief Změněné hodiny
             *
             * Binární checkbox, označuje hodiny, na kterých se projevila nějaká
             * změna. Tyto hodiny jsou potom odlišně zobrazeny.
             */
            int changes;

            QList<int> classIndexes;        /** @brief ID předmětů v jednotlivých hodinách */
        };

        ClassesModel* classesModel;     /** @brief Model předmětů */
        TimetableModel* timetableModel; /** @brief Model rozvrhů */
        ChangesModel* changesModel;     /** @brief Model změn */
        QList<Absence> absences;        /** @brief List s absencemi */

        /**
         * @brief Načtení ID tříd pro danou absenci
         *
         * Naplní list s předměty pro daný index.
         * @param       index       Index, který se má naplnit
         */
        void loadClassIds(int index);
};

}

#endif