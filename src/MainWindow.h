#ifndef ABSENCOID_MAINWINDOW_H
#define ABSENCIOD_MAINWINDOW_H

#include <QMainWindow>

class QTabWidget;

namespace Absencoid {

class MainWindow: public QMainWindow {
    Q_OBJECT

    public:
        /**
         * @brief Konstruktor
         */
        MainWindow();

    private:
        QTabWidget* tabWidget;
};

};

#endif