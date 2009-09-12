#include "ClassesModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFont>

#include "TeachersModel.h"

namespace Absencoid {

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
    return 2;
}

/* Počet řádků */
int ClassesModel::rowCount(const QModelIndex& parent) const {
    return classes.count();
}

/* Data hlaviček */
QVariant ClassesModel::headerData(int section, Qt::Orientation orientation, int role) const {

    /* Horizontální hlavičky */
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case 0: return tr("Předmět");
            case 1: return tr("Učitel");
        }
    }

    /* Vertikální hlavičky */
    else if(orientation == Qt::Vertical && section < classes.count()) {

        /* Pokud je nějaký záznam ještě neuložený, má v hlavičce hvězdičku místo ID */
        if(role == Qt::DisplayRole)
            return classes[section].id == 0 ? QVariant("*") : QVariant(classes[section].id);

        /* Nové položky mají zvýrazněnou hlavičku (tučný text) */
        if(role == Qt::FontRole && classes[section].id == 0) {
            QFont font;
            font.setBold(true);
            return font;
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

/* Čtecí přístup k datům */
QVariant ClassesModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid() ||
        index.column() > 1 ||
        index.row() >= classes.count()) return false;

    /* Jméno */
    if(index.column() == 0 && (role == Qt::DisplayRole || role == Qt::EditRole))
        return classes[index.row()].name;

    /* Učitel */
    if(index.column() == 1) {
        /* Vrácení dat z TeachersModel, aby byly aktuální */
        if(role == Qt::DisplayRole)
            return teachersModel->indexFromId(classes[index.row()].teacherId).data();

        /* Vrácení čísla řádku z modelu učitelů */
        if(role == Qt::EditRole)
            return teachersModel->indexFromId(classes[index.row()].teacherId).row();
    }

    /* Něco jiného */
    return QVariant();
}

/* Flags */
Qt::ItemFlags ClassesModel::flags(const QModelIndex& index) const {
    if(!index.isValid() ||
        index.column() > 1 ||
        index.row() >= classes.count()) return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

/* Zápisový přístup k datům */
bool ClassesModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid() ||
        index.column() > 1 ||
        index.row() >= classes.count()) return false;

    QSqlQuery query;

    /* Jméno předmětu */
    if(index.column() == 0 && role == Qt::EditRole) {
        if(value.toString().isEmpty()) return false;

        /* Aktualizace dat */
        classes[index.row()].name = value.toString();

        /* Když se jedná o nový záznam, nemůžeme dělat UPDATE, ale INSERT.
            Emitujeme signál a zkusíme řádek uložit. */
        if(classes[index.row()].id == 0) {
            emit dataChanged(index, index);
            return saveRow(index.row());
        }

        /* SQL dotaz */
        query.prepare("UPDATE classes SET name = :name WHERE id = :id;");
        query.bindValue(":name", classes[index.row()].name);
        query.bindValue(":id", classes[index.row()].id);

    /* Učitel */
    } else if(index.column() == 1 && role == Qt::EditRole) {
        /* Zjištění ID učitele z indexu */
        int teacherId = teachersModel->idFromIndex(value.toInt());

        /* Aktualizace dat */
        classes[index.row()].teacherId = teacherId;

        /* Když se jedná o nový záznam, nemůžeme dělat UPDATE, ale INSERT.
            Emitujeme signál a zkusíme řádek uložit. */
        if(classes[index.row()].id == 0) {
            emit dataChanged(index, index);
            return saveRow(index.row());
        }

        /* SQL dotaz */
        query.prepare("UPDATE classes SET teacherId = :teacherId WHERE id = :id;");
        query.bindValue(":teacherId", classes[index.row()].teacherId);
        query.bindValue(":id", classes[index.row()].id);

    /* Něco jiného */
    } else return false;

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepovedlo se aktualizovat předmět!") << query.lastError()
        << query.lastQuery();
        return false;
    }

    emit dataChanged(index, index);
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
                emit dataChanged(index(i, 1), index(i, 1));
        }
    }
}

/* Přidání nového předmětu */
bool ClassesModel::insertRow(int row, const QModelIndex& parent) {
    beginInsertRows(parent, row, row);

    /* Předmět s prázdným ID a ID učitele */
    Class c;
    c.id = 0;
    c.teacherId = 0;
    classes.insert(row, c);

    /* Do DB se zapíše až při zadání jména a učitele */

    endInsertRows();
    return true;
}

/* Odebrání předmětů */
bool ClassesModel::removeRows(int row, int count, const QModelIndex& parent) {
    beginRemoveRows(parent, row, row+count-1);

    QSqlQuery query;
    query.prepare("DELETE FROM classes WHERE id = :id;");
    for(int i = 0; i != count; ++i) {

        /* Pokud není předmět ještě neuložen, smazání z DB */
        if(classes[row+i].id != 0) {
            query.bindValue(":id", classes[row+i].id);

            /* Chyba */
            if(!query.exec()) {
                qDebug() << tr("Nelze odstranit předmět!") << query.lastError()
                         << query.lastQuery();
                return false;
            }
        }

        /* Pokud nenastala chyba při mazání z DB, smazání z listu */
        classes.removeAt(row+i);
    }

    endRemoveRows();
    return true;
}

/* Uložení nového ředmětu do DB */
bool ClassesModel::saveRow(int row) {
    /* Špatný index */
    if(row < 0 || row >= classes.count()) return false;

    /* Pokud ještě není vše vyplněno, konec. Vrací se true, protože k žádné chybě
        nedošlo a toto je očekáváné chování (uloží se jindy). */
    if(classes[row].name.isEmpty() || classes[row].teacherId == 0) return true;

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
    emit headerDataChanged(Qt::Vertical, row, row);
    return true;
}

}
