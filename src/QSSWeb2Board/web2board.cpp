#include "web2board.h"
#include <QtWebSockets/QWebSocket>

#include "commsprotocol.h"


Web2Board::Web2Board(QObject *parent):
    QObject(parent)
{
}

Web2Board::~Web2Board(){
}


void Web2Board::processCommands(){

    QJsonObject reply;
    reply.insert("ID",jsonMessage.value("ID").toInt());
    reply.insert("hub",jsonMessage.value("hub").toString());

    QString function = jsonMessage.value("function").toString();

    try{
        if(function == CommsProtocol::VERSION){
            reply.insert("reply",QJsonValue("2.1.3"));

        }else if(function == CommsProtocol::LIBVERSION){
            reply.insert("reply",QJsonValue());

        }else if(function == CommsProtocol::VERIFY){
            //Send to client VERIFYING Status
            reply.insert("function",QJsonValue(CommsProtocol::VERIFYING));
            reply.insert("args",QJsonValue(QJsonArray()));

            m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
            m_pClient->flush();

            //write skectch stored in jsonMessage.value("args").toArray().at(0)
            arduino.writeSketch(jsonMessage.value("args").toArray().at(0).toString());
            arduino.setBoardNameID(jsonMessage.value("args").toArray().at(1).toString());

            arduino.verify();

            reply.insert("function",CommsProtocol::VERIFY);
            reply.insert("reply",QJsonValue(true));

        }else if (function == CommsProtocol::UPLOAD){

            arduino.writeSketch(jsonMessage.value("args").toArray().at(0).toString());
            arduino.setBoardNameID(jsonMessage.value("args").toArray().at(1).toString());

            //SEND VERIFYING
            reply.insert("function",QJsonValue(CommsProtocol::VERIFYING));
            reply.insert("args",QJsonValue(QJsonArray()));

            m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
            m_pClient->flush();

            arduino.verify();

            //SEND UPLOADING
            arduino.autoDetectBoardPort();
            QString port = arduino.getBoardPort();

            reply.insert("function",QJsonValue(CommsProtocol::UPLOADING));
            reply.insert("args",QJsonValue(QJsonArray({port})));
            m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
            m_pClient->flush();

            arduino.upload();

            reply.insert("function",QJsonValue(CommsProtocol::UPLOAD));
            reply.insert("reply",QJsonValue(port));
/*
        }else if (function == CommsProtocol::OPENSERIALMONITOR){

            arduino.setBoardNameID(jsonMessage.value("args").toArray().at(1).toString());
            arduino.autoDetectBoardPort();
            //TOD -> arduino.openSerialMonitor(messageHandler.baudrate);
            QObject::connect(arduino.serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(sendIncomingSerialToClient(QString)));

        }else if (function == CommsProtocol::SENDSERIAL){

            // -> TODO qDebug() << "Serial Message" << messageHandler.serialMessage;
            //arduino.serialMonitor->sendToArduino(messageHandler.serialMessage);

        }else if (function == CommsProtocol::CLOSESERIALMONITOR){
            returnMessage.action = ReturnJSONMessage::Action::CLOSESERIALMONITOR;
            QObject::disconnect(arduino.serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(sendIncomingSerialToClient(QString)));
            arduino.closeSerialMonitor();
*/
        }else{
            //UNKNOWN MESSAGE
            return;
        }

        //success response
        reply.insert("success", QJsonValue(true));

    }catch(FileNotCreatedException &e){
        qDebug()<<e.message;

    }catch(DirNotCreatedException &e){
        qDebug()<<e.message;


    }catch(BoardNotKnownException &e){
        qDebug()<<e.message;

    }catch(FileNotFoundException &e){
        qDebug()<<e.message;

    }catch(BoardNotDetectedException &e){
        qDebug()<<e.message;

    }catch(BoardNotSetException &e){
        qDebug()<<e.message;

    }catch(SketchNotSetException &e){
        qDebug()<<e.message;

    }catch(UploadException &e){
        qDebug()<<e.message;

    }catch(VerifyException &e){
        qDebug()<<e.message;

    }catch(ArduinoNotFoundException &e){
        qDebug()<<e.message;
    }

    m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
    qInfo() << QJsonDocument(reply).toJson();
    m_pClient->flush();

}



void Web2Board::processTextMessage(QString message)
{
    m_pClient = qobject_cast<QWebSocket *>(sender());

    if(message.startsWith("setBitbloqLibsVersion")){
        //INIT LEGACY MESSAGE
        m_pClient->sendTextMessage("{}");
    }else{
        jsonMessage = QJsonDocument::fromJson(message.toUtf8()).object();
        processCommands();
    }
}

void Web2Board::sendIncomingSerialToClient(QString message){
    //QString msg = CommsProtocol::SERIALMESSAGE.first + message + CommsProtocol::SERIALMESSAGE.second;
    //m_pClient->sendTextMessage(msg.toLatin1());
}
