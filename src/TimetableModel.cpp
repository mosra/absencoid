#include "TimetableModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "ClassesModel.h"

namespace Absencoid {

/* Konstruktor */
TimetableModel::TimetableModel(ClassesModel* _classesModel, QObject* parent):
QAbstractTableModel(parent), classesModel(_classesModel), timetableId(0) {

    /* Propojení změn v předmětech se změnami zde */
    connect(classesModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(checkClassChanges(QModelIndex,QModelIndex)));
}

/* Načtení rozvrhu */
void TimetableModel::load(int id) {
    /* Uložení id pro použití při ukládání dat */
    timetableId = id;

    /* Smazání dosavadních dat */
    timetableData.clear();

    QSqlQuery query;
    query.prepare("SELECT dayHour, classId FROM timetableData WHERE timetableId = :id;");
    query.bindValue(":id", timetableId);
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se načíst data rozvrhu!") << query.lastError()
                 << query.lastQuery();
        return;
    }

    /* Naplnění dat */
    while(query.next()) {
        timetableData.insert(query.value(0).toInt(), query.value(1).toInt());
    }

    /* Vyslání signálu, že se data kompletně změnila */
    reset();
}


/* Počet sloupců */
int TimetableModel::columnCount(const QModelIndex& parent) const {
    return 5;
}

/* Počet řádků */
int TimetableModel::rowCount(const QModelIndex& parent) const {
    return 10;
}

/* Hlavičky */
QVariant TimetableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole) return QVariant();

    /* Hodiny */
    if(orientation == Qt::Vertical)
        return QString::number(section) + tr(".");

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
    if(!index.isValid() ||
        index.column() > 4 ||
        index.row() > 9) return QVariant();

    /* Číslo dne/hodiny */
    int dayhour = index.column() << 4 | index.row();

    /* ID předmětu pod tímto číslem dne/hodiny */
    int id = timetableData[dayhour];

    /* Index odpovídající tomuto ID předmětu */
    int idx = classesModel->indexFromId(id);

    /* Vrácení textu */
    if(role == Qt::DisplayRole)
        return classesModel->index(idx, 0).data();

    /* Vrácení indexu předmětu pro editaci */
    if(role == Qt::EditRole)
        return idx;

    /* Něco jiného */
    return QVariant();
}

/* Flags */
Qt::ItemFlags TimetableModel::flags(const QModelIndex& index) const {
    if(!index.isValid() ||
        index.column() > 4 ||
        index.row() > 10) return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

/* Zápisový přístup k datům */
bool TimetableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid() || role != Qt::EditRole) return false;

    int dayHour = index.column() << 4 | index.row();

    QSqlQuery query;

    /* ID předmětu odpovídající aktuálnímu indexu */
    int idx = classesModel->idFromIndex(value.toInt());

    /* Jestli upravujeme stávající záznam, provedeme UPDATE */
    if(timetableData.contains(dayHour)) {
        /* Rušíme hodinu */
        if(idx == 0) {
            timetableData.remove(dayHour);
            query.prepare("DELETE FROM timetableData "
                          "WHERE timetableId = :timetableId "
                          "AND dayHour = :dayHour;");

        /* Aktualizujeme hodinu */
        } else {
            timetableData[dayHour] = idx;
            query.prepare("UPDATE timetableData SET classId = :classId "
                          "WHERE dayHour = :dayHour "
                          "AND timetableId = :timetableId;");
            query.bindValue(":classId", timetableData[dayHour]);
        }

    /* Přidáváme nový záznam, takže INSERT */
    } else {
        /* Nahrazení prázdné hodiny prázdnou, tj. nic neaktualizujeme */
        if(idx == 0) return true;

        /* Vkládáme novou hodinu */
        timetableData.insert(dayHour, idx);
        query.prepare("INSERT INTO timetableData (gradeId, timetableId, dayHour, classId) "
                       "VALUES (1, :timetableId, :dayHour, :classId);");
        query.bindValue(":classId", timetableData[dayHour]);

    }

    /* Naplnění dotazu daty */
    query.bindValue(":timetableId", timetableId);
    query.bindValue(":dayHour", dayHour);

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se upravit data rozvrhu!") << query.lastError()
                 << query.lastQuery();
        return false;
    }

    emit dataChanged(index, index);
    return true;
}

/* Zjištění, zda se změny v modelu předmětů projeví zde */
void TimetableModel::checkClassChanges(const QModelIndex& topLeft, const QModelIndex& bottomRight) {
    /* Projití jednotlivých řádků a zjištění, zda takové předměty máme v rozvrhu */
    for(int i = topLeft.row(); i <= bottomRight.row(); ++i) {
        /* ID předmětu odpovídající indexu řádku */
        int id = classesModel->idFromIndex(i);

        /* Všechny hodiny s tímto předmětem */
        QList<int> hours = timetableData.keys(id);

        /* Procházení ovliněných hodin */
        int hour; foreach(hour, hours) {
            /* Spočítání souřadnic */
            QModelIndex idx = index(hour & 0x0F, (hour & 0xF0) >> 4);

            /* Vyslání signálu */
            emit dataChanged(idx, idx);
        }
    }
}

}
