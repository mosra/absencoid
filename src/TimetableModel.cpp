#include "TimetableModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QBrush>

#include "configure.h"
#include "ClassesModel.h"
#include "ChangesModel.h"

namespace Absencoid {

/* Konstruktor */
TimetableModel::TimetableModel(ClassesModel* _classesModel, QObject* parent):
QAbstractItemModel(parent), classesModel(_classesModel) {
    reload();
}

/* (Znovu)načtení dat z databáze */
void TimetableModel::reload() {
    timetables.clear();

    /* Načtení seznamu rozvrhů (kořenové položky) */
    QSqlQuery query("SELECT id, description, validFrom, followedBy FROM timetables ORDER BY description, validFrom;");

    /* Ukládání výsledků dotazu */
    while(query.next()) {
        Timetable t;
        t.id = query.value(0).toInt();
        t.description = query.value(1).toString();
        t.validFrom = query.value(2).toDate();
        /* Pokud není zadáno, čím je následován, je následován sám sebou */
        t.followedBy = query.value(3).toInt() == 0 ? t.id : query.value(3).toInt();
        t.flags = 0;

        /* Načtení dat rozvrhu */
        QSqlQuery dataQuery;
        dataQuery.prepare("SELECT dayHour, classId FROM timetableData WHERE timetableId = :id;");
        dataQuery.bindValue(":id", t.id);
        if(!dataQuery.exec()) {
            qDebug() << tr("Nepodařilo se načíst data rozvrhu!") << query.lastError()
            << dataQuery.lastQuery();
            return;
        }

        /* Naplnění dat */
        while(dataQuery.next()) {
            t.data.insert(dataQuery.value(0).toInt(), dataQuery.value(1).toInt());
        }

        timetables.append(t);
    }

    reset();
}

/* Vytvoření indexu příslušné položky */
QModelIndex TimetableModel::index(int row, int column, const QModelIndex& parent) const {
    /* Pokud nadřazený index nemá žádného potomka v této pozici, vrátíme
        neplatný index (testuje se _asi_ rowCount(parent) a columnCount(parent) */
    if(!hasIndex(row, column, parent)) return QModelIndex();

    /* Interní ID */
    quint32 internalId;

    /* Rodičovská položka neexistuje => index v seznamu rozvrhů */
    if(!parent.isValid())
        internalId = NO_PARENT;

    /* Nadřazená položka je v seznamu rozvrhů => index dat rozvrhu */
    else if(parent.internalId() == NO_PARENT)
        internalId = parent.row();  /* Interní ID = index nadřazené položky */

    /* Nějaká chyba (např. pokud o získání indexu pro potomka dat rozvrhu,
        přestože data rozvrhu žádné potomky nemají) */
    else return QModelIndex();

    /* Vytvoření indexu */
    return createIndex(row, column, internalId);
}

/* Zjištění rodičovské položky daného indexu */
QModelIndex TimetableModel::parent(const QModelIndex& child) const {
    /* Neplatný index */
    if(!child.isValid()) return QModelIndex();

    /* Index v seznamu rozvrhů */
    if(child.internalId() == NO_PARENT)
        return QModelIndex();       /* Rodičovská položka neexistuje */

    /* Index dat rozvrhu, nadřazená položka je specifikována v internalId */
    else return index(child.internalId(), 0);
}

/* Počet sloupců */
int TimetableModel::columnCount(const QModelIndex& parent) const {
    /* Pro kořenový seznam rozvrhů (4 sloupce) i položku rozvrhu (počet dnů)
        vracíme stejně (kvůli hlavičkám Po - Pá, pokud není žádný rozvrh v tabulce) */
    if(!parent.isValid() || parent.internalId() == NO_PARENT)
        return 5;

    /* Rodič je nejspíše položka v datech rozvrhu, ta už nemá žádné potomky */
    return 0;
}

/* Počet řádků */
int TimetableModel::rowCount(const QModelIndex& parent) const {
    /* Neplatný index, vracíme počet řádků kořenového seznamu rozvrhů */
    if(!parent.isValid()) return timetables.count();

    /* Rodič je položka v seznamu rozvrhů => vracíme počet řádků (hodin) v rozvrhu */
    if(parent.internalId() == NO_PARENT)
        return 10;

    /* Rodič je nejspíče položka v datech rozvrhu, ta už nemá žádné potomky */
    return 0;
}

/* Hlavičky (jen pro data rozvrhu) */
QVariant TimetableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole) return QVariant();

    /* Hodiny */
    if(orientation == Qt::Vertical)
        return QString::number(section) + ".";

    /* Názvy dnů */
    else {
        switch(section) {
            case 0: return tr("Pondělí");
            case 1: return tr("Úterý");
            case 2: return tr("Středa");
            case 3: return tr("Čtvrtek");
            case 4: return tr("Pátek");
        }
    }

    /* Něco jinýho */
    return QVariant();
}

/* Data rozvrhu */
QVariant TimetableModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid()) return QVariant();

    /* Kořenový index - seznam rozvrhů */
    if(index.internalId() == NO_PARENT) {
        /* Popisek: Rozvrh (platný od 01.09.2009) */
        if((index.column() == 0 && role == Qt::DisplayRole) ||
           (index.column() == 1 && role == Qt::ToolTipRole)) {

            QString prefix, validTo;

            /* Aktivní rozvrh - před popisek dáme hvězdičku */
            if(timetables[index.row()].flags & ACTIVE)
                prefix = "* ";

            /* Pokud není rozvrh následován sebou samým, má nějaký konec platnosti */
            if(timetables[index.row()].followedBy != idFromIndex(index.row())) {
                int _index = indexFromId(timetables[index.row()].followedBy);

                /* Jen pokud následující rozvrh existuje */
                if(_index != -1) validTo = tr(" do %1")
                    .arg(timetables[_index].validFrom.addDays(-1).toString("ddd dd.MM.yyyy"));
            }

            return tr("%1%2 (platný od %3%4)").arg(prefix)
                .arg(timetables[index.row()].description)
                .arg(timetables[index.row()].validFrom.toString("ddd dd.MM.yyyy")).arg(validTo);

        /* Popisek samotný */
        } if(index.column() == 1 && role == Qt::DisplayRole)
            return timetables[index.row()].description;

        /* Platný od */
        if(index.column() == 2 && role == Qt::DisplayRole)
            return timetables[index.row()].validFrom;

        /* Následující rozvrh - nevracíme ID, ale index! */
        if(index.column() == 3 && role == Qt::DisplayRole)
            return timetables[index.row()].followedBy;

    /* Data rozvrhu */
    } else if(index.parent().internalId() == NO_PARENT) {

        /* ID předmětu pod tímto číslem dne/hodiny */
        int id;
        if(!timetables[index.parent().row()].data.contains(dayHour(index.column(), index.row()))) id = 0;
        else id = timetables[index.parent().row()].data[dayHour(index.column(), index.row())];

        /* Zda je položka zamknutá */
        if(role == Qt::UserRole) return id == 0 ? FIXED : id & FIXED;

        /* Zamknutá položka, editovatelná jen správcem */
        if(id & FIXED) {
            #ifdef ADMIN_VERSION
            if(role == Qt::BackgroundRole) return QBrush("#dddddd");
            #endif

            id &= ~FIXED;
        }

        /* Index odpovídající tomuto ID předmětu */
        int classIndex = classesModel->indexFromId(id);

        /* Vrácení textu */
        if(role == Qt::DisplayRole)
            return classesModel->index(classIndex, 0).data();

        /* Vrácení indexu předmětu pro editaci */
        if(role == Qt::EditRole)
            return classIndex;
    }

    /* Něco jiného */
    return QVariant();
}

/* Flags */
Qt::ItemFlags TimetableModel::flags(const QModelIndex& index) const {
    if(!index.isValid()) return Qt::ItemIsEnabled;

    /* Položka v seznamu rozvrhů */
    if(index.internalId() == NO_PARENT) {

        /* Dlouhý popisek není editovatelný */
        if(index.column() == 0) return QAbstractItemModel::flags(index);

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;

    /* Data rozvrhu */
    } else if(index.parent().internalId() == NO_PARENT) {
        #ifndef ADMIN_VERSION
        /* Zamknuté položky jsou editovatelné jen správcem */
        if(timetables[index.parent().row()].data.contains(dayHour(index.column(), index.row())) &&
           timetables[index.parent().row()].data[dayHour(index.column(), index.row())] & FIXED)
            return 0;
        #endif

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;

    /* Něco jiného */
    } else return Qt::ItemIsEnabled;
}

/* Zápisový přístup k datům */
bool TimetableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid()) return false;

    /* Seznam rozvrhů */
    if(index.internalId() == NO_PARENT && role == Qt::EditRole) {

        QSqlQuery query;

        /* Popisek - povolujeme jen neprázdný */
        if(index.column() == 1 && !value.toString().isEmpty()) {
            timetables[index.row()].description = value.toString();

            query.prepare("UPDATE timetables SET description = :description WHERE id = :id;");
            query.bindValue(":description", timetables[index.row()].description);

        /* Začátek platnosti */
        } else if(index.column() == 2) {
            timetables[index.row()].validFrom = value.toDate();

            /* Nalezení předchozích rozvrhů a emitování signálu o změně jejich
                popisku (změnil se jejich konec platnosti) */
            for(int i = 0; i != timetables.count(); ++i) {
                if(timetables[i].followedBy == timetables[index.row()].id)
                    emit dataChanged(index.sibling(i, 0), index.sibling(i, 0));
            }

            query.prepare("UPDATE timetables SET validFrom = :validFrom WHERE id = :id;");
            query.bindValue(":validFrom", timetables[index.row()].validFrom.toString(Qt::ISODate));

        /* Následující rozvrh */
        } else if(index.column() == 3) {
            timetables[index.row()].followedBy = value.toInt();

            /* Pokud je tento rozvrh aktuální, načteme znova řetězec aktivních rozvrhů */
            if(timetables[index.row()].flags & ACTIVE)
                setActualTimetable(index.row());

            query.prepare("UPDATE timetables SET followedBy = :followedBy WHERE id = :id;");
            query.bindValue(":validTo", timetables[index.row()].followedBy);

        /* Něco jiného / neplatné hodiny */
        } else return false;

        /* Provedení dotazu */
        query.bindValue(":id", timetables[index.row()].id);
        if(!query.exec()) {
            qDebug() << tr("Nepodařilo se aktualizovat rozvrh!") << query.lastError()
            << query.lastQuery();
            return false;
        }

        /* Všechny změny v popisku, datumu i následujícím rozvrhu se projeví v "souhrnném" sloupci */
        emit dataChanged(index.sibling(index.row(), 0), index.sibling(index.row(), 0));

        emit dataChanged(index, index);
        return true;

    /* Data rozvrhu */
    } else if(index.parent().internalId() == NO_PARENT &&
             (role == Qt::EditRole || role == Qt::UserRole)) {
        QSqlQuery query;

        /* Spočítání dne/hodiny */
        int _dayHour = dayHour(index.column(), index.row());

        int classId;

        /* Upravujeme na jinou hodinu */
        if(role == Qt::EditRole) {
            /* ID předmětu odpovídající aktuálnímu indexu */
            classId = classesModel->idFromIndex(value.toInt());

            /* Pokud byla předchozí položka zamknutá, tato bude taky */
            if(timetables[index.parent().row()].data.contains(_dayHour) &&
              (timetables[index.parent().row()].data[_dayHour] & FIXED))
                classId |= FIXED;

        /* Zamykáme / odemykáme */
        } else if(role == Qt::UserRole) {
            /* Neexistující hodina, nemáme co odemykat / zamykat */
            if(!timetables[index.parent().row()].data.contains(_dayHour)) return true;

            classId = timetables[index.parent().row()].data[_dayHour];

            /* Odemykáme */
            if(value.toInt() == 0) classId &= ~FIXED;

            /* Zamykáme */
            else                   classId |= FIXED;
        }

        /* V rozvrhu může být cokoli, jen ne "Cokoli" */
        if(classId == ClassesModel::WHATEVER) return false;

        /* Jestli upravujeme stávající záznam, provedeme UPDATE */
        if(timetables[index.parent().row()].data.contains(_dayHour)) {
            /* Rušíme hodinu */
            if(classId == 0) {
                timetables[index.parent().row()].data.remove(_dayHour);
                query.prepare("DELETE FROM timetableData "
                            "WHERE timetableId = :timetableId "
                            "AND dayHour = :dayHour;");

            /* Aktualizujeme hodinu */
            } else {
                timetables[index.parent().row()].data[_dayHour] = classId;
                query.prepare("UPDATE timetableData SET classId = :classId "
                            "WHERE dayHour = :dayHour "
                            "AND timetableId = :timetableId;");
                query.bindValue(":classId", timetables[index.parent().row()].data[_dayHour]);
            }

        /* Přidáváme nový záznam, takže INSERT */
        } else {
            /* Nahrazení prázdné hodiny prázdnou, tj. nic neaktualizujeme */
            if(classId == 0) return true;

            /* Vkládáme novou hodinu */
            timetables[index.parent().row()].data.insert(_dayHour, classId);
            query.prepare("INSERT INTO timetableData (gradeId, timetableId, dayHour, classId) "
                        "VALUES (1, :timetableId, :dayHour, :classId);");
            query.bindValue(":classId", timetables[index.parent().row()].data[_dayHour]);
        }

        /* Naplnění dotazu daty */
        query.bindValue(":timetableId", timetables[index.parent().row()].id);
        query.bindValue(":dayHour", _dayHour);

        /* Provedení dotazu */
        if(!query.exec()) {
            qDebug() << tr("Nepodařilo se upravit data rozvrhu!") << query.lastError()
                    << query.lastQuery();
            return false;
        }

        emit dataChanged(index, index);
        return true;
    }

    /* Něco jiného */
    else return false;
}

bool TimetableModel::insertRow(int row, const QModelIndex& parent) {
    /* Pokud je parent platný, nejsme v kořenovém seznamu rozvrhů, tudíž neděláme nic */
    if(parent.isValid()) return false;

    beginInsertRows(parent, row, row);

    /* Zjištění, jestli už zde není nějaký "Nový rozvrh" */
    for(int i = 0; i != timetables.count(); ++i) {
        if(timetables[i].description == tr("Nový rozvrh")) {
            qDebug() << tr("Nelze přidat další nepojmenovaný rozvrh!");
            return false;
        }
    }

    /* Default hodnoty */
    Timetable t;
    t.description = tr("Nový rozvrh");
    t.validFrom = QDate::currentDate();
    t.flags = 0;

    /* SQL dotaz */
    QSqlQuery query;
    query.prepare("INSERT INTO timetables (gradeId, description, validFrom) VALUES (1, :description, :validFrom);");
    query.bindValue(":description", t.description);
    query.bindValue(":validFrom", t.validFrom);

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se přidat nový rozvrh!") << query.lastError()
        << query.lastQuery();
        return false;
    }

    /* Upravení nového ID */
    t.id = query.lastInsertId().toInt();
    t.followedBy = t.id;
    timetables.insert(row, t);

    endInsertRows();
    return true;
}

/* Ostranění rozvrhu */
bool TimetableModel::removeRow(int row, const QModelIndex& parent) {
    /* Pokud je parent platný, neodpovídá kořenovému seznamu rozvrhů - neděláme nic */
    if(parent.isValid()) return false;

    beginRemoveRows(parent, row, row);

    /* SQL dotaz */
    QSqlQuery query;
    query.prepare("DELETE FROM timetables WHERE id = :id;");
    query.bindValue(":id", timetables[row].id);

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se odstranit rozvrh!") << query.lastError()
        << query.lastQuery();
        return false;
    }

    /* Smazání dat rozvrhu */
    query.prepare("DELETE FROM timetableData WHERE timetableId = :idB;");
    query.bindValue(":idB", timetables[row].id);

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se odstranit rozvrh!") << query.lastError()
        << query.lastQuery();
        return false;
    }

    /* Pokud se povedlo odstranit z DB, odstranění z dat */
    timetables.removeAt(row);

    endRemoveRows();
    return true;
}

/* Zjištění ID rozvrhu z indexu */
int TimetableModel::idFromIndex(int index) const {
    /* Pokud je chybný index, vrácení nuly (a takové ID žádný rozvrh nemá) */
    if(index < 0 || index >= timetables.count()) return 0;

    return timetables[index].id;
}

/* Zjištění indexu rozvrhu z ID rozvrhu */
int TimetableModel::indexFromId(int id) const {
    /* Procházení a hledání ID */
    for(int i = 0; i != timetables.count(); ++i)
        if(timetables[i].id == id) return i;

    /* Nenalezeno, vrácení neplatného indexu */
    return -1;
}

/* Rozvrhy platné v daný datum */
QList<int> TimetableModel::validTimetables(QDate date, bool activeOnly) {
    QList<int> valid;

    /* Pokud je datum mimo rozsah pololetí, nevarcíme nic */
    if(date < _beginDate || date > _endDate) return valid;

    /* Procházení rozvrhů */
    for(int i = 0; i != timetables.count(); ++i) {
        /* Pokud je rozvrh platný... */
        if(timetables[i].validFrom <= date &&
          /* ...a nemá následovníka... */
          (timetables[i].id == timetables[i].followedBy ||
          /* ...nebo jeho následovník ještě nezačal platit */
          timetables[indexFromId(timetables[i].followedBy)].validFrom > date)) {

            /* Chceme jen aktivní */
            if(activeOnly) {

                /* Tento rozvrh aktivní je, vrátíme jej */
                if(timetables[i].flags & ACTIVE) {
                    valid.append(i);
                    return valid;
                }

            /* Chceme jakýkoliv, přidáme jej do seznamu */
            } else valid.append(i);
        }
    }

    /* Vrácení seznamu */
    return valid;
}

/* Počet platných rozvrhů, které obsahují daný předmět */
int TimetableModel::timetablesWithThisLesson(QDate date, int hour, int classId, bool activeOnly) {
    /* Zjištění platných rozvrhů */
    QList<int> valid = validTimetables(date, activeOnly);

    /* Den v týdnu (pondělí == 0) */
    int day = date.dayOfWeek()-1;

    /* Ha, víkend! */
    if(day > 4) return 0;

    /* Počet nalezených rozvrhů */
    int count = 0;

    /* Procházení platných rozvrhů a hledání hodiny. Bacha na to, v každém
        rozvrhu můžu inkrementovat jenom jednou! */
    foreach(int timetableIndex, valid) {
        /* Hledáme předmět ve všech hodinách */
        if(hour == ChangesModel::ALL_HOURS) for(int _hour = 0; _hour != 10; ++_hour) {
            int _dayHour = dayHour(day, _hour);

            /* Pokud je předmět jakýkoli, musí daná hodina alespoň existovat */
            if(timetables[timetableIndex].data.contains(_dayHour) && (classId == ClassesModel::WHATEVER ||
               (timetables[timetableIndex].data[_dayHour] & ~FIXED) == classId)) {
                count++;
                break;
            }

        /* Hledáme předmět v dané hodině */
        } else if(hour < 10) {
            int _dayHour = dayHour(day, hour);

            /* Pokud je předmět jakýkoli, musí daná hodina alespoň existovat */
            if(timetables[timetableIndex].data.contains(_dayHour) && (classId == ClassesModel::WHATEVER ||
                (timetables[timetableIndex].data[_dayHour] & ~FIXED) == classId))
                count++;

        /* Hledáme v celém rozvrhu */
        } else {
            /* Hledáme jakýkoli předmět v celém rozvrhu, tedy rozvrh musí obsahovat alespoň jeden předmět */
            if(classId == ClassesModel::WHATEVER && timetables[timetableIndex].data.count() != 0) count++;

            /* Procházení všech hodin v rozvrhu a hledání příslušené hodiny */
            else foreach(int _classId, timetables[timetableIndex].data) {
                if((_classId & ~FIXED) == classId) {
                    count++;
                    break;
                }
            }
        }
    }

    /* Vrácení počtu odpovídajících rozvrhů */
    return count;
}

/* Nalezení předchozího rozvrhu */
int TimetableModel::previousTimetable(int index) {
    /* Špatný index */
    if(index < 0 || index >= timetables.count()) return -1;

    /* ID rozvrhu */
    int id = timetables[index].id;

    /* Hledání předka */
    for(int i = 0; i != timetables.count(); ++i) {
        /* Nalezli jsme předka a není to ten samý rozvrh */
        if(timetables[i].id != id && timetables[i].followedBy == id)
            return i;
    }

    /* Žádný předek nenalezen */
    return -1;
}

/* Počet hodin daného předmětu v daný den */
int TimetableModel::lessonCount(QDate date, int classId) {
    /* Platný rozvrh v ten den */
    QList<int> timetableIndexes = validTimetables(date, true);

    /* Žádný rozvrh v ten den neplatil */
    if(timetableIndexes.isEmpty()) return 0;

    int count = 0;
    int day = date.dayOfWeek()-1;

    /* Jednotlivé hodiny */
    for(int i = 0; i != 10; ++i)
        if(timetables[timetableIndexes[0]].data.contains(dayHour(day, i)) &&
          (classId == 0 || (classId == 1 && (timetables[timetableIndexes[0]].data[dayHour(day, i)] & ~FIXED) == classId)))
            count++;

    return count;
}

/* Počet hodin daného předmětu */
int TimetableModel::lessonCount(int classId, bool tillNow) {
    int count = 0;

    /* Platný rozvrh v první den */
    QList<int> timetableIndexes = validTimetables(_beginDate, true);
    if(timetableIndexes.isEmpty()) return 0;

    /* Koncové datum buď dnešek, nebo konec pololetí */
    QDate __endDate = tillNow ? QDate::currentDate() : _endDate;

    /* Procházíme rozvrhy, dokud jsme v rozsahu pololetí */
    int timetableIndex = timetableIndexes[0];
    QDate date = _beginDate;
    while(date <= __endDate) {
        /* Index dalšího rozvrhu */
        int next = indexFromId(timetables[timetableIndexes[0]].followedBy);

        /* Datum konce platnosti aktuálního rozvrhu */
        QDate end = timetables[next].validFrom.addDays(-1);

        /* Pokud je tento rozvrh poslední v řadě (následován sebou samým),
            nebo je datum konce platnosti za koncem pololetí, nastavíme
            konec platnosti na konec pololetí */
        if(next == timetableIndex || end > __endDate) end = __endDate;

        /* Počet dnů, po které je daný rozvrh platný (včetně koncového, tedy +1) */
        int days = date.daysTo(end)+1;

        /* Počet všech hodin v celých týdnech */
        if(classId == 0) count += timetables[timetableIndex].data.count()*(days/7);

        /* Počet konkrétních hodin v celých týdnech (musíme počítat i s FIXED hodinami) */
        else count += (timetables[timetableIndex].data.keys(classId).count()
                +timetables[timetableIndex].data.keys(classId | FIXED).count())*(days/7);

        /* Číslo prvního dne ze zbývajících = číslo prvního dne pololetí */
        int firstDay = date.dayOfWeek()-1;

        /* Projití dnů z posledního necelého týdne */
        for(int i = 0; i != days%7; ++i) {

            /* Číslo dne nesmí být větší než 6 => tedy zbytek po dělení 7 */
            int day = (firstDay+i)%7;

            /* Procházení hodin a hledání předmětů */
            for(int hour = 0; hour != 10; ++hour) {
                /* Pokud danou hodinu předmět existuje a (pokud hledáme konkrétní
                    předmět) je ten správný, přičteme */
                if(timetables[timetableIndex].data.contains(dayHour(day, hour)) &&
                  (classId == 0 || (timetables[timetableIndex].data[dayHour(day, hour)] & ~FIXED) == classId))
                    count++;
            }
        }

        /* Přeskočení na další den - buď to bude den začátku platnosti
            dalšího rozvrhu, nebo první den mimo rozsah pololetí, oboje
            je správně */
        date = end.addDays(1);

        /* ID dalšího rozvrhu */
        timetableIndex = next;
    }

    return count;
}

/* Nastavení rozvrhu jako aktuálního */
void TimetableModel::setActualTimetable(int _index) {

    /* Nalezení prvního předka tohoto rozvrhu */
    int oldIndex = _index;

    while(_index != -1) {
        oldIndex = _index;
        _index = previousTimetable(_index);
    }

    _index = oldIndex;

    /* Neplatný index */
    if(_index == -1) return;

    /* Zrušení flagu ACTIVE u všech rozvrhů */
    for(int i = 0; i != timetables.count(); ++i)
        timetables[i].flags &= ~ACTIVE;

    /* Nastavení flagu ACTIVE u prvního rozvrhu */
    timetables[_index].flags |= ACTIVE;

    /* Projití všech následujících rozvrhů a nastavení flagu i jim */
    while(timetables[_index].id != timetables[_index].followedBy) {
        _index = indexFromId(timetables[_index].followedBy);
        timetables[_index].flags |= ACTIVE;
    }

    /* Emitování signálu o změně aktuálního rozvrhu */
    emit actualTimetableChanged();

    /* Emitování signálu o změně všech popisků (co já to budu počítat) */
    emit dataChanged(index(0, 0), index(timetables.count()-1, 0));
}

/* Nastavení data začátku pololetí */
void TimetableModel::setBeginDate(QDate date) {
    _beginDate = date;
    emit dateRangeChanged();
}

/* Nastavení data konce pololetí */
void TimetableModel::setEndDate(QDate date) {
    _endDate = date;
    emit dateRangeChanged();
}

}
