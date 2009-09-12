#ifndef CLASSESTAB_H
#define CLASSESTAB_H

#include <QWidget>

class QPushButton;
class QTableView;

namespace Absencoid {

class ClassesModel;
class TeachersModel;

/**
 * @brief Tab s předměty
 */
class ClassesTab: public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Konstruktor
         *
         * @param   teachersModel   Ukazatel na model učitelů
         * @param   parent          Rodičovský widget
         */
        ClassesTab(TeachersModel* teachersModel, QWidget* parent = 0);

        /**
         * @brief Vrácení ukazatele na model
         */
        ClassesModel* getClassesModel(void) const { return classesModel; }

    private:
        ClassesModel* classesModel;         /** @brief Model předmětů */
        QTableView* classesView;            /** @brief Tabulka předmětů */
        QPushButton* removeClassesButton;   /** @brief Tlačítko pro odstranění předmětů */

    private slots:
        /**
         * @brief Přidání předmětu
         */
        void addClass();

        /**
         * @brief Odebrání vybraných předmětů
         */
        void removeClasses();

        /**
         * @brief Zašedne / povolí tlačítko pro odebrání předmětů
         *
         * Volané při změně výběru. Pokud není nic vybrané, políčko je zašedlé.
         */
        void updateRemoveButton();
};

}

#endif
