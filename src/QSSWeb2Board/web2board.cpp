#include "web2board.h"
#include <QtWebSockets/QWebSocket>

#include "literals.h"
#include "bitbloqlibsupdater.h"

Web2Board::Web2Board(int clientID, QObject *parent):
    QObject(parent),
    __byteArrayFromArduinoToBitbloq{QByteArray()},
    __timer{Q_NULLPTR},
    __timeout{50},
    __clientID{clientID}
{
    __timer = new QTimer();
}

Web2Board::~Web2Board(){
    if(__timer) delete __timer;
}

void Web2Board::byteArrayFromArduinoToBitbloq(const QByteArray & bA){
    __byteArrayFromArduinoToBitbloq.append(bA);
}

void Web2Board::sendVerifying(QJsonObject msg){
    QJsonObject reply;
    reply.insert("ID",msg.value("ID").toInt());
    reply.insert("hub",msg.value("hub").toString());
    reply.insert("function",QJsonValue(Literals::VERIFYING));
    reply.insert("args",QJsonValue(QJsonArray()));

    m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
    m_pClient->flush();
}

void Web2Board::sendUploading(QJsonObject msg){
    QJsonObject reply;
    reply.insert("ID",msg.value("ID").toInt());
    reply.insert("hub",msg.value("hub").toString());
    reply.insert("function",QJsonValue(Literals::UPLOADING));
    reply.insert("args",QJsonValue(QJsonArray({arduino.getBoardPort()})));

    m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
    m_pClient->flush();
}

void Web2Board::sendSuccess(QJsonObject msg, QJsonValue const & replyValue){
    QJsonObject reply;
    qInfo() << "Success: " << msg.value("function").toString();
    reply.insert("ID",msg.value("ID").toInt());
    //qDebug() << "Send success ID: " << msg.value("ID").toInt();
    reply.insert("hub",msg.value("hub").toString());
    reply.insert("function",msg.value("function").toString());
    reply.insert("reply",QJsonValue(replyValue));
    reply.insert("success",QJsonValue(true));

    m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
    m_pClient->flush();
}

void Web2Board::sendNotSuccess(QJsonObject msg, QJsonValue const & replyValue){
    QJsonObject reply;

    qCritical() << "Failure: " << msg.value("function").toString();
    reply.insert("ID",msg.value("ID").toInt());
    reply.insert("hub",msg.value("hub").toString());
    reply.insert("function",msg.value("function").toString());
    reply.insert("reply",QJsonValue(replyValue));
    reply.insert("success",QJsonValue(false));

    m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
    m_pClient->flush();
}

QJsonObject Web2Board::makeVerifyError(int column, int line, QString file, QString error){
    QJsonObject jsonError;
    jsonError.insert("column", column);
    jsonError.insert("line",line);
    jsonError.insert("file",file);
    jsonError.insert("error",error);

    return jsonError;
}



void Web2Board::handleMessage(QJsonObject msg){


    if(!msg.contains("ID")){
        qCritical() << "Wrong message: No ID" ;
        return; ///invalid message
    }

    //qDebug() << "received: " << QJsonDocument(msg).toJson();
    __messageID = msg.value("ID").toInt();
    //qDebug() << "Message ID: " << __messageID;

    if(! msg.contains("function")){
        qCritical() << "Discarded message: No Function" ;
        return; ///invalid message
    }

    QString function = msg.value("function").toString();

    try{
        if(function == Literals::VERSION){
            sendSuccess(msg, QJsonValue(QString(APP_VERSION)));

        }else if(function == Literals::LIBVERSION){

            BitbloqLibsUpdater libs(arduino.getArduinoDefaultDir());
            if (libs.existsNewVersion()){
                libs.update();
                arduino.updateArduinoBoards();
            }


            sendSuccess(msg, QJsonValue());

        }else if(function == Literals::VERIFY){
            sendVerifying(msg);
            //write skectch stored in jsonMessage.value("args").toArray().at(0)
            QString sketch = msg.value("args").toArray().at(0).toString();
            QString boardName = msg.value("args").toArray().at(1).toString();

            arduino.writeSketch(sketch);
            arduino.setBoardNameID(boardName);
            //non-blocking verify
            arduino.asyncVerify(__clientID);
            sendSuccess(msg,  QJsonValue(arduino.getHex()));

        }else if (function == Literals::UPLOAD){
            sendVerifying(msg);

            QString sketch = msg.value("args").toArray().at(0).toString();
            QString boardName = msg.value("args").toArray().at(1).toString();

            arduino.writeSketch(sketch);
            arduino.setBoardNameID(boardName);

            arduino.verify();

            arduino.closeSerialMonitor(); //just in case it was opened

            arduino.autoDetectBoardPort();
            sendUploading(msg);
            arduino.upload();

            sendSuccess(msg, QJsonValue(arduino.getBoardPort()));

        }else if (function == Literals::FINDBOARDPORT){
            QString boardName = msg.value("args").toArray().at(0).toString();

            arduino.setBoardNameID(boardName);
            arduino.autoDetectBoardPort();


            sendSuccess(msg, QJsonValue(arduino.getBoardPort()));

        }else if (function == Literals::SUBSCRIBETOPORT){
            QString port = msg.value("args").toArray().at(0).toString();

            arduino.setBoardPort(port);

            sendSuccess(msg, QJsonValue(true));

        }else if (function == Literals::OPENSERIALMONITOR){
            QString port = msg.value("args").toArray().at(0).toString();
            int baudrate = msg.value("args").toArray().at(1).toInt();

            arduino.setBoardPort(port);
            arduino.openSerialMonitor(baudrate);

            QObject::connect(arduino.serialMonitor,SIGNAL(dataReceived(const QByteArray &)),this,SLOT(byteArrayFromArduinoToBitbloq(const QByteArray &)));
            QObject::connect(__timer, SIGNAL(timeout()), this, SLOT(sendIncomingSerialToClient()));

            __timer->start(__timeout);


            sendSuccess(msg, QJsonValue(true));

        }else if (function == Literals::CHANGEBAUDRATE){
            QString port = msg.value("args").toArray().at(0).toString();
            int baudrate = msg.value("args").toArray().at(1).toInt();

            arduino.setBoardPort(port);

            arduino.openSerialMonitor(baudrate);
            QObject::connect(arduino.serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(feedMessageFromArduinoToBitbloq(QString)));
            QObject::connect(__timer, SIGNAL(timeout()), this, SLOT(sendIncomingSerialToClient()));
            __timer->start(__timeout);


            sendSuccess(msg, QJsonValue(true));

        }else if (function == Literals::CLOSESERIALMONITOR){

            QObject::disconnect(arduino.serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(feedMessageFromArduinoToBitbloq(QString)));
            QObject::disconnect(__timer, SIGNAL(timeout()), this, SLOT(sendIncomingSerialToClient()));
            arduino.closeSerialMonitor();

            sendSuccess(msg, QJsonValue(true));

        }else if (function == Literals::SENDSERIAL){
            QString serial_msg = msg.value("args").toArray().at(1).toString();
            //qDebug() << "Sending to Arduino: " << serial_msg;
            arduino.serialMonitor->sendToArduino(serial_msg);
            sendSuccess(msg, QJsonValue());

        }else{
            //UNKNOWN MESSAGE
        }
    }catch(FileNotCreatedException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(msg, QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(DirNotCreatedException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(msg, QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(BoardNotKnownException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(msg, QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(FileNotFoundException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(msg, QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(BoardNotDetectedException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(msg, QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(BoardNotSetException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(msg, QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(SketchNotSetException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(msg, QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(UploadException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(msg, QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(VerifyException &e){
        QJsonObject error = makeVerifyError(1,1,"arduino",e.message);
        sendNotSuccess(msg, QJsonValue(error));
        qCritical()<<e.message;
    }catch(ArduinoNotFoundException &e){
        QJsonObject error = makeVerifyError(1,1,"arduino",e.message);
        sendNotSuccess(msg, QJsonValue(error));
        qCritical()<<e.message;
    }catch(CannotMoveTmpLibsException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(msg, QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(GetTimeOutException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(msg, QJsonValue(replyObject));
        qCritical()<<e.message;qCritical()<<e.message;
    }catch(HexFileException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(msg, QJsonValue(replyObject));
        qCritical()<<e.message;qCritical()<<e.message;
    }
}



void Web2Board::handleTextMessage(QString message)
{
    m_pClient = qobject_cast<QWebSocket *>(sender());

    if(message.startsWith("setBitbloqLibsVersion")){
        //INIT LEGACY MESSAGE
        m_pClient->sendTextMessage("{}");
        return;
    }

    QJsonDocument rdocmessage = QJsonDocument::fromJson(message.toUtf8());
    if (rdocmessage.isNull() || rdocmessage.isEmpty()){
        qCritical() << "Invalid JSON message received: " << message;
        return;
    }

    QJsonObject rmessage = rdocmessage.object();

    //qDebug() << "Received JSON: " << QJsonDocument(rmessage).toJson();

    handleMessage(rmessage);
}

void Web2Board::sendIncomingSerialToClient(){

    if(__byteArrayFromArduinoToBitbloq.isEmpty()) return;

    __messageID++;

    QJsonObject reply;

    reply.insert("ID",__messageID);
    reply.insert("hub","SerialMonitorHub");
    reply.insert("function","received");
    reply.insert("args",QJsonValue(QJsonArray({arduino.getBoardPort(),QString(__byteArrayFromArduinoToBitbloq)})));
    m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
    m_pClient->flush();
    __byteArrayFromArduinoToBitbloq.clear();
}

