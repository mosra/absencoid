#include "TimetableTab.h"
#include <QComboBox>
#include <QBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QPushButton>
#include <QDateEdit>

namespace Absencoid {

/* Konstruktor */
TimetableTab::TimetableTab(ClassesModel* classesModel, QWidget* parent): QWidget(parent) {
    /* Výběr rozvrhu */
    QComboBox* selectTimetable = new QComboBox;
    QHBoxLayout* selectTimeTableLayout = new QHBoxLayout;
    selectTimeTableLayout->addWidget(new QLabel(tr("Aktuální rozvrh: ")));
    selectTimeTableLayout->addWidget(selectTimetable, 1);

    /* Tabulka rozvrhu */
    QTableView* timetableView = new QTableView(this);

    /* Tlačítka atd. vpravo */
    QPushButton* addTimetable = new QPushButton(tr("Nový rozvrh"));
    QPushButton* deleteTimetable = new QPushButton(tr("Smazat rozvrh"));

    QPushButton* switchDirection = new QPushButton(tr("Svislý směr"));
    switchDirection->setCheckable(true);

    QPushButton* deleteLessons = new QPushButton(tr("Smazat vybrané"));

    QDateEdit* validFrom = new QDateEdit;
    validFrom->setDisplayFormat("dd.MM.yyyy");
    QDateEdit* validTo = new QDateEdit;
    validTo->setDisplayFormat("dd.MM.yyyy");

    /* Layout tlačítek vpravo */
    QVBoxLayout* buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(addTimetable, 0, Qt::AlignTop);
    buttonsLayout->addWidget(deleteTimetable, 0, Qt::AlignTop);
    buttonsLayout->addSpacing(16);
    buttonsLayout->addWidget(switchDirection, 0, Qt::AlignTop);
    buttonsLayout->addSpacing(16);
    buttonsLayout->addWidget(deleteLessons, 0, Qt::AlignTop);
    buttonsLayout->addSpacing(16);
    buttonsLayout->addWidget(new QLabel(tr("Platnost od:")), 0, Qt::AlignTop);
    buttonsLayout->addWidget(validFrom, 0, Qt::AlignTop);
    buttonsLayout->addSpacing(8);
    buttonsLayout->addWidget(new QLabel(tr("Platnost do:")), 0, Qt::AlignTop);
    buttonsLayout->addWidget(validTo, 1, Qt::AlignTop);

    /* Layout tabulky a tlačítek */
    QHBoxLayout* bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(timetableView);
    bottomLayout->addLayout(buttonsLayout);

    /* Celkový layout */
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(selectTimeTableLayout);
    layout->addLayout(bottomLayout);

    setLayout(layout);
}

}
