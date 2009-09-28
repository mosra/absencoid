#ifndef ABSENCOID_ABSENCESTAB_H
#define ABSENCOID_ABSENCESTAB_H

#include <QWidget>

class QTableView;
class QPushButton;

namespace Absencoid {

class AbsencesModel;
class ClassesModel;
class TimetableModel;
class ChangesModel;

class AbsencesTab: public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Konstruktor
         */
        AbsencesTab(ClassesModel* classesModel, TimetableModel* timetablesModel, ChangesModel* changesModel, QWidget* parent = 0);

    private:
        AbsencesModel* absencesModel;       /** @brief Model absencí */
        QTableView* absencesView;           /** @brief Tabulka absencí */
        QPushButton* removeSelectedButton;  /** @brief Tlačítko pro odstranění vybraných absencí */

    private slots:
        /**
         * @brief Přidání absence
         */
        void addAbsence();

        /**
         * @brief Odstranění vybraných absencí
         */
        void removeSelected();

        /**
         * @brief Zašednutí mazacího tlačítka, pokud není nic vybrané
         */
        void updateRemoveButton();
};

}

#endif
