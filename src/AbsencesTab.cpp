#include "AbsencesTab.h"

#include <QTableView>
#include <QPushButton>
#include <QBoxLayout>

#include "ClassesModel.h"
#include "TimetableModel.h"
#include "ChangesModel.h"
#include "AbsencesModel.h"

namespace Absencoid {

/* Konstruktor */
AbsencesTab::AbsencesTab(ClassesModel* classesModel, TimetableModel* timetableModel, ChangesModel* changesModel, QWidget* parent): QWidget(parent) {
    /* Tabulka s absencemi */
    QTableView* absencesView = new QTableView;
    absencesView->setModel(new AbsencesModel(classesModel, timetableModel, changesModel));
    absencesView->setColumnWidth(1, 70);
    absencesView->setColumnWidth(2, 70);
    absencesView->setColumnWidth(3, 70);
    absencesView->setColumnWidth(4, 70);
    absencesView->setColumnWidth(5, 70);
    absencesView->setColumnWidth(6, 70);
    absencesView->setColumnWidth(7, 70);
    absencesView->setColumnWidth(8, 70);
    absencesView->setColumnWidth(9, 70);
    absencesView->setColumnWidth(10, 70);
    absencesView->setColumnWidth(11, 70);

    /* Tlačítka nahoře */
    QPushButton* addAbsenceButton = new QPushButton(tr("Přidat absenci"));
    QPushButton* removeSelectedButton = new QPushButton(tr("Odstranit vybrané"));

    /* Horní layout */
    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(addAbsenceButton, 1, Qt::AlignRight);
    buttonsLayout->addWidget(removeSelectedButton);

    /* Celkový layout */
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(absencesView);
    layout->addLayout(buttonsLayout);
}

}
