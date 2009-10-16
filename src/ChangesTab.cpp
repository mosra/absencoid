#include "ChangesTab.h"

#include <QTableView>
#include <QMessageBox>
#include <QPushButton>
#include <QBoxLayout>

#include "configure.h"
#include "TimetableModel.h"
#include "ClassesModel.h"
#include "ChangesModel.h"
#include "ComboBoxDelegate.h"
#include "DateEditDelegate.h"
#include "SpinBoxDelegate.h"
#include "Style.h"

namespace Absencoid {

/* Konstruktor */
ChangesTab::ChangesTab(TimetableModel* timetableModel, ClassesModel* classesModel, QWidget* parent): QWidget(parent) {
    changesModel = new ChangesModel(classesModel, timetableModel, this);

    /* Tabulka se změnami */
    changesView = new QTableView;
    changesView->setModel(changesModel);
    changesView->setColumnWidth(0, 120);
    changesView->setColumnWidth(1, 80);

    /* Delegát pro datum */
    DateEditDelegate* dateEditDelegate = new DateEditDelegate(changesView);
    changesView->setItemDelegateForColumn(0, dateEditDelegate);

    /* Delegát pro číslo hodiny */
    SpinBoxDelegate* spinBoxDelegate = new SpinBoxDelegate(-1, 9, changesView);
    changesView->setItemDelegateForColumn(1, spinBoxDelegate);

    /* Combobox delegáty pro výběr předmětů */
    ComboBoxDelegate* comboBoxDelegate = new ComboBoxDelegate(classesModel, changesView);
    changesView->setItemDelegateForColumn(2, comboBoxDelegate);
    changesView->setItemDelegateForColumn(3, comboBoxDelegate);

    /* Tlačítka vpravo */
    QPushButton* addChangeButton = new QPushButton(Style::style()->icon(Style::PlusIcon), tr("Přidat změnu"));
    removeChangesButton = new QPushButton(Style::style()->icon(Style::MinusIcon), tr("Odstranit vybrané"));
    removeChangesButton->setDisabled(true);

    /* Layout pro tlačítka vpravo */
    QVBoxLayout* buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(addChangeButton, 0, Qt::AlignTop);
    buttonsLayout->addWidget(removeChangesButton, 1, Qt::AlignTop);

    /* Layout celkový */
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(changesView, 1);
    layout->addLayout(buttonsLayout, 0);

    /* Propojení tlačítek s jejich funkcemi */
    connect(addChangeButton, SIGNAL(clicked(bool)), this, SLOT(addChange()));
    connect(removeChangesButton, SIGNAL(clicked(bool)), this, SLOT(removeChanges()));

    /* Při zrušení výběru se deaktivuje tlačítko pro smazání změn */
    connect(changesView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(updateRemoveButton()));

    #ifndef ADMIN_VERSION
    addChangeButton->setDisabled(true);
    #endif

    setLayout(layout);
}

/* Přidání změny */
void ChangesTab::addChange() {
    changesModel->insertRow(changesModel->rowCount());
    changesView->scrollToBottom();
}

/* Odstranění vybraných změn */
void ChangesTab::removeChanges() {
    /* Projití výběru, aby zde byly unikátní řádky */
    QList<int> rows;
    foreach(QModelIndex index, changesView->selectionModel()->selectedIndexes()) {
        if(!rows.contains(index.row())) rows.append(index.row());
    }

    /* Ověření */
    if(QMessageBox::warning(this, tr("Smazat změněné předměty"),
        tr("Opravdu smazat vybrané změny") + " <strong>(" + QString::number(rows.count()) + ")</strong>?",
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;

    /* Seřazení čísel řádků od největšího k nejmenšímu (vysvětleno v
        TeachersTab::removeTeachers() ) */
    qSort(rows.begin(), rows.end(), qGreater<int>());

    /* Smazání jednotlivých unikátních řádků */
    foreach(int row, rows) {
        changesModel->removeRow(row);
    }
}

/* Zašednutí / aktivace tlačítka pro mazání */
void ChangesTab::updateRemoveButton() {
    #ifdef ADMIN_VERSION
    if(changesView->selectionModel()->hasSelection())
        removeChangesButton->setDisabled(false);
    else
        removeChangesButton->setDisabled(true);
    #endif
}

}
