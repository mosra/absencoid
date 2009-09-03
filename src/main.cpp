#include <QApplication>
#include <QTextCodec>

#include "MainWindow.h"

using namespace Absencoid;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    MainWindow w;
    w.show();

    return app.exec();
}
