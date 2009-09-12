#ifndef TIMETABLELISTMODEL_H
#define TIMETABLELISTMODEL_H

#include <QAbstractTableModel>
#include <QDate>

namespace Absencoid {

/**
 * @brief Model pro seznam rozvrhů
 */
class TimetableListModel: public QAbstractTableModel {
    public:
        /**
         * @brief Konstruktor
         */
        TimetableListModel(QObject* parent = 0);

        /**
         * @brief Počet sloupců
         */
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * @brief Počet řádků
         */
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * @brief Data
         */
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

        /**
         * @brief Zjištění indexu z ID rozvrhu
         */
        int indexFromId(int id) const;

    private:
        /** @brief Struktura pro rozvrh */
        struct Timetable {
            int id;                 /** @brief ID rozvrhu */
            int followedBy;         /** @brief ID rozvrhu, který tento následuje */
            QString description;    /** @brief Popisek rozvrhu */
            QDate validFrom;        /** @brief Platný od */
            QDate validTo;          /** @brief Platný do */
        };

        /** @brief List s rozvrhy */
        QList<Absencoid::TimetableListModel::Timetable> timetableList;
};

}

#endif
