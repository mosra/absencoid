#include "MainWindow.h"

#include <QWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QStyle>
#include <QStatusBar>
#include "configure.h"

namespace Absencoid {

/* Konstruktor */
MainWindow::MainWindow(): tabWidget(new QTabWidget(this)) {
    tabWidget->setTabPosition(QTabWidget::West);
    tabWidget->setUsesScrollButtons(false);

    /* Tab "Souhrn" */
    QWidget* summary = new QWidget(tabWidget);

    /* Vrchní řada tlačítek */
    QPushButton* updateDatabase =
        new QPushButton(style()->standardIcon(QStyle::SP_BrowserReload).pixmap(24,24), tr("Aktualizovat databázi"));
    updateDatabase->setIconSize(QSize(24, 24));
    QPushButton* addAbsence =
        new QPushButton(style()->standardIcon(QStyle::SP_FileIcon).pixmap(24,24), tr("Přidat absenci"));
    addAbsence->setIconSize(QSize(24, 24));
    QPushButton* modifyAbsence =
        new QPushButton(style()->standardIcon(QStyle::SP_DialogApplyButton).pixmap(24,24), tr("Upravit absence"));
    modifyAbsence->setIconSize(QSize(24, 24));

    setCentralWidget(tabWidget);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(updateDatabase);
    buttonLayout->addWidget(addAbsence);
    buttonLayout->addWidget(modifyAbsence);

    /* Levá skupina labelů */
    QGroupBox* leftGroup = new QGroupBox(tr("Statistika"));

    QLabel* hoursNow = new QLabel("<strong>231</strong>");
    QLabel* absentNow = new QLabel("<strong>12</strong> (<strong>5.19%</strong>)");
    QLabel* hoursCancelled = new QLabel("<strong>-5</strong>");
    QLabel* hoursAdded = new QLabel("<strong>+2</strong>");
    QLabel* lastUpdate = new QLabel("<strong>03.09.2009</strong>");

    QGridLayout* leftGroupLayout = new QGridLayout;
    leftGroupLayout->addWidget(new QLabel(tr("Počet hodin dosud:")), 0, 0);
    leftGroupLayout->addWidget(hoursNow, 0, 1);
    leftGroupLayout->addWidget(new QLabel(tr("Hodin absence:")), 1, 0);
    leftGroupLayout->addWidget(absentNow, 1, 1);
    leftGroupLayout->addWidget(new QLabel(tr("Odpadnutých hodin:")), 2, 0);
    leftGroupLayout->addWidget(hoursCancelled, 2, 1);
    leftGroupLayout->addWidget(new QLabel(tr("Přidaných hodin:")), 3, 0);
    leftGroupLayout->addWidget(hoursAdded, 3, 1);
    leftGroupLayout->addWidget(new QLabel(tr("Poslední aktualizace databáze:")), 4, 0, Qt::AlignTop);
    leftGroupLayout->addWidget(lastUpdate, 4, 1, Qt::AlignTop);

    /* Roztahuje se jen levý sloupec a poslední řádek */
    leftGroupLayout->setColumnStretch(0, 1);
    leftGroupLayout->setRowStretch(4, 1);
    leftGroup->setLayout(leftGroupLayout);

    /* Pravá skupina labelů */
    QGroupBox* rightGroup = new QGroupBox(tr("Největší průsery"));
    QHBoxLayout* rightGroupLayout = new QHBoxLayout;
    rightGroupLayout->addWidget(new QLabel(tr("Zatím nemáš žádný průsery")), 0, Qt::AlignCenter);
    rightGroup->setLayout(rightGroupLayout);

    /* Layout pro skupiny */
    QHBoxLayout* groupLayout = new QHBoxLayout;
    groupLayout->addWidget(leftGroup, 1);
    groupLayout->addWidget(rightGroup, 1);

    /* Celkový layout */
    QVBoxLayout* summaryLayout = new QVBoxLayout(summary);
    summaryLayout->addLayout(buttonLayout);
    summaryLayout->addLayout(groupLayout);

    /* Přidání tabu "Souhrn" */
    summary->setLayout(summaryLayout);
    tabWidget->addTab(summary, tr("Souhrn"));

    /* Další "dummy" taby */
    tabWidget->addTab(new QLabel(tr("Zde bude seznam absencí")), tr("Absence"));
    tabWidget->addTab(new QLabel(tr("Zde bude rozvrh")), tr("Rozvrh"));

    /* Administrátorské taby */
    int teachers = tabWidget->addTab(new QLabel(), tr("Učitelé"));
    int cancelledHours = tabWidget->addTab(new QLabel(), tr("Suply"));
    int vacations = tabWidget->addTab(new QLabel(), tr("Volna"));
    tabWidget->setTabEnabled(teachers, false);
    tabWidget->setTabEnabled(cancelledHours, false);
    tabWidget->setTabEnabled(vacations, false);

    /* Nastavení */
    tabWidget->addTab(new QLabel(tr("Krapet toho nastavení")), tr("Nastavení"));

    /* Stavový řádek */
    setStatusBar(new QStatusBar(this));
    statusBar()->addWidget(new QLabel(tr("Absencoid k vašim službám.")), 1);
    statusBar()->addWidget(new QLabel(SVN_VERSION));

    /* Změna velikosti na nejmenší výšku v poměru stran 4:3 */
    resize(sizeHint().height()*8/5, sizeHint().height());
}

};