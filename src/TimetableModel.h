#ifndef TIMETABLEMODEL_H
#define TIMETABLEMODEL_H
#include <QAbstractTableModel>

namespace Absencoid {

class ClassesModel;

class TimetableModel: public QAbstractTableModel {
    Q_OBJECT

    public:
        /**
         * @brief Konstruktor
         *
         * @param   classesModel    Ukazatel na model předmětů
         * @param   parent          Rodičovský widget
         */
        TimetableModel(ClassesModel* classesModel, QObject* parent = 0);

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

    public slots:
        /**
         * @brief Přehození směru rozvrhu
         */
        void switchDirection();

    private:
        bool horizontalLessons; /** @brief Zda jsou hodiny zobrazeny v horizontálním směru */

};

}

#endif
