#include "AbsencesTab.h"

#include <QTableView>
#include <QPushButton>
#include <QBoxLayout>
#include <QMessageBox>

#include "ClassesModel.h"
#include "TimetableModel.h"
#include "ChangesModel.h"
#include "AbsencesModel.h"
#include "DateEditDelegate.h"
#include "Style.h"

namespace Absencoid {

/* Konstruktor */
AbsencesTab::AbsencesTab(ClassesModel* classesModel, TimetableModel* timetableModel, ChangesModel* changesModel, QWidget* parent): QWidget(parent) {
    /* Model */
    absencesModel = new AbsencesModel(classesModel, timetableModel, changesModel);

    /* Tabulka s absencemi */
    absencesView = new QTableView;
    absencesView->setModel(absencesModel);
    absencesView->setColumnWidth(0, 120);
    absencesView->setItemDelegateForColumn(0, new DateEditDelegate(absencesView));
    absencesView->setColumnWidth(1, 66);
    absencesView->setColumnWidth(2, 66);
    absencesView->setColumnWidth(3, 66);
    absencesView->setColumnWidth(4, 66);
    absencesView->setColumnWidth(5, 66);
    absencesView->setColumnWidth(6, 66);
    absencesView->setColumnWidth(7, 66);
    absencesView->setColumnWidth(8, 66);
    absencesView->setColumnWidth(9, 66);
    absencesView->setColumnWidth(10, 66);
    absencesView->setColumnWidth(11, 66);

    /* Tlačítka nahoře */
    QPushButton* addAbsenceButton = new QPushButton(Style::style()->icon(Style::PlusIcon), tr("Přidat absenci"));
    removeSelectedButton = new QPushButton(Style::style()->icon(Style::MinusIcon), tr("Odstranit vybrané"));
    removeSelectedButton->setDisabled(true);

    /* Horní layout */
    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(addAbsenceButton, 1, Qt::AlignRight);
    buttonsLayout->addWidget(removeSelectedButton);

    /* Celkový layout */
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(absencesView);
    layout->addLayout(buttonsLayout);

    /* Propojení tlačítek s jejich funkcemi */
    connect(addAbsenceButton, SIGNAL(clicked(bool)), this, SLOT(addAbsence()));
    connect(removeSelectedButton, SIGNAL(clicked(bool)), this, SLOT(removeSelected()));

    /* Při změně výběru zkontrolovat mazací tlačítko */
    connect(absencesView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(updateRemoveButton()));
}

/* Přidání absence */
void AbsencesTab::addAbsence() {
    absencesModel->insertRow(absencesModel->rowCount());
    absencesView->scrollToBottom();
}

/* Odstranění absencí */
void AbsencesTab::removeSelected() {

    /* Projití výběru, aby zde byly unikátní řádky */
    QList<int> rows;
    foreach(QModelIndex index, absencesView->selectionModel()->selectedIndexes()) {
        if(!rows.contains(index.row())) rows.append(index.row());
    }

    /* Ověření */
    if(QMessageBox::warning(this, tr("Smazat absence"),
        tr("Opravdu smazat vybrané absence <strong>(%1)</strong>?").arg(QString::number(rows.count())),
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;

    /* Seřazení čísel řádků od největšího k nejmenšímu (vysvětleno v
        TeachersTab::removeTeachers() ) */
    qSort(rows.begin(), rows.end(), qGreater<int>());

    /* Smazání jednotlivých unikátních řádků */
    foreach(int row, rows) {
        absencesModel->removeRow(row);
    }
}

/* Zašednutí mazacího tlačítka, pokud není nic vybráno */
void AbsencesTab::updateRemoveButton() {
    if(absencesView->selectionModel()->hasSelection())
        removeSelectedButton->setDisabled(false);
    else
        removeSelectedButton->setDisabled(true);
}

}
