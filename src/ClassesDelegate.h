#ifndef TEACHERSDELEGATE_H
#define TEACHERSDELEGATE_H

#include <QItemDelegate>

namespace Absencoid {

class TeachersModel;

/**
 * @brief Delegát poskytující editaci předmětů
 *
 * Poskytuje rozbalovací seznam učitelů
 */
class ClassesDelegate: public QItemDelegate {
    public:
        /**
         * @brief Konstruktor
         *
         * @param   _teachersModel  Model učitelů (pro získání seznamu)
         * @param   parent          Rodičovský widget
         */
        ClassesDelegate(TeachersModel* _teachersModel, QObject* parent = 0);

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
        TeachersModel* teachersModel;
};

}

#endif