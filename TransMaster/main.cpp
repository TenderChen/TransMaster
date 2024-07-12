#include "stdafx.h"
#include "TransMaster.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TransMaster w;
    w.show();
    return a.exec();
}
