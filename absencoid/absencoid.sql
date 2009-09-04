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
-- validTo      Datum konce platnosti (ten den je ještě platný)
-- followedBy   ID rozvrhu, který tento po konci platnosti nahradí
CREATE TABLE timetables (
    gradeId INTEGER NOT NULL,
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    description TEXT NOT NULL UNIQUE,
    validFrom TEXT NOT NULL,
    validTo TEXT,
    followedBy INTEGER
);

-- DATA ROZVRHŮ
-- gradeId      ID třídy
-- timetableId  ID rozvrhu
-- dateHour     Den v týdnu a hodina (binárně oddělitelné)
--                  0x0F - den v týdnu (0x00 = pondělí)
--                  0xF0 - hodina (0x10 = nultá hodina)
-- classId      ID předmětu
CREATE TABLE timetableData (
    gradeId INTEGER NOT NULL,
    timetableId INTEGER NOT NULL,
    dateHour INTEGER NOT NULL,
    classId INTEGER NOT NULL
);

-- VOLNÉ DNY / ODPADNUTÉ HODINY
-- gradeId      ID třídy
-- date         Datum
-- hours        Hodiny (binární "checkbox") - příklad:
--                  0x0001 - nultá hodina odpadla
--                  0x00F0 == 0b11110000 - odpadla 4. - 7. hodina
CREATE TABLE holidays (
    gradeId INTEGER NOT NULL,
    date TEXT NOT NULL UNIQUE,
    hours INTEGER NOT NULL
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
