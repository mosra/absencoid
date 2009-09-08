#include "TeachersTab.h"

#include <QTableView>
#include <QPushButton>
#include <QBoxLayout>

#include "TeachersModel.h"

namespace Absencoid {

/* Konstruktor */
TeachersTab::TeachersTab(QWidget* parent): QWidget(parent) {
    /* Model */
    teachersModel = new TeachersModel(this);

    QTableView* teachersView = new QTableView(this);
    teachersView->setModel(teachersModel);
    /** @todo Automaticky? */
    teachersView->setColumnWidth(1, 130);
    teachersView->setColumnWidth(2, 130);

    /* Tlačítka pro přidání / odebrání učitele */
    QPushButton* addTeacher = new QPushButton(tr("Přidat učitele"));
    QPushButton* deleteTeacher = new QPushButton(tr("Odebrat učitele"));

    connect(addTeacher, SIGNAL(clicked(bool)), teachersModel, SLOT(addTeacher()));

    /* Layout */
    QVBoxLayout* teachersButtonLayout = new QVBoxLayout;
    teachersButtonLayout->addWidget(addTeacher, 0, Qt::AlignTop);
    teachersButtonLayout->addWidget(deleteTeacher, 1, Qt::AlignTop);
    QHBoxLayout* teachersLayout = new QHBoxLayout;
    teachersLayout->addWidget(teachersView);
    teachersLayout->addLayout(teachersButtonLayout);

    setLayout(teachersLayout);
}

}
