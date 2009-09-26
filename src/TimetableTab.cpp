#include "TimetableTab.h"

#include <QComboBox>
#include <QBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QPushButton>
#include <QDateEdit>
#include <QCalendarWidget>
#include <QLineEdit>
#include <QMessageBox>

#include "configure.h"
#include "ComboBoxDelegate.h"
#include "ClassesModel.h"
#include "TimetableModel.h"

namespace Absencoid {

/* Konstruktor */
TimetableTab::TimetableTab(ClassesModel* classesModel, QWidget* parent):
QWidget(parent), timetableCombo(new QComboBox), description(new QLineEdit),
validFrom(new QDateEdit), followedBy(new QComboBox) {

    QHBoxLayout* selectTimeTableLayout = new QHBoxLayout;
    selectTimeTableLayout->addWidget(new QLabel(tr("Aktuální rozvrh: ")));
    selectTimeTableLayout->addWidget(timetableCombo, 1);

    /* Tabulka rozvrhu */
    timetableView = new QTableView(this);
    timetableModel = new TimetableModel(classesModel, this);
    timetableView->setModel(timetableModel);
    timetableView->setItemDelegate(new ComboBoxDelegate(classesModel, this));

    /* Tlačítka atd. vpravo */
    QPushButton* addTimetableButton = new QPushButton(tr("Nový rozvrh"));
    removeTimetableButton = new QPushButton(tr("Odstranit rozvrh"));
    removeLessonsButton = new QPushButton(tr("Odstranit vybrané"));

    descriptionLabel = new QLabel(tr("Popisek:"));
    validFromLabel = new QLabel(tr("Platnost od:"));
    followedByLabel = new QLabel(tr("Následován s:"));

    /* Layout tlačítek vpravo */
    QVBoxLayout* buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(addTimetableButton,    0, Qt::AlignTop);
    buttonsLayout->addWidget(removeTimetableButton, 0, Qt::AlignTop);
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
    timetableCombo->setModel(timetableModel);

    /* Políčko pro začátek platnosti */
    validFrom->setDisplayFormat("ddd dd.MM.yyyy");

    /* Po kliknutí na šipku vyleze kalendář */
    QCalendarWidget* calendarWidget = new QCalendarWidget(validFrom);
    calendarWidget->setFirstDayOfWeek(Qt::Monday);
    validFrom->setCalendarPopup(true);
    validFrom->setCalendarWidget(calendarWidget);

    /* Nastavení modelu a max. šířky comba pro následující rozvrh podle nejširšího
        tlačítka */
    followedBy->setModel(timetableModel);
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

    /* Mazání vybraných hodin, zašednutí pokud není nic vybráno */
    removeLessonsButton->setDisabled(true);
    connect(removeLessonsButton, SIGNAL(clicked()), this, SLOT(removeLessons()));
    connect(timetableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(updateRemoveButton()));

    /** @todo Propojit dataChanged() s inputy */

    /* Načtení rozvrhu */
    loadTimetable(timetableCombo->currentIndex());

    #ifndef ADMIN_VERSION
    addTimetableButton->setDisabled(true);
    removeTimetableButton->setDisabled(true);
    descriptionLabel->setDisabled(true);
    description->setDisabled(true);
    validFromLabel->setDisabled(true);
    validFrom->setDisabled(true);
    followedByLabel->setDisabled(true);
    followedBy->setDisabled(true);
    #endif

    setLayout(layout);
}

/* Načtení rozvrhu */
void TimetableTab::loadTimetable(int index) {
    /* Pokud je index záporný (tj. neexistuje žádný rozvrh), zašednutí políček */
    if(index == -1) {
        timetableView->setDisabled(true);
        removeTimetableButton->setDisabled(true);
        descriptionLabel->setDisabled(true);
        description->setDisabled(true);
        validFromLabel->setDisabled(true);
        validFrom->setDisabled(true);
        followedByLabel->setDisabled(true);
        followedBy->setDisabled(true);
        removeLessonsButton->setDisabled(true);

    /* Jinak odšednutí */
    } else {
        timetableView->setDisabled(false);
        #ifdef ADMIN_VERSION
        removeTimetableButton->setDisabled(false);
        descriptionLabel->setDisabled(false);
        description->setDisabled(false);
        validFromLabel->setDisabled(false);
        validFrom->setDisabled(false);
        followedByLabel->setDisabled(false);
        followedBy->setDisabled(false);
        #endif
    }

    /* Přepnutí tabulky na daný index */
    timetableView->setRootIndex(timetableModel->index(index, 0));

    /* Nastavení popisku */
    description->setText(timetableModel->index(index, 1).data().toString());

    /* Nastavení začátku a konce platnosti */
    validFrom->setDate(timetableModel->index(index, 2).data().toDate());

    /* Nastavení následujícího rozvrhu */
    followedBy->setCurrentIndex(
        timetableModel->indexFromId(timetableModel->index(index, 3).data().toInt())
    );
}

/* Přidání a načtení nového rozvrhu */
void TimetableTab::addTimetable() {
    int index = timetableModel->rowCount();

    /* Pokud se podaří přidat, změníme index v comboboxu na nový, což jej
        současně i načte */
    if(timetableModel->insertRow(index))
        timetableCombo->setCurrentIndex(index);
}

/* Odebrání aktuálně načteného rozvrhu */
void TimetableTab::removeTimetable() {
    /* Ověření rozhodnutí */
    if(QMessageBox::warning(this, tr("Odstranit rozvrh"),
        tr("<strong>Opravdu ostranit tento rozvrh?</strong><br/>") + description->text() +
        tr("<br/>(Platný od ") + validFrom->date().toString("ddd dd.MM.yyyy") + tr(")"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
            return;

    timetableModel->removeRow(timetableCombo->currentIndex());
}

/* Nastavení popisku rozvrhu */
void TimetableTab::setDescription() {
    /* Pokud se nepovede uložit (např. prázdný popisek), vrácení starých dat zpět */
    if(!timetableModel->setData(
        timetableModel->index(timetableCombo->currentIndex(), 1),
        description->text()
    ))
        description->setText(timetableModel->index(timetableCombo->currentIndex(), 1).data().toString());
}

/* Nastavení začátku platnosti */
void TimetableTab::setValidFrom() {
    /* Nastavení dat */
    timetableModel->setData(
        timetableModel->index(timetableCombo->currentIndex(), 2),
        validFrom->date());
}

/* Nastavení následujícího rozvrhu */
void TimetableTab::setFollowedBy() {
    /* Přepočteme index položky na ID rozvrhu */
    int id = timetableModel->idFromIndex(followedBy->currentIndex());

    /* Nastavení dat */
    timetableModel->setData(
        timetableModel->index(timetableCombo->currentIndex(), 3), id);

    /* Změna tooltipu a aktuální (aby byl vidět celý název) */
    followedBy->setToolTip(timetableModel->index(followedBy->currentIndex(), 0).data().toString());
}

/* Odstranění vybraných hodin z rozvrhu */
void TimetableTab::removeLessons() {
    QModelIndexList selected = timetableView->selectionModel()->selectedIndexes();

    /* Ověření */
    if(QMessageBox::warning(this, tr("Odstranit vybrané hodiny"),
        tr("Opravdu odstranit vybrané hodiny <strong>(") +
        QString::number(selected.count()) + tr(")</strong> z rozvrhu?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
        != QMessageBox::Yes) return;

    foreach(QModelIndex index, selected) {
        timetableModel->setData(index, -1);
    }
}

/* Povolení / zašednutí mazacího tlačítka */
void TimetableTab::updateRemoveButton() {
    if(timetableView->selectionModel()->hasSelection())
        removeLessonsButton->setDisabled(false);
    else
        removeLessonsButton->setDisabled(true);
}

}
