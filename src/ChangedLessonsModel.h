#ifndef ABSENCOID_CHANGEDLESSONSMODEL_H
#define ABSENCOID_CHANGEDLESSONSMODEL_H

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
 *  <li>Číslo hodiny (0-9)</li>
 *  <li>Předmět, ze kterého se mění</li>
 *  <li>Předmět, na který se mění</li>
 *  <li>Počet rozvrhů, které tato změna ovlivní (needitovatelné)</li>
 * </ul>
 * @bug A co změny z "volné hodiny" na nějakou hodinu? Komu se to započítá?
 *      Jedině započítávat jen ty "na které se mění", které jsou v uživatelově
 *      rozvrhu. Ale přesto můžou nastat některé vyjímky.
 */
class ChangedLessonsModel: public QAbstractTableModel {
    public:
        /**
         * @brief Konstruktor
         */
        ChangedLessonsModel(ClassesModel* _classesModel, TimetableModel* _timetableModel, QObject* parent = 0);

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

    private:
        /** @brief Struktura změněné hodiny */
        struct ChangedLesson {
            QDate date;         /** @brief Datum */
            int hour;           /** @brief Číslo hodiny (0-9) */
            int fromClassId;    /** @brief ID předmětu, ze kterého se mění */
            int toClassId;      /** @brief ID předmětu, na který se mění */
        };

        ClassesModel* classesModel;             /** @brief Model předmětů */
        TimetableModel* timetableModel;         /** @brief Model rozvrhů */
        QList<ChangedLesson> changedLessons;    /** @brief List se změnami */
};

}

#endif
