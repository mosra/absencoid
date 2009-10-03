#ifndef ABSENCOID_UPDATE_H
#define ABSENCOID_UPDATE_H

#include <exception>
#include <QThread>
#include <QDomDocument>
#include <QSqlQuery>

namespace Absencoid {

using std::exception;

class Update: public QThread {
    Q_OBJECT

    public:
        /**
         * @brief Konstruktor
         *
         * @param   data        Data XML souboru
         * @param   doUpdate    Zda provést aktualizaci nebo obnovu zálohy
         */
        Update(const QString& data, bool doUpdate, QObject* parent = 0);

        /**
         * @brief Datum souboru
         */
        QString date();

        /**
         * @brief Popisek aktualizace
         */
        QString updateNote();

        /**
         * @brief Načtení aktualizace / zálohy
         * @todo Ověřit, jestli se emitujou signály started(), finished()...
         */
        virtual void run();

    signals:
        /**
         * @brief Nastala chyba
         *
         * @param   error   Popis chyby
         */
        void error(QString error);

        /**
         * @brief SIgnál o stavu
         *
         * @param   what    Co bylo právě hotovo
         * @param   count   Počet ovlivněných záznamů (záporný - některé byly
         *                  smazané, kladný - některé byly přidané)
         */
        void updated(QString what, int count);

    private:
        /**
         * @brief Třída pro zpracování SQL vyjímek
         */
        class SqlException: public exception {
            public:
                /**
                 * @brief Konstruktor
                 *
                 * @param   _error      Popis chyby
                 * @param   _query      Ukazatel na dotaz, který chybu vyvolal
                 */
                SqlException(const QString& __error, const QSqlQuery& __query):
                    _query(__query), _error(__error) {}

                /** @brief Destruktor */
                ~SqlException() throw() {}

                /** @brief Databázový dotaz */
                inline const QSqlQuery& query() const { return _query; }

                /** @brief Popis chyby */
                inline const QString& error() const { return _error; }

                /** @brief Popis chyby */
                virtual const char* what() const throw() { return _error.toUtf8(); }

            private:
                QSqlQuery _query;    /** @brief Databázový dotaz */
                QString _error;      /** @brief Chyba */
        };

        QDomDocument doc;       /** @brief Načtený XML soubor */
        bool doUpdate;          /** @brief Zda provádět aktualizaci nebo obnovu zálohy */
};

}

#endif
