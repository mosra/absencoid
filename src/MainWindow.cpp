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
#include "SummaryTab.h"
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

    /* Taby */
    tabWidget->setTabPosition(QTabWidget::West);
    tabWidget->setUsesScrollButtons(false);
    setCentralWidget(tabWidget);

    /* Souhrn */
    SummaryTab* summaryTab = new SummaryTab(this);
    tabWidget->addTab(summaryTab, tr("Souhrn"));

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

    /* Změna velikosti na nejmenší výšku v poměru stran 16:10 */
    resize(sizeHint().height()*8/5, sizeHint().height());
}

}
