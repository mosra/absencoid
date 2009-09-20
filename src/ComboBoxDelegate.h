#ifndef ABSENCOID_COMBOBOXDELEGATE_H
#define ABSENCOID_COMBOBOXDELEGATE_H

#include <QItemDelegate>

class QAbstractItemModel;

namespace Absencoid {

/**
 * @brief Delegát poskytující editaci pomocí rozbalovacího seznamu
 *
 * Poskytuje rozbalovací seznam pro editaci
 */
class ComboBoxDelegate: public QItemDelegate {
    public:
        /**
         * @brief Konstruktor
         *
         * @param   _model          Ukazatel na model pro získání seznamu
         * @param   parent          Rodičovský widget
         */
        ComboBoxDelegate(QAbstractItemModel* _model, QObject* parent = 0);

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

    private:
        QAbstractItemModel* model;
};

}

#endif
