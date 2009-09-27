#ifndef ABSENCOID_CHANGESTAB_H
#define ABSENCOID_CHANGESTAB_H

#include <QWidget>

class QTableView;
class QPushButton;

namespace Absencoid {

class ChangesModel;
class ClassesModel;
class TimetableModel;

/**
 * @brief Tab pro zapisování změněných hodin
 * @todo Globální přístup k aktuálnímu rozvrhu? Nebo vybírat rozvrh u každé změny?
 *      - což povede k vícenásobnému otevření rozvrhu a nekonzistenci dat
 *      - hierarchické uspořádání rozvrhů? Otevřeny všechny? => Zrušit TimetableListModel?
 */
class ChangesTab: public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Konstruktor
         */
        ChangesTab(TimetableModel* timetableModel, ClassesModel* classesModel, QWidget* parent = 0);

    private:
        ChangesModel* changesModel;         /** @brief Model změněných hodin */
        QTableView* changesView;            /** @brief Tabulka se změnami */
        QPushButton* removeChangesButton;   /** @brief Tlačítko pro odebrání změn */

    private slots:
        /**
         * @brief Přidat změněnou hodinu
         */
        void addChange();

        /**
         * @brief Smazání vybraných změn
         */
        void removeChanges();

        /**
         * @brief Zašednutí / aktivace tlačítka pro mazání
         *
         * Pokud není nic vybráno, tlačítko pro mazání je zašedlé.
         */
        void updateRemoveButton();
};

}

#endif
