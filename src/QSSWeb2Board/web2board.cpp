#include "web2board.h"
#include <QtWebSockets/QWebSocket>

#include "commsprotocol.h"
#include "messagehandler.h"


Web2Board::Web2Board(QObject *parent):
    QObject(parent)
{
}

Web2Board::~Web2Board(){
}


void Web2Board::processCommands(){

    ReturnMessage returnMessage;

    try{


        if(messageHandler.action == MessageHandler::Action::VERSION){
            returnMessage.action = ReturnMessage::Action::VERSION;
            qInfo() << "Received Version Command";

        }else if(messageHandler.action == MessageHandler::Action::VERIFY){    
            returnMessage.action = ReturnMessage::Action::VERIFY;
            //Send to client WORKING Status
            returnMessage.status = ReturnMessage::Status::WORKING;
            m_pClient->sendTextMessage(returnMessage.makeReturnMessage());
            m_pClient->flush();

            arduino.writeSketch(messageHandler.sketch);
            arduino.setBoardNameID(messageHandler.boardID);
            arduino.verify();

        }else if (messageHandler.action == MessageHandler::Action::UPLOAD){
            returnMessage.action = ReturnMessage::Action::UPLOAD;
            //Send to client WORKING Status
            returnMessage.status = ReturnMessage::Status::WORKING;
            m_pClient->sendTextMessage(returnMessage.makeReturnMessage());
            m_pClient->flush();

            arduino.writeSketch(messageHandler.sketch);
            arduino.setBoardNameID(messageHandler.boardID);
            arduino.verify();
            arduino.upload();

        }else if (messageHandler.action == MessageHandler::Action::OPENSERIALMONITOR){
            returnMessage.action = ReturnMessage::Action::OPENSERIALMONITOR;           
            arduino.setBoardNameID(messageHandler.boardID);
            arduino.autoDetectBoardPort();
            arduino.openSerialMonitor(messageHandler.baudrate);
            QObject::connect(arduino.serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(sendIncomingSerialToClient(QString)));

        }else if (messageHandler.action == MessageHandler::Action::SENDSERIAL){
            returnMessage.action = ReturnMessage::Action::NONSET;
            qDebug() << "Serial Message" << messageHandler.serialMessage;
            arduino.serialMonitor->sendToArduino(messageHandler.serialMessage);

        }else if (messageHandler.action == MessageHandler::Action::CLOSESERIALMONITOR){
            returnMessage.action = ReturnMessage::Action::CLOSESERIALMONITOR;
            QObject::disconnect(arduino.serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(sendIncomingSerialToClient(QString)));
            arduino.closeSerialMonitor();
        }

        returnMessage.status=ReturnMessage::Status::SUCCESS;

    }catch(FileNotCreatedException &e){
        qDebug()<<e.message;
        returnMessage.status=ReturnMessage::Status::FAIL;
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;
    }catch(DirNotCreatedException &e){
        qDebug()<<e.message;
        returnMessage.status=ReturnMessage::Status::FAIL;
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(BoardNotKnownException &e){
        qDebug()<<e.message;
        returnMessage.status=ReturnMessage::Status::FAIL;
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(FileNotFoundException &e){
        qDebug()<<e.message;
        returnMessage.status=ReturnMessage::Status::FAIL;
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(BoardNotDetectedException &e){
        qDebug()<<e.message;
        returnMessage.status=ReturnMessage::Status::FAIL;
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(BoardNotSetException &e){
        qDebug()<<e.message;
        returnMessage.status=ReturnMessage::Status::FAIL;
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(SketchNotSetException &e){
        qDebug()<<e.message;
        returnMessage.status=ReturnMessage::Status::FAIL;
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(UploadException &e){
        qDebug()<<e.message;
        returnMessage.status=ReturnMessage::Status::FAIL;
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(VerifyException &e){
        qDebug()<<e.message;
        returnMessage.status=ReturnMessage::Status::FAIL;
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(ArduinoNotFoundException &e){
        qDebug()<<e.message;
        returnMessage.status=ReturnMessage::Status::FAIL;
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;
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
