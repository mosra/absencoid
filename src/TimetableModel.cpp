#include "TimetableModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "ClassesModel.h"

namespace Absencoid {

/* Konstruktor */
TimetableModel::TimetableModel(ClassesModel* _classesModel, QObject* parent):
QAbstractTableModel(parent), classesModel(_classesModel), horizontalLessons(true),
timetableId(0) {

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
    query.prepare("SELECT dateHour, classId FROM timetableData WHERE timetableId = :id;");
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
    /* Na horizontální ose jsou zobrazeny hodiny */
    if(horizontalLessons) return 10;

    /* Na horizontální ose jsou zobrazeny dny */
    else return 5;
}

/* Počet řádků */
int TimetableModel::rowCount(const QModelIndex& parent) const {
    /* Na vertikální ose jsou zobrazeny dny */
    if(horizontalLessons) return 5;

    /* Na vertikální ose jsou zobrazeny hodiny */
    else return 10;
}

/* Hlavičky */
QVariant TimetableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole) return QVariant();

    /* Dlouhé názvy hodin pro horizontální hlavičky */
    if(orientation == Qt::Horizontal && horizontalLessons)
        return QString::number(section) + tr(". hodina");

    /* Krátké názvy hodin pro vertikální hlavičky */
    else if(orientation == Qt::Vertical && !horizontalLessons)
        return QString::number(section) + tr(".");

    /* Dlouhé názvy dnů pro horizontální hlavičky */
    else if(orientation == Qt::Horizontal && !horizontalLessons) {
        switch(section) {
            case 0: return tr("Pondělí");
            case 1: return tr("Úterý");
            case 2: return tr("Středa");
            case 3: return tr("Čtvrtek");
            case 4: return tr("Pátek");
        }
    }

    /* Krátké názvy dnů pro vertikální hlavičky */
    else if(orientation == Qt::Vertical && horizontalLessons) {
        switch(section) {
            case 0: return tr("Po");
            case 1: return tr("Út");
            case 2: return tr("St");
            case 3: return tr("Čt");
            case 4: return tr("Pá");
        }
    }

    /* Něco jinýho */
    return QVariant();
}

/* Data rozvrhu */
QVariant TimetableModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid()) return QVariant();

    /* Číslo dne/hodiny */
    int dayhour;

    /* Hodiny horizontálně */
    if(horizontalLessons && index.row() < 5 && index.column() < 10)
        dayhour = index.row() << 4 | index.column();

    /* Hodiny vertikálně */
    else if(!horizontalLessons && index.row() < 10 && index.column() < 5)
        dayhour = index.column() << 4 | index.row();

    /* Nějakej mišmaš */
    else return QVariant();

    /* ID předmětu pod tímto číslem dne/hodiny */
    int id = timetableData[dayhour];

    /* Index odpovídající tomuto ID předmětu */
    int idx = classesModel->indexFromId(id);

    /* Vrácení textu */
    if(role == Qt::DisplayRole) {
        /* Žádný předmět tuto hodinu není */
        if(idx == -1) return QVariant();

        /* Text ve formátu: Předmět (učitel) */
        return classesModel->index(idx, 0).data().toString() + " (" +
                classesModel->index(idx, 1).data().toString() + ")";
    }

    /* Vrácení indexu předmětu pro editaci */
    if(role == Qt::EditRole)
        return idx;

    /* Něco jiného */
    return QVariant();
}

/* Flags */
Qt::ItemFlags TimetableModel::flags(const QModelIndex& index) const {
    if(!index.isValid() ||
        (horizontalLessons && (index.row() > 4 || index.column() > 9)) ||
        (!horizontalLessons && (index.row() > 9 || index.column() > 4)))
            return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

/* Zápisový přístup k datům */
bool TimetableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid() || role != Qt::EditRole) return false;

    int dayHour;

    /* Hodiny jsou na horizontální ose */
    if(horizontalLessons && index.row() < 5 && index.column() < 10)
        dayHour = index.row() << 4 | index.column();

    /* Hodiny jsou na vertikální ose */
    if(!horizontalLessons && index.row() < 10 && index.column() < 5)
        dayHour = index.column() << 4 | index.row();

    /* Nějakej mišmaš */
    else return false;

    QSqlQuery query;

    /* ID předmětu odpovídající aktuálnímu indexu */
    int idx = classesModel->idFromIndex(value.toInt());

    /* Jestli upravujeme stávající záznam, provedeme UPDATE */
    if(timetableData.contains(dayHour))
        query.prepare("UPDATE timetableData SET classId = :classId "
                      "WHERE dateHour = :dayHour "
                      "AND timetableId = :timetableId;");

    /* Jinak provádíme INSERT */
    else query.prepare("INSERT INTO timetableData (gradeId, timetableId, dateHour, classId) "
                       "VALUES (1, :timetableId, :dayHour, :classId);");

    /* Aktualizujeme lokální data */
    timetableData[dayHour] = idx;

    /* Naplnění dotazu daty */
    query.bindValue(":timetableId", timetableId);
    query.bindValue(":dayHour", dayHour);
    query.bindValue(":classId", timetableData[dayHour]);

    /* Provedení dotazu */
    if(!query.exec()) {
        qDebug() << tr("Nepodařilo se upravit data rozvrhu!") << query.lastError()
                 << query.lastQuery();
        return false;
    }

    emit dataChanged(index, index);
    return true;
}

/* Přehození směru */
void TimetableModel::switchDirection() {
    horizontalLessons = horizontalLessons ? false : true;

    /* Kompletně se změnila struktura, takže je lepší zavolat reset, než
        milionkrát vysílat dataChanged() a headerDataChanged() */
    reset();
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
            QModelIndex idx;
            if(horizontalLessons) idx = index((hour & 0xF0) >> 4, hour & 0x0F);
            else                  idx = index(hour & 0x0F, (hour & 0xF0) >> 4);

            /* Vyslání signálu */
            emit dataChanged(idx, idx);
        }
    }
}

}
