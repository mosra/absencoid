#include "ClassesTab.h"

#include <QTableView>
#include <QPushButton>
#include <QBoxLayout>
#include <QMessageBox>

#include "configure.h"
#include "ComboBoxDelegate.h"
#include "TeachersModel.h"
#include "ClassesModel.h"
#include "Style.h"

namespace Absencoid {

/* Konstruktor */
ClassesTab::ClassesTab(TeachersModel* teachersModel, QWidget* parent):
QWidget(parent), classesModel(new ClassesModel(teachersModel, this)),
classesView(new QTableView(this)) {

    classesView->setModel(classesModel);
    /* První sloupec (souhrn) skryjeme */
    classesView->setColumnHidden(0, true);
    /* Pro třetí sloupec delegáta pro editaci pomocí comboboxu */
    classesView->setItemDelegateForColumn(2, new ComboBoxDelegate(teachersModel, classesView));

    /* První dva speciální řádky také skryjeme */
    classesView->setRowHidden(0, true);
    classesView->setRowHidden(1, true);

    /* Tlačítka pro přidání / odebrání třídy */
    QPushButton* addClass = new QPushButton(Style::style()->icon(Style::PlusIcon), tr("Přidat předmět"));
    removeClassesButton = new QPushButton(Style::style()->icon(Style::MinusIcon), tr("Odstranit vybrané"));

    /* Propojení tlačítek s jejich funkcemi */
    connect(addClass, SIGNAL(clicked(bool)), this, SLOT(addClass()));
    connect(removeClassesButton, SIGNAL(clicked(bool)), this, SLOT(removeClasses()));

    /* Tlačítko pro mazání je aktivní, jen pokud je něco vybráno */
    removeClassesButton->setDisabled(true);
    connect(classesView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(updateRemoveButton()));

    /* Layout */
    QVBoxLayout* classesButtonLayout = new QVBoxLayout;
    classesButtonLayout->addWidget(addClass, 0, Qt::AlignTop);
    classesButtonLayout->addWidget(removeClassesButton, 1, Qt::AlignTop);
    QHBoxLayout* classesLayout = new QHBoxLayout;
    classesLayout->addWidget(classesView);
    classesLayout->addLayout(classesButtonLayout);

    #ifndef ADMIN_VERSION
    addClass->setDisabled(true);
    #endif

    setLayout(classesLayout);
}

/* Přidání předmětu */
void ClassesTab::addClass() {
    classesModel->insertRow(classesModel->rowCount());
    classesView->scrollToBottom();
}

/* Odebrání předmětů */
void ClassesTab::removeClasses() {
    /* Nic není vybráno */
    if(!classesView->selectionModel()->hasSelection()) return;

    /* Protože se vybírají jednotlivé buňky, je možné, že bude vybráno více
        buněk na stejném řádku. Pročistění, aby byly řádky unikátní. */
    QList<int> rows; QStringList names; QModelIndex index;
    foreach(index, classesView->selectionModel()->selectedIndexes()) {

        /* Je to unikátní záznam */
        if(!rows.contains(index.row())) {
            rows.append(index.row());

            QString name = index.sibling(index.row(), 0).data().toString();

            /* Při mazání čerstvě přidaných předmětů */
            if(name == " ()") name = tr("(prázdný)");

            names.append(name);
        }
    }

    /* Seřazení jmen, čísel řádků podle abecedy (důvod k řazení čísel v
        TeachersTab::removeTeachers()) */
    qSort(names.begin(), names.end());
    qSort(rows.begin(), rows.end(), qGreater<int>());

    /* Uživatel neví, co chce */
    if(QMessageBox::warning(this, tr("Potvrzení"),
        tr("<strong>Opravdu smazat tyto předměty?</strong><br/>") + names.join("<br/>"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
            return;

    /* Mazání jednotlivých unikátních řádků */
    int row; foreach(row, rows) {
        classesModel->removeRows(row, 1);  /** @todo Mazat celé skupiny! */
    }
}

/* Zakázaní / povolení tlačítka pro mazání předmětů */
void ClassesTab::updateRemoveButton() {
    #ifdef ADMIN_VERSION
    if(!classesView->selectionModel()->hasSelection())
        removeClassesButton->setDisabled(true);
    else
        removeClassesButton->setDisabled(false);
    #endif
}

}
