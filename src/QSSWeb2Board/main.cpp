#include <QCoreApplication>
#include <QProcess>
#include <QDebug>

#include "arduinohandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ArduinoHandler arduino;
    arduino.setBoardNameID("ZUMCore");
    arduino.setExecutableDir();

    arduino.setFilePath("/home/avalero/arduino-1.8.5/examples/01.Basics/Blink/");
    arduino.setFileName("Blink.ino");

    if(arduino.setBoardPort()){
        arduino.verify();
        arduino.load();
    }

    return 1; //a.exec();
}
