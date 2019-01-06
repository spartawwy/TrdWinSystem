#include "auto_forcast_server.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto_forcast_server w;
    w.show();
    return a.exec();
}
