#include <QCoreApplication>
#include <QProcess>
#include <QDebug>

#include "sslserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ArduinoHandler arduino;
    arduino.setSketchWithAbsolutePath("/home/avalero/arduino-1.8.5/examples/01.Basics/Blink/Blink.ino");
    return 1;
    qDebug() << "here";
    SSLServer server(1234);
    Q_UNUSED(server);
    return a.exec();
}
