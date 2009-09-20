#include "ChangedLessonsTab.h"

#include <QTableView>
#include <QPushButton>
#include <QBoxLayout>

#include "TimetableModel.h"
#include "ClassesModel.h"

namespace Absencoid {

/* Konstruktor */
ChangedLessonsTab::ChangedLessonsTab(TimetableModel* timetableModel, ClassesModel* classesModel, QWidget* parent): QWidget(parent) {

    /* Tabulka se změnami */
    QTableView* changedLessonsView = new QTableView;

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
