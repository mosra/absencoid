#include "Dump.h"

#include <QDomDocument>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QDebug>

#include "configure.h"
#include "ClassesModel.h"
#include "TimetableModel.h"
#include "TeachersModel.h"
#include "ConfigurationModel.h"

namespace Absencoid {

/* Konstruktor */
Dump::Dump(bool indent): flags(indent ? INDENT : 0), _deltaTeachers(0),
_deltaClasses(0), _deltaTimetables(0), _deltaTimetableData(0), _deltaChanges(0),
_deltaAbsences(0) {
    /* Inicializace doctype */
    QDomImplementation i;
    doctype = i.createDocumentType("absencoid", QString(), "absencoid.dtd");
}

/* Vytvoření dumpu / aktualizace */
QString Dump::create(int flags, const QString& note) {
    /* Inicializace dokumentu a kořenového elementu */
    QDomDocument doc(doctype);
    QDomElement r = doc.createElement("absencoid");
    doc.appendChild(r);
    QDomElement root;

    /* Inicializace <dump> */
    if(flags & DUMP) {
        root = doc.createElement("dump");
        root.setAttribute("version", DUMP_VERSION);

    /* Inicializace <update> */
    } else if(flags & UPDATE) {
        root = doc.createElement("update");
        root.setAttribute("version", UPDATE_VERSION);

    /* Nevím co chci */
    } else {
        qDebug() << tr("Musíte vytvořit buď zálohu nebo aktualizaci!");
        return QString();
    }

    r.appendChild(root);

    /* <date> */
    QDomElement date = doc.createElement("date");
    doc.doctype();
    root.appendChild(date);
    date.appendChild(doc.createTextNode(QDate::currentDate().toString(Qt::ISODate)));

    /* <note> (jen u update) */
    if(flags & UPDATE) {
        QDomElement _note = doc.createElement("note");
        root.appendChild(_note);
        _note.appendChild(doc.createTextNode(note));
    }

    /* <configuration> */
    QDomElement configuration = doc.createElement("configuration");
    root.appendChild(configuration);

    /* Dotaz do databáze na konfiguraci */
    QSqlQuery configurationQuery
        ("SELECT beginDate, endDate, webUpdateUrl, lastUpdate, activeTimetableId, flags "
         "FROM configuration LIMIT 1;");

    /* Chyba zpracování dotazu (musí zde být řádek!) */
    if(!configurationQuery.next()) {
        qDebug() << tr("Nepodařilo se získat data z konfigurační tabulky!")
                 << configurationQuery.lastError() << configurationQuery.lastQuery();
        return QString();
    }

    /*   <beginDate> */
    QDomElement beginDate = doc.createElement("beginDate");
    configuration.appendChild(beginDate);
    beginDate.appendChild(doc.createTextNode(configurationQuery.value(0).toString()));

    /*   <endDate> */
    QDomElement endDate = doc.createElement("endDate");
    configuration.appendChild(endDate);
    endDate.appendChild(doc.createTextNode(configurationQuery.value(1).toString()));

    /*   <webUpdateUrl> */
    QDomElement webUpdateUrl = doc.createElement("webUpdateUrl");
    configuration.appendChild(webUpdateUrl);
    webUpdateUrl.appendChild(doc.createTextNode(configurationQuery.value(2).toString()));

    /* Uživatelská data patřící jen do zálohy */
    if(flags & DUMP) {
        /* <lastUpdate> */
        QDomElement lastUpdate = doc.createElement("lastUpdate");
        configuration.appendChild(lastUpdate);
        lastUpdate.appendChild(doc.createTextNode(configurationQuery.value(3).toString()));

        /* <updateOnStart> */
        QDomElement updateOnStart = doc.createElement("updateOnStart");
        configuration.appendChild(updateOnStart);
        updateOnStart.setAttribute("value",
            configurationQuery.value(5).toInt() & ConfigurationModel::UPDATE_ON_START ? "true" : "false");

        /* <dumpOnExit> */
        QDomElement dumpOnExit = doc.createElement("dumpOnExit");
        configuration.appendChild(dumpOnExit);
        dumpOnExit.setAttribute("value",
            configurationQuery.value(5).toInt() & ConfigurationModel::DUMP_ON_EXIT ? "true" : "false");
    }

    /* <teachers> */
    QDomElement teachers = doc.createElement("teachers");
    root.appendChild(teachers);

    /* Dotaz do databáze na učitele */
    QSqlQuery teachersQuery;
    if(!teachersQuery.exec("SELECT id, name, flags FROM teachers ORDER BY name;")) {
        qDebug() << tr("Nepodařilo se získat seznam učitelů!")
                 << teachersQuery.lastError() << teachersQuery.lastQuery();
        return QString();
    }

    /*   <teacher> */
    while(teachersQuery.next()) {
        QDomElement teacher = doc.createElement("teacher");
        teachers.appendChild(teacher);
        teacher.setAttribute("id", "p" + teachersQuery.value(0).toString());
        teacher.setAttribute("counts",
            teachersQuery.value(2).toInt() & TeachersModel::COUNTS ? "true" : "false");
        teacher.setAttribute("accepts",
            teachersQuery.value(2).toInt() & TeachersModel::ACCEPTS ? "true" : "false");
        teacher.appendChild(doc.createTextNode(teachersQuery.value(1).toString()));

        /* Inkrementace počtu ovlivněných učitelů */
        ++_deltaTeachers;
    }

    /* <classes> */
    QDomElement classes = doc.createElement("classes");
    root.appendChild(classes);

    /* Doaz do databáze na předměty */
    QSqlQuery classesQuery;
    if(!classesQuery.exec("SELECT id, name, teacherId FROM classes ORDER BY name;")) {
        qDebug() << tr("Nepodařilo se získat seznam předmětů!")
                 << classesQuery.lastError() << classesQuery.lastQuery();
        return QString();
    }

    /*   <class> */
    while(classesQuery.next()) {
        QDomElement _class = doc.createElement("class");
        classes.appendChild(_class);
        _class.setAttribute("id", "c" + classesQuery.value(0).toString());
        _class.setAttribute("teacherId", "p" + classesQuery.value(2).toString());
        _class.appendChild(doc.createTextNode(classesQuery.value(1).toString()));

        /* Inkrementace počtu ovlivněných předmětů */
        ++_deltaClasses;
    }

    /* <timetables> */
    QDomElement timetables = doc.createElement("timetables");
    root.appendChild(timetables);

    /* ID aktuaálního rozvrhu jen u zálohy, aktualizace jej měnit nesmí */
    /** @todo Nastavovat, jen pokud existuje nějaký rozvrh (potom DTD řve o neplatném IDREF) */
    if(flags & DUMP) timetables.setAttribute("activeId", "t" + configurationQuery.value(4).toString());

    /* Dotaz do databáze na rozvrhy */
    QSqlQuery timetablesQuery;
    if(!timetablesQuery.exec("SELECT id, description, validFrom, followedBy "
                             "FROM timetables ORDER BY description;")) {
        qDebug() << tr("Nepodařilo se získat seznam rozvrhů!")
                 << timetablesQuery.lastError() << timetablesQuery.lastQuery();
        return QString();
    }

    /*   <timetable> */
    while(timetablesQuery.next()) {
        QDomElement timetable = doc.createElement("timetable");
        timetables.appendChild(timetable);
        timetable.setAttribute("id", "t" + timetablesQuery.value(0).toString());
        timetable.setAttribute("next", "t" + timetablesQuery.value(3).toString());

        /* <name> */
        QDomElement name = doc.createElement("name");
        timetable.appendChild(name);
        name.appendChild(doc.createTextNode(timetablesQuery.value(1).toString()));

        /* <validFrom> */
        QDomElement validFrom = doc.createElement("validFrom");
        timetable.appendChild(validFrom);
        validFrom.appendChild(doc.createTextNode(timetablesQuery.value(2).toString()));

        /* <data> */
        QDomElement data = doc.createElement("data");
        timetable.appendChild(data);

        /* Dotaz do databáze na data rozvrhu */
        QSqlQuery timetableDataQuery;

        /* Výběr jen zamknutých hodin pro aktualizaci */
        QString onlyUpdate; if(flags & UPDATE)
            onlyUpdate = " AND classId >= " + QString::number(TimetableModel::FIXED);

        if(!timetableDataQuery.exec("SELECT dayHour, classId FROM timetableData "
                "WHERE timetableId = " + timetablesQuery.value(0).toString() +
                onlyUpdate + " ORDER BY dayHour;")) {
            qDebug() << tr("Nepodařilo se získat data rozvrhu!")
                     << timetableDataQuery.lastError() << timetableDataQuery.lastQuery();
            return QString();
        }

        /*   <lesson> */
        while(timetableDataQuery.next()) {
            QDomElement lesson = doc.createElement("lesson");
            data.appendChild(lesson);
            lesson.setAttribute("classId",
                "c" + QString::number(timetableDataQuery.value(1).toInt() & ~TimetableModel::FIXED));
            lesson.setAttribute("fixed",
                timetableDataQuery.value(1).toInt() & TimetableModel::FIXED ? "true" : "false");

            /* <day> */
            QDomElement day = doc.createElement("day");
            lesson.appendChild(day);
            day.appendChild(doc.createTextNode(
                QString::number((timetableDataQuery.value(0).toInt() & 0x70) >> 4)));

            /* <hour> */
            QDomElement hour = doc.createElement("hour");
            lesson.appendChild(hour);
            hour.appendChild(doc.createTextNode(
                QString::number(timetableDataQuery.value(0).toInt() & 0x0F)));

            /* Inkrementace počtu ovlivněných vyučovacích hodin */
            ++_deltaTimetableData;
        }

        /* Inkrementace počtu ovlivněných rozvrhů */
        ++_deltaTimetables;
    }

    /* <changes> */
    QDomElement changes = doc.createElement("changes");
    root.appendChild(changes);

    /* Dotaz do databáze na změny */
    QSqlQuery changesQuery;
    if(!changesQuery.exec("SELECT id, date, hour, fromClassId, toClassId "
                          "FROM changes ORDER BY date;")) {
        qDebug() << tr("Nepodařilo se získat seznam změn!")
                 << changesQuery.lastError() << changesQuery.lastQuery();
        return QString();
    }

    /*   <change> */
    while(changesQuery.next()) {
        QDomElement change = doc.createElement("change");
        changes.appendChild(change);
        change.setAttribute("id", "x" + changesQuery.value(0).toString());

        /* <date> */
        QDomElement date = doc.createElement("date");
        change.appendChild(date);
        date.appendChild(doc.createTextNode(changesQuery.value(1).toString()));

        /* <allHours> */
        if(changesQuery.value(2).toInt() == -1) {
            QDomElement allHours = doc.createElement("allHours");
            change.appendChild(allHours);

        /* <hour> */
        } else {
            QDomElement hour = doc.createElement("hour");
            change.appendChild(hour);
            hour.appendChild(doc.createTextNode(changesQuery.value(2).toString()));
        }

        /* <allClasses> */
        /** @todo Změnit WHATEVER na ALL_CLASSES */
        if(changesQuery.value(3).toInt() == ClassesModel::WHATEVER) {
            QDomElement allClasses = doc.createElement("allClasses");
            change.appendChild(allClasses);

        /* <fromClass> */
        } else {
            QDomElement fromClass = doc.createElement("fromClass");
            change.appendChild(fromClass);

            /* Pokud je classId == 0, atribut se neuvádí */
            if(changesQuery.value(3).toInt() != 0)
                fromClass.setAttribute("id", "c" + changesQuery.value(3).toString());
        }

        /* <toClass> */
        QDomElement toClass = doc.createElement("toClass");
        change.appendChild(toClass);

        /* Pokud je classId == 0, atribut se neuvádí */
        if(changesQuery.value(4).toInt() != 0)
            toClass.setAttribute("id", "c" + changesQuery.value(4).toString());

        /* Inkrementace počtu ovlivněných změn */
        ++_deltaChanges;
    }

    /* <absences> (jen při dumpu) */
    if(flags & DUMP) {
        QDomElement absences = doc.createElement("absences");
        root.appendChild(absences);

        /* Dotaz do databáze na absence */
        QSqlQuery absencesQuery;
        if(!absencesQuery.exec("SELECT id, date, hours FROM absences ORDER BY date, hours;")) {
            qDebug() << tr("Nepodařilo se získat seznam absencí!")
                     << absencesQuery.lastError() << absencesQuery.lastQuery();
            return QString();
        }

        /* <absence> */
        while(absencesQuery.next()) {
            QDomElement absence = doc.createElement("absence");
            absences.appendChild(absence);
            absence.setAttribute("id", "a" + absencesQuery.value(0).toString());

            /* <date> */
            QDomElement date = doc.createElement("date");
            absence.appendChild(date);
            date.appendChild(doc.createTextNode(absencesQuery.value(1).toString()));

            /* <hour> */
            int hours = absencesQuery.value(2).toInt();
            /** @todo <allHours> ! */
            for(int i = 0; i != 10; ++i) if(hours >> i & 0x01) {
                QDomElement hour = doc.createElement("hour");
                absence.appendChild(hour);
                hour.appendChild(doc.createTextNode(QString::number(i)));
            }

            ++_deltaAbsences;
        }
    }

    return doc.toString(flags & INDENT ? 4 : -1);
}

}
