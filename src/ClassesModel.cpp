#include "ClassesModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFont>

#include "configure.h"
#include "TeachersModel.h"

namespace Absencoid {

const int ClassesModel::WHATEVER = -1;

/* Konstruktor */
ClassesModel::ClassesModel(TeachersModel* _teachersModel, QObject* parent):
QAbstractTableModel(parent), teachersModel(_teachersModel) {
    /* Dotaz */
    QSqlQuery query("SELECT id, name, teacherId FROM classes ORDER by name;");

    /** @todo Ošetřit errory */

    /* Procházení a umisťování do listu */
    while(query.next()) {
        Class c;
        c.id = query.value(0).toInt();
        c.name = query.value(1).toString();
        c.teacherId = query.value(2).toInt();
        classes.append(c);
    }

    /* Propojení změn v TeachersModel se změnami zde */
    connect(teachersModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(checkTeacherChanges(QModelIndex,QModelIndex)));
}

/* Počet sloupců */
int ClassesModel::columnCount(const QModelIndex& parent) const {
    return 3;
}

/* Počet řádků */
int ClassesModel::rowCount(const QModelIndex& parent) const {
    return classes.count()+2;
}

/* Data hlaviček */
QVariant ClassesModel::headerData(int section, Qt::Orientation orientation, int role) const {

    /* Horizontální hlavičky */
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case 0: return tr("Předmět (Učitel)");
            case 1: return tr("Předmět");
            case 2: return tr("Učitel");
        }
    }

    /* Vertikální hlavičky */
    else if(orientation == Qt::Vertical && section <= classes.count()+1) {

        /* Speciální hlavičky */
        if(section == 0 || section == 1) {
            if(role == Qt::DisplayRole) return tr("S");
            else return QAbstractItemModel::headerData(section, orientation, role);
        }

        /* Pokud je nějaký záznam ještě neuložený, má v hlavičce hvězdičku místo ID */
        if(role == Qt::DisplayRole)
            return classes[section-2].id == 0 ? QVariant("*") : QVariant(classes[section-2].id);

        /* Nové položky mají zvýrazněnou hlavičku (tučný text) */
        if(role == Qt::FontRole && classes[section-2].id == 0) {
            QFont font;
            font.setBold(true);
            return font;
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

/* Čtecí přístup k datům */
QVariant ClassesModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid()) return false;

    /* Souhrn */
    if(index.column() == 0 && role == Qt::DisplayRole) {
        /* 1. řádek - synonymum pro žádný předmět */
        if(index.row() == 0) return "";

        /* 2. řádek - synonymum pro všechny předměty */
        if(index.row() == 1) return tr("Jakýkoli");

        return classes[index.row()-2].name + " (" +
            teachersModel->indexFromId(classes[index.row()-2].teacherId).data().toString() + ")";

    /* Jméno */
    } if(index.column() == 1 && (role == Qt::DisplayRole || role == Qt::EditRole)) {

        /* 1. řádek - synonymum pro žádný předmět */
        if(index.row() == 0) return "";

        /* 2. řádek - synonymum pro všechny předměty */
        if(index.row() == 1) return tr("Cokoli");

        return classes[index.row()-2].name;

    /* Učitel */
    } if(index.column() == 2) {
        /* První dva (speciální řádky) nemají žádného učitele */
        if(index.row() == 0 || index.row() == 1) {
            if(role == Qt::DisplayRole) return "";
            else return QVariant();
        }

        /* Vrácení dat z TeachersModel, aby byly aktuální */
        if(role == Qt::DisplayRole)
            return teachersModel->indexFromId(classes[index.row()-2].teacherId).data();

        /* Vrácení čísla řádku z modelu učitelů */
        if(role == Qt::EditRole)
            return teachersModel->indexFromId(classes[index.row()-2].teacherId).row();
    }

    /* Něco jiného */
    return QVariant();
}

/* Flags */
Qt::ItemFlags ClassesModel::flags(const QModelIndex& index) const {
    #ifndef ADMIN_VERSION
    return QAbstractItemModel::flags(index);
    #endif

    if(!index.isValid()) return Qt::ItemIsEnabled;

    /* První sloupec (popisek) a první dva (speciální) řádky jsou needistovatelné */
    if(index.column() == 0 || index.row() == 0 || index.row() == 1)
        return  QAbstractItemModel::flags(index);

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

/* Zápisový přístup k datům */
bool ClassesModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid() || index.row() < 2) return false;

    QSqlQuery query;

    /* Jméno předmětu */
    if(index.column() == 1 && role == Qt::EditRole) {
        if(value.toString().isEmpty()) return false;

        /* Aktualizace dat */
        classes[index.row()-2].name = value.toString();

        /* Když se jedná o nový záznam, nemůžeme dělat UPDATE, ale INSERT.
            Emitujeme signál a zkusíme řádek uložit. */
        if(classes[index.row()-2].id == 0) {
            emit dataChanged(index, index);

            /* Se změnou učitele se mění i popisek */
            emit dataChanged(index.sibling(index.row(), 0), index.sibling(index.row(), 0));

            return saveRow(index.row()-2);
        }

        /* SQL dotaz */
        query.prepare("UPDATE classes SET name = :name WHERE id = :id;");
        query.bindValue(":name", classes[index.row()-2].name);

    /* Učitel */
    } else if(index.column() == 2 && role == Qt::EditRole) {
        /* Zjištění ID učitele z indexu */
        int teacherId = teachersModel->idFromIndex(value.toInt());

        /* Aktualizace dat */
        classes[index.row()-2].teacherId = teacherId;

        /* Když se jedná o nový záznam, nemůžeme dělat UPDATE, ale INSERT.
            Emitujeme signál a zkusíme řádek uložit. */
        if(classes[index.row()-2].id == 0) {
            emit dataChanged(index, index);

            /* Se změnou učitele se mění i popisek */
            emit dataChanged(index.sibling(index.row(), 0), index.sibling(index.row(), 0));

            return saveRow(index.row()-2);
        }

        /* SQL dotaz */
        query.prepare("UPDATE classes SET teacherId = :teacherId WHERE id = :id;");
        query.bindValue(":teacherId", classes[index.row()-2].teacherId);

    /* Něco jiného */
    } else return false;

    query.bindValue(":id", classes[index.row()-2].id);

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepovedlo se aktualizovat předmět!") << query.lastError()
        << query.lastQuery();
        return false;
    }

    emit dataChanged(index, index);

    /* Se změnou čehokoliv se mění i popisek */
    emit dataChanged(index.sibling(index.row(), 0), index.sibling(index.row(), 0));

    return true;
}

/* Zjištění změn v modelu učitelů */
void ClassesModel::checkTeacherChanges(const QModelIndex& topLeft, const QModelIndex& bottomRight) {
    /* Pokud nejsou změny ve sloupci se jmény, nás se netýkají */
    if(topLeft.column() != 0) return;

    /* Z hlediska výkonnosti je jedno, jestli procházíme změněné řádky a až v nich
        třídy, nebo naopak - není zde žádné break pro předčasné ukončení cyklu,
        takže vyjde oboje úplně nastejno */

    /* Procházení změněných řádků a zjišťování, jestli jsme změnami ovlivněni */
    for(int row = topLeft.row(); row <= bottomRight.row(); ++row) {
        /* Zjištění ID učitele */
        int teacherId = teachersModel->idFromIndex(row);

        /* Procházení tříd a hledání tohoto učitele */
        for(int i = 0; i != classes.count(); ++i) {
            /* Vyslání signálu, že se změnila data ve druhém sloupci daného řádku,
               do nadřazeného cyklu se nevracíme, protože víc předmětů může mít
               stejného učitele. */
            if(classes[i].teacherId == teacherId)
                /* Data se změnila ve druhém sloupci (učitel)   daného řádku */
                emit dataChanged(index(i+2, 1), index(i+2, 1));
        }
    }
}

/* Přidání nového předmětu */
bool ClassesModel::insertRow(int row, const QModelIndex& parent) {
    /* Mezi první dva řádky se nesmí přidávat */
    if(row < 2) return false;

    beginInsertRows(parent, row, row);

    /* Předmět s prázdným ID a ID učitele */
    Class c;
    c.id = 0;
    c.teacherId = 0;
    classes.insert(row-2, c);

    /* Do DB se zapíše až při zadání jména a učitele */

    endInsertRows();
    return true;
}

/* Odebrání předmětů */
bool ClassesModel::removeRows(int row, int count, const QModelIndex& parent) {
    /* První dva řádky se nesmí odebírat */
    if(row < 2) return false;

    beginRemoveRows(parent, row, row+count-1);

    QSqlQuery query;
    query.prepare("DELETE FROM classes WHERE id = :id;");
    for(int i = 0; i != count; ++i) {

        /* Pokud není předmět ještě neuložen, smazání z DB */
        if(classes[row+i-2].id != 0) {
            query.bindValue(":id", classes[row+i-2].id);

            /* Chyba */
            if(!query.exec()) {
                qDebug() << tr("Nelze odstranit předmět!") << query.lastError()
                         << query.lastQuery();
                return false;
            }
        }

        /* Pokud nenastala chyba při mazání z DB, smazání z listu */
        classes.removeAt(row+i-2);
    }

    endRemoveRows();
    return true;
}

/* Zjištění indexu z ID předmětu */
int ClassesModel::indexFromId(int id) {
    /* Speciální index */
    if(id == WHATEVER) return 1;

    for(int i = 0; i != classes.count(); ++i) {
        if(classes[i].id == id) return i+2;
    }

    /* Nenašli jsme žádné odpovídající ID, vracíme neplatný index */
    return -1;
}

/* Zjištění ID předmětu z indexu */
int ClassesModel::idFromIndex(int index) {
    /* Pokud je index neplatný, vracíme nula (a takové ID žádný předmět nemá) */
    if(index < 1 || index > classes.count()+1) return 0;

    /* Speciální řádek "Cokoli" - vracíme konstantu */
    if(index == 1) return WHATEVER;

    return classes[index-2].id;
}

/* Uložení nového ředmětu do DB */
bool ClassesModel::saveRow(int row) {
    /* Špatný index */
    if(row < 0 || row >= classes.count()) return false;

    /* Pokud ještě není vše vyplněno, konec. Vrací se true, protože k žádné chybě
        nedošlo a toto je očekáváné chování (uloží se jindy). Pokud ID není
        rovno nule, je řádek již uložený a tato funkce jej tedy nesmí ukládat
        znova. */
    if(classes[row].id != 0 || classes[row].name.isEmpty() || classes[row].teacherId == 0)
        return true;

    /* SQL dotaz */
    QSqlQuery query;
    query.prepare("INSERT INTO classes (gradeId, name, teacherId) VALUES (1, :name, :teacherId)");
    query.bindValue(":name", classes[row].name);
    query.bindValue(":teacherId", classes[row].teacherId);

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se uložit předmět do databáze!") << query.lastError()
                 << query.lastQuery();
        return false;
    }

    /* Aktualizace ID třídy, vyslání signálu o změně hlavičky */
    classes[row].id = query.lastInsertId().toInt();

    /* Signál o změně hlavičky */
    emit headerDataChanged(Qt::Vertical, row+2, row+2);

    /* Signál o změně zobrazení (aby se hlavička přizpůsobila dlouhým ID) */
    emit layoutChanged();

    return true;
}

}
