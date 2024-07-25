#include "stdafx.h"
#include "TransMaster.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, QObject::tr("Systray"),
            QObject::tr("I couldn't detect any system tray "
                "on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    QSettings settings("TransMaster.ini", QSettings::IniFormat);
    TransMaster w(settings);
    w.show();
    return a.exec();
}