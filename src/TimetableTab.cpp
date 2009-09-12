#include "TimetableTab.h"

#include <QComboBox>
#include <QBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QPushButton>
#include <QDateEdit>

#include "TimetableListModel.h"
#include <QLineEdit>

namespace Absencoid {

/* Konstruktor */
TimetableTab::TimetableTab(ClassesModel* classesModel, QWidget* parent):
QWidget(parent), timetableListModel(new TimetableListModel(this)),
timetableCombo(new QComboBox), description(new QLineEdit),
validFrom(new QDateEdit), validTo(new QDateEdit), followedBy(new QComboBox) {
    /* Nastavení modelu pro výběr rozvrhu */
    timetableCombo->setModel(timetableListModel);

    /* Při změně rozvrhu načíst nový */
    connect(timetableCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(loadTimetable(int)));

    QHBoxLayout* selectTimeTableLayout = new QHBoxLayout;
    selectTimeTableLayout->addWidget(new QLabel(tr("Aktuální rozvrh: ")));
    selectTimeTableLayout->addWidget(timetableCombo, 1);

    /* Tabulka rozvrhu */
    QTableView* timetableView = new QTableView(this);

    /* Tlačítka atd. vpravo */
    QPushButton* addTimetable = new QPushButton(tr("Nový rozvrh"));
    QPushButton* deleteTimetable = new QPushButton(tr("Odstranit rozvrh"));

    QPushButton* switchDirection = new QPushButton(tr("Svislý směr"));
    switchDirection->setCheckable(true);

    validFrom->setDisplayFormat("dd.MM.yyyy");
    validTo->setDisplayFormat("dd.MM.yyyy");

    QPushButton* deleteLessons = new QPushButton(tr("Odstranit vybrané"));

    /* Nastavení modelu a max. šířky comba pro následující rozvrh */
    followedBy->setModel(timetableListModel);
    followedBy->setModelColumn(1);
    followedBy->setMaximumWidth(deleteLessons->sizeHint().width());

    /* Při změně aktuální položky změnit tooltip */
    connect(followedBy, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFollowedByTooltip(int)));

    /* Layout tlačítek vpravo */
    QVBoxLayout* buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(addTimetable, 0, Qt::AlignTop);
    buttonsLayout->addWidget(deleteTimetable, 0, Qt::AlignTop);
    buttonsLayout->addSpacing(16);
    buttonsLayout->addWidget(switchDirection, 0, Qt::AlignTop);
    buttonsLayout->addSpacing(16);
    buttonsLayout->addWidget(new QLabel(tr("Popisek:")), 0, Qt::AlignTop);
    buttonsLayout->addWidget(description, 0, Qt::AlignTop);
    buttonsLayout->addSpacing(8);
    buttonsLayout->addWidget(new QLabel(tr("Platnost od:")), 0, Qt::AlignTop);
    buttonsLayout->addWidget(validFrom, 0, Qt::AlignTop);
    buttonsLayout->addSpacing(8);
    buttonsLayout->addWidget(new QLabel(tr("Platnost do:")), 0, Qt::AlignTop);
    buttonsLayout->addWidget(validTo, 0, Qt::AlignTop);
    buttonsLayout->addSpacing(8);
    buttonsLayout->addWidget(new QLabel(tr("Následován s:")), 0, Qt::AlignTop);
    buttonsLayout->addWidget(followedBy, 0, Qt::AlignTop);
    buttonsLayout->addSpacing(16);
    buttonsLayout->addWidget(deleteLessons, 1, Qt::AlignTop);

    /* Layout tabulky a tlačítek */
    QHBoxLayout* bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(timetableView, 1);
    bottomLayout->addLayout(buttonsLayout);

    /* Celkový layout */
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(selectTimeTableLayout);
    layout->addLayout(bottomLayout);

    /* Načtení rozvrhu */
    loadTimetable(timetableCombo->currentIndex());

    setLayout(layout);
}

/* Načtení rozvrhu */
void TimetableTab::loadTimetable(int index) {
    /* Nastavení popisku */
    description->setText(timetableListModel->index(index, 1).data().toString());

    /* Nastavení začátku a konce platnosti */
    validFrom->setDate(timetableListModel->index(index, 2).data().toDate());
    validTo->setDate(timetableListModel->index(index, 3).data().toDate());

    /* Nastavení následujícího rozvrhu */
    followedBy->setCurrentIndex(
        timetableListModel->indexFromId(timetableListModel->index(index, 3).data().toInt())
    );
}

/* Změna tooltipu u položky pro následující rozvrh */
void TimetableTab::changeFollowedByTooltip(int index) {
    followedBy->setToolTip(timetableListModel->index(index, 0).data().toString());
}

}
