#ifndef ABSENCOID_HOTTESTMODEL_H
#define ABSENCOID_HOTTESTMODEL_H

#include <QAbstractTableModel>

namespace Absencoid {

class ChangesModel;
class TimetableModel;
class ClassesModel;
class TeachersModel;
class AbsencesModel;

/**
 * @brief Nejžhavější absence
 */
class HottestModel: public QAbstractTableModel {
    Q_OBJECT

    public:
        /**
         * @brief Konstruktor
         */
        HottestModel(TeachersModel* _teachersModel, ClassesModel* _classesModel, TimetableModel* _timetableModel, ChangesModel* _changesModel, AbsencesModel* _absencesModel, QObject* parent = 0);

        /** @brief Počet sloupců */
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

        /** @brief Počet řádků */
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

        /** @brief Hlavičky */
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        /** @brief Čtecí přístup k datům */
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    public slots:
        /** @brief (Znovu)naplnění modelu daty */
        void reload();

    private:
        /** @brief Souhrn absencí v daném předmětu */
        struct Class {
            int id;             /**< @brief ID předmětu */
            int hours;          /**< @brief Počet hodin dosud */
            int absences;       /**< @brief Počet absencí dosud */
            int schoolEvents;    /**< @brief Počet školních absencí */
            int hoursForecast;  /**< @brief Předpověď počtu hodin */

            /** @brief Porovnávací operátor pro řazení */
            bool operator<(const Class& c) const;
        };

        QList<Class> classes;   /**< @brief Seznam souhrnů */

        TeachersModel* teachersModel;   /**< @brief Model učitelů */
        ClassesModel* classesModel;     /**< @brief Model předmětů */
        TimetableModel* timetableModel; /**< @brief Model rozvrhů */
        ChangesModel* changesModel;     /**< @brief Model změn */
        AbsencesModel* absencesModel;   /**< @brief Model absencí */
};

}

#endif
