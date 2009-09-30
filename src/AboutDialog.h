#ifndef ABSENCOID_ABOUTDIALOG_H
#define ABSENCOID_ABOUTDIALOG_H

#include <QDialog>

namespace Absencoid {

class AboutDialog: public QDialog {
    public:
        /**
         * @brief Konstruktor
         */
        AboutDialog(QWidget* parent = 0);
};

}

#endif
