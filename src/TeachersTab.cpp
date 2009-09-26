#include "TeachersTab.h"

#include <QTableView>
#include <QPushButton>
#include <QBoxLayout>
#include <QMessageBox>

#include "TeachersModel.h"

namespace Absencoid {

/* Konstruktor */
TeachersTab::TeachersTab(QWidget* parent): QWidget(parent),
teachersModel(new TeachersModel(this)), teachersView(new QTableView(this)) {

    /* Nastavení modelu pro zobrazení */
    teachersView->setModel(teachersModel);
    /** @todo Automaticky? */
    teachersView->setColumnWidth(1, 130);
    teachersView->setColumnWidth(2, 130);

    /* Tlačítka pro přidání / odebrání učitele */
    QPushButton* addTeacher = new QPushButton(tr("Přidat učitele"));
    removeTeachersButton = new QPushButton(tr("Odstranit vybrané"));

    /* Propojení tlačítek s funkcemi */
    connect(addTeacher, SIGNAL(clicked(bool)), this, SLOT(addTeacher()));
    connect(removeTeachersButton, SIGNAL(clicked(bool)), this, SLOT(removeTeachers()));

    /* Tlačítko mazání je aktivní jen pokud je něco vybráno */
    removeTeachersButton->setDisabled(true);
    connect(teachersView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(updateRemoveButton()));

    /* Layout */
    QVBoxLayout* teachersButtonLayout = new QVBoxLayout;
    teachersButtonLayout->addWidget(addTeacher, 0, Qt::AlignTop);
    teachersButtonLayout->addWidget(removeTeachersButton, 1, Qt::AlignTop);
    QHBoxLayout* teachersLayout = new QHBoxLayout;
    teachersLayout->addWidget(teachersView);
    teachersLayout->addLayout(teachersButtonLayout);

    #ifndef ADMIN_VERSION
    addTeacher->setDisabled(true);
    #endif

    setLayout(teachersLayout);
}

/* Přidání učitele */
void TeachersTab::addTeacher() {
    teachersModel->insertRow(teachersModel->rowCount());
}

/* Odebrání vybraných učitelů */
void TeachersTab::removeTeachers() {
    /* Nic není vybráno */
    if(!teachersView->selectionModel()->hasSelection()) return;

    /* Protože se vybírají jednotlivé buňky, může dojít k tomu, že je vybráno
        více buněk ve stejném řádku. Pročištění, aby byly řádky unikátní. */
    QList<int> rows; QStringList names; QModelIndex index;
    foreach(index, teachersView->selectionModel()->selectedIndexes()) {

        /* Je to unikátní záznam */
        if(!rows.contains(index.row())) {
            rows.append(index.row());

            /* Jméno leží v prvním (0) sloupci daného řádku */
            QString name = index.sibling(index.row(), 0).data().toString();

            /* Při mazání čerstvě přidaných učitelů */
            if(name.isEmpty()) name = tr("(prázdný)");

            names.append(name);
        }
    }

    /* Seřazení jmen podle abecedy */
    qSort(names.begin(), names.end());

    /* Uživatel neví, co chce */
    if(QMessageBox::warning(this, tr("Potvrzení"),
        tr("<strong>Opravdu smazat tyto učitele?</strong><br/>") + names.join("<br/>"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
            return;

    /* Seřazení řádků od největších čásel k nejmenším. Kdybychom totiž odstranili
        řádek 1, všechna další čísla by byla o jedničku posunutá, takže by se
        mazaly jiné řádky! */
    qSort(rows.begin(), rows.end(), qGreater<int>());

    /* Mazání jednotlivých unikátních řádků */
    int row; foreach(row, rows) {
        teachersModel->removeRows(row, 1); /** @todo Mazat celé skupiny? */
    }
}

/* Zašednutí / povolení mazacího tlačítka */
void TeachersTab::updateRemoveButton() {
    #ifdef ADMIN_VERSION
    if(!teachersView->selectionModel()->hasSelection())
        removeTeachersButton->setDisabled(true);
    else
        removeTeachersButton->setDisabled(false);
    #endif
}

}
