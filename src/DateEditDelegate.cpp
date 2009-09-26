#include "DateEditDelegate.h"

#include <QDateEdit>
#include <QCalendarWidget>

#include "configure.h"

namespace Absencoid {

/* Konstruktor */
DateEditDelegate::DateEditDelegate(QObject* parent): QItemDelegate(parent) {}

/* Vytvoření editoru */
QWidget* DateEditDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QDateEdit* dateEdit = new QDateEdit(parent);

    /* Kalendářový popup (nelze ho inicializovat centrálně a používat pokaždé) */
    QCalendarWidget* calendar = new QCalendarWidget(parent);
    calendar->setFirstDayOfWeek(Qt::Monday);

    /* Nastavení editoru, kalendářový popup */
    dateEdit->setDisplayFormat("ddd dd.MM.yyyy");
    dateEdit->setCalendarPopup(true);
    dateEdit->setCalendarWidget(calendar);

    return dateEdit;
}

/* Nastavení dat editoru */
void DateEditDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QDateEdit* _editor = static_cast<QDateEdit*>(editor);

    _editor->setDate(index.data(Qt::EditRole).toDate());
}

/* Nastavení dat modelu */
void DateEditDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QDateEdit* _editor = static_cast<QDateEdit*>(editor);

    /* Uložení aktuálního indexu do modelu */
    model->setData(index, _editor->date());
}

/* Aktualizace geometrie editoru */
void DateEditDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    editor->setGeometry(option.rect);
}

}
