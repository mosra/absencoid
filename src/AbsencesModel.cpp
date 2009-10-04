#include "AbsencesModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFont>
#include <QBrush>

#include "ClassesModel.h"
#include "TimetableModel.h"
#include "ChangesModel.h"

namespace Absencoid {

const int AbsencesModel::SCHOOL_ACTION = 0x8000;

/* Konstruktor */
AbsencesModel::AbsencesModel(ClassesModel* _classesModel, TimetableModel* _timetableModel, ChangesModel* _changesModel, QObject* parent): QAbstractTableModel(parent), classesModel(_classesModel), timetableModel(_timetableModel), changesModel(_changesModel) {
    reload();
}

/* (Znovu)načtení dat z databáze */
void AbsencesModel::reload() {
    absences.clear();

    /* SQL dotaz */
    QSqlQuery query;
    if(!query.exec("SELECT id, date, hours FROM absences ORDER BY date;")) {
        qDebug() << tr("Nepodařilo se získat sezman absencí!")
                 << query.lastError() << query.lastQuery();
        return;
    }

    /* Naplňování dat */
    while(query.next()) {
        /* Inicializace známých hodnot */
        Absence a;
        a.id = query.value(0).toInt();
        a.date = query.value(1).toDate();
        a.hours = query.value(2).toInt();
        absences.append(a);

        /* Zjištění názvů předmětů pro jednotlivé hodiny */
        loadClassIds(absences.count()-1);
    }

    /* Propojení signálu o změnách v rozvrzích s ověřovacími fcemi */
    connect(timetableModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(checkTimetableChanges(QModelIndex,QModelIndex)));

    /* Propojení singálu o změně aktuálního rozvrhu s resetovací fcí */
    connect(timetableModel, SIGNAL(actualTimetableChanged()),
            this, SLOT(reloadAllClassIds()));

    /* Propojení signálu o odebrání rozvrhu s resetovací fcí */
    connect(timetableModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(reloadAllClassIds()));

    reset();
}

/* Počet sloupců */
int AbsencesModel::columnCount(const QModelIndex& parent) const {
    return 12;
}

/* Počet řádků */
int AbsencesModel::rowCount(const QModelIndex& parent) const {
    return absences.count();
}

/* Data hlaviček */
QVariant AbsencesModel::headerData(int section, Qt::Orientation orientation, int role) const {
    /* Horizontální hlavičky */
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case 0:     return tr("Datum");
            case 1:     return tr("Školní");
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
                return QString::number(section-2) + tr(". h");
        }
    }

    /* Vertikální hlavičky */
    if(orientation == Qt::Vertical && section < absences.count()) {
        /* Nová položka, zobrazujeme hvězdičku */
        if(absences[section].id == 0) {
            /* Hvězdička zobrazena tučně */
            if(role == Qt::FontRole) {
                QFont font; font.setBold(true);
                return font;
            }

            if(role == Qt::DisplayRole) return "*";
        }

        else if(role == Qt::DisplayRole) return absences[section].id;
    }

    /* Něco jiného */
    return QAbstractTableModel::headerData(section, orientation, role);
}

/* Datová funkce */
QVariant AbsencesModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid()) return QVariant();

    /* Datum */
    if(index.column() == 0) {
        if(role == Qt::DisplayRole)
            return absences[index.row()].date.toString("ddd dd.MM.yyyy");
        if(role == Qt::EditRole)
            return absences[index.row()].date;

    /* Zda je to školní akce */
    } else if(index.column() == 1) {
        if(role == Qt::DisplayRole)
            return absences[index.row()].hours & SCHOOL_ACTION ? tr("Ano") : tr("Ne");

        if(role == Qt::CheckStateRole)
            return absences[index.row()].hours & SCHOOL_ACTION ? Qt::Checked : Qt::Unchecked;

    /* Zameškané hodiny */
    } else if(index.column() < 12) {
        if(role == Qt::DisplayRole)
            return classesModel->index(absences[index.row()].classIndexes[index.column()-2], 1).data();
        if(role == Qt::CheckStateRole)
            return (absences[index.row()].hours >> (index.column()-2)) & 0x01 ? Qt::Checked : Qt::Unchecked;
        if(role == Qt::BackgroundRole) {
            if((absences[index.row()].changes >> (index.column()-2)) & 0x01)
                return QBrush("#dddddd");
        }
    }

    return QVariant();
}

/* Flags */
Qt::ItemFlags AbsencesModel::flags(const QModelIndex& index) const {
    if(!index.isValid()) return Qt::ItemIsEnabled;

    /* První sloupec je editovatelný */
    if(index.column() == 0) return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;

    /* Ostatní jsou zaškrtávatelné */
    return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
}

/* Zápisový přístup k datům */
bool AbsencesModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid()) return false;

    QSqlQuery query;

    /* Datum */
    if(index.column() == 0 && role == Qt::EditRole) {
        absences[index.row()].date = value.toDate();

        loadClassIds(index.row());

        /* Nový záznam, nemůžeme provést UPDATE, ale INSERT */
        if(absences[index.row()].id == 0) {
            /* Emitujeme signál o změně dat a pokusíme se řádek uložit */
            emit dataChanged(index, index);

            return saveRow(index.row());
        }

        query.prepare("UPDATE absences SET date = :date WHERE id = :id;");
        query.bindValue(":date", absences[index.row()].date);

    /* Zaškrtávací políčka */
    } else if(index.column() > 0 && index.column() < 12 && role == Qt::CheckStateRole) {
        /* Školní akce */
        if(index.column() == 1) {
            /* Zaškrtnutí */
            if(value.toBool())  absences[index.row()].hours |= SCHOOL_ACTION;

            /* Odškrtnutí */
            else                absences[index.row()].hours &= ~SCHOOL_ACTION;

        /* Hodiny */
        } else {
            /* Zaškrtnutí */
            if(value.toBool())  absences[index.row()].hours |= 1 << (index.column()-2);

            /* Odškrtnutí */
            else                absences[index.row()].hours &= ~(1 << (index.column()-2));
        }

        /* Nový záznam, nemůžeme provést UPDATE ale INSERT */
        if(absences[index.row()].id == 0) {
            /* Emitujeme signál o změně dat a pokusíme se řádek uložit */
            emit dataChanged(index, index);

            return saveRow(index.row());
        }

        query.prepare("UPDATE absences SET hours = :hours WHERE id = :id;");
        query.bindValue(":hours", absences[index.row()].hours);

    /* Něco jiného */
    } else return false;

    query.bindValue(":id", absences[index.row()].id);

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se aktualizovat absenci!")
                 << query.lastError() << query.lastQuery();
        return false;
    }

    /* Pokud je to datum, přepsaly se hodiny => změna celého řádku */
    if(index.column() == 0)
        emit dataChanged(index.sibling(index.row(), 0), index.sibling(index.row(), 11));

    /* Jinak emitujeme signál jen o změně aktuálního */
    else
        emit dataChanged(index, index);

    return true;
}

/* Přidání řádku */
bool AbsencesModel::insertRow(int row, const QModelIndex& parent) {
    beginInsertRows(parent, row, row);

    Absence a;
    a.id = 0;
    a.date = QDate::currentDate();
    a.hours = 0;
    absences.append(a);

    /* Načtení hodin v tento den */
    loadClassIds(absences.count()-1);

    endInsertRows();
    return true;
}

/* Odstranění řádků */
bool AbsencesModel::removeRows(int row, int count, const QModelIndex& parent) {
    beginRemoveRows(parent, row, row+count-1);

    QSqlQuery query;
    query.prepare("DELETE FROM absences WHERE id = :id;");

    /* Procházení a mazání po jednom */
    for(int i = row; i != row+count; ++i) {
        /* Pokud je řádek v databázi, smažeme ho */
        if(absences[i].id != 0) {
            query.bindValue(":id", absences[i].id);

            /* Provedení dotazu */
            if(!query.exec()) {
                qDebug() << tr("Nepodařilo se odstranit absenci")
                         << query.lastError() << query.lastQuery();
                return false;
            }
        }

        /* Smazání lokálně */
        absences.removeAt(i);
    }

    endRemoveRows();
    return true;
}

/* Zkontrolování změn v rozvrzích a aplikování jich sem */
void AbsencesModel::checkTimetableChanges(QModelIndex topLeft, QModelIndex bottomRight) {
    /* Zda nás nějaká změna ovlivňbuje */
    bool affects = false;

    /* Index má platného rodiče, jsou tedy změněny data (jednoho) rozvrhu */
    if(topLeft.parent().isValid()) {
        /* Ověříme, jestli je rozvrh aktivní (jiné nás nezajímají) */
        if(timetableModel->isActive(topLeft.parent().row()))
            affects = true;

    /* Jsou změněny vlastnosti (více) rozvrhů */
    } else {
        for(int index = topLeft.row(); index <= bottomRight.row(); ++index) {
            /* Ověříme, jestli je rozvrh aktivní (jiné nás nezajímají) */
            if(timetableModel->isActive(topLeft.row()))
                affects = true;
        }
    }

    /* Pokud nás nějaká změna ovlivňuje, plošný reload všech ID tříd */
    if(affects) reloadAllClassIds();
}

/* Znovunačtení všech ID tříd */
void AbsencesModel::reloadAllClassIds() {
    for(int i = 0; i != absences.count(); ++i)
        loadClassIds(i);
}

/* Zjištění předmětů v jednotlivých hodinách pro daný index */
void AbsencesModel::loadClassIds(int index) {
    /* Pročištění */
    absences[index].changes = 0;
    absences[index].classIndexes.clear();

    /* Aktivní rozvrh platný ten den */
    QList<int> timetableIndex = timetableModel->validTimetables(absences[index].date, true);

    /* Den v týdnu pro toto datum */
    int day = absences[index].date.dayOfWeek()-1;

    /* Žádný rozvrh ten den neplatil, nebo ten den je víkend
        => naplnění prázdnými hodinami */
    if(timetableIndex.count() == 0 || day > 4) {
        for(int hour = 0; hour != 10; ++hour)
            absences[index].classIndexes.append(0);
        return;
    }

    /* Zjištění indexů všech souvisejících změn daný den */
    QList<int> changeIndexesList = changesModel->relatedChanges(absences[index].date);

    /* Zjištění jednotlivých hodin z rozvrhu */
    for(int hour = 0; hour != 10; ++hour) {
        absences[index].classIndexes.append(
            timetableModel->index(timetableIndex[0], 0).child(hour, day).data(Qt::EditRole).toInt());
    }

    /* Aplikování suplů */
    foreach(int changeIndex, changeIndexesList) {
        /* Číslo hodiny absence */
        int hourNumber = changesModel->index(changeIndex, 1).data(Qt::EditRole).toInt();

        /* Index hodiny, ze které měníme */
        int fromHour = changesModel->index(changeIndex, 2).data(Qt::EditRole).toInt();

        /* Index hodiny, na kterou měníme */
        int toHour = changesModel->index(changeIndex, 3).data(Qt::EditRole).toInt();

        /* Změna, který mění všechny hodiny */
        if(hourNumber == -1) {
            /* Změna všech hodin */
            for(int hour = 0; hour != 10; ++hour) {
                /* Pokud měníme z jakéhokoli předmětu a nebo předmět odpovídá, změníme jej */
                if(fromHour == classesModel->indexFromId(ClassesModel::WHATEVER) ||
                   absences[index].classIndexes[hour] == fromHour) {
                    absences[index].classIndexes[hour] = toHour;

                    /* Označení dané hodiny jako změněné */
                    absences[index].changes |= 1 << hour;
                }
            }

            /* Další změny už neaplikujeme */
            return;

        /* Změna, která mění jen jednu hodinu */
        } else {
            /* Pokud měníme z jakéhokoli předmětu a nebo předmět odpovídá, změníme jej */
            if(fromHour == classesModel->indexFromId(ClassesModel::WHATEVER) ||
               absences[index].classIndexes[hourNumber] == fromHour) {
                absences[index].classIndexes[hourNumber] = toHour;

                /* Označení dané hodiny jako změněné */
                absences[index].changes |= 1 << hourNumber;
            }
        }
    }
}

/* Uložení řádku do databáze */
bool AbsencesModel::saveRow(int index) {
    /* Špatný řádek, konec */
    if(index < 0 || index >= absences.count()) return false;

    /* Řádek je již uložený, nebo nemá ještě vyplněnou ani jednu absenci, konec.
        Vracíme true, protože chyba nenastala, uloží se případně jindy. */
    if(absences[index].id != 0 || (absences[index].hours & ~SCHOOL_ACTION) == 0)
        return true;

    QSqlQuery query;
    query.prepare("INSERT INTO absences (gradeId, date, hours) VALUES (1, :date, :hours);");
    query.bindValue(":date", absences[index].date.toString(Qt::ISODate));
    query.bindValue(":hours", absences[index].hours);

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se uložit absenci!")
                 << query.lastError() << query.lastQuery();
        return false;
    }

    /* Uložení nového ID */
    absences[index].id = query.lastInsertId().toInt();

    /* Signál o změně hlavičky, také aby se šířka hlaviček přizpůsobila dlouhým ID */
    emit headerDataChanged(Qt::Vertical, index, index);
    emit layoutChanged();

    return true;
}

}
