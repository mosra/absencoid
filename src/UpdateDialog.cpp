#include "UpdateDialog.h"

#include <QLabel>
#include <QProgressBar>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QBoxLayout>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDate>
#include <QFont>
#include <QTimer>

#include "Update.h"

namespace Absencoid {

/* Aktualizace ze souboru */
UpdateDialog::UpdateDialog(int _flags, const QDate& _lastUpdate, const QString& _file, QWidget* parent): QDialog(parent), flags(_flags), lastUpdate(_lastUpdate), file(_file), progressItem(0), manager(0), reply(0), thread(0) {
    if(flags & DO_UPDATE && flags & FROM_WEB)
        setWindowTitle(tr("Aktualizace z internetu"));
    else if(flags & DO_UPDATE && flags &FROM_FILE)
        setWindowTitle(tr("Aktualizace ze souboru"));
    else
        setWindowTitle(tr("Obnovení zálohy"));

    /* Progressbar */
    progressBar = new QProgressBar;
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);

    /* Log s první položkou */
    log = new QListWidget;

    buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setDisabled(true);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

    /* Celkový layout */
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Průběh instalace:")));
    layout->addWidget(progressBar);
    layout->addWidget(log);
    layout->addWidget(buttons);

    setLayout(layout);

    setFixedSize(480, 320);

    /* Započetí stahování / zobrazení dialogu pro výběr souboru hned po zobrazení */
    QTimer::singleShot(0, this, SLOT(start()));
}

/* Započetí stahování / zobrazení dialogu pro výběr souboru */
void UpdateDialog::start() {

    /* Aktualizace z internetu */
    if(flags & FROM_WEB && flags & DO_UPDATE) {
        addLogMessage(tr("Stahování aktualizačního souboru z %1").arg(file), ACTION);

        /* Dotaz na zadanou URL */
        manager = new QNetworkAccessManager(this);
        reply = manager->get(QNetworkRequest(QUrl(file)));

        /* Propojení */
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(downloadError(QNetworkReply::NetworkError)));
        connect(reply, SIGNAL(finished()), this, SLOT(downloadFinished()));
        connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));

    /* Aktualizace / načtení zálohy ze souboru */
    } else if(flags & FROM_FILE) {
        /* Nebyl předán žádný název souboru, otevření dialogu pro výběr */
        if(file.isEmpty())
            file = QFileDialog::getOpenFileName(this, tr("Vyberte soubor"), QString(), tr("XML soubory (*.xml)"));

        /* Uživatel nevybral žádný soubor, konec */
        if(file.isEmpty()) {
            reject();
            return;
        }

        /* Načtení souboru a zahájení instalace */
        QFile _file(file);
        _file.open(QIODevice::ReadOnly | QIODevice::Text);
        updateStart(QString::fromUtf8(_file.readAll()), flags & DO_UPDATE ? true : false);
    }
}

/* Destruktor */
UpdateDialog::~UpdateDialog() {
    if(thread && !thread->isRunning()) delete thread;
}

/* Průběh stahování */
void UpdateDialog::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    /* Inicializace položky logu */
    if(progressItem == 0) progressItem = addLogMessage("", INFORMATION);

    /* Známá velikost souboru */
    if(bytesTotal > 0) {
        progressItem->setText(tr("  Stahování: %1 z %2 kB").arg(bytesReceived/1024).arg(bytesTotal/1024));
        progressBar->setValue(bytesReceived*50/bytesTotal);

    /* Neznámá velikost souboru */
    } else if(bytesTotal == -1)
        progressItem->setText(tr("  Stahování: %1 kB (neznámá velikost souboru)").arg(bytesReceived/1024));
}

/* Chyba při stahování */
void UpdateDialog::downloadError(QNetworkReply::NetworkError code) {
    QString str;
    switch(code) {
        case 3:
            str = tr("Chyba při stahování: server nenalezen!");
            break;
        case 4:
            str = tr("Chyba při stahování: čas vypršel!");
            break;
        case 201:
            str = tr("Chyba při stahování: přístup odmítnut!");
            break;
        case 203:
            str = tr("Chyba při stahování: soubor nenalezen!");
            break;
        case 204:
            str = tr("Chyba při stahování: požadováno ověření uživatele!");
            break;
        default:
            str = tr("Chyba při stahování souboru!");
    }
    flags |= ERROR_OCCURED;
    addLogMessage(str, ERROR);
}

/* Stahování dokončeno */
void UpdateDialog::downloadFinished() {
    int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    /* Přesměrování */
    if(code == 302) {
        QUrl url(file);
        QUrl redirect(reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl());

        /* Vypořádáme se i s tím, když je adresa relativní */
        url = url.resolved(redirect);

        progressItem->setText(tr("  Přesměrováno na %1").arg(url.toString()));

        /* Nový požadavek (odpojíme starý a připojíme na nový) */
        reply->disconnect();
        reply->close();
        reply->deleteLater();
        reply = manager->get(QNetworkRequest(url));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(downloadError(QNetworkReply::NetworkError)));
        connect(reply, SIGNAL(finished()), this, SLOT(downloadFinished()));
        connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));

        /* Uvolníme položku logu pro stahování, aby se další stahování zapisovalo do nové */
        progressItem = 0;

    /* Normální chování */
    } else if(code == 200 && !(flags & ERROR_OCCURED)) {
        reply->close();
        addLogMessage(tr("Stahování bylo dokončeno"), DONE);

        /* Zahájení aktualizace */
        updateStart(QString::fromUtf8(reply->readAll()), true);
    }
}

/* Zahájení aktualizace */
void UpdateDialog::updateStart(const QString& data, bool doUpdate) {
    /* Pokud je aktivní vlákno */
    if(thread) {
        if(thread->isFinished()) delete thread;
        else return;
    }

    /* Zašednutí stornovacího tlačítka */
    buttons->button(QDialogButtonBox::Cancel)->setDisabled(true);

    /* Nové vlákno pro aktualizaci */
    thread = new Update(data, doUpdate);
    connect(thread, SIGNAL(updated(QString,int)), this, SLOT(updateStatus(QString,int)));
    connect(thread, SIGNAL(error(QString)), this, SLOT(updateError(QString)));
    connect(thread, SIGNAL(finished()), this, SLOT(updateFinished()));

    QDate date = QDate::fromString(thread->date(), Qt::ISODate);

    /* Provedení aktualizace */
    if(doUpdate) {
        /* Pokud je datum stejné nebo starší než datum poslední aktualizace,
            nemáme co aktualizovat */
        if(flags & CHECK_DATE && date <= lastUpdate) {
            if(!flags & CHECK_DATE_SILENT) QMessageBox::information(this, tr("Žádné aktualizace k dispozici"),
                tr("Nebyla nalezena žádná aktualizace novější než z %1.").arg(lastUpdate.toString("ddd dd.MM.yyyy")));
            return reject();
        }

        QString text = tr("<strong>Datum aktualizačního souboru:</strong> %1").arg(date.toString("ddd dd.MM.yyyy"));

        /* Poznámka k aktualizaci */
        QString note = thread->updateNote();
        if(!note.isEmpty()) text += tr("<br /><strong>Popisek aktualizace:</strong><br /> %2").arg(note);

        /* Potvrzení aktualizace */
        if(QMessageBox::information(this, tr("Instalovat aktualizaci?"), text, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) != QMessageBox::Yes)
            return reject();

    /* Obnovení zálohy */
    } else {
        if(QMessageBox::information(this, tr("Obnovit ze zálohy?"), tr("<strong>Datum zálohy:</strong> %1").arg(date.toString("ddd dd.MM.yyyy")), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) != QMessageBox::Yes)
            return reject();
    }

    addLogMessage(tr("Zahájena instalace"), ACTION);

    /* Spustíme vlákno */
    thread->start();
}

/* Nastavení statusu aktualizace */
void UpdateDialog::updateStatus(const QString& what, int count) {
    QString text;
    if(count == 0) text = what;
    if(count < 0) text = tr("%1 (odebráno %2 položek)").arg(what).arg(-count);
    if(count > 0) text = tr("%1 (přidáno %2 položek)").arg(what).arg(count);

    addLogMessage(text, INFORMATION);

    if(flags & DO_UPDATE && flags & FROM_WEB)
        progressBar->setValue(progressBar->value()+8);
    else if(flags & DO_UPDATE && flags & FROM_FILE)
        progressBar->setValue(progressBar->value()+16);
    else
        progressBar->setValue(progressBar->value()+14);
}

/* Chybové hlášení */
void UpdateDialog::updateError(const QString& what) {
    addLogMessage(what, ERROR);

    flags |= ERROR_OCCURED;

    /* Aktivace tlačítka Storno */
    buttons->button(QDialogButtonBox::Cancel)->setDisabled(false);
}

/* Aktualizace dokončena */
void UpdateDialog::updateFinished() {
    /* Pokud došlo k chybě, žádné orgie */
    if(flags & ERROR_OCCURED) return;

    addLogMessage(tr("Instalace dokončena"), DONE);

    progressBar->setValue(100);

    /* Aktivace tlačítka OK */
    buttons->button(QDialogButtonBox::Ok)->setDisabled(false);
}

/* Zrušení dialogu */
void UpdateDialog::reject() {
    /* Pokud ještě jede aktualizace, nebudeme ji ukončovat */
    if(thread && thread->isRunning()) return;

    if(reply != 0) {
        reply->abort();
        reply->deleteLater();
    }

    done(QDialog::Rejected);
}

/* Přidání zprávy do logu */
QListWidgetItem* UpdateDialog::addLogMessage(const QString& text, UpdateDialog::LogMessageType type) {
    QListWidgetItem* item = new QListWidgetItem(type == INFORMATION ? "  " + text : text);

    /* Akce */
    if(type == ACTION) {
        QFont font;
        font.setBold(true);
        item->setFont(font);

    /* Chyba */
    } else if(type == ERROR) {
        item->setForeground(QBrush(QColor("#ff0000")));

    /* Dokončení akce */
    } else if(type == DONE) {
        item->setForeground(QBrush(QColor("#009900")));
    }

    log->addItem(item);
    log->scrollToBottom();
    return item;
}

}
