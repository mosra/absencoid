#ifndef ABSENCOID_CHANGEDLESSONSTAB_H
#define ABSENCOID_CHANGEDLESSONSTAB_H

#include <QWidget>

class QTableView;
class QPushButton;

namespace Absencoid {

class ChangedLessonsModel;
class ClassesModel;
class TimetableModel;

/**
 * @brief Tab pro zapisování změněných hodin
 * @todo Globální přístup k aktuálnímu rozvrhu? Nebo vybírat rozvrh u každé změny?
 *      - což povede k vícenásobnému otevření rozvrhu a nekonzistenci dat
 *      - hierarchické uspořádání rozvrhů? Otevřeny všechny? => Zrušit TimetableListModel?
 */
class ChangedLessonsTab: public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Konstruktor
         */
        ChangedLessonsTab(TimetableModel* timetableModel, ClassesModel* classesModel, QWidget* parent = 0);

    private:
        ChangedLessonsModel* changedLessonsModel;   /** @brief Model změněných hodin */
        QTableView* changedLessonsView;             /** @brief Tabulka se změnami */
        QPushButton* removeChangedLessonsButton;    /** @brief Tlačítko pro odebrání změn */

    private slots:
        /**
         * @brief Přidat změněnou hodinu
         */
        void addChangedLesson();

        /**
         * @brief Smazání vybraných změn
         */
        void removeChangedLessons();

        /**
         * @brief Zašednutí / aktivace tlačítka pro mazání
         *
         * Pokud není nic vybráno, tlačítko pro mazání je zašedlé.
         */
        void updateRemoveButton();
};

}

#endif
