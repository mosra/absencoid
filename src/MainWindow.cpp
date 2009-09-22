#include "MainWindow.h"

#include <QTabWidget>
#include <QPushButton>
#include <QBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QStyle>
#include <QStatusBar>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDebug>

#include "configure.h"
#include "TeachersModel.h"
#include "TeachersTab.h"
#include "ClassesTab.h"
#include "TimetableTab.h"
#include "ChangedLessonsTab.h"

namespace Absencoid {

/* Konstruktor */
MainWindow::MainWindow(): tabWidget(new QTabWidget(this)) {
    setWindowTitle(tr("Absencoid"));

    /* Databáze */
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("absencoid.db");
    db.open();

    /* Pokud databáze nemá žádné tabulky, jejich inicializace z přidaného SQL souboru */
    if(db.tables().empty()) {
        qDebug() << tr("Inicializace nové prázdné databáze.");

        QSqlQuery query;
        QFile file(":/absencoid.sql");
        file.open(QIODevice::ReadOnly | QIODevice::Text);

        /* Načtení vstupního SQL a rozdělění podle ; na jednotlivé dotazy (QtSql
            neumí provést multidotaz) */
        QStringList queries = QString::fromUtf8(file.readAll().data()).split(";\n", QString::SkipEmptyParts);

        /* Provádění jednotlivých dotazů */
        for(int i = 0; i != queries.count(); ++i) {
            if(!query.exec(queries[i])) {
                qDebug() << tr("Nepodařilo se inicializovat novou prázdnou databázi!")
                         << query.lastError() << query.lastQuery();
                break;
            }
        }
    }

    /* ##################### Taby ########################################### */

    tabWidget->setTabPosition(QTabWidget::West);
    tabWidget->setUsesScrollButtons(false);

    /* -------------------- Tab "Souhrn" ------------------------------------ */
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
    summary->setDisabled(true);
    tabWidget->addTab(summary, tr("Souhrn"));

    /* Učitelé */
    TeachersTab* teachersTab = new TeachersTab(tabWidget);
    tabWidget->addTab(teachersTab, tr("Učitelé"));

    /* Předměty */
    ClassesTab* classesTab = new ClassesTab(teachersTab->getTeachersModel(), tabWidget);
    tabWidget->addTab(classesTab, tr("Předměty"));

    /* Rozvrh hodin */
    TimetableTab* timetableTab = new TimetableTab(classesTab->getClassesModel(), tabWidget);
    tabWidget->addTab(timetableTab, tr("Rozvrhy hodin"));

    /* Změny */
    ChangedLessonsTab* changedLessonsTab =
        new ChangedLessonsTab(timetableTab->getTimetableModel(), classesTab->getClassesModel(), this);
    tabWidget->addTab(changedLessonsTab, tr("Změny"));

    /* Absence */
    tabWidget->addTab(new QLabel(tr("Zde bude seznam absencí")), tr("Absence"));

    /* Stavový řádek */
    setStatusBar(new QStatusBar(this));
    statusBar()->addWidget(new QLabel(tr("Absencoid k vašim službám.")), 1);
    statusBar()->addWidget(new QLabel(SVN_VERSION));

    /* Změna velikosti na nejmenší výšku v poměru stran 4:3 */
    resize(sizeHint().height()*8/5, sizeHint().height());
}

}
