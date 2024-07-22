#include "stdafx.h"
#include "TransMaster.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSettings settings("TransMaster.ini", QSettings::IniFormat);
    TransMaster w(settings);
    w.show();
    return a.exec();
}
