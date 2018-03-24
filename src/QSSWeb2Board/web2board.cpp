#include "web2board.h"
#include <QtWebSockets/QWebSocket>

#include "commsprotocol.h"
#include "messagehandler.h"


Web2Board::Web2Board(QObject *parent):
    QObject(parent),
    serialMonitor(Q_NULLPTR)
{    try{
        //arduino.setFileWithFullPath("/home/avalero/arduino-1.8.5/examples/01.Basics/Blink/Blink.ino");
        //arduino.setFileWithFullPath("/home/avalero/workspace/QSSWeb2Board/src/build-QSSWeb2Board-Desktop-Debug/res/arduino/hardware/BQ/avr/libraries/BQZUMI2C7SegmentDisplay/examples/7Segment_Characters/7Segment_Characters.ino");
        //arduino.setFileWithFullPath("/home/avalero/arduino-1.8.5/examples/error/error.ino");
    }catch(FileNotFoundException &e){
        qDebug() << e.message;
    }

}

Web2Board::~Web2Board(){
    if(serialMonitor){
        delete serialMonitor;
    }
}


void Web2Board::processCommands(){

    ReturnMessage returnMessage;

    try{
        arduino.writeSketch(messageHandler.sketch);
        if(messageHandler.action == MessageHandler::Action::VERIFY){
            returnMessage.action = ReturnMessage::Action::VERIFY;
            arduino.verify(messageHandler.boardID);
            returnMessage.success="TRUE";

        }else if (messageHandler.action == MessageHandler::Action::UPLOAD){
            returnMessage.action = ReturnMessage::Action::UPLOAD;
            arduino.verify(messageHandler.boardID);
            arduino.upload(messageHandler.boardID);
            returnMessage.success="TRUE";
            returnMessage.action = ReturnMessage::Action::UPLOAD;
        }else if (messageHandler.action == MessageHandler::Action::OPENSERIALMONITOR){
            returnMessage.action = ReturnMessage::Action::OPENSERIALMONITOR;
            arduino.setBoardNameID(messageHandler.boardID);
            arduino.setBoardPort();
            serialMonitor = new ArduinoSerialMonitor(arduino.getBoardPort(),messageHandler.baudrate);
            serialMonitor->open();
            QObject::connect(serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(sendIncomingSerialToClient(QString)));
            returnMessage.success="TRUE";
        }else if (messageHandler.action == MessageHandler::Action::SENDSERIAL){
            returnMessage.action = ReturnMessage::Action::NONSET;
            serialMonitor->sendToArduino(messageHandler.serialMessage);

        }else if (messageHandler.action == MessageHandler::Action::CLOSESERIALMONITOR){
            returnMessage.action = ReturnMessage::Action::CLOSESERIALMONITOR;
            QObject::disconnect(serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(sendIncomingSerialToClient(QString)));

            if(serialMonitor){
                serialMonitor->close();
                delete serialMonitor;
                serialMonitor = Q_NULLPTR;
            }

            returnMessage.success="TRUE";
        }

    }catch(SerialPortOpenException &e){
        qDebug()<<e.message;
        returnMessage.success="FALSE";
        returnMessage.errorType=CommsProtocol::SERIAL_PORT_NOT_OPEN;
        returnMessage.errorDesc=e.message;

    }catch(FileNotCreatedException &e){
        qDebug()<<e.message;
        returnMessage.success="FALSE";
        returnMessage.errorType=CommsProtocol::FILE_NOT_CREATED_ERROR;
        returnMessage.errorDesc=e.message;

    }catch(BoardNotKnownException &e){
        qDebug()<<e.message;
        returnMessage.success="FALSE";
        returnMessage.errorType=CommsProtocol::BOARD_NOT_KNOWN_ERROR;
        returnMessage.errorDesc=e.message;

    }catch(FileNotFoundException &e){
        qDebug()<<e.message;
        returnMessage.success="FALSE";
        returnMessage.errorType=CommsProtocol::FILE_NOT_FOUND_ERROR;
        returnMessage.errorDesc=e.message;

    }catch(BoardNotDetectedException &e){
        qDebug()<<e.message;
        returnMessage.success="FALSE";
        returnMessage.errorType=CommsProtocol::BOARD_NOT_DETECTED_ERROR;
        returnMessage.errorDesc=e.message;

    }catch(UploadException &e){
        qDebug()<<e.message;
        returnMessage.success="FALSE";
        returnMessage.errorType=CommsProtocol::UPLOAD_ERROR;
        returnMessage.errorDesc=e.message;

    }catch(VerifyException &e){
        qDebug()<<e.message;
        returnMessage.success="FALSE";
        returnMessage.errorType=CommsProtocol::VERIFY_ERROR;
        returnMessage.errorDesc=e.getErrorsLists();
    }

    m_pClient->sendTextMessage(returnMessage.makeReturnMessage());
    m_pClient->flush();
}



void Web2Board::processTextMessage(QString message)
{
    m_pClient = qobject_cast<QWebSocket *>(sender());
    messageHandler.handle(message);
    processCommands();
}

void Web2Board::sendIncomingSerialToClient(QString message){
    QString msg = CommsProtocol::SERIALMESSAGE.first + message + CommsProtocol::SERIALMESSAGE.second;
    m_pClient->sendTextMessage(msg.toLatin1());
}
