#include <QCoreApplication>
#include <QProcess>
#include <QDebug>

#include "sslserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ArduinoHandler arduino;
    try{
        arduino.setBoardNameID("ZUMCore");
        arduino.autoDetectBoardPort();
    }catch(BoardNotDetectedException e){
        qDebug()<< e.message;
    }
    return 1;

    SSLServer server(1234);
    Q_UNUSED(server);
    return a.exec();
}
