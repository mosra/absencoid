#ifndef ABSENCOID_DATEEDITDELEGATE_H
#define ABSENCOID_DATEEDITDELEGATE_H

#include <QItemDelegate>

class QCalendarWidget;
class QAbstractItemModel;

namespace Absencoid {

/**
 * @brief Delegát poskytující editaci pomocí popup kalendáře
 *
 * Poskytuje kalendář pro editaci
 */
class DateEditDelegate: public QItemDelegate {
    public:
        /**
         * @brief Konstruktor
         *
         * @param   parent          Rodičovský widget
         */
        DateEditDelegate(QObject* parent = 0);

        /**
         * @brief Vytvoření editoru
         */
        virtual QWidget* createEditor(QWidget* parent,
            const QStyleOptionViewItem& option, const QModelIndex& index) const;

        /**
         * @brief Nastavení dat editoru
         */
        virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

        /**
         * @brief Uložení dat editoru zpět do modelu
         */
        virtual void setModelData(QWidget* editor, QAbstractItemModel* model,
            const QModelIndex& index) const;

        /**
         * @brief Aktualizace geometrie editoru
         */
        virtual void updateEditorGeometry(QWidget* editor,
            const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

}

#endif
