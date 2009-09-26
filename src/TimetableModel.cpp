#include "TimetableModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "ClassesModel.h"

namespace Absencoid {

const quint32 TimetableModel::NO_PARENT = 0xFFFF;

/* Konstruktor */
TimetableModel::TimetableModel(ClassesModel* _classesModel, QObject* parent):
QAbstractItemModel(parent), classesModel(_classesModel) {

    /* Propojení změn v předmětech se změnami zde */
    connect(classesModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(checkClassChanges(QModelIndex,QModelIndex)));

    /* Načtení seznamu rozvrhů (kořenové položky) */
    QSqlQuery query("SELECT id, description, validFrom, followedBy FROM timetables;");

    /* Ukládání výsledků dotazu */
    while(query.next()) {
        Timetable t;
        t.id = query.value(0).toInt();
        t.description = query.value(1).toString();
        t.validFrom = query.value(2).toDate();
        /* Pokud není zadáno, čím je následován, je následován sám sebou */
        t.followedBy = query.value(3).toInt() == 0 ? t.id : query.value(3).toInt();

        timetables.append(t);
    }
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
    /* Neplatný index, vracíme počet sloupců kořenového seznamu rozvrhů */
    if(!parent.isValid()) return 4;

    /* Rodič je položka v seznamu rozvrhů => vracíme počet sloupců (dnů) v rozvrhu */
    if(parent.internalId() == NO_PARENT)
        return 5;

    /* Rodič je nejspíše položka v datech rozvrhu, ta už nemá žádné potomky */
    return 0;
}

/* Počet řádků */
int TimetableModel::rowCount(const QModelIndex& parent) const {
    /* Neplatný index, vracíme počet řádků kořenového seznamu rozvrhů */
    if(!parent.isValid()) return timetables.count();

    /* Rodič je položka v seznamu rozvrhů => vracíme počet řádků (hodin) v rozvrhu
        (pokud je rozvrh načtený) */
    if(parent.internalId() == NO_PARENT && timetables[parent.row()].isLoaded)
        return 10;

    /* Rodič je nejspíče položka v datech rozvrhu, ta už nemá žádné potomky,
        nebo rozvrh ještě není načtený */
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
            (index.column() == 1 && role == Qt::ToolTipRole))
            return timetables[index.row()].description + tr(" (platný od ")
            + timetables[index.row()].validFrom.toString("ddd dd.MM.yyyy") + ")";

        /* Popisek samotný */
        if(index.column() == 1 && role == Qt::DisplayRole)
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
        int id = timetables[index.parent().row()].data[dayHour(index.column(), index.row())];

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
    } else if(index.parent().internalId() == NO_PARENT)
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;

    /* Něco jiného */
    else return Qt::ItemIsEnabled;
}

/* Zápisový přístup k datům */
bool TimetableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid() || role != Qt::EditRole) return false;

    /* Seznam rozvrhů */
    if(index.internalId() == NO_PARENT) {

        QSqlQuery query;

        /* Popisek - povolujeme jen neprázdný */
        if(index.column() == 1 && !value.toString().isEmpty()) {
            timetables[index.row()].description = value.toString();

            query.prepare("UPDATE timetables SET description = :description WHERE id = :id;");
            query.bindValue(":description", timetables[index.row()].description);

        /* Začátek platnosti */
        } else if(index.column() == 2) {
            timetables[index.row()].validFrom = value.toDate();

            query.prepare("UPDATE timetables SET validFrom = :validFrom WHERE id = :id;");
            query.bindValue(":validFrom", timetables[index.row()].validFrom.toString(Qt::ISODate));

        /* Následující rozvrh */
        } else if(index.column() == 3) {
            timetables[index.row()].followedBy = value.toInt();

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

        /* Změny v popisku a datumu se projeví i v "souhrnném" sloupci */
        if(index.column() == 1 || index.column() == 2)
            emit dataChanged(index.sibling(index.row(), 0), index.sibling(index.row(), 0));

        emit dataChanged(index, index);
        return true;

    /* Data rozvrhu */
    } else if(index.parent().internalId() == NO_PARENT) {
        QSqlQuery query;

        /* Spočítání dne/hodiny */
        int _dayHour = dayHour(index.column(), index.row());

        /* ID předmětu odpovídající aktuálnímu indexu */
        int classId = classesModel->idFromIndex(value.toInt());

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
    t.isLoaded = true;

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

/* Zda lze načíst potomky daného indexu */
bool TimetableModel::canFetchMore(const QModelIndex& parent) const {
    /* Pokud jsme v nějakém kořenovém indexu a data ještě nebyla načtena, lze */
    if(parent.isValid() && parent.internalId() == NO_PARENT && !timetables[parent.row()].isLoaded)
        return true;

    /* Jinak nelze */
    return false;
}

/* Načtení potomků daného indexu */
void TimetableModel::fetchMore(const QModelIndex& parent) {
    /* Pokud je index neplatný či nejsme v kořenovém indexu anebo jsou data již
        načtena, konec */
    if(!parent.isValid() || parent.internalId() != NO_PARENT || timetables[parent.row()].isLoaded)
        return;

    /* SQL dotaz */
    QSqlQuery query;
    query.prepare("SELECT dayHour, classId FROM timetableData WHERE timetableId = :id;");
    query.bindValue(":id", timetables[parent.row()].id);
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se načíst data rozvrhu!") << query.lastError()
                 << query.lastQuery();
        return;
    }

    beginInsertRows(parent, 0, 9);

    /* Naplnění dat */
    while(query.next()) {
        timetables[parent.row()].data.insert(query.value(0).toInt(), query.value(1).toInt());
    }

    /* Označení rozvrhu jako načteného */
    timetables[parent.row()].isLoaded = true;

    endInsertRows();
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

/* Kolik rozvrhů má v tento den/hodinu daný předmět */
int TimetableModel::timetablesWithThisClass(int dayHour, int classId) {
    /* Celkový počet odpovídajících rozvrhů */
    int count = 0;

    /* Procházení všech rozvrhů. V každém rozvrhu můžeme count inkrementovat
        jen jednou! */
    for(int i = 0; i != timetables.count(); ++i) {

        /* Pokud rozvrh ještě není načtený, načteme jej */
        if(!timetables[i].isLoaded) fetchMore(index(i, 0));

        /* Pokud jsou označeny "všechny" hodiny, hledáme postupně v každé hodině */
        if(dayHour & 0x0F) for(int hour = 0; hour != 10; ++hour) {
            int _dayHour = (dayHour & 0x70) | hour;
            /* Testujeme, zda je taková den/hodina přítomná a jestli může být
                předmět jakýkoli, pokud ne, jestli je tam ten správný */
            if(timetables[i].data.contains(_dayHour) &&
            (classId == ClassesModel::WHATEVER || timetables[i].data[_dayHour] == classId)) {
                count++; break;
            }
        }

        /* Pokud je označena jen jediná hodina */
        else if(timetables[i].data.contains(dayHour) &&
        (classId == ClassesModel::WHATEVER || timetables[i].data[dayHour] == classId))
            count++;
    }

    return count;
}


/* Zjištění, zda se změny v modelu předmětů projeví zde */
void TimetableModel::checkClassChanges(const QModelIndex& topLeft, const QModelIndex& bottomRight) {
    /* Projití jednotlivých řádků a zjištění, zda takové předměty máme v rozvrhu */
    for(int i = topLeft.row(); i <= bottomRight.row(); ++i) {
        /* ID předmětu odpovídající indexu řádku */
        int id = classesModel->idFromIndex(i);

        /* Projití všech rozvrhů */
        for(int ii = 0; ii != timetables.size(); ++ii) {

            /* Všechny hodiny s tímto předmětem */
            QList<int> hours = timetables[ii].data.keys(id);

            /* Procházení ovliněných hodin */
            int hour; foreach(hour, hours) {
                /* Spočítání indexu */
                QModelIndex dataIndex = index(hour & 0x0F, (hour & 0x70) >> 4, index(i, 0));

                /* Vyslání signálu */
                emit dataChanged(dataIndex, dataIndex);
            }
        }
    }
}

}
