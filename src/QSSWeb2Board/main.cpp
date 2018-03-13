#include <QCoreApplication>
#include <QProcess>
#include <QDebug>

#include "arduinohandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ArduinoHandler arduino;
    arduino.setBoardName("hola");
    arduino.setExecutableDir("cara de bola");
    arduino.setFileName("uyyy");
    arduino.setSourceDir("Vaya hombre");
    qDebug() << "Lanzo Arduino";
    arduino.verify();
    //arduino.load();
    return a.exec();
}
