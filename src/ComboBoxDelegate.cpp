#include "ComboBoxDelegate.h"

#include <QLineEdit>
#include <QComboBox>

#include "TeachersModel.h"

namespace Absencoid {

/* Konstruktor */
ComboBoxDelegate::ComboBoxDelegate(QAbstractItemModel* _model, QObject* parent): QItemDelegate(parent),
    model(_model) {}

/* Vytvoření editoru */
QWidget* ComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QComboBox* comboBox = new QComboBox(parent);
    comboBox->setModel(model);
    return comboBox;
}

/* Nastavení dat editoru */
void ComboBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QComboBox* _editor = static_cast<QComboBox*>(editor);

    /* data() v edit roli musí poskytovat index (číslo řádku) */
    _editor->setCurrentIndex(index.data(Qt::EditRole).toInt());
}

/* Nastavení dat modelu */
void ComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QComboBox* _editor = static_cast<QComboBox*>(editor);

    /* Uložení aktuálního indexu do modelu */
    model->setData(index, _editor->currentIndex());
}

/* Aktualizace geometrie editoru */
void ComboBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    editor->setGeometry(option.rect);
}

}
