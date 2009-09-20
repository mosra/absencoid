#ifndef ABSENCOID_CHANGEDLESSONSTAB_H
#define ABSENCOID_CHANGEDLESSONSTAB_H

#include <QWidget>

namespace Absencoid {

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

    private slots:
        /**
         * @brief Přidat změněnou hodinu
         */
        void addChangedLesson();

        /**
         * @brief Smazání vybraných změn
         */
        void removeLessons();
};

}

#endif
