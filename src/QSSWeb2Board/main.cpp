#include <QCoreApplication>
#include <QProcess>
#include <QDebug>

#include "arduinohandler.h"
#include "arduinoexceptions.h"
#include "sslserver.h"
#include "web2board.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    QString ms = "<VERIFY><SKETCH>Esto es un sketch</SKETCH><BOARD>Board boardID</BOARD></VERIFY>";

    MessageHandler msHandler;
    msHandler.handle(ms);
    if (msHandler.action == MessageHandler::Action::VERIFY)
        qDebug() << QString("Verify");
    qDebug() << msHandler.sketch;
    qDebug() << msHandler.boardID;

    return 1;

    //Web2Board * web2board = new Web2Board;
    SSLServer server(1234);

    Q_UNUSED(server);

    return a.exec();

    /*

    try{
        arduino.setFileWithFullPath("/home/avalero/arduino-1.8.5/examples/01.Basics/Blink/Blink.ino");
        arduino.setFileWithFullPath("/home/avalero/workspace/QSSWeb2Board/src/build-QSSWeb2Board-Desktop-Debug/res/arduino/hardware/BQ/avr/libraries/BQZUMI2C7SegmentDisplay/examples/7Segment_Characters/7Segment_Characters.ino");
        //arduino.setFileWithFullPath("/home/avalero/arduino-1.8.5/examples/error/error.ino");

        arduino.verify("ZUMJunior");
        arduino.upload("ZUMJunior");
    }catch(FileNotFoundException &e){
        qDebug() << e.message;
    }catch(BoardNotKnownException &e){
        qDebug() << e.getMessage();
    }catch(BoardNotDetectedException &e){
        qDebug() << e.getMessage();
    }catch(VerifyException &e){
        qDebug() << "Found " << e.errorsList.size() << " verification errors";
        for(int i=0;i<e.errorsList.size();i++){
            qDebug() << i + 1 << ".- " << e.errorsList.at(i);
        }
    }catch(UploadException &e){
        qDebug() << e.message;
    }
*/
    return 1; //a.exec();
}
