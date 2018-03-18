#include "web2board.h"
#include <QtWebSockets/QWebSocket>

#include "commsprotocol.h"


Web2Board::Web2Board(QObject *parent):
    QObject(parent)
{    try{
        //arduino.setFileWithFullPath("/home/avalero/arduino-1.8.5/examples/01.Basics/Blink/Blink.ino");
        //arduino.setFileWithFullPath("/home/avalero/workspace/QSSWeb2Board/src/build-QSSWeb2Board-Desktop-Debug/res/arduino/hardware/BQ/avr/libraries/BQZUMI2C7SegmentDisplay/examples/7Segment_Characters/7Segment_Characters.ino");
        //arduino.setFileWithFullPath("/home/avalero/arduino-1.8.5/examples/error/error.ino");
    }catch(FileNotFoundException &e){
        qDebug() << e.message;
    }

}

/*
void Web2Board::manageVerifyCommand(QWebSocket* pClient){
    if (pClient)
    {
        pClient->sendTextMessage("VERIFYING");
        pClient->flush();
        try{
            arduino.verify("ZUMJunior");
        }catch(FileNotFoundException &e){
            qDebug() << e.message;
            pClient->sendTextMessage("SKETCH_NOT_FOUND " + e.message);
            pClient->flush();
            return;
        }catch(BoardNotKnownException &e){
            qDebug() << e.message;
            pClient->sendTextMessage("BOARD_NOT_KNOWN " + e.message);
            pClient->flush();
            return;
        }catch(BoardNotDetectedException &e){
            qDebug() << e.message;
            pClient->sendTextMessage("BOARD_NOT_DETECTED " + e.message);
            pClient->flush();
            return;
        }catch(VerifyException &e){
            qDebug() << "Found " << e.errorsList.size() << " verification errors";
            pClient->sendTextMessage("VERFICATION_ERROR");
            pClient->sendTextMessage("Found " + QString(e.errorsList.size()) + " verification errors");
            for(int i=0;i<e.errorsList.size();i++){
                qDebug() << i + 1 << ".- " << e.errorsList.at(i);
                pClient->sendTextMessage(QString(i + 1) + ".- " + e.errorsList.at(i));
            }
            pClient->flush();
            return;
        }
        pClient->sendTextMessage("VERIFICATION_OK");
    }
}

void Web2Board::manageUploadCommand(QWebSocket* pClient){
    if (pClient)
    {
        pClient->sendTextMessage(QStringLiteral("UPLOADING"));
        pClient->flush();
        try{
            arduino.upload("ZUMJunior");
        }catch(FileNotFoundException &e){
            qDebug() << e.message;
            pClient->sendTextMessage(QStringLiteral("SKETCH_NOT_FOUND ") + e.message);
            pClient->flush();
            return;
        }catch(BoardNotKnownException &e){
            qDebug() << e.message;
            pClient->sendTextMessage(QStringLiteral("BOARD_NOT_KNOWN ") + e.message);
            pClient->flush();
            return;
        }catch(BoardNotDetectedException &e){
            qDebug() << e.message;
            pClient->sendTextMessage(QStringLiteral("BOARD_NOT_DETECTED ") + e.message);
            pClient->flush();
            return;
        }catch(VerifyException &e){
            qDebug() << "Found " << e.errorsList.size() << " verification errors";
            pClient->sendTextMessage(QStringLiteral("VERFICATION_ERROR"));
            pClient->sendTextMessage(QStringLiteral("Found ") + QString(e.errorsList.size()) + QStringLiteral(" verification errors"));
            for(int i=0;i<e.errorsList.size();i++){
                qDebug() << i + 1 << ".- " << e.errorsList.at(i);
                pClient->sendTextMessage(QString(i + 1) + ".- " + e.errorsList.at(i));
            }
            pClient->flush();
            return;
        }catch(UploadException &e){
            qDebug() << e.message;
            pClient->sendTextMessage(QStringLiteral("UPLOAD_ERROR"));
            pClient->flush();
            return;
        }


        pClient->sendTextMessage(QStringLiteral("UPLOAD_OK"));
        pClient->flush();
    }
}
*/

void Web2Board::processCommands(){
//    arduino.setBoardNameID(data.boardID);
//    arduino.setSketch(data.sketch);
}

void Web2Board::verify(){}

void Web2Board::upload(){}



void Web2Board::processTextMessage(QString message)
{
    m_pClient = qobject_cast<QWebSocket *>(sender());
    messageHandler.handle(message);
    processCommands();
    if (messageHandler.action == MessageHandler::Action::VERIFY){
        verify();
    }else if (messageHandler.action == MessageHandler::Action::UPLOAD){
        upload();
    }else{
        m_pClient->sendTextMessage("UNKNOWN");
    }
}
