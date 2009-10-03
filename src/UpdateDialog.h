#ifndef ABSENCOID_UPDATEDIALOG_H
#define ABSENCOID_UPDATEDIALOG_H

#include <QDialog>
#include <QNetworkReply>
#include <QDate>

class QDialogButtonBox;
class QListWidgetItem;
class QAbstractButton;
class QListWidget;
class QProgressBar;

namespace Absencoid {

class Update;

class UpdateDialog: public QDialog {
    Q_OBJECT

    public:
        /** @brief Flags */
        enum Flags {
            DO_UPDATE = 1,      /** @brief Provést aktualizaci */
            LOAD_DUMP = 2,      /** @brief Načíst zálohu */
            FROM_FILE = 4,      /** @brief Aktualizovat ze soubor */
            FROM_WEB = 8,       /** @brief Aktualizovat z internetu */
            CHECK_DATE = 16     /** @brief Kontrolovat datum aktualizace, pokud není novější, neaktualizovat */
        };

        /**
         * @brief Konstruktor pro aktualizaci ze souboru
         *
         * @param   flags       Viz Flags. Je třeba uvést zdroj a co chceme dělat
         * @param   lastUpdate  Datum poslední aktualizace
         * @param   url         Adresa, ze které se bude stahovat
         * @param   parent      Rodičovský widget
         */
        UpdateDialog(int _flags, const QDate& _lastUpdate, const QString& url = QString(), QWidget* parent = 0);

        /**
         * @brief Destruktor
         *
         * Smaže aktualizační vlákno, pokud je hotové
         */
        ~UpdateDialog();

    private slots:
        /** @brief Začne stahovat nebo zobrazí dialog pro výběr souboru */
        void start();

        /** @brief Postup stahování */
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

        /** @brief Chyba při stahování */
        void downloadError(QNetworkReply::NetworkError code);

        /** @brief Stahování dokončeno */
        void downloadFinished();

        /** @brief Zahájení aktualizace */
        void updateStart(const QString& data, bool doUpdate);

        /** @brief Stav aktualizace */
        void updateStatus(const QString& what, int count);

        /** @brief Nastala chyba při aktualizaci */
        void updateError(const QString& what);

        /** @brief Aktualizace dokončena */
        void updateFinished();

        /** @brief Zrušení aktualizace */
        virtual void reject();

    private:
        /** @brief Typ informační zprávy v logu */
        enum LogMessageType {
            ACTION = 1,             /** @brief Započetí nějaké akce (stahování, aktualizace) */
            INFORMATION = 2,        /** @brief Informace */
            ERROR = 3,              /** @brief Chyba */
            DONE = 4                /** @brief Dokončení akce (stahování, aktualizace) */
        };

        /** @brief Privátní flags */
        enum PrivateFlags {
            ERROR_OCCURED = 128     /** @brief Došlo k chybě */
        };

        /**
         * @brief Přidání zprávy do logu
         *
         * @param   text    Text zprávy
         * @param   type    Typ zprávy
         * @return  Ukazatel na položku v logu
         */
        QListWidgetItem* addLogMessage(const QString& text, LogMessageType type);

        int flags;                  /** @brief Flags */
        QDate lastUpdate;           /** @brief Datum poslední aktualizace */
        QString file;               /** @brief Název souboru nebo URL */
        QProgressBar* progressBar;  /** @brief Progressbar */
        QListWidget* log;           /** @brief Seznam sloužící jako log */
        QListWidgetItem* progressItem;  /** @brief Položka s postupem stahování */
        QNetworkAccessManager* manager; /** @brief Přístup k síti */
        QNetworkReply* reply;       /** @brief Odpověď od serveru */
        Update* thread;             /** @brief Zahájení aktualizace */
        QDialogButtonBox* buttons;  /** @brief Tlačítka */
};

}

#endif
