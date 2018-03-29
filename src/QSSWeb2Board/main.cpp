#include <QCoreApplication>
#include <QProcess>
#include <QDebug>
#include <QtTest/QtTest>

#include "sslserver.h"

ArduinoHandler* Arduino(){
    #if (defined (Q_OS_WIN))
        WindowsArduinoHandler *arduino = new WindowsArduinoHandler();
    #elif (defined (Q_OS_LINUX))
        LinuxArduinoHandler * arduino = new LinuxArduinoHandler();
    #elif (defined (Q_OS_MAC))
        MacArduinoHandler *arduino = new MacArduinoHandler();
    #endif
    return arduino;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    SSLServer server(1234);
    Q_UNUSED(server);
    return a.exec();
}
