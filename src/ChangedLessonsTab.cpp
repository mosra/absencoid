#include "ChangedLessonsTab.h"

#include <QTableView>
#include <QPushButton>
#include <QBoxLayout>

#include "TimetableModel.h"
#include "ClassesModel.h"
#include "ChangedLessonsModel.h"
#include "ComboBoxDelegate.h"

namespace Absencoid {

/* Konstruktor */
ChangedLessonsTab::ChangedLessonsTab(TimetableModel* timetableModel, ClassesModel* classesModel, QWidget* parent): QWidget(parent) {

    /* Tabulka se změnami */
    QTableView* changedLessonsView = new QTableView;
    changedLessonsView->setModel(new ChangedLessonsModel(classesModel, timetableModel, this));

    /* Combobox delegáty pro výběr předmětů */
    ComboBoxDelegate* comboBoxDelegate = new ComboBoxDelegate(classesModel, changedLessonsView);
    changedLessonsView->setItemDelegateForColumn(2, comboBoxDelegate);
    changedLessonsView->setItemDelegateForColumn(3, comboBoxDelegate);

    /* Zvětšení posledního sloupce, protože má dlouhou hlavičku */
    changedLessonsView->resizeColumnToContents(4);

    /* Tlačítka vpravo */
    QPushButton* addChangedLessonButton = new QPushButton(tr("Přidat změnu"));
    QPushButton* removeLessonsButton = new QPushButton(tr("Odebrat vybrané"));

    /* Layout pro tlačítka vpravo */
    QVBoxLayout* buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(addChangedLessonButton, 0, Qt::AlignTop);
    buttonsLayout->addWidget(removeLessonsButton, 1, Qt::AlignTop);

    /* Layout celkový */
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(changedLessonsView, 1);
    layout->addLayout(buttonsLayout, 0);

    setLayout(layout);
}

/* Přidání změny */
void ChangedLessonsTab::addChangedLesson() {

}

/* Odstranění vybraných změn */
void ChangedLessonsTab::removeLessons() {

}

}
