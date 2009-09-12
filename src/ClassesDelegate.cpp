#include "ClassesDelegate.h"

#include <QLineEdit>
#include <QComboBox>

#include "TeachersModel.h"

namespace Absencoid {

/* Konstruktor */
ClassesDelegate::ClassesDelegate(TeachersModel* _teachersModel, QObject* parent): QItemDelegate(parent),
    teachersModel(_teachersModel) {}

/* Vytvoření editoru */
QWidget* ClassesDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    /* 2. sloupec - výběr z učitelů */
    if(index.column() == 1) {
        QComboBox* comboBox = new QComboBox(parent);
        comboBox->setModel(teachersModel);
        return comboBox;
    }

    /* 1. sloupec - default */
    return QItemDelegate::createEditor(parent, option, index);
}

/* Nastavení dat editoru */
void ClassesDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    /* 2. sloupec - jméno */
    if(index.column() == 1) {
        QComboBox* _editor = static_cast<QComboBox*>(editor);

        /* data() v edit roli poskytují index učitele (číslo řádku) */
        _editor->setCurrentIndex(index.data(Qt::EditRole).toInt());
    }

    /* Ostatní sloupce - default */
    else QItemDelegate::setEditorData(editor, index);
}

/* Nastavení dat modelu */
void ClassesDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    /* 2. sloupec - jméno */
    if(index.column() == 1) {
        QComboBox* _editor = static_cast<QComboBox*>(editor);

        /* Uložení aktuálního indexu do modelu */
        model->setData(index, _editor->currentIndex());
    }

    else QItemDelegate::setModelData(editor, model, index);
}

/* Aktualizace geometrie editoru */
void ClassesDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    editor->setGeometry(option.rect);
}

}