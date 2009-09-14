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
         * @param   _classesModel   Ukazatel na model předmětů
         * @param   parent          Rodičovský widget
         */
        TimetableModel(ClassesModel* _classesModel, QObject* parent = 0);

        /**
         * @brief Načtení rozvrhu
         *
         * Načte rozvrh s určeným ID
         * @param   id              ID rozvrhu, který chceme načíst
         */
        void load(int id);

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

        /**
         * @brief Flags
         */
        virtual Qt::ItemFlags flags(const QModelIndex& index) const;

        /**
         * @brief Zápisový přístup k datům
         */
        virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    public slots:
        /**
         * @brief Přehození směru rozvrhu
         */
        void switchDirection();

    private slots:
        /**
         * @brief Zjištění změn v modelu předmětů
         *
         * Zjistí, zda se změny v modelu předmětů projeví zde, pokud ani, vyšle
         * signál dataChanged()
         */
        void checkClassChanges(const QModelIndex& topLeft, const QModelIndex& bottomRight);

    private:
        ClassesModel* classesModel; /** @brief Ukazatel na model předmětů */
        bool horizontalLessons;     /** @brief Zda jsou hodiny zobrazeny v horizontálním směru */
        int timetableId;            /** @brief Číslo právě používaného rozvrhu hodin */

        /**
         * @brief Data aktuálního rozvrhu
         *
         * Párové hodnoty den/hodina - ID předmětu. Den/hodina je uložena v tomto
         * tvaru:
         * <tt>0xF0</tt> - dny (<tt>0x00</tt> = pondělí, <tt>0x40</tt> = pátek)
         * <tt>0x0F</tt> - hodiny (<tt>0x00</tt> = nultá hodina, <tt>0x09</tt> = devátá hodina)
         * Tedy <tt>0x23</tt> znamená třetí hodinu ve středu.
         */
        QHash<int, int> timetableData;

};

}

#endif
