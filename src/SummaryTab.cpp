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

namespace Absencoid {

/* Konstruktor */
SummaryTab::SummaryTab(QWidget* parent): QWidget(parent) {
    /* Políčka pro editaci data */
    QDateEdit* begin = new QDateEdit(QDate(2009,9,7));
    QDateEdit* end   = new QDateEdit(QDate(2010,1,29));
    begin->setDisplayFormat("ddd dd.MM.yyyy");
    end  ->setDisplayFormat("ddd dd.MM.yyyy");

    /* Kalendářový popup pro editaci data */
    QCalendarWidget* beginPopup = new QCalendarWidget;
    QCalendarWidget* endPopup = new QCalendarWidget;
    beginPopup->setFirstDayOfWeek(Qt::Monday);
    endPopup->setFirstDayOfWeek(Qt::Monday);
    begin->setCalendarPopup(true);
    end  ->setCalendarPopup(true);
    begin->setCalendarWidget(beginPopup);
    end  ->setCalendarWidget(endPopup);

    /* Tlačítko pro aktualizaci */
    QPushButton* updateButton = new QPushButton(tr("Aktualizovat"));
    QMenu* updateButtonMenu = new QMenu(updateButton);
    updateButtonMenu->addAction("Z internetu");
    updateButtonMenu->addAction("Ze souboru");
    updateButton->setMenu(updateButtonMenu);

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
    settingsLayout->addWidget(new QLabel(tr("Začátek pololetí:")), 0, 0);
    settingsLayout->addWidget(begin, 0, 1);
    settingsLayout->addWidget(new QLabel(tr("Konec pololetí:")), 1, 0);
    settingsLayout->addWidget(end, 1, 1);
    settingsLayout->addWidget(new QLabel(tr("Použitý rozvrh:")), 2, 0);
    settingsLayout->addWidget(new QComboBox, 2, 1);
    settingsLayout->addWidget(new QWidget, 3, 0);
    settingsLayout->setRowStretch(3, 1);
    settingsGroup->setLayout(settingsLayout);

    /* PRAVÝ SPODNÍ GROUPBOX (AKTUALIZACE) */
    QGroupBox* updateGroup = new QGroupBox(tr("Aktualizace (naposledy st 23.09.2009)"));

    /* Layout pro checkbox */
    QHBoxLayout* updateAutoLayout = new QHBoxLayout;
    updateAutoLayout->addWidget(new QCheckBox, 0);
    updateAutoLayout->addWidget(new QLabel(tr("Zjišťovat aktualizace při startu")), 1);

    /* Layout pro tlačítka */
    QHBoxLayout* updateButtonLayout = new QHBoxLayout;
    updateButtonLayout->addWidget(updateButton);
    updateButtonLayout->addWidget(new QPushButton(tr("Vytvořit aktualizaci")));

    /* Celkový layout */
    /** @todo Check funkce pro webovou adresu */
    QVBoxLayout* updateLayout = new QVBoxLayout;
    updateLayout->addLayout(updateAutoLayout);
    updateLayout->addWidget(new QLineEdit("http://disk.jabbim.cz/mosra@jabbim.cz/absencoid-update.zip"));
    updateLayout->addLayout(updateButtonLayout, 1);
    updateGroup->setLayout(updateLayout);

    /* PRAVÝ SPODNÍ GROUPBOX (ZÁLOHA) */
    QGroupBox* dumpGroup = new QGroupBox(tr("Záloha dat"));

    /* Layout pro checkbox */
    QHBoxLayout* dumpAutoLayout = new QHBoxLayout;
    dumpAutoLayout->addWidget(new QCheckBox, 0);
    dumpAutoLayout->addWidget(new QLabel(tr("Automaticky zálohovat při ukončení programu")), 1);

    /* Layout pro tlačítka */
    QHBoxLayout* dumpButtonsLayout = new QHBoxLayout;
    dumpButtonsLayout->addWidget(new QPushButton(tr("Zálohovat")));
    dumpButtonsLayout->addWidget(new QPushButton(tr("Obnovit ze zálohy")));

    /* Celkový layout */
    QVBoxLayout* dumpLayout = new QVBoxLayout;
    dumpLayout->addLayout(dumpAutoLayout);
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
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);

    setLayout(layout);
}

}
