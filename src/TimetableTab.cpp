#include "TimetableTab.h"

#include <QComboBox>
#include <QBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QPushButton>
#include <QDateEdit>
#include <QCalendarWidget>
#include <QLineEdit>
#include <QDebug>

#include "TimetableListModel.h"
#include <QMessageBox>

namespace Absencoid {

/* Konstruktor */
TimetableTab::TimetableTab(ClassesModel* classesModel, QWidget* parent):
QWidget(parent), timetableListModel(new TimetableListModel(this)),
timetableCombo(new QComboBox), description(new QLineEdit),
validFrom(new QDateEdit), followedBy(new QComboBox) {

    QHBoxLayout* selectTimeTableLayout = new QHBoxLayout;
    selectTimeTableLayout->addWidget(new QLabel(tr("Aktuální rozvrh: ")));
    selectTimeTableLayout->addWidget(timetableCombo, 1);

    /* Tabulka rozvrhu */
    QTableView* timetableView = new QTableView(this);

    /* Tlačítka atd. vpravo */
    QPushButton* addTimetableButton = new QPushButton(tr("Nový rozvrh"));
    removeTimetableButton = new QPushButton(tr("Odstranit rozvrh"));
    switchDirectionButton = new QPushButton(tr("Svislý směr"));
    switchDirectionButton->setCheckable(true);
    removeLessonsButton = new QPushButton(tr("Odstranit vybrané"));

    descriptionLabel = new QLabel(tr("Popisek:"));
    validFromLabel = new QLabel(tr("Platnost od:"));
    followedByLabel = new QLabel(tr("Následován s:"));

    /* Layout tlačítek vpravo */
    QVBoxLayout* buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(addTimetableButton,    0, Qt::AlignTop);
    buttonsLayout->addWidget(removeTimetableButton, 0, Qt::AlignTop);
    buttonsLayout->addSpacing(16);
    buttonsLayout->addWidget(switchDirectionButton, 0, Qt::AlignTop);
    buttonsLayout->addSpacing(16);
    buttonsLayout->addWidget(descriptionLabel,      0, Qt::AlignTop);
    buttonsLayout->addWidget(description,           0, Qt::AlignTop);
    buttonsLayout->addSpacing(8);
    buttonsLayout->addWidget(validFromLabel,        0, Qt::AlignTop);
    buttonsLayout->addWidget(validFrom,             0, Qt::AlignTop);
    buttonsLayout->addSpacing(8);
    buttonsLayout->addWidget(followedByLabel,       0, Qt::AlignTop);
    buttonsLayout->addWidget(followedBy,            0, Qt::AlignTop);
    buttonsLayout->addSpacing(16);
    buttonsLayout->addWidget(removeLessonsButton,   1, Qt::AlignTop);

    /* Layout tabulky a tlačítek */
    QHBoxLayout* bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(timetableView, 1);
    bottomLayout->addLayout(buttonsLayout);

    /* Celkový layout */
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(selectTimeTableLayout);
    layout->addLayout(bottomLayout);

    /* Nastavení modelu pro výběr rozvrhu */
    timetableCombo->setModel(timetableListModel);

    /* Políčko pro začátek platnosti */
    validFrom->setDisplayFormat("dd.MM.yyyy");

    /* Po kliknutí na šipku vyleze kalendář */
    QCalendarWidget* calendarWidget = new QCalendarWidget(validFrom);
    calendarWidget->setFirstDayOfWeek(Qt::Monday);
    validFrom->setCalendarPopup(true);
    validFrom->setCalendarWidget(calendarWidget);

    /* Nastavení modelu a max. šířky comba pro následující rozvrh podle nejširšího
        tlačítka */
    followedBy->setModel(timetableListModel);
    followedBy->setModelColumn(1);
    followedBy->setMaximumWidth(removeLessonsButton->sizeHint().width());

    /* Při změně rozvrhu načíst nový */
    connect(timetableCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(loadTimetable(int)));

    /* Nový / smazat rozvrh */
    connect(addTimetableButton, SIGNAL(clicked(bool)), this, SLOT(addTimetable()));
    connect(removeTimetableButton, SIGNAL(clicked(bool)), this, SLOT(removeTimetable()));

    /* Uložit změny v popisku a datech */
    connect(description, SIGNAL(editingFinished()), this, SLOT(setDescription()));
    connect(validFrom, SIGNAL(editingFinished()), this, SLOT(setValidFrom()));
    connect(followedBy, SIGNAL(currentIndexChanged(int)), this, SLOT(setFollowedBy()));

    /** @todo Propojit dataChanged() s inputy */

    /* Načtení rozvrhu */
    loadTimetable(timetableCombo->currentIndex());

    setLayout(layout);
}

/* Načtení rozvrhu */
void TimetableTab::loadTimetable(int index) {
    /* Pokud je index záporný (tj. neexistuje žádný rozvrh), zašednutí políček */
    if(index == -1) {
        removeTimetableButton->setDisabled(true);
        switchDirectionButton->setDisabled(true);
        descriptionLabel->setDisabled(true);
        description->setDisabled(true);
        validFromLabel->setDisabled(true);
        validFrom->setDisabled(true);
        followedByLabel->setDisabled(true);
        followedBy->setDisabled(true);
        removeLessonsButton->setDisabled(true);

    /* Jinak odšednutí */
    } else {
        removeTimetableButton->setDisabled(false);
        switchDirectionButton->setDisabled(false);
        descriptionLabel->setDisabled(false);
        description->setDisabled(false);
        validFromLabel->setDisabled(false);
        validFrom->setDisabled(false);
        followedByLabel->setDisabled(false);
        followedBy->setDisabled(false);
        removeLessonsButton->setDisabled(false);
    }

    /* Nastavení popisku */
    description->setText(timetableListModel->index(index, 1).data().toString());

    /* Nastavení začátku a konce platnosti */
    validFrom->setDate(timetableListModel->index(index, 2).data().toDate());

    /* Nastavení následujícího rozvrhu */
    followedBy->setCurrentIndex(
        timetableListModel->indexFromId(timetableListModel->index(index, 3).data().toInt())
    );
}

/* Přidání a načtení nového rozvrhu */
void TimetableTab::addTimetable() {
    int index = timetableListModel->rowCount();
    if(timetableListModel->insertRow(index))
        loadTimetable(index);
}

/* Odebrání aktuálně načteného rozvrhu */
void TimetableTab::removeTimetable() {
    /* Ověření rozhodnutí */
    if(QMessageBox::warning(this, tr("Odstranit rozvrh"),
        tr("<strong>Opravdu ostranit tento rozvrh?</strong><br/>") + description->text() +
        tr("<br/>(Platný od ") + validFrom->date().toString("dd.MM.yyyy") + tr(")"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
            return;

    timetableListModel->removeRow(timetableCombo->currentIndex());
}

/* Nastavení popisku rozvrhu */
void TimetableTab::setDescription() {
    /* Pokud se nepovede uložit (např. prázdný popisek), vrácení starých dat zpět */
    if(!timetableListModel->setData(
        timetableListModel->index(timetableCombo->currentIndex(), 1),
        description->text()
    ))
        description->setText(timetableListModel->index(timetableCombo->currentIndex(), 1).data().toString());
}

/* Nastavení začátku platnosti */
void TimetableTab::setValidFrom() {
    /* Nastavení dat */
    timetableListModel->setData(
        timetableListModel->index(timetableCombo->currentIndex(), 2),
        validFrom->date());
}

/* Nastavení následujícího rozvrhu */
void TimetableTab::setFollowedBy() {
    /* Přepočteme index položky na ID rozvrhu */
    int id = timetableListModel->idFromIndex(followedBy->currentIndex());

    /* Nastavení dat */
    timetableListModel->setData(
        timetableListModel->index(timetableCombo->currentIndex(), 3), id);

    /* Změna tooltipu a aktuální (aby byl vidět celý název) */
    followedBy->setToolTip(timetableListModel->index(followedBy->currentIndex(), 0).data().toString());
}

}
