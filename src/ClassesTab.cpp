#include "ClassesTab.h"

#include <QTableView>
#include <QPushButton>
#include <QBoxLayout>

#include "ClassesModel.h"
#include "ClassesDelegate.h"

namespace Absencoid {

/* Konstruktor */
ClassesTab::ClassesTab(TeachersModel* teachersModel, QWidget* parent): QWidget(parent) {
    /* Model předmětů */
    classesModel = new ClassesModel(teachersModel, this);

    /* Tabulka předmětů */
    QTableView* classesView = new QTableView(this);
    classesView->setModel(classesModel);
    classesView->setItemDelegate(new ClassesDelegate(teachersModel, classesView));

    /* Tlačítka pro přidání / odebrání třídy */
    QPushButton* addClass = new QPushButton(tr("Přidat předmět"));
    QPushButton* deleteClass = new QPushButton(tr("Odebrat předmět"));

    /* Propojení tlačítek s jejich funkcemi */
    connect(addClass, SIGNAL(clicked(bool)), this, SLOT(addClass()));

    /* Layout */
    QVBoxLayout* classesButtonLayout = new QVBoxLayout;
    classesButtonLayout->addWidget(addClass, 0, Qt::AlignTop);
    classesButtonLayout->addWidget(deleteClass, 1, Qt::AlignTop);
    QHBoxLayout* classesLayout = new QHBoxLayout;
    classesLayout->addWidget(classesView);
    classesLayout->addLayout(classesButtonLayout);

    setLayout(classesLayout);
}

/* Přidání třídy */
void ClassesTab::addClass() {
    classesModel->insertRow(classesModel->rowCount());
}

}
