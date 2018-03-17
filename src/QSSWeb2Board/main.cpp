#include <QCoreApplication>
#include <QProcess>
#include <QDebug>

#include "arduinohandler.h"
#include "arduinoexceptions.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ArduinoHandler arduino;

    arduino.setFilePath("/home/avalero/arduino-1.8.5/examples/01.Basics/Blink/");
    arduino.setFileName("Blink.ino");

    arduino.setFilePath("/home/avalero/arduino-1.8.5/examples/error/");
    arduino.setFileName("error.ino");


    try{
        arduino.setBoardNameID("ZUMCore");
        arduino.setBoardPort();
        arduino.verify("ZUMCore");
    }catch(BoardNotKnownException &e){
        qDebug() << e.getMessage();
    }catch(BoardNotDetectedException &e){
        qDebug() << e.getMessage();
    }catch(VerifyException &e){
        qDebug() << e.getMessage();
    }


    //arduino.upload("ZUMJunior");

    return 1; //a.exec();
}
