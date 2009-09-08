#ifndef TEACHERSTAB_H
#define TEACHERSTAB_H

#include <QWidget>

namespace Absencoid {

class TeachersModel;

/**
 * @brief Tab s daty učitelů
 */
class TeachersTab: public QWidget {
    public:
        /**
         * @brief Konstruktor
         */
        TeachersTab(QWidget* parent = 0);

    private:
        TeachersModel* teachersModel;   /** @brief Model s daty učitelů */
};

}

#endif
