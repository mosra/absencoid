#include "AbsencesTab.h"

#include <QTableView>
#include <QPushButton>
#include <QBoxLayout>

namespace Absencoid {

/* Konstruktor */
AbsencesTab::AbsencesTab(QWidget* parent): QWidget(parent) {
    /* Tabulka s absencemi */
    QTableView* absencesView = new QTableView;

    /* Tlačítka vpravo */
    QPushButton* addAbsenceButton = new QPushButton(tr("Přidat absenci"));
    QPushButton* removeSelectedButton = new QPushButton(tr("Odstranit vybrané"));

    /* Pravý layout */
    QVBoxLayout* buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(addAbsenceButton);
    buttonsLayout->addWidget(removeSelectedButton, 1, Qt::AlignTop);

    /* Celkový layout */
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(absencesView);
    layout->addLayout(buttonsLayout);
}

}
