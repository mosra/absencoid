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
#include <QMenuBar>
#include <QApplication>
#include <QDesktopServices>
#include <QDir>

#include "configure.h"
#include "SummaryTab.h"
#include "TeachersModel.h"
#include "TeachersTab.h"
#include "ClassesModel.h"
#include "ClassesTab.h"
#include "TimetableModel.h"
#include "TimetableTab.h"
#include "ChangesModel.h"
#include "ChangesTab.h"
#include "AbsencesModel.h"
#include "AbsencesTab.h"
#include "AboutDialog.h"
#include "Style.h"

namespace Absencoid {

/* Konstruktor */
MainWindow::MainWindow(): tabWidget(new QTabWidget(this)) {
    setWindowIcon(QIcon(":/icon.png"));

    new Style(":/icons.png", this);

    #ifdef ADMIN_VERSION
    setWindowTitle(tr("Absencoid %1 [správce]").arg(APP_VERSION_LONG));
    #else
    setWindowTitle(tr("Absencoid %1 [uživatel]").arg(APP_VERSION_LONG));
    #endif

    /* Místo pro uložení aplikačních dat */
    QString location = QDesktopServices::storageLocation(QDesktopServices::DataLocation);

    /* Pokud je místo prázdné, ukládáme do domovského adresáře */
    if(location.isEmpty())
        location = QDir::homePath() + "/" + QApplication::applicationName();

    /* Vytvoříme adresář, pokud ještě neexistuje */
    if(!QFile::exists(location)) {
        QDir dir;
        dir.mkpath(location);
    }

    /* Databáze */
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(location+"/absencoid.db");
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

    /* Menu */
    QMenuBar* menu = new QMenuBar();

    /* Menu Soubor */
    QMenu* fileMenu = menu->addMenu(tr("Soubor"));
    QAction* quitAction = fileMenu->addAction(Style::style()->icon(Style::ExitIcon), tr("Ukončit"));
    connect(quitAction, SIGNAL(triggered(bool)), this, SLOT(close()));

    /* Menu Nápověda */
    QMenu* helpMenu = menu->addMenu(tr("Nápověda"));
    QAction* helpAction = helpMenu->addAction(Style::style()->icon(Style::HelpIcon), tr("Nápověda"));
    helpAction->setDisabled(true);
    QAction* aboutAction = helpMenu->addAction(QIcon(":/icon.png"), tr("O programu"));
    helpMenu->addSeparator();
    QAction* aboutQtAction = helpMenu->addAction(QIcon(":/qt.png"), tr("O Qt"));
    connect(aboutAction, SIGNAL(triggered(bool)), this, SLOT(about()));
    connect(aboutQtAction, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));

    setMenuBar(menu);

    /* Taby */
    tabWidget->setTabPosition(QTabWidget::South);
    tabWidget->setUsesScrollButtons(false);

    /* Učitelé */
    TeachersTab* teachersTab = new TeachersTab(tabWidget);
    tabWidget->addTab(teachersTab, Style::style()->icon(Style::TeachersTab), tr("Učitelé"));

    /* Předměty */
    ClassesTab* classesTab = new ClassesTab(teachersTab->getTeachersModel(), tabWidget);
    tabWidget->addTab(classesTab, Style::style()->icon(Style::ClassesTab), tr("Předměty"));

    /* Rozvrh hodin */
    TimetableTab* timetableTab = new TimetableTab(classesTab->getClassesModel(), tabWidget);
    tabWidget->addTab(timetableTab, Style::style()->icon(Style::TimetableTab), tr("Rozvrhy hodin"));

    /* Změny */
    ChangesTab* changesTab =
        new ChangesTab(timetableTab->getTimetableModel(), classesTab->getClassesModel());
    tabWidget->addTab(changesTab, Style::style()->icon(Style::ChangesTab), tr("Změny"));

    /* Absence */
    AbsencesTab* absencesTab = new AbsencesTab(classesTab->getClassesModel(), timetableTab->getTimetableModel(), changesTab->getChangesModel());
    tabWidget->addTab(absencesTab, Style::style()->icon(Style::AbsencesTab), tr("Absence"));

    /* Souhrn - potřebuje data rozvrhu, ale absence potřebují jeho aktuální
        rozvrh, proto je zde */
    SummaryTab* summaryTab = new SummaryTab(teachersTab->getTeachersModel(), classesTab->getClassesModel(), timetableTab, changesTab->getChangesModel(), absencesTab->getAbsencesModel());
    tabWidget->insertTab(0, summaryTab, Style::style()->icon(Style::SummaryTab), tr("Souhrn"));
    tabWidget->setCurrentIndex(0);

    /* Propojení signálu o aktualizaci databáze s reload funkcemi modelů.
        Bacha, pořadí je důležité! ConfigurationModel potřebuje aktualizovaná
        data z TimetableTab */
    connect(summaryTab, SIGNAL(updated()), teachersTab->getTeachersModel(), SLOT(reload()));
    connect(summaryTab, SIGNAL(updated()), classesTab->getClassesModel(), SLOT(reload()));
    connect(summaryTab, SIGNAL(updated()), timetableTab->getTimetableModel(), SLOT(reload()));
    connect(summaryTab, SIGNAL(updated()), changesTab->getChangesModel(), SLOT(reload()));
    connect(summaryTab, SIGNAL(updated()), absencesTab->getAbsencesModel(), SLOT(reload()));
    connect(summaryTab, SIGNAL(updated()), summaryTab, SLOT(reload()));

    setCentralWidget(tabWidget);

    /* Stavový řádek */
    setStatusBar(new QStatusBar(this));
    statusBar()->addWidget(new QLabel(tr("Absencoid k vašim službám.")), 1);
    statusBar()->addWidget(new QLabel(tr("%1").arg(APP_VERSION_LONG)));

    /* Změna velikosti na nejmenší možnou v poměru 16:10 */
    resize(480*8/5, 480);
}

/* Dialog O programu */
void MainWindow::about() {
    AboutDialog* about = new AboutDialog(this);
    about->exec();
}

}
