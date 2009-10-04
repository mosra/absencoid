#include "SummaryTab.h"

#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QLineEdit>
#include <QDateEdit>
#include <QCalendarWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>
#include <QBoxLayout>
#include <QTableView>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

#include "configure.h"
#include "Dump.h"
#include "CreateUpdateDialog.h"
#include "ConfigurationModel.h"
#include "TimetableTab.h"
#include "TimetableModel.h"
#include "UpdateDialog.h"

namespace Absencoid {

/* Konstruktor */
SummaryTab::SummaryTab(TimetableTab* _timetableTab, QWidget* parent): QWidget(parent), timetableTab(_timetableTab) {
    /* Inicializace konfiguračního modelu, propojení aktualizace DB s jeho reloadem */
    configurationModel = new ConfigurationModel(timetableTab->getTimetableModel(), this);
    connect(configurationModel, SIGNAL(modelReset()), this, SLOT(loadData()));

    /* Políčka pro editaci data */
    beginDate = new QDateEdit;
    endDate   = new QDateEdit;
    beginDate->setDisplayFormat("ddd dd.MM.yyyy");
    endDate  ->setDisplayFormat("ddd dd.MM.yyyy");

    /* Kalendářový popup pro editaci data */
    QCalendarWidget* beginPopup = new QCalendarWidget;
    QCalendarWidget* endPopup = new QCalendarWidget;
    beginPopup->setFirstDayOfWeek(Qt::Monday);
    endPopup->setFirstDayOfWeek(Qt::Monday);
    beginDate->setCalendarPopup(true);
    endDate  ->setCalendarPopup(true);
    beginDate->setCalendarWidget(beginPopup);
    endDate  ->setCalendarWidget(endPopup);

    /* Combobox pro výběr aktuálního rozvrhu */
    activeTimetable = new QComboBox;
    activeTimetable->setModel(timetableTab->getTimetableModel());
    activeTimetable->setModelColumn(1);

    /* Nastavení aktuálního indexu do modelu rozvrhů, načtení aktuálního rozvrhu */
    timetableTab->getTimetableModel()->setActualTimetable(activeTimetable->currentIndex());
    timetableTab->loadTimetable(activeTimetable->currentIndex());

    /* Checkboxy */
    updateOnStart = new QCheckBox(configurationModel->headerData(5, Qt::Horizontal).toString());
    dumpOnExit = new QCheckBox(configurationModel->headerData(6, Qt::Horizontal).toString());

    /* Tlačítko pro aktualizaci s popup menu */
    QPushButton* updateButton = new QPushButton(tr("Aktualizovat"));
    QMenu* updateButtonMenu = new QMenu(updateButton);
    updateButton->setMenu(updateButtonMenu);

    /* Položky v menu */
    updateFromWebAction =           updateButtonMenu->addAction("Z internetu");
    QAction* updateFromFileAction = updateButtonMenu->addAction("Ze souboru");
    connect(updateFromWebAction,    SIGNAL(triggered(bool)), this, SLOT(updateFromWeb()));
    connect(updateFromFileAction,   SIGNAL(triggered(bool)), this, SLOT(updateFromFile()));

    /* Políčko s URL */
    webUpdateUrl = new QLineEdit;
    webUpdateUrl->setValidator(new QRegExpValidator(
        QRegExp("(http|https)://([^\\./]+\\.){1,2}([a-zA-Z]{2,4}\\.)?[a-zA-Z]{2,4}/.*"), webUpdateUrl));

    /* Ověřování zadané URL */
    connect(webUpdateUrl, SIGNAL(textChanged(QString)), this, SLOT(validateUrlEdit()));

    /* Tlačítko pro vytvoření aktualizace, zálohy, obnovení ze zálohy */
    QPushButton* createUpdateButton = new QPushButton(tr("Vytvořit aktualizaci"));
    QPushButton* createDumpButton = new QPushButton(tr("Zálohovat"));
    QPushButton* loadDumpButton = new QPushButton(tr("Obnovit ze zálohy"));
    connect(createUpdateButton, SIGNAL(clicked(bool)), this, SLOT(createUpdate()));
    connect(createDumpButton, SIGNAL(clicked(bool)), this, SLOT(createDump()));
    connect(loadDumpButton, SIGNAL(clicked(bool)), this, SLOT(loadDump()));

    /* Propojení změn v políčkách s ukládacími akcemi */
    connect(beginDate, SIGNAL(editingFinished()), this, SLOT(setBeginDate()));
    connect(endDate, SIGNAL(editingFinished()), this, SLOT(setEndDate()));
    connect(activeTimetable, SIGNAL(currentIndexChanged(int)), this, SLOT(setActiveTimetable()));
    connect(webUpdateUrl, SIGNAL(editingFinished()), this, SLOT(setWebUpdateUrl()));
    connect(updateOnStart, SIGNAL(toggled(bool)), this, SLOT(setUpdateOnStart()));
    connect(dumpOnExit, SIGNAL(toggled(bool)), this, SLOT(setDumpOnExit()));

    /* LEVÝ VRCHNÍ GROUPBOX (STATISTIKA) */
    QGroupBox* statisticsGroup = new QGroupBox(tr("Statistika"));
    QGridLayout* statisticsLayout = new QGridLayout;
    statisticsLayout->addWidget(new QLabel(tr("Počet absencí:")), 0, 0);
    statisticsLayout->addWidget(new QLabel("0 (0%)"), 0, 1);
    statisticsLayout->addWidget(new QLabel(tr("Počet hodin dosud:")), 1, 0);
    statisticsLayout->addWidget(new QLabel("123"), 1, 1);
    statisticsLayout->addWidget(new QLabel(tr("Počet přidaných hodin:")), 2, 0);
    statisticsLayout->addWidget(new QLabel("4"), 2, 1);
    statisticsLayout->addWidget(new QLabel(tr("Počet odebraných hodin:")), 3, 0);
    statisticsLayout->addWidget(new QLabel("15"), 3, 1);
    statisticsLayout->addWidget(new QLabel(tr("Odebrané / přidané hodiny:")), 4, 0, Qt::AlignTop);
    statisticsLayout->addWidget(new QLabel("+ 11"), 4, 1, Qt::AlignTop);
    statisticsLayout->setColumnStretch(0, 1);
    statisticsLayout->setRowStretch(4, 1);
    statisticsGroup->setLayout(statisticsLayout);

    /* LEVÝ SPODNÍ GROUPBOX (NEJŽHAVĚJŠÍ ABSENCE) */
    QGroupBox* hottestGroup = new QGroupBox(tr("Nejžhavější absence"));
    QHBoxLayout* hottestLayout = new QHBoxLayout;
    hottestLayout->addWidget(new QTableView, 0, Qt::AlignCenter);
    hottestGroup->setLayout(hottestLayout);

    /* PRAVÝ VRCHNÍ GROUPBOX (NASTAVENÍ) */
    QGroupBox* settingsGroup = new QGroupBox(tr("Nastavení"));
    QGridLayout* settingsLayout = new QGridLayout;
    settingsLayout->addWidget(new QLabel(configurationModel->headerData(0, Qt::Horizontal).toString()+":"), 0, 0);
    settingsLayout->addWidget(beginDate, 0, 1);
    settingsLayout->addWidget(new QLabel(configurationModel->headerData(1, Qt::Horizontal).toString()+":"), 1, 0);
    settingsLayout->addWidget(endDate, 1, 1);
    settingsLayout->addWidget(new QLabel(configurationModel->headerData(2, Qt::Horizontal).toString()+":"), 2, 0);
    settingsLayout->addWidget(activeTimetable, 2, 1);
    settingsLayout->addWidget(new QWidget, 3, 0);
    settingsLayout->setRowStretch(3, 1);
    settingsGroup->setLayout(settingsLayout);

    /* PRAVÝ SPODNÍ GROUPBOX (AKTUALIZACE) */
    updateGroup = new QGroupBox;

    /* Layout pro tlačítka */
    QHBoxLayout* updateButtonLayout = new QHBoxLayout;
    updateButtonLayout->addWidget(updateButton);
    updateButtonLayout->addWidget(createUpdateButton);

    /* Celkový layout */
    QVBoxLayout* updateLayout = new QVBoxLayout;
    updateLayout->addWidget(new QLabel(configurationModel->headerData(3, Qt::Horizontal).toString()+":"));
    updateLayout->addWidget(webUpdateUrl);
    updateLayout->addWidget(updateOnStart);
    updateLayout->addLayout(updateButtonLayout, 1);
    updateGroup->setLayout(updateLayout);

    /* PRAVÝ SPODNÍ GROUPBOX (ZÁLOHA) */
    QGroupBox* dumpGroup = new QGroupBox(tr("Záloha dat"));

    /* Layout pro tlačítka */
    QHBoxLayout* dumpButtonsLayout = new QHBoxLayout;
    dumpButtonsLayout->addWidget(createDumpButton);
    dumpButtonsLayout->addWidget(loadDumpButton);

    /* Celkový layout */
    QVBoxLayout* dumpLayout = new QVBoxLayout;
    dumpLayout->addWidget(dumpOnExit);
    dumpLayout->addLayout(dumpButtonsLayout, 1);
    dumpGroup->setLayout(dumpLayout);

    /* SPOJENÍ PRAVÝCH SPODNÍCH GROUPBOXŮ DO JEDNOHO */
    QVBoxLayout* bottomRightLayout = new QVBoxLayout;
    bottomRightLayout->addWidget(updateGroup);
    bottomRightLayout->addWidget(dumpGroup);

    /* CELKOVÝ 2X2 LAYOUT */
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(statisticsGroup, 0, 0);
    layout->addWidget(hottestGroup, 1, 0);
    layout->addWidget(settingsGroup, 0, 1);
    layout->addLayout(bottomRightLayout, 1, 1);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 0);

    #ifndef ADMIN_VERSION
    beginDate->setDisabled(true);
    endDate->setDisabled(true);
    createUpdateButton->setDisabled(true);
    #endif

    setLayout(layout);

    /* Načtení dat */
    loadData();

    /* Aktualizace z internetu po startu (jen když je platná adresa) */
    if(updateOnStart->isChecked() && webUpdateUrl->hasAcceptableInput())
        QTimer::singleShot(0, this, SLOT(updateFromWebSilent()));
}

/* Vytvoření zálohy */
void SummaryTab::createDump() {
    QString filename = QFileDialog::getSaveFileName(this,
        tr("Uložit zálohu"), QString(), tr("XML soubory (*.xml)"));

    /* Uživatel nevybral žádný soubor */
    if(filename.isEmpty()) return;

    Dump dump;
    QString data = dump.createDump();

    /* Data jsou prázdná - něco se nezdařilo */
    if(data.isEmpty()) {
        QMessageBox::critical(this, tr("Chyba"), tr("Nepodařilo se vytvořit zálohu!"),
                             QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    /* Zapsání souboru (i když je prázdný) */
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Chyba"), tr("Nepodařilo se otevřít soubor!"),
                             QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    /* Chyba při zápisu */
    int filesize = file.write(data.toUtf8());
    if(filesize == -1) {
        QMessageBox::critical(this, tr("Chyba"), tr("Nepodařilo se zapsat zálohu do souboru!"),
                             QMessageBox::Ok, QMessageBox::Ok);
        file.close();
        return;
    }

    file.close();

    /* Zobrazení statistiky */
    QMessageBox::information(this, tr("Záloha dokončena"),
        tr("Velikost zálohy: <strong>%1 kB</strong><br /><br />"
        "Zálohovaných učitelů: <strong>%2</strong><br />"
        "Zálohovaných předmětů: <strong>%3</strong><br />"
        "Zálohovaných rozvrhů: <strong>%4</strong><br />"
        "Zálohovaných vyučovacích hodin: <strong>%5</strong><br />"
        "Zálohovaných změn: <strong>%6</strong><br />"
        "Zálohovaných absencí: <strong>%7</strong>")
        .arg(filesize/1024).arg(dump.deltaTeachers()).arg(dump.deltaClasses())
        .arg(dump.deltaTimetables()).arg(dump.deltaTimetableData())
        .arg(dump.deltaChanges()).arg(dump.deltaAbsences()),
        QMessageBox::Ok, QMessageBox::Ok);
}

/* Vytvoření zálohy */
void SummaryTab::createUpdate() {
    QString filename, note;
    CreateUpdateDialog dialog(filename, note, this);
    if(dialog.exec() != QDialog::Accepted) return;

    /* Vytvoření aktualizace */
    Dump dump;
    QString data = dump.createUpdate(note);

    /* Data jsou prázdná - něco se nezdařilo */
    if(data.isEmpty()) {
        QMessageBox::critical(this, tr("Chyba"), tr("Nepodařilo se vytvořit zálohu!"),
                             QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    /* Zapsání souboru (i když je prázdný) */
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Chyba"), tr("Nepodařilo se otevřít soubor!"),
                             QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    /* Chyba při zápisu */
    int filesize = file.write(data.toUtf8());
    if(filesize == -1) {
        QMessageBox::critical(this, tr("Chyba"), tr("Nepodařilo se zapsat aktualizaci do souboru!"),
                             QMessageBox::Ok, QMessageBox::Ok);
        file.close();
        return;
    }

    file.close();

    /* Pokud je popisek prázdný, změna textu pro statistiku */
    if(note.isEmpty()) note = tr("(prázdný)");

    /* Zobrazení statistiky */
    QMessageBox::information(this, tr("Aktualizace vytvořena"),
        tr("Popisek aktualizace:<br /><br /><em>%1</em><br /><br />"
        "Velikost souboru: <strong>%2 kB</strong><br /><br />"
        "Uložených učitelů: <strong>%3</strong><br />"
        "Uložených předmětů: <strong>%4</strong><br />"
        "Uložených rozvrhů: <strong>%5</strong><br />"
        "Uložených vyučovacích hodin: <strong>%6</strong><br />"
        "Uložených změn: <strong>%7</strong><br />")
        .arg(note).arg(filesize/1024).arg(dump.deltaTeachers())
        .arg(dump.deltaClasses()).arg(dump.deltaTimetables())
        .arg(dump.deltaTimetableData()).arg(dump.deltaChanges()),
        QMessageBox::Ok, QMessageBox::Ok);
}

/* Ověření URL v editovacím políčku */
void SummaryTab::validateUrlEdit() {
    QPalette p = webUpdateUrl->palette();

    if(!webUpdateUrl->hasAcceptableInput()) {
        p.setColor(QPalette::Base, QColor("#ffcccc"));
        updateFromWebAction->setDisabled(true);
    } else {
        p.setColor(QPalette::Base, QColor("#ffffff"));
        updateFromWebAction->setDisabled(false);
    }

    webUpdateUrl->setPalette(p);
}

/* (Znovu)načtení dat do políček */
void SummaryTab::loadData() {
    /* Datum začátku a konce pololetí */
    beginDate->setDate(configurationModel->index(0, 0).data(Qt::EditRole).toDate());
    endDate->setDate(configurationModel->index(0, 1).data(Qt::EditRole).toDate());

    /* Aktivní rozvrh */
    activeTimetable->setCurrentIndex(configurationModel->index(0, 2).data(Qt::EditRole).toInt());

    /* Zda aktualizovat po startu, zálohovat při ukončení */
    updateOnStart->setChecked(configurationModel->index(0, 5).data(Qt::EditRole).toBool());
    dumpOnExit->setChecked(configurationModel->index(0, 6).data(Qt::EditRole).toBool());

    /* Datum poslední aktualizace v nadpisku */
    QString lastUpdate = configurationModel->index(0, 4).data().toString();
    updateGroup->setTitle(tr("Aktualizace") + (!lastUpdate.isEmpty() ? tr(" (naposledy %1)").arg(lastUpdate) : ""));

    /* URL pro aktualizace po internetu */
    webUpdateUrl->setText(configurationModel->index(0, 3).data().toString());
}

/* Nastavení začátku pololetí */
void SummaryTab::setBeginDate() {
    configurationModel->setData(configurationModel->index(0, 0), beginDate->date(), Qt::EditRole);
}

/* Nastavení konce pololetí */
void SummaryTab::setEndDate() {
    configurationModel->setData(configurationModel->index(0, 1), endDate->date(), Qt::EditRole);
}

/* Nastavení aktuálního rozvrhu */
void SummaryTab::setActiveTimetable() {
    configurationModel->setData(configurationModel->index(0, 2), activeTimetable->currentIndex(), Qt::EditRole);

    /* Nastavení rozvrhu jako aktuálního */
    timetableTab->getTimetableModel()->setActualTimetable(activeTimetable->currentIndex());

    /* Načtení vybraného rozvrhu v tabu */
    timetableTab->loadTimetable(activeTimetable->currentIndex());
}

/* Nastavení URL pro aktualizace z internetu */
void SummaryTab::setWebUpdateUrl() {
    /* Jen pokud je URL správně formátovaná */
    if(!webUpdateUrl->hasAcceptableInput()) return;

    configurationModel->setData(configurationModel->index(0, 3), webUpdateUrl->text(), Qt::EditRole);
}

/* Nastavení aktualizace po startu */
void SummaryTab::setUpdateOnStart() {
    configurationModel->setData(configurationModel->index(0, 5), updateOnStart->checkState(), Qt::CheckStateRole);
}

/* Nastavení zálohování při ukončení */
void SummaryTab::setDumpOnExit() {
    configurationModel->setData(configurationModel->index(0, 6), dumpOnExit->checkState(), Qt::CheckStateRole);
}

/* Aktualizace z internetu */
void SummaryTab::updateFromWeb() {
    UpdateDialog dialog(UpdateDialog::DO_UPDATE|UpdateDialog::FROM_WEB,
                        configurationModel->index(0, 4).data(Qt::EditRole).toDate(),
                        webUpdateUrl->text());

    /* Pokud úspěšně proběhla aktualizace, vyslání signálu o změně dat v DB */
    if(dialog.exec() == QDialog::Accepted) emit updated();
}

/* Aktualizace z internetu (tichá) */
void SummaryTab::updateFromWebSilent() {
    UpdateDialog dialog(UpdateDialog::DO_UPDATE|UpdateDialog::FROM_WEB|UpdateDialog::CHECK_DATE_SILENT,
                        configurationModel->index(0, 4).data(Qt::EditRole).toDate(),
                        webUpdateUrl->text());

    /* Pokud úspěšně proběhla aktualizace, vyslání signálu o změně dat v DB */
    if(dialog.exec() == QDialog::Accepted) emit updated();
}

/* Aktualizace ze souboru */
void SummaryTab::updateFromFile() {
    UpdateDialog dialog(UpdateDialog::DO_UPDATE|UpdateDialog::FROM_FILE,
                        configurationModel->index(0, 4).data(Qt::EditRole).toDate());

    /* Pokud úspěšně proběhla aktualizace, vyslání signálu o změně dat v DB */
    if(dialog.exec() == QDialog::Accepted) emit updated();
}

/* Načtení zálohy */
void SummaryTab::loadDump() {
    UpdateDialog dialog(UpdateDialog::LOAD_DUMP|UpdateDialog::FROM_FILE,
                        configurationModel->index(0, 4).data(Qt::EditRole).toDate());

    /* Pokud úspěšně proběhla aktualizace, vyslání signálu o změně dat v DB */
    if(dialog.exec() == QDialog::Accepted) emit updated();
}

}
