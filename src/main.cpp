#include <QApplication>
#include <QTextCodec>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "MainWindow.h"
#include "Style.h"

/** @todo Dokumentace */
/** @todo Není zálohován / obnovován stav, zda je daná absence školní! */
/** @todo Hangups při navazujících rozvrzích */

using namespace Absencoid;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    /* Název organizace a aplikce (pro DataLocation) */
    app.setOrganizationName("Mosra");
    app.setApplicationName("Absencoid");

    /* Místo pro uložení aplikačních dat */
    QString location = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    if(location.isEmpty())
        location = QDir::homePath() + "/" + QApplication::applicationName();
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
        qDebug() << QApplication::tr("Inicializace nové prázdné databáze.");

        QSqlQuery query;
        QFile file(":/absencoid.sql");
        file.open(QIODevice::ReadOnly | QIODevice::Text);

        /* Načtení vstupního SQL a rozdělění podle ; na jednotlivé dotazy (QtSql
            neumí provést multidotaz) */
        QStringList queries = QString::fromUtf8(file.readAll().data()).split(";\n", QString::SkipEmptyParts);

        /* Provádění jednotlivých dotazů */
        for(int i = 0; i != queries.count(); ++i) {
            if(!query.exec(queries[i])) {
                qDebug() << QApplication::tr("Nepodařilo se inicializovat novou prázdnou databázi!")
                         << query.lastError() << query.lastQuery();
                break;
            }
        }
    }

    /* Styl pro načítání ikonek */
    new Style(":/icons.png", qApp);

    MainWindow w;
    w.show();

    return app.exec();
}
