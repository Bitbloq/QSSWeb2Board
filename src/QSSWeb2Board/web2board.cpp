#include "web2board.h"
#include <QtWebSockets/QWebSocket>

#include "commsprotocol.h"


Web2Board::Web2Board(QObject *parent):
    QObject(parent)
{
}

Web2Board::~Web2Board(){
}

void Web2Board::sendVerifying(){
    QJsonObject reply;
    reply.insert("ID",jsonMessage.value("ID").toInt());
    reply.insert("hub",jsonMessage.value("hub").toString());
    reply.insert("function",QJsonValue(CommsProtocol::VERIFYING));
    reply.insert("args",QJsonValue(QJsonArray()));

    m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
    m_pClient->flush();
}

void Web2Board::sendUploading(){
    QJsonObject reply;
    reply.insert("ID",jsonMessage.value("ID").toInt());
    reply.insert("hub",jsonMessage.value("hub").toString());
    reply.insert("function",QJsonValue(CommsProtocol::UPLOADING));
    reply.insert("args",QJsonValue(QJsonArray({arduino.getBoardPort()})));

    m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
    m_pClient->flush();
}

void Web2Board::sendSuccess(QJsonObject const & jsonObj, QJsonValue const & replyValue){
    QJsonObject reply;

    reply.insert("ID",jsonObj.value("ID").toInt());
    reply.insert("hub",jsonObj.value("hub").toString());
    reply.insert("function",jsonObj.value("function").toString());
    reply.insert("reply",QJsonValue(replyValue));
    reply.insert("success",QJsonValue(true));

    m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
    m_pClient->flush();
}

void Web2Board::sendNotSuccess(QJsonObject const & jsonObj, QJsonValue const & replyValue){
    QJsonObject reply;

    reply.insert("ID",jsonObj.value("ID").toInt());
    reply.insert("hub",jsonObj.value("hub").toString());
    reply.insert("function",jsonObj.value("function").toString());
    reply.insert("reply",QJsonValue(replyValue));
    reply.insert("success",QJsonValue(false));

    m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
    m_pClient->flush();
}



void Web2Board::processCommands(){

    QString function = jsonMessage.value("function").toString();

    try{
        if(function == CommsProtocol::VERSION){
            sendSuccess(jsonMessage,QJsonValue("2.1.3"));

        }else if(function == CommsProtocol::LIBVERSION){
            sendSuccess(jsonMessage,QJsonValue());

        }else if(function == CommsProtocol::VERIFY){
            sendVerifying();
            //write skectch stored in jsonMessage.value("args").toArray().at(0)
            QString sketch = jsonMessage.value("args").toArray().at(0).toString();
            QString boardName = jsonMessage.value("args").toArray().at(1).toString();

            arduino.writeSketch(sketch);
            arduino.setBoardNameID(boardName);

            arduino.verify();
            sendSuccess(jsonMessage, QJsonValue(true));

        }else if (function == CommsProtocol::UPLOAD){
            sendVerifying();

            QString sketch = jsonMessage.value("args").toArray().at(0).toString();
            QString boardName = jsonMessage.value("args").toArray().at(1).toString();

            arduino.writeSketch(sketch);
            arduino.setBoardNameID(boardName);
            arduino.verify();

            arduino.autoDetectBoardPort();
            sendUploading();
            arduino.upload();

            sendSuccess(jsonMessage,QJsonValue(arduino.getBoardPort()));

        }else if (function == CommsProtocol::FINDBOARDPORT){
            QString boardName = jsonMessage.value("args").toArray().at(0).toString();

            arduino.setBoardNameID(boardName);
            arduino.autoDetectBoardPort();

            sendSuccess(jsonMessage,QJsonValue(arduino.getBoardPort()));

        }else if (function == CommsProtocol::OPENSERIALMONITOR){
            QString port = jsonMessage.value("args").toArray().at(0).toString();
            int baudrate = jsonMessage.value("args").toArray().at(1).toInt();

            arduino.setBoardPort(port);
            arduino.openSerialMonitor(baudrate);
            QObject::connect(arduino.serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(sendIncomingSerialToClient(QString)));

/*        }else if (function == CommsProtocol::SENDSERIAL){

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


    }catch(FileNotCreatedException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);

        sendNotSuccess(jsonMessage,QJsonValue(replyObject));

        qDebug()<<e.message;

    }catch(DirNotCreatedException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);

        sendNotSuccess(jsonMessage,QJsonValue(replyObject));

        qDebug()<<e.message;


    }catch(BoardNotKnownException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);

        sendNotSuccess(jsonMessage,QJsonValue(replyObject));

        qDebug()<<e.message;

    }catch(FileNotFoundException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);

        sendNotSuccess(jsonMessage,QJsonValue(replyObject));

        qDebug()<<e.message;

    }catch(BoardNotDetectedException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);

        sendNotSuccess(jsonMessage,QJsonValue(replyObject));
        qDebug()<<e.message;

    }catch(BoardNotSetException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);

        sendNotSuccess(jsonMessage,QJsonValue(replyObject));

        qDebug()<<e.message;

    }catch(SketchNotSetException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);

        sendNotSuccess(jsonMessage,QJsonValue(replyObject));

        qDebug()<<e.message;

    }catch(UploadException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);

        sendNotSuccess(jsonMessage,QJsonValue(replyObject));

        qDebug()<<e.message;

    }catch(VerifyException &e){
        sendNotSuccess(jsonMessage,QJsonValue(QJsonArray({e.message})));
        qDebug()<<e.message;

    }catch(ArduinoNotFoundException &e){
        qDebug()<<e.message;
    }
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
