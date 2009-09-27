#ifndef ABSENCOID_CONFIGURATIONMODEL_H
#define ABSENCOID_CONFIGURATIONMODEL_H

#include <QAbstractTableModel>
#include <QDate>

namespace Absencoid {
class TimetableModel;

/**
 * @brief Model konfigurace
 *
 * Jednořádková tabulka s těmito sloupci:
 * <ul>
 *  <li>Začátek pololetí</li>
 *  <li>Konec pololetí</li>
 *  <li>ID aktivního rozvrhu</li>
 *  <li>URL pro provádění aktualizací z internetu</li>
 *  <li>Datum poslední aktualizace</li>
 *  <li>Zda zjišťovat aktualizace po startu</li>
 *  <li>Zda vytvářet zálohu při ukončení programu</li>
 * </ul>
 *
 * Pro Qt::DisplayRole poskytuje datum v čitelném tvaru, název aktivního
 * rozvrhu a boolean hodnoty jako "Ano/Ne". Pro Qt::EditRole poskytuje datum
 * jako QDate, index aktuálního rozvrhu (z TimetableModel) a boolean
 * hodnoty jako boolean.
 */
class ConfigurationModel: public QAbstractTableModel {
    public:
        /**
         * @brief Konstruktor
         *
         * @param _timetableModel   Ukazatel na model rozvrhů
         * @param parent            Rodičovský objekt
         */
        ConfigurationModel(TimetableModel* _timetableModel, QObject* parent = 0);

        /**
         * @brief Počet řádků
         */
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * @brief Počet sloupců
         */
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * @brief Hlavičky
         */
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        /**
         * @brief Čtecí přístup k datům
         */
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    private:
        TimetableModel* timetableModel; /** @brief Ukazatel na model rozvrhů */
        QDate beginDate;        /** @brief Začátek pololetí */
        QDate endDate;          /** @brief Konec pololetí */
        QDate lastUpdate;       /** @brief Poslední aktualizace */
        int activeTimetableId;  /** @brief ID aktivního rozvrhu */
        int flags;              /** @brief Flags (boolean hodnoty) */
        QString webUpdateUrl;   /** @brief URL pro aktualizaci z internetu */
};

}

#endif
