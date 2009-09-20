-- TŘÍDY
-- id           ID třídy
-- name         Název třídy
CREATE TABLE grades (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE
);

-- UČITELÉ
-- gradeId      ID třídy (stejný učitel může mít v různých třídách různé
--                  vlastnosti)
-- id           ID učitele
-- name         Jméno (unikátní)
-- flags        Vlastnosti (binárně spojovatelné)
--                  1 - zapisuje absence
--                  2 - uznává školní akce
CREATE TABLE teachers (
    gradeId INTEGER NOT NULL,
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    flags INTEGER NOT NULL DEFAULT 1
);

-- PŘEDMĚTY
-- gradeId      ID třídy
-- id           ID předmětu
-- name         Název předmětu
-- teacherId    ID učitele (unikátní s názvem předmětu)
-- flags        Vlastnosti (rezervováno pro pozdější použití)
CREATE TABLE classes (
    gradeId INTEGER NOT NULL,
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    teacherId INTEGER NOT NULL,
    flags INTEGER NOT NULL DEFAULT 0,
    UNIQUE (name, teacherId)
);

-- ROZVRHY
-- gradeId      ID třídy
-- id           ID rozvrhu
-- description  Popisek rozvrhu
-- validFrom    Datum začátku platnosti
-- followedBy   ID rozvrhu, který tento po konci platnosti nahradí
CREATE TABLE timetables (
    gradeId INTEGER NOT NULL,
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    description TEXT NOT NULL UNIQUE,
    validFrom TEXT NOT NULL,
    followedBy INTEGER
);

-- DATA ROZVRHŮ
-- gradeId      ID třídy
-- timetableId  ID rozvrhu
-- dayHour      Den v týdnu a hodina (binárně oddělitelné)
--                  0xF0 - den v týdnu (0x10 = úterý)
--                  0x0F - hodina (0x02 = druhá hodina)
-- classId      ID předmětu
CREATE TABLE timetableData (
    gradeId INTEGER NOT NULL,
    timetableId INTEGER NOT NULL,
    dayHour INTEGER NOT NULL,
    classId INTEGER NOT NULL
);

-- ZMĚNĚNÉ HODINY / ODPADNUTÉ HODINY
-- gradeId      ID třídy
-- date         Datum
-- hour         Hodina, ve které proběhla změna
-- fromClassId  ID předmětu, ze kterého se mění. Tato změna se aplikuje jen na
--                  rozvrhy, které mají v danou dobu tuto hodinu.
-- toClassId    ID předmětu, na který se mění
CREATE TABLE changedLessons (
    gradeId INTEGER NOT NULL,
    date TEXT NOT NULL,
    hour INTEGER NOT NULL,
    fromClassId INTEGER NOT NULL,
    toClassId INTEGER NOT NULL,
    UNIQUE(date, hour, fromClassId)
);

-- ABSENCE
-- gradeId      ID třídy
-- date         Datum
-- hours        Hodiny (binární "checkbox") - příklad:
--                  0x0001 - absence nultou hodinu
--                  0x00F0 == 0b11110000 - absence 4. - 7. hodinu
CREATE TABLE absences (
    gradeId INTEGER NOT NULL,
    date TEXT NOT NULL UNIQUE,
    hours INTEGER NOT NULL
);
