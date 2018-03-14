#include <QCoreApplication>
#include <QProcess>
#include <QDebug>

#include "arduinohandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ArduinoHandler arduino;

    arduino.setFilePath("/home/avalero/arduino-1.8.5/examples/01.Basics/Blink/");
    arduino.setFileName("Blink.ino");
    //arduino.setBoardNameID("ZUMJunior");
    //arduino.setBoardPort();

    //if(arduino.setBoardPort()){
        arduino.verify("ZUMJunior");
        arduino.load("ZUMJunior");
    //}

    return 1; //a.exec();
}
