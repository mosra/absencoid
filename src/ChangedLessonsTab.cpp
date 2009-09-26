#include "ChangedLessonsTab.h"

#include <QTableView>
#include <QPushButton>
#include <QBoxLayout>

#include "TimetableModel.h"
#include "ClassesModel.h"
#include "ChangedLessonsModel.h"
#include "ComboBoxDelegate.h"
#include "DateEditDelegate.h"
#include "SpinBoxDelegate.h"
#include <QMessageBox>

namespace Absencoid {

/* Konstruktor */
ChangedLessonsTab::ChangedLessonsTab(TimetableModel* timetableModel, ClassesModel* classesModel, QWidget* parent): QWidget(parent) {
    changedLessonsModel = new ChangedLessonsModel(classesModel, timetableModel, this);

    /* Tabulka se změnami */
    changedLessonsView = new QTableView;
    changedLessonsView->setModel(changedLessonsModel);

    /* Delegát pro datum */
    DateEditDelegate* dateEditDelegate = new DateEditDelegate(changedLessonsView);
    changedLessonsView->setItemDelegateForColumn(0, dateEditDelegate);

    /* Delegát pro číslo hodiny */
    SpinBoxDelegate* spinBoxDelegate = new SpinBoxDelegate(-1, 9, changedLessonsView);
    changedLessonsView->setItemDelegateForColumn(1, spinBoxDelegate);

    /* Combobox delegáty pro výběr předmětů */
    ComboBoxDelegate* comboBoxDelegate = new ComboBoxDelegate(classesModel, changedLessonsView);
    changedLessonsView->setItemDelegateForColumn(2, comboBoxDelegate);
    changedLessonsView->setItemDelegateForColumn(3, comboBoxDelegate);

    /* Zvětšení posledního sloupce, protože má dlouhou hlavičku */
    changedLessonsView->resizeColumnToContents(4);

    /* Tlačítka vpravo */
    QPushButton* addChangedLessonButton = new QPushButton(tr("Přidat změnu"));
    removeChangedLessonsButton = new QPushButton(tr("Odebrat vybrané"));
    removeChangedLessonsButton->setDisabled(true);

    /* Layout pro tlačítka vpravo */
    QVBoxLayout* buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(addChangedLessonButton, 0, Qt::AlignTop);
    buttonsLayout->addWidget(removeChangedLessonsButton, 1, Qt::AlignTop);

    /* Layout celkový */
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(changedLessonsView, 1);
    layout->addLayout(buttonsLayout, 0);

    /* Propojení tlačítek s jejich funkcemi */
    connect(addChangedLessonButton, SIGNAL(clicked(bool)), this, SLOT(addChangedLesson()));
    connect(removeChangedLessonsButton, SIGNAL(clicked(bool)), this, SLOT(removeChangedLessons()));

    /* Při zrušení výběru se deaktivuje tlačítko pro smazání změn */
    connect(changedLessonsView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(updateRemoveButton()));

    #ifndef ADMIN_VERSION
    addChangedLessonButton->setDisabled(true);
    #endif

    setLayout(layout);
}

/* Přidání změny */
void ChangedLessonsTab::addChangedLesson() {
    changedLessonsModel->insertRow(changedLessonsModel->rowCount());
}

/* Odstranění vybraných změn */
void ChangedLessonsTab::removeChangedLessons() {
    /* Projití výběru, aby zde byly unikátní řádky */
    QList<int> rows;
    foreach(QModelIndex index, changedLessonsView->selectionModel()->selectedIndexes()) {
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
        changedLessonsModel->removeRow(row);
    }
}

/* Zašednutí / aktivace tlačítka pro mazání */
void ChangedLessonsTab::updateRemoveButton() {
    #ifdef ADMIN_VERSION
    if(changedLessonsView->selectionModel()->hasSelection())
        removeChangedLessonsButton->setDisabled(false);
    else
        removeChangedLessonsButton->setDisabled(true);
    #endif
}

}
