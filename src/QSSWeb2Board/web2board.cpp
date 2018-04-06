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
            returnMessage.success="TRUE";
            qInfo() << "Received Version Command";

        }else if(messageHandler.action == MessageHandler::Action::VERIFY){
            arduino.writeSketch(messageHandler.sketch);
            returnMessage.action = ReturnMessage::Action::VERIFY;
            arduino.setBoardNameID(messageHandler.boardID);
            arduino.verify();
            returnMessage.success="TRUE";

        }else if (messageHandler.action == MessageHandler::Action::UPLOAD){
            arduino.writeSketch(messageHandler.sketch);
            returnMessage.action = ReturnMessage::Action::UPLOAD;
            arduino.setBoardNameID(messageHandler.boardID);
            arduino.verify();
            arduino.upload();
            returnMessage.success="TRUE";

        }else if (messageHandler.action == MessageHandler::Action::OPENSERIALMONITOR){
            returnMessage.action = ReturnMessage::Action::OPENSERIALMONITOR;
            arduino.setBoardNameID(messageHandler.boardID);
            arduino.autoDetectBoardPort();
            arduino.openSerialMonitor(messageHandler.baudrate);
            QObject::connect(arduino.serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(sendIncomingSerialToClient(QString)));
            returnMessage.success="TRUE";
        }else if (messageHandler.action == MessageHandler::Action::SENDSERIAL){
            returnMessage.action = ReturnMessage::Action::NONSET;
            arduino.serialMonitor->sendToArduino(messageHandler.serialMessage);

        }else if (messageHandler.action == MessageHandler::Action::CLOSESERIALMONITOR){
            returnMessage.action = ReturnMessage::Action::CLOSESERIALMONITOR;
            QObject::disconnect(arduino.serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(sendIncomingSerialToClient(QString)));
            arduino.closeSerialMonitor();
            returnMessage.success="TRUE";
        }

    }catch(SerialPortOpenException &e){
        qDebug()<<e.message;
        returnMessage.success="FALSE";
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(FileNotCreatedException &e){
        qDebug()<<e.message;
        returnMessage.success="FALSE";
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(BoardNotKnownException &e){
        qDebug()<<e.message;
        returnMessage.success="FALSE";
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(FileNotFoundException &e){
        qDebug()<<e.message;
        returnMessage.success="FALSE";
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(BoardNotDetectedException &e){
        qDebug()<<e.message;
        returnMessage.success="FALSE";
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(BoardNotSetException &e){
        qDebug()<<e.message;
        returnMessage.success="FALSE";
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(SketchNotSetException &e){
        qDebug()<<e.message;
        returnMessage.success="FALSE";
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(UploadException &e){
        qDebug()<<e.message;
        returnMessage.success="FALSE";
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(VerifyException &e){
        qDebug()<<e.message;
        returnMessage.success="FALSE";
        returnMessage.errorType=e.errorType;
        returnMessage.errorDesc=e.message;

    }catch(ArduinoNotFoundException &e){
    qDebug()<<e.message;
        returnMessage.success="FALSE";
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
