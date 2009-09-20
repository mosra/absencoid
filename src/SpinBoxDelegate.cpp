#include "SpinBoxDelegate.h"

#include <QSpinBox>

namespace Absencoid {

/* Konstruktor */
SpinBoxDelegate::SpinBoxDelegate(int _min, int _max, QObject* parent):
QItemDelegate(parent), min(_min), max(_max) {}

/* Vytvoření editoru */
QWidget* SpinBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QSpinBox* spinBox = new QSpinBox(parent);
    spinBox->setRange(min, max);
    return spinBox;
}

/* Nastavení dat editoru */
void SpinBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QSpinBox* _editor = static_cast<QSpinBox*>(editor);

    _editor->setValue(index.data(Qt::EditRole).toInt());
}

/* Nastavení dat modelu */
void SpinBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QSpinBox* _editor = static_cast<QSpinBox*>(editor);

    /* Uložení aktuální hodnoty do modelu */
    model->setData(index, _editor->value());
}

/* Aktualizace geometrie editoru */
void SpinBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    editor->setGeometry(option.rect);
}

}
