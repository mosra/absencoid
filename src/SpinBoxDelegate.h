#ifndef ABSENCOID_SPINBOXDELEGATE_H
#define ABSENCOID_SPINBOXDELEGATE_H

#include <QItemDelegate>

class QAbstractItemModel;

namespace Absencoid {

/**
 * @brief Delegát poskytující editaci pomocí spinboxu
 *
 * Poskytuje spinbox v daném rozsahu pro editaci
 */
class SpinBoxDelegate: public QItemDelegate {
    public:
        /**
         * @brief Konstruktor
         *
         * @param   _min            Minimální hodnota
         * @param   _max            Maximální hodnota
         * @param   parent          Rodičovský widget
         */
        SpinBoxDelegate(int _min, int _max, QObject* parent = 0);

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
        int min;        /** @brief Minimální hodnota */
        int max;        /** @brief Maximální hodnota */
};

}

#endif
