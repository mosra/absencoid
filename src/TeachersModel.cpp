#include "TeachersModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QList>
#include <QStyle>
#include <QApplication>
#include <QDebug>
#include <QFont>

namespace Absencoid {

/* Konstruktor */
TeachersModel::TeachersModel(QObject* parent): QAbstractTableModel(parent) {
    /* Výběr všech učitelů */
    QSqlQuery query("SELECT id, name, flags FROM teachers ORDER BY name;");

    /* Procházení a umisťování do listu */
    while(query.next()) {
        Teacher t;
        t.id = query.value(0).toInt();
        t.name = query.value(1).toString();
        t.flags = query.value(2).toInt();
        teachers.append(t);
    }
}

/* Počet řádků */
int TeachersModel::columnCount(const QModelIndex& parent) const {
    return 3;
}

/* Počet řádků */
int TeachersModel::rowCount(const QModelIndex& parent) const {
    return teachers.count();
}

/* Hlavičky */
QVariant TeachersModel::headerData(int section, Qt::Orientation orientation, int role) const {

    /* Horizontální hlavičky */
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case 0:     return tr("Jméno");
            case 1:     return tr("Absence");
            case 2:     return tr("Školní akce");
        }
    }

    /* Vertikální hlavičky = ID učitele */
    if(orientation == Qt::Vertical && section >= 0 && section < teachers.count()) {

        /* Pokud je nějaký učitel neuložený, má v hlavičce hvězdičku místo ID */
        if(role == Qt::DisplayRole)
            return teachers[section].id == 0 ? QVariant("*") : QVariant(teachers[section].id);

        /* Nové položky mají zvýrazněnou hlavičku (tučný text) */
        if(role == Qt::FontRole && teachers[section].id == 0) {
            QFont font;
            font.setBold(true);
            return font;
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

/* Přístup k datům */
QVariant TeachersModel::data(const QModelIndex& index, int role) const {
    /* Jestli index patří k modelu a je platný */
    if(!index.isValid() ||
        index.row() > teachers.size() ||
        index.column() > 2) return QVariant();

    /* Jméno */
    if(index.column() == 0 && (role == Qt::DisplayRole || role == Qt::EditRole))
            return teachers[index.row()].name;

    /* Zda zapisuje absence */
    else if(index.column() == 1) {
        /* Text */
        if(role == Qt::DisplayRole)
            return teachers[index.row()].flags & 0x01 ? tr("Zapisuje") : tr("Nezapisuje");

        /* Dekorace */
        else if(role == Qt::DecorationRole)
            return QApplication::style()->standardIcon(teachers[index.row()].flags & 0x01 ?
                QStyle::SP_DialogNoButton : QStyle::SP_DialogYesButton);

        /* Checkovatelnost položky */
        else if(role == Qt::CheckStateRole)
            return teachers[index.row()].flags & 0x01 ? Qt::Checked : Qt::Unchecked;

    /* Zda uznává školní akce */
    } else if(index.column() == 2) {
        /* Text */
        if(role == Qt::DisplayRole)
            return teachers[index.row()].flags & 0x02 ? tr("Uznává") : tr("Neuznává");

        /* Dekorace */
        else if(role == Qt::DecorationRole)
            return QApplication::style()->standardIcon(teachers[index.row()].flags & 0x02 ?
            QStyle::SP_DialogYesButton : QStyle::SP_DialogNoButton);

        /* Checkovatelnost položky */
        else if(role == Qt::CheckStateRole)
            return teachers[index.row()].flags & 0x02 ? Qt::Checked : Qt::Unchecked;
    }

    /* Nejspíše se nehodila role */
    return QVariant();
}

/* Flags */
Qt::ItemFlags TeachersModel::flags(const QModelIndex& index) const {
    if(!index.isValid() ||
        index.row() > teachers.size() ||
        index.column() > 2) return Qt::ItemIsEnabled;

    /* Slupec jména učitele */
    if(index.column() == 0)
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;

    /* Boolean sloupce */
    return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
}

/* Zápisový přístup k datům */
bool TeachersModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid() ||
        index.row() > teachers.size() ||
        index.column() > 2) return false;

    QSqlQuery query;

    /* Jméno učitele */
    if(index.column() == 0 && role == Qt::EditRole) {
        if(value.toString().isEmpty()) return false;

        /* Aktualizace dat */
        teachers[index.row()].name = value.toString();

        /* Když se jedná o nový záznam, nemůžeme dělat UPDATE ale INSERT.
            Emitujeme signál o změně dat a pokusíme se záznam uložit. */
        if(teachers[index.row()].id == 0) {
            emit dataChanged(index, index);
            return saveRow(index.row());
        }

        /* SQL dotaz */
        query.prepare("UPDATE teachers SET name = :name WHERE id = :id;");
        query.bindValue(":name", teachers[index.row()].name);
        query.bindValue(":id", teachers[index.row()].id);

        /** @todo Ověřit duplicitu jmen */
        /** @todo Zavolat sort() po změně jména */

    /* Flags */
    } else if(index.column() >= 1 && index.column() <= 2 && role == Qt::CheckStateRole) {

        /* Trochu moc hackoidní přístup, ale budiž. ID sloupce zrovna
            odpovídá použitým flagům, takže to lze takto zkrátit. */
        if(value.toBool())
            teachers[index.row()].flags |= index.column();
        else
            teachers[index.row()].flags &= ~index.column();

        /* SQL dotaz */
        query.prepare("UPDATE teachers SET flags = :flags WHERE id = :id;");
        query.bindValue(":flags", teachers[index.row()].flags);
        query.bindValue(":id", teachers[index.row()].id);

    /* Něco jiného */
    } else return false;

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepovedlo se aktualizovat učitele!") << query.lastError().text()
        << query.lastQuery();
        return false;
    }

    emit dataChanged(index, index);
    return true;
}

/* Přidání nového učitele */
bool TeachersModel::insertRow(int row, const QModelIndex& parent) {
    beginInsertRows(parent, row, row);

    Teacher t;
    t.id = 0;
    t.flags = 2;
    teachers.insert(row, t);

    /* Do DB se ukládá až při zadání jména */

    endInsertRows();
}

/* Odstranění učitelů */
bool TeachersModel::removeRows(int row, int count, const QModelIndex& parent) {
    beginRemoveRows(parent, row, row+count-1);

    QSqlQuery query;
    query.prepare("DELETE FROM teachers WHERE id = :id;");
    for(int i = 0; i != count; i++) {

        /* Pokud není učitel ještě neuložený, smažeme ho z DB */
        if(teachers[row+i].id != 0) {
            /* Smazání z DB (připravený dotaz, aby to nebylo tak příšerně pomalý). */
            query.bindValue(":id", teachers[row+i].id);

            /* Chybička se vloudila... */
            if(!query.exec()) {
                qDebug() << tr("Nelze odstranit učitele!") << query.lastError().text()
                        << query.lastQuery();
                return false;
            }
        }

        /* Pokud nenastala chyba při mazání z DB, smažeme i z listu */
        teachers.removeAt(row+i);
    }

    endRemoveRows();
    return true;
}

/* Uložení řádku do DB */
bool TeachersModel::saveRow(int row) {
    /* Špatný řádek */
    if(row < 0 || row >= teachers.count()) return false;

    /* Pokud není vyplněno jméno učitele, konec. Vracíme true, protože k chybě
        nedošlo (toto je očekáváné chování). Uloží se při další změně jména. */
    if(teachers[row].name.isEmpty()) return true;

    /* SQL dotaz */
    QSqlQuery query;
    query.prepare("INSERT INTO teachers (gradeId, name, flags) VALUES (1, :name, :flags);");
    query.bindValue(":name", teachers[row].name);
    query.bindValue(":flags", teachers[row].flags);

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se uložit učitele do databáze!") << query.lastError()
                 << query.lastQuery();
        return false;
    }

    /* Aktualizace ID učitele, signál o změně hlavičky */
    teachers[row].id = query.lastInsertId().toInt();
    emit headerDataChanged(Qt::Vertical, row, row);
    return true;
}


/* Získání indexu z ID */
QModelIndex TeachersModel::indexFromId(int id) const {
    /* Hledání ID */
    /** @todo Nějaká lepší optimalizace */
    for(QList<Teacher>::size_type i = 0; i != teachers.count(); ++i)
        if(teachers[i].id == id) return index(i, 0);

    /* Vrácení neplatného indexu */
    return index(-1,0);
}

/* Získání ID z indexu */
int TeachersModel::idFromIndex(int row) const {
    /* Pokud je číslo řádku špatné, vrácení 0 (a takové ID v DB neexistuje) */
    if(row < 0 || row > teachers.count()) return 0;

    return teachers[row].id;
}

}
