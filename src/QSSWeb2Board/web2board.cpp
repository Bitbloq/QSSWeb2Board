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


void Web2Board::processCommands(){
    try{
        arduino.writeSketch(messageHandler.sketch);
        if(messageHandler.action == MessageHandler::Action::VERIFY){
            m_pClient->sendTextMessage("VERIFYNG");
            m_pClient->flush();
            arduino.verify(messageHandler.boardID);
            m_pClient->sendTextMessage("VERIFICATION FINISHED CORRECTLY");
            m_pClient->flush();
        }else if (messageHandler.action == MessageHandler::Action::UPLOAD){
            m_pClient->sendTextMessage("VERIFYNG");
            m_pClient->flush();
            arduino.verify(messageHandler.boardID);
            m_pClient->sendTextMessage("VERIFICATION FINISHED CORRECTLY");
            m_pClient->flush();
            m_pClient->sendTextMessage("UPLOADING");
            m_pClient->flush();
            arduino.upload(messageHandler.boardID);
            m_pClient->sendTextMessage("SKETCH UPLOADED TO BOARD");
            m_pClient->flush();
        }
    }catch(FileNotCreatedException &e){
        qDebug()<<e.message;
        m_pClient->sendTextMessage(e.message);
        //TODO
    }catch(BoardNotKnownException &e){
        qDebug()<<e.message;
        m_pClient->sendTextMessage(e.message);
        //TODO
    }catch(FileNotFoundException &e){
        qDebug()<<e.message;
        m_pClient->sendTextMessage(e.message);
        //TODO
    }catch(BoardNotDetectedException &e){
        qDebug()<<e.message;
        m_pClient->sendTextMessage(e.message);
        //TODO
    }catch(UploadException &e){
        qDebug()<<e.message;
        m_pClient->sendTextMessage(e.message);
        //TODO
    }catch(VerifyException &e){
        qDebug()<<e.message;
        m_pClient->sendTextMessage(e.message);
        //TODO
    }
    m_pClient->flush();


}



void Web2Board::processTextMessage(QString message)
{
    m_pClient = qobject_cast<QWebSocket *>(sender());
    messageHandler.handle(message);
    processCommands();
}
