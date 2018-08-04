#include "web2board.h"
#include <QtWebSockets/QWebSocket>

#include "literals.h"
#include "bitbloqlibsupdater.h"

Web2Board::Web2Board(int clientID, QObject *parent):
    QObject(parent),
    __messageFromArduinoToBitbloq{QString("")},
    __timer{Q_NULLPTR},
    __timeout{50},
    __clientID{clientID}
{
    __timer = new QTimer();
}

Web2Board::~Web2Board(){
    if(__timer) delete __timer;
}

void Web2Board::feedMessageFromArduinoToBitbloq(QString message){
    __messageFromArduinoToBitbloq += message;
}

void Web2Board::sendVerifying(){
    QJsonObject reply;
    reply.insert("ID",receivedJSONMessage.value("ID").toInt());
    reply.insert("hub",receivedJSONMessage.value("hub").toString());
    reply.insert("function",QJsonValue(Literals::VERIFYING));
    reply.insert("args",QJsonValue(QJsonArray()));

    m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
    m_pClient->flush();
}

void Web2Board::sendUploading(){
    QJsonObject reply;
    reply.insert("ID",receivedJSONMessage.value("ID").toInt());
    reply.insert("hub",receivedJSONMessage.value("hub").toString());
    reply.insert("function",QJsonValue(Literals::UPLOADING));
    reply.insert("args",QJsonValue(QJsonArray({arduino.getBoardPort()})));

    m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
    m_pClient->flush();
}

void Web2Board::sendSuccess(QJsonValue const & replyValue){
    QJsonObject reply;

    reply.insert("ID",receivedJSONMessage.value("ID").toInt());
    qInfo() << "Send success ID: " << receivedJSONMessage.value("ID").toInt();
    reply.insert("hub",receivedJSONMessage.value("hub").toString());
    reply.insert("function",receivedJSONMessage.value("function").toString());
    reply.insert("reply",QJsonValue(replyValue));
    reply.insert("success",QJsonValue(true));


    //qInfo() << reply ;
    m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
    m_pClient->flush();
}

void Web2Board::sendNotSuccess(QJsonValue const & replyValue){
    QJsonObject reply;

    reply.insert("ID",receivedJSONMessage.value("ID").toInt());
    reply.insert("hub",receivedJSONMessage.value("hub").toString());
    reply.insert("function",receivedJSONMessage.value("function").toString());
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



void Web2Board::processCommands(){

    __messageID = receivedJSONMessage.value("ID").toInt();
    qInfo() << "Message ID: " << __messageID;

    QString function = receivedJSONMessage.value("function").toString();

    try{
        if(function == Literals::VERSION){
            sendSuccess(QJsonValue("18.08.14"));

        }else if(function == Literals::LIBVERSION){

            BitbloqLibsUpdater libs(arduino.getArduinoDefaultDir());
            if (libs.existsNewVersion()){
                libs.update();
                arduino.updateArduinoBoards();
            }


            sendSuccess(QJsonValue());

        }else if(function == Literals::VERIFY){
            sendVerifying();
            //write skectch stored in jsonMessage.value("args").toArray().at(0)
            QString sketch = receivedJSONMessage.value("args").toArray().at(0).toString();
            QString boardName = receivedJSONMessage.value("args").toArray().at(1).toString();

            arduino.writeSketch(sketch);
            arduino.setBoardNameID(boardName);
            //non-blocking verify
            arduino.asyncVerify(__clientID);
            sendSuccess( QJsonValue(arduino.getHex()));

        }else if (function == Literals::UPLOAD){
            sendVerifying();

            QString sketch = receivedJSONMessage.value("args").toArray().at(0).toString();
            QString boardName = receivedJSONMessage.value("args").toArray().at(1).toString();

            arduino.writeSketch(sketch);
            arduino.setBoardNameID(boardName);

            arduino.verify();

            arduino.closeSerialMonitor(); //just in case it was opened

            arduino.autoDetectBoardPort();
            sendUploading();
            arduino.upload();

            sendSuccess(QJsonValue(arduino.getBoardPort()));

        }else if (function == Literals::FINDBOARDPORT){
            QString boardName = receivedJSONMessage.value("args").toArray().at(0).toString();

            arduino.setBoardNameID(boardName);
            arduino.autoDetectBoardPort();


            sendSuccess(QJsonValue(arduino.getBoardPort()));

        }else if (function == Literals::SUBSCRIBETOPORT){
            QString port = receivedJSONMessage.value("args").toArray().at(0).toString();

            arduino.setBoardPort(port);

            sendSuccess(QJsonValue(true));

        }else if (function == Literals::OPENSERIALMONITOR){
            QString port = receivedJSONMessage.value("args").toArray().at(0).toString();
            int baudrate = receivedJSONMessage.value("args").toArray().at(1).toInt();

            arduino.setBoardPort(port);
            arduino.openSerialMonitor(baudrate);

            QObject::connect(arduino.serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(feedMessageFromArduinoToBitbloq(QString)));
            QObject::connect(__timer, SIGNAL(timeout()), this, SLOT(sendIncomingSerialToClient()));

            __timer->start(__timeout);


            sendSuccess(QJsonValue(true));

        }else if (function == Literals::CHANGEBAUDRATE){
            QString port = receivedJSONMessage.value("args").toArray().at(0).toString();
            int baudrate = receivedJSONMessage.value("args").toArray().at(1).toInt();

            arduino.setBoardPort(port);

            arduino.openSerialMonitor(baudrate);
            QObject::connect(arduino.serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(feedMessageFromArduinoToBitbloq(QString)));
            QObject::connect(__timer, SIGNAL(timeout()), this, SLOT(sendIncomingSerialToClient()));
            __timer->start(__timeout);


            sendSuccess(QJsonValue(true));

        }else if (function == Literals::CLOSESERIALMONITOR){

            QObject::disconnect(arduino.serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(feedMessageFromArduinoToBitbloq(QString)));
            QObject::disconnect(__timer, SIGNAL(timeout()), this, SLOT(sendIncomingSerialToClient()));
            arduino.closeSerialMonitor();

            sendSuccess(QJsonValue(true));

        }else if (function == Literals::SENDSERIAL){
            QString msg = receivedJSONMessage.value("args").toArray().at(1).toString();
            qDebug() << "Sending to Arduino: " << msg;
            arduino.serialMonitor->sendToArduino(msg);
            sendSuccess(QJsonValue());

        }else{
            //UNKNOWN MESSAGE
        }
    }catch(FileNotCreatedException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(DirNotCreatedException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(BoardNotKnownException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(FileNotFoundException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(BoardNotDetectedException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(BoardNotSetException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(SketchNotSetException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(UploadException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(VerifyException &e){
        QJsonObject error = makeVerifyError(1,1,"arduino",e.message);
        sendNotSuccess(QJsonValue(error));
        qCritical()<<e.message;
    }catch(ArduinoNotFoundException &e){
        QJsonObject error = makeVerifyError(1,1,"arduino",e.message);
        sendNotSuccess(QJsonValue(error));
        qCritical()<<e.message;
    }catch(CannotMoveTmpLibsException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(GetTimeOutException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(QJsonValue(replyObject));
        qCritical()<<e.message;qCritical()<<e.message;
    }catch(HexFileException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(QJsonValue(replyObject));
        qCritical()<<e.message;qCritical()<<e.message;
    }
}



void Web2Board::processTextMessage(QString message)
{
    m_pClient = qobject_cast<QWebSocket *>(sender());

    if(message.startsWith("setBitbloqLibsVersion")){
        //INIT LEGACY MESSAGE
        m_pClient->sendTextMessage("{}");
    }else{
        receivedJSONMessage = QJsonDocument::fromJson(message.toUtf8()).object();
        processCommands();
    }
}

void Web2Board::sendIncomingSerialToClient(){

    if(__messageFromArduinoToBitbloq.isEmpty()) return;

    __messageID++;

    QJsonObject reply;

    reply.insert("ID",__messageID);
    reply.insert("hub","SerialMonitorHub");
    reply.insert("function","received");
    reply.insert("args",QJsonValue(QJsonArray({arduino.getBoardPort(),__messageFromArduinoToBitbloq})));
    m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
    m_pClient->flush();
    __messageFromArduinoToBitbloq = QString("");
}

