#include "Update.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

#include "Dump.h"
#include "TimetableModel.h"
#include "TeachersModel.h"
#include "ChangesModel.h"
#include "ClassesModel.h"

namespace Absencoid {

/* Konstruktor */
Update::Update(const QString& data, bool _doUpdate, QObject* parent):
QThread(parent), doUpdate(_doUpdate) {

    /* Načtení dat */
    QString errorMsg;
    int errorLine, errorColumn;
    if(!doc.setContent(data, &errorMsg, &errorLine, &errorColumn)) {
        QString err = tr("Nepodařilo se načíst XML soubor!");
        emit error(err);
        qDebug() << err << errorMsg << errorLine << errorColumn;
        return;
    }
}

QString Update::date() {
    QDomElement root = doc.documentElement();

    /* Buď <update> nebo <dump>, obě mají podelement <date>. */
    QDomElement something = root.firstChildElement();

    QDomElement date = something.firstChildElement("date");
    if(date.isNull()) {
        QString err = tr("Nepodařilo se najít datum vytvoření souboru!");
        emit error(err);
        qDebug() << err;
        return "";
    }

    return date.text();
}

/* Poznámka k aktualizaci */
QString Update::updateNote() {
    try {
        QDomElement root = doc.documentElement();
        QDomElement update = root.firstChildElement("update");
        if(update.isNull()) throw tr("Toto není aktualizační soubor!");

        QDomElement note = update.firstChildElement("note");
        if(note.isNull()) throw tr("Nepodařilo se najít popisek aktualizace!");

        return note.text();

    /* Ošetření výjimek */
    } catch (QString e) {
        emit error(e);
        qDebug() << e;
        return "";
    }
}

/* Načtení updatu */
void Update::run() {
    try {
        /* Započetí transakce */
        QSqlQuery query;
        if(!query.exec("BEGIN EXCLUSIVE TRANSACTION;"))
            throw SqlException(tr("Nepodařilo se začít aktualizaci!"), query);

        QDomElement r = doc.documentElement();
        QDomElement root = r.firstChildElement();

        /* Zjištění typu souboru */
        bool isUpdate;

        /* Aktualizační soubor */
        if(root.tagName() == "update") {
            isUpdate = true;

            /* Pokud chceme obnovovat zálohu z aktualizačního souboru, konec */
            if(!doUpdate) throw tr("Nelze obnovovat zálohu z aktualizačního souboru!");

            int version = root.attribute("version").toInt();

            /* Ověření verze */
            if(version != Dump::UPDATE_VERSION)
                throw tr("Nepodporovaná verze aktualizačního souboru %1 (podporovaná je %2)")
                    .arg(version).arg(Dump::UPDATE_VERSION);

        /* Zálohovací soubor */
        } else if(root.tagName() == "dump") {
            isUpdate = false;

            /* Pokud chceme aktualizovat ze zálohy, konec */
            if(doUpdate) throw tr("Nelze provádět aktualizaci ze souboru zálohy!");

            int version = root.attribute("version").toInt();

            /* Ověření verze */
            if(version != Dump::DUMP_VERSION)
                throw tr("Nepodporovaná verze souboru zálohy %1 (podporovaná: %2)")
                    .arg(version).arg(Dump::DUMP_VERSION);

        /* Něco jiného */
        } else throw tr("Neplatný soubor!");

        /* Každá sekce je prováděná v jednom bloku {}, aby se nehromadily dávno
            nepoužívané proměnné QDomElement */

        /* <configuration> */ {
        QDomElement configuration = root.firstChildElement("configuration");
        if(configuration.isNull()) throw tr("Nenalezena sekce s konfigurací!");

        /*   <beginDate> */
        QDomElement _beginDate = configuration.firstChildElement("beginDate");
        if(_beginDate.isNull()) throw tr("Nenalezeno datum začátku pololetí!");
        QString beginDate = _beginDate.text();

        /*   <endDate> */
        QDomElement _endDate = configuration.firstChildElement("endDate");
        if(_endDate.isNull()) throw tr("Nenalezeno datum konce pololetí!");
        QString endDate = _endDate.text();

        /*   <webUpdateUrl> */
        QDomElement _webUpdateUrl = configuration.firstChildElement("webUpdateUrl");
        if(_webUpdateUrl.isNull()) throw tr("Nenalezena adresa pro aktualizaci z internetu!");
        QString webUpdateUrl = _webUpdateUrl.text();

        query.prepare("UPDATE configuration SET "
            "beginDate = :beginDate, "
            "endDate = :endDate, "
            "webUpdateUrl = :webUpdateUrl;");
        query.bindValue(":beginDate", beginDate);
        query.bindValue(":endDate", endDate);
        query.bindValue(":webUpdateUrl", webUpdateUrl);
        if(!query.exec())
            throw SqlException(tr("Nepodařilo se aktualizovat konfiguraci!"), query);

        /* Elementy jen v zálohách */
        if(!isUpdate) {
            /* <lastUpdate> */
            QDomElement _lastUpdate = configuration.firstChildElement("lastUpdate");
            if(_lastUpdate.isNull()) throw tr("Nenalezeno datum poslední aktualizace!");
            QString lastUpdate = _lastUpdate.text();

            int flags = 0;

            /* <updateOnStart> */
            QDomElement _updateOnStart = configuration.firstChildElement("updateOnStart");
            if(_updateOnStart.isNull()) throw tr("Nenalezena volba, zda aktualizovat po startu!");
            if(!_updateOnStart.hasAttribute("value")) throw tr("Nenalezena hodnota volby, zda aktualizovat po startu!");
            flags |= _updateOnStart.attribute("value", "true") == "true" ? Dump::UPDATE_ON_START : 0;

            /* <dumpOnExit> */
            QDomElement _dumpOnExit = configuration.firstChildElement("dumpOnExit");
            if(_dumpOnExit.isNull()) throw tr("Nenalezena volba, zda zálohovat při ukončení!");
            if(!_dumpOnExit.hasAttribute("value")) throw tr("Nenalezena hodnota volby, zda zálohovat při ukončení!");
            flags |= _dumpOnExit.attribute("value", "false") == "true" ? Dump::DUMP_ON_EXIT : 0;

            query.prepare("UPDATE configuration SET "
                "lastUpdate = :lastUpdate, flags = :flags;");
            query.bindValue(":lastUpdate", lastUpdate);
            query.bindValue(":flags", flags);
            if(!query.exec())
                throw SqlException(tr("Nepodařilo se aktualizovat uživatelskou konfiguraci!"), query);
        }

        /* Signál o dokončení aktualizace konfigurace */
        emit updated(tr("Dokončena aktualizace konfigurace"), 0);

        /* <teachers> */ }{
        QDomElement teachers = root.firstChildElement("teachers");
        if(teachers.isNull()) throw tr("Nenalezena sekce s učiteli!");

        /* Smazání dosavadních učitelů, zapsání jejich počtu do proměnné (pro statistiku) */
        if(!query.exec("DELETE FROM teachers WHERE id > 0;"))
            throw SqlException(tr("Nelpodařilo se smazat staré učitele!"), query);
        int teachersCount = -query.numRowsAffected();

        /* Resetování AUTOINCREMENT */
        if(!query.exec("DELETE FROM sqlite_sequence WHERE name = \"teachers\";"))
            throw SqlException(tr("Nelpodařilo se resetovat tabulku učitelů!"), query);

        /* Předpřipravení dotazu pro vkládání nových učitelů */
        query.prepare("INSERT INTO TEACHERS (gradeId, id, name, flags) VALUES (1, :id, :name, :flags);");

        /*   <teacher> */
        QDomNodeList teacherNodes = teachers.elementsByTagName("teacher");
        for(int i = 0; i != teacherNodes.count(); ++i) {
            QDomElement teacher = teacherNodes.item(i).toElement();

            /* Ošetření chyb (tj. i té, když je teacher nulový element) */
            if(!teacher.hasAttribute("id")) throw tr("Nenalezeno ID učitele!");
            if(!teacher.hasAttribute("counts")) throw tr("Nelze zjistit, zda učitel zapisuje absence!");
            if(!teacher.hasAttribute("accepts")) throw tr("Nelze zjistit, zda učitel uznává školní akce!");

            /* Odseknutí znaku 't' z ID, převedení na int */
            /** @todo Nějaká kontrola */
            int id = teacher.attribute("id").mid(1).toInt();

            /* Flags */
            int flags = (teacher.attribute("counts") == "true" ? TeachersModel::COUNTS : 0) |
                        (teacher.attribute("accepts") == "true" ? TeachersModel::ACCEPTS : 0);

            /* Provedení dotazu */
            query.bindValue(":id", id);
            query.bindValue(":name", teacher.text());
            query.bindValue(":flags", flags);
            if(!query.exec()) throw SqlException(tr("Nepodařilo se přidat učitele!"), query);

            /* Přičtení přidaného učitele do statistiky */
            teachersCount++;
        }

        /* Emitujeme signál o dokončení importu učitelů */
        emit updated(tr("Dokončena aktualizace učitelů"), teachersCount);

        /* <classes> */ }{
        QDomElement classes = root.firstChildElement("classes");
        if(classes.isNull()) throw tr("Nenalezena sekce s předměty!");

        /* Smazání dosavadních předmětů, zapsání jejich počtu do proměnné (statistika) */
        if(!query.exec("DELETE FROM classes WHERE id > 0;"))
            throw SqlException(tr("Nepodařilo se smazat staré předměty!"), query);
        int classesCount = -query.numRowsAffected();

        /* Resetování AUTOINCREMENT */
        if(!query.exec("DELETE FROM sqlite_sequence WHERE name = \"classes\";"))
            throw SqlException(tr("Nepodařilo se resetovat tabulku předmětů!"), query);

        /* Předpřipravení dotazu pro vkládání nových předmětů */
        query.prepare("INSERT INTO classes (gradeId, id, name, teacherId) VALUES (1, :id, :name, :teacherId);");

        /*   <class> */
        QDomNodeList classesNode = classes.elementsByTagName("class");
        for(int i = 0; i != classesNode.count(); ++i) {
            QDomElement _class = classesNode.item(i).toElement();

            /* Ošetření chyb (tj. i té, kdy je _class nulový element) */
            if(!_class.hasAttribute("id")) throw tr("Nenalezeno ID předmětu!");
            if(!_class.hasAttribute("teacherId")) throw tr("Nenalezeno ID učitele!");

            /* Odseknutí znaku 'c' z ID, převedení na int */
            /** @todo Nějaká kontrola */
            int id = _class.attribute("id").mid(1).toInt();

            /* Odesknutí znaku 't' z ID učitele, převedení na int */
            int teacherId = _class.attribute("teacherId").mid(1).toInt();

            /* Provedení dotazu */
            query.bindValue(":id", id);
            query.bindValue(":name", _class.text());
            query.bindValue(":teacherId", teacherId);
            if(!query.exec()) throw SqlException(tr("Nelpodařilo se přidat předmět!"), query);

            /* Přičtení přidaného předmětu do statistiky */
            classesCount++;
        }

        /* Emitování signálu o dokončení importu předmětů */
        emit updated(tr("Dokončena aktualizace předmětů"), classesCount);

        /* <timetables> */ }{
        QDomElement timetables = root.firstChildElement("timetables");
        if(timetables.isNull()) throw tr("Nenalezena sekce s rozvrhy!");

        /* Nastavení ID aktuálního rozvrhu (ne pokud jenom aktualizujeme) */
        if(!isUpdate) {
            /* pokud parametr neexistuje (např. žádné rozvrhy nejsou), nebudeme nadávat */
            int actualTimetableId = timetables.attribute("activeId", "1").mid(1).toInt();

            query.prepare("UPDATE configuration SET activeTimetableId = :activeTimetableId;");
            query.bindValue(":activeTimetableId", actualTimetableId);
            if(!query.exec()) throw SqlException(tr("Nepodařilo se aktualizovat použitý rozvrh!"), query);
        }

        /* Smazání dosavadních rozvrhů, zapsání jejich počtu pro statistiku */
        if(!query.exec("DELETE FROM timetables WHERE id > 0;"))
            throw SqlException(tr("Nepodařilo se smazat staré rozvrhy!"), query);
        int timetablesCount = -query.numRowsAffected();
        int timetableDataCount = 0;

        /* Resetování AUTOINCREMENT */
        if(!query.exec("DELETE FROM sqlite_sequence WHERE name = \"timetables\";"))
            throw SqlException(tr("Nepodařilo se resetovat tabulku rozvrhů!"), query);

        /* Předpřipravení dotazu pro vkládání nových předmětů */
        query.prepare("INSERT INTO timetables (gradeId, id, description, validFrom, followedBy) "
                "VALUES (1, :id, :description, :validFrom, :followedBy);");

        /*   <timetable> */
        QDomNodeList timetableNodes = timetables.elementsByTagName("timetable");
        for(int i = 0; i != timetableNodes.count(); ++i) {
            QDomElement timetable = timetableNodes.item(i).toElement();

            /* Ošetření chyb */
            if(!timetable.hasAttribute("id")) throw tr("Nenalezeno ID rozvrhu!");
            if(!timetable.hasAttribute("next")) throw tr("Nenalezeno ID následujícího rozvrhu!");

            /* Atributy (odseknutí 't' a převod na int) */
            int id = timetable.attribute("id").mid(1).toInt();
            int followedBy = timetable.attribute("next").mid(1).toInt();

            /* <name> */
            QDomElement _name = timetable.firstChildElement("name");
            if(_name.isNull()) throw tr("Nenalezen popisek rozvrhu!");
            QString name = _name.text();

            /* <validFrom> */
            QDomElement _validFrom = timetable.firstChildElement("validFrom");
            if(_validFrom.isNull()) throw tr("Nenalezeno datum začátku platnosti rozvrhu!");
            QString validFrom = _validFrom.text();

            /* Přidání rozvrhu */
            query.bindValue(":id", id);
            query.bindValue(":description", name);
            query.bindValue(":validFrom", validFrom);
            query.bindValue(":followedBy", followedBy);
            if(!query.exec()) throw SqlException(tr("Nepodařilo se přidat rozvrh!"), query);

            /* Přičtení přidaného rozvrhu do statistiky rozvrhů */
            timetablesCount++;

            /* <lessons> */
            QDomElement lessons = timetable.firstChildElement("lessons");
            if(lessons.isNull()) throw tr("Nenalezeny předměty rozvrhu!");

            /* Smazání dat tohoto rozvrhu (pokud aktualizujeme, smažeme jen pevná data) */
            QSqlQuery lessonsQuery;
            lessonsQuery.prepare("DELETE FROM timetableData WHERE timetableId = :timetableId AND classId > :classId;");
            lessonsQuery.bindValue(":timetableId", id);

            /* Pokud děláme aktualizaci, mazáme jen zamknuté hodiny */
            if(isUpdate)    lessonsQuery.bindValue(":classId", TimetableModel::FIXED);
            else            lessonsQuery.bindValue(":classId", 0);

            if(!lessonsQuery.exec())
                throw SqlException(tr("Nepodařilo se smazat staré předměty z rozvrhu!"), lessonsQuery);
            timetableDataCount -= lessonsQuery.numRowsAffected();

            /* Připravení dotazu pro vkládání předmětů */
            lessonsQuery.prepare("INSERT INTO timetableData (gradeId, timetableId, dayHour, classId) "
                "VALUES (1, :timetableId, :dayHour, :classId);");

            /*   <lesson> */
            QDomNodeList lessonNodes = lessons.elementsByTagName("lesson");
            for(int i = 0; i != lessonNodes.count(); ++i) {
                QDomElement lesson = lessonNodes.item(i).toElement();

                /* Ověření atributů */
                if(!lesson.hasAttribute("classId")) throw tr("Nenalezen předmět rozvrhu!");
                if(!lesson.hasAttribute("fixed")) throw tr("Nenalezena volba, zda je předmět zamknutý");

                /* Odseknutí znaku 'c' z ID, převedení na int, přiORování hodnoty FIXED */
                int classId = lesson.attribute("classId").mid(1).toInt();
                classId |= lesson.attribute("fixed") == "true" ? TimetableModel::FIXED : 0;

                /* <day> */
                QDomElement day = lesson.firstChildElement("day");
                if(day.isNull()) throw tr("Nelze zjistit den předmětu!");

                /* <hour> */
                QDomElement hour = lesson.firstChildElement("hour");
                if(hour.isNull()) throw tr("Nelze zjistit hodinu předmětu!");

                int dayHour = TimetableModel::dayHour(day.text().toInt(), hour.text().toInt());

                lessonsQuery.bindValue(":timetableId", id);
                lessonsQuery.bindValue(":dayHour", dayHour);
                lessonsQuery.bindValue(":classId", classId);
                if(!lessonsQuery.exec())
                    throw SqlException(tr("Nepodařilo se přidat předmět do rozvrhu!"), lessonsQuery);

                /* Přičtení přidaného předmětu do statistiky předmětů */
                timetableDataCount++;
            }
        }

        /* Emitování signálu o dokončení importu rozvrhů */
        emit updated(tr("Dokončena aktualizace rozvrhů"), timetablesCount);

        /* Emitování signálu o dokončení importu předmětů rozvrhu */
        emit updated(tr("Dokončena aktualizace předmětů v rozvrzích"), timetableDataCount);

        /* <changes> */ }{
        QDomElement changes = root.firstChildElement("changes");
        if(changes.isNull()) throw tr("Nenalezena sekce se změnami!");

        /* Smazání dosavadních změn, zapsání jejich počtu pro statistiku */
        if(!query.exec("DELETE FROM changes WHERE id > 0;"))
            throw SqlException(tr("Nelze smazat staré změny!"), query);
        int changesCount = -query.numRowsAffected();

        /* Resetování AUTOINCREMENT */
        if(!query.exec("DELETE FROM sqlite_sequence WHERE name = \"changes\";"))
            throw SqlException(tr("Nelze resetovat tabulku změn!"), query);

        /* Připravení dotazu pro vkládání změn */
        query.prepare("INSERT INTO changes (gradeId, id, date, hour, fromClassId, toClassId) "
            "VALUES (1, :id, :date, :hour, :fromClassId, :toClassId);");

        /*   <change> */
        QDomNodeList changeNodes = changes.elementsByTagName("change");
        for(int i = 0; i != changeNodes.count(); ++i) {
            QDomElement change = changeNodes.item(i).toElement();

            /* ID změny (odseknutí 'x' a převod na int) */
            if(!change.hasAttribute("id")) throw tr("Nenalezeno ID změny!");
            int id = change.attribute("id").mid(1).toInt();

            /* <date> */
            QDomElement _date = change.firstChildElement("date");
            if(_date.isNull()) throw tr("Nenalezeno datum změny!");
            QString date = _date.text();

            /* <hour> */
            QDomElement _hour = change.firstChildElement("hour");
            int hour;

            /* <allHours> */
            if(_hour.isNull()) {
                QDomElement allHours = change.firstChildElement("allHours");
                if(allHours.isNull()) throw tr("Nenalezena hodina změny!");
                hour = ChangesModel::ALL_HOURS;

            /* Dosazení hodiny */
            } else hour = _hour.text().toInt();

            /* <fromClass> */
            QDomElement _fromClass = change.firstChildElement("fromClass");
            int fromClassId;

            /* <allClasses> */
            if(_fromClass.isNull()) {
                QDomElement allClasses = change.firstChildElement("allClasses");
                if(allClasses.isNull()) throw tr("Nenalezen předmět, ze kterého se mění!");
                fromClassId = ClassesModel::WHATEVER;

            } else {
                /* Pokud neexistuje atribut id, znamená to, že měníme z prádzného předmětu */
                if(!_fromClass.hasAttribute("id")) fromClassId = 0;

                /* Dosazení ID z atributu (odseknutí 'c' a převod na int) */
                fromClassId = _fromClass.attribute("id").mid(1).toInt();
            }

            /* <toClass> */
            QDomElement _toClass = change.firstChildElement("toClass");
            if(_toClass.isNull()) throw tr("Nenalezen předmět, na který se mění!");
            int toClassId;

            /* Pokud neexistuje atribut id, znamená to, že měníme na prázdný předmět */
            if(!_toClass.hasAttribute("id")) toClassId = 0;

            /* Dosazení ID z atributu (odseknutí 'c' a převod na int) */
            else toClassId = _toClass.attribute("id").mid(1).toInt();

            /* Provedení dotazu */
            query.bindValue(":id", id);
            query.bindValue(":date", date);
            query.bindValue(":hour", hour);
            query.bindValue(":fromClassId", fromClassId);
            query.bindValue(":toClassId", toClassId);
            if(!query.exec()) throw SqlException(tr("Nelze přidat změnu!"), query);

            /* Přičtení přidané změny ke statistice */
            changesCount++;
        }

        /* Emitování signálu o dokončení importu změn */
        emit updated(tr("Dokončena aktualizace změn"), changesCount);

        /* <absences> (jen když načítáme zálohu) */ }
        if(!isUpdate) {
            QDomElement absences = root.firstChildElement("absences");
            if(absences.isNull()) throw tr("Nebyla nalezena sekce s absencemi!");

            /* Smazání starých absencí, uložení jejich počtu pro statistiku */
            if(!query.exec("DELETE FROM absences WHERE id > 0;"))
                throw SqlException(tr("Nelze smazat staré absence!"), query);
            int absencesCount = -query.numRowsAffected();

            /* Reset AUTOINCREMENT */
            if(!query.exec("DELETE FROM sqlite_sequence WHERE name = \"absences\";"))
                throw SqlException(tr("Nelze resetovat tabulku absencí!"), query);

            /* Připravení dotazu pro vkládání absencí */
            query.prepare("INSERT INTO absences (gradeId, id, date, hours) "
                "VALUES (1, :id, :date, :hours);");

            /* <absence> */
            QDomNodeList absenceNodes = absences.elementsByTagName("absence");
            for(int i = 0; i != absenceNodes.count(); ++i) {
                QDomElement absence = absenceNodes.item(i).toElement();

                /* ID absence (odseknutí 'x' a převod na int) */
                if(!absence.hasAttribute("id")) throw tr("Nenalezeno ID absence!");
                int id = absence.attribute("id").mid(1).toInt();

                /* <date> */
                QDomElement _date = absence.firstChildElement("date");
                if(_date.isNull()) throw tr("Nenalezeno datum absence!");
                QString date = _date.text();

                /* <allHours> */
                QDomElement allHours = absence.firstChildElement("allHours");
                int hours = 0;
                if(!allHours.isNull()) hours = ChangesModel::ALL_HOURS;

                /* <hour> */
                else {
                    QDomNodeList hourNodes = absence.elementsByTagName("hour");
                    for(int i = 0; i != hourNodes.count(); ++i) {
                        QDomElement hour = hourNodes.item(i).toElement();
                        hours |= 1 << hour.text().toInt();
                    }
                }

                /* Provedení dotazu */
                query.bindValue(":id", id);
                query.bindValue(":date", date);
                query.bindValue(":hours", hours);
                if(!query.exec()) throw SqlException(tr("Nelze přidat absenci!"), query);

                /* Přičtení přidané absence ke statistice */
                absencesCount++;
            }

            /* Emitování signálu o dokončení importu absencí */
            emit updated(tr("Dokončena aktualizace absencí"), absencesCount);
        }

        /* Nastavení data poslední aktualizace */
        if(isUpdate) {
            query.prepare("UPDATE configuration SET lastUpdate = :lastUpdate;");
            query.bindValue(":lastUpdate", QDate::currentDate().toString(Qt::ISODate));
            if(!query.exec())
                throw SqlException(tr("Nepodařilo se nastavit datum poslední aktualizace!"), query);
        }

        /* Vše proběhlo OK, commit */
        if(!query.exec("COMMIT TRANSACTION;"))
            throw SqlException(tr("Nepodařilo se korektně ukončit aktualizaci!"), query);

        return;

    /* Ošetření chyb zpracování XML */
    } catch(QString e) {
        emit error(e);
        qDebug() << e;

    /* Ošetření chyb při aktualizaci DB */
    } catch(SqlException e) {
        emit error(e.error());
        qDebug() << e.error() << e.query().lastError() << e.query().lastQuery();
    }

    /* Sem se funkce normálně nedostane, takže zrušení transakce */
    QSqlQuery queryEndTransaction;
    if(!queryEndTransaction.exec("ROLLBACK TRANSACTION;")) {
        /* Tohle by opravdu nemělo nikdy nastat! */
        qDebug() << tr("Nepodařilo se zrušit transakci!")
                 << queryEndTransaction.lastError() << queryEndTransaction.lastQuery();
        return;
    }

    emit updated(tr("Databáze navrácena zpět do stavu před aktualizací"), 0);
}

}
