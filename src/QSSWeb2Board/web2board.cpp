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
    reply.insert("ID",jsonMessage.value("ID").toInt());
    reply.insert("hub",jsonMessage.value("hub").toString());
    reply.insert("function",QJsonValue(Literals::VERIFYING));
    reply.insert("args",QJsonValue(QJsonArray()));

    m_pClient->sendTextMessage(QJsonDocument(reply).toJson());
    m_pClient->flush();
}

void Web2Board::sendUploading(){
    QJsonObject reply;
    reply.insert("ID",jsonMessage.value("ID").toInt());
    reply.insert("hub",jsonMessage.value("hub").toString());
    reply.insert("function",QJsonValue(Literals::UPLOADING));
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

    //if(jsonObj.contains("hex")) reply.insert("hex",jsonObj.value("hex"));

    //qInfo() << reply ;
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

QJsonObject Web2Board::makeVerifyError(int column, int line, QString file, QString error){
    QJsonObject jsonError;
    jsonError.insert("column", column);
    jsonError.insert("line",line);
    jsonError.insert("file",file);
    jsonError.insert("error",error);

    return jsonError;
}



void Web2Board::processCommands(){

    __messageID = jsonMessage.value("ID").toInt();

    QString function = jsonMessage.value("function").toString();

    try{
        if(function == Literals::VERSION){
            sendSuccess(jsonMessage,QJsonValue("18.07.17"));

        }else if(function == Literals::LIBVERSION){

            BitbloqLibsUpdater libs(arduino.getArduinoDefaultDir());
            if (libs.existsNewVersion()){
                libs.update();
            }


            sendSuccess(jsonMessage,QJsonValue());

        }else if(function == Literals::VERIFY){
            sendVerifying();
            //write skectch stored in jsonMessage.value("args").toArray().at(0)
            QString sketch = jsonMessage.value("args").toArray().at(0).toString();
            QString boardName = jsonMessage.value("args").toArray().at(1).toString();

            #if (defined (Q_OS_WIN))
                WindowsArduinoHandler* arduinoLocal = new WindowsArduinoHandler();
            #elif (defined (Q_OS_LINUX))
                LinuxArduinoHandler* arduinoLocal = new LinuxArduinoHandler();
            #elif (defined (Q_OS_MAC))
                MacArduinoHandler* arduinoLocal = new MacArduinoHandler();
            #endif

            arduinoLocal->writeSketch(sketch);
            arduinoLocal->setBoardNameID(boardName);

            //arduino.verify();
            arduinoLocal->asyncVerify( __clientID);

            QObject::connect(arduinoLocal, SIGNAL(verifyFinished(int)), this, SLOT(verificationFinished(int)));


        }else if (function == Literals::UPLOAD){
            sendVerifying();

            QString sketch = jsonMessage.value("args").toArray().at(0).toString();
            QString boardName = jsonMessage.value("args").toArray().at(1).toString();

            arduino.writeSketch(sketch);
            arduino.setBoardNameID(boardName);
            arduino.verify();

            arduino.closeSerialMonitor(); //just in case it was opened

            arduino.autoDetectBoardPort();
            sendUploading();
            arduino.upload();

            sendSuccess(jsonMessage,QJsonValue(arduino.getBoardPort()));

        }else if (function == Literals::FINDBOARDPORT){
            QString boardName = jsonMessage.value("args").toArray().at(0).toString();

            arduino.setBoardNameID(boardName);
            arduino.autoDetectBoardPort();


            sendSuccess(jsonMessage,QJsonValue(arduino.getBoardPort()));

        }else if (function == Literals::SUBSCRIBETOPORT){
            QString port = jsonMessage.value("args").toArray().at(0).toString();

            arduino.setBoardPort(port);

            sendSuccess(jsonMessage,QJsonValue(true));

        }else if (function == Literals::OPENSERIALMONITOR){
            QString port = jsonMessage.value("args").toArray().at(0).toString();
            int baudrate = jsonMessage.value("args").toArray().at(1).toInt();

            arduino.setBoardPort(port);
            arduino.openSerialMonitor(baudrate);

            QObject::connect(arduino.serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(feedMessageFromArduinoToBitbloq(QString)));
            QObject::connect(__timer, SIGNAL(timeout()), this, SLOT(sendIncomingSerialToClient()));

            __timer->start(__timeout);


            sendSuccess(jsonMessage,QJsonValue(true));

        }else if (function == Literals::CHANGEBAUDRATE){
            QString port = jsonMessage.value("args").toArray().at(0).toString();
            int baudrate = jsonMessage.value("args").toArray().at(1).toInt();

            arduino.setBoardPort(port);

            arduino.openSerialMonitor(baudrate);
            QObject::connect(arduino.serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(feedMessageFromArduinoToBitbloq(QString)));
            QObject::connect(__timer, SIGNAL(timeout()), this, SLOT(sendIncomingSerialToClient()));
            __timer->start(__timeout);


            sendSuccess(jsonMessage,QJsonValue(true));

        }else if (function == Literals::CLOSESERIALMONITOR){

            QObject::disconnect(arduino.serialMonitor,SIGNAL(lineReceived(QString)),this,SLOT(feedMessageFromArduinoToBitbloq(QString)));
            QObject::disconnect(__timer, SIGNAL(timeout()), this, SLOT(sendIncomingSerialToClient()));
            arduino.closeSerialMonitor();

            sendSuccess(jsonMessage,QJsonValue(true));

        }else if (function == Literals::SENDSERIAL){
            QString msg = jsonMessage.value("args").toArray().at(1).toString();
            qDebug() << "Sending to Arduino: " << msg;
            arduino.serialMonitor->sendToArduino(msg);
            sendSuccess(jsonMessage,QJsonValue());

        }else{
            //UNKNOWN MESSAGE
        }
    }catch(FileNotCreatedException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(jsonMessage,QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(DirNotCreatedException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(jsonMessage,QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(BoardNotKnownException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(jsonMessage,QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(FileNotFoundException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(jsonMessage,QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(BoardNotDetectedException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(jsonMessage,QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(BoardNotSetException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(jsonMessage,QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(SketchNotSetException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(jsonMessage,QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(UploadException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(jsonMessage,QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(VerifyException &e){
        QJsonObject error = makeVerifyError(1,1,"arduino",e.message);
        sendNotSuccess(jsonMessage,QJsonValue(error));
        qCritical()<<e.message;
    }catch(ArduinoNotFoundException &e){
        QJsonObject error = makeVerifyError(1,1,"arduino",e.message);
        sendNotSuccess(jsonMessage,QJsonValue(error));
        qCritical()<<e.message;
    }catch(CannotMoveTmpLibsException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(jsonMessage,QJsonValue(replyObject));
        qCritical()<<e.message;
    }catch(GetTimeOutException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(jsonMessage,QJsonValue(replyObject));
        qCritical()<<e.message;qCritical()<<e.message;
    }catch(HexFileException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(jsonMessage,QJsonValue(replyObject));
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
        jsonMessage = QJsonDocument::fromJson(message.toUtf8()).object();
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

void Web2Board::verificationFinished(int exitCode){

    qInfo() << "Verification Finished. Exit Code: " << exitCode;
#if (defined (Q_OS_WIN))
    WindowsArduinoHandler* arduinoLocal = qobject_cast<WindowsArduinoHandler*>(QObject::sender());
#elif (defined (Q_OS_LINUX))
    LinuxArduinoHandler* arduinoLocal = qobject_cast<LinuxArduinoHandler*>(QObject::sender());
#elif (defined (Q_OS_MAC))
    MacArduinoHandler* arduinoLocal = qobject_cast<MacArduinoHandler*>(QObject::sender());
#endif

    if(!arduinoLocal) qInfo()<< "Arduino Local is not null";

    QString output = QString(arduinoLocal->proc->readAllStandardError());

    //qInfo() << "output: " << output;

    arduinoLocal->proc->close();

    try{
        switch(exitCode){
        case 0:
            qDebug()<<"Verify OK";
            break;
        case 1:
            throw VerifyException(output);
            break;
        case 2:
            throw VerifyException("Sketch not found");
            break;
        case 3:
            throw VerifyException("Invalid (argument for) commandline option");
            break;
        case 4:
            throw VerifyException("Preference passed to --get-pref does not exist");
            break;
        }

        sendSuccess(jsonMessage, QJsonValue(arduinoLocal->getHex()));
        //QObject::disconnect(arduinoLocal, SIGNAL(verifyFinished(int)), this, SLOT(verificationFinished(int)));
        if (arduinoLocal) delete arduinoLocal;

    }catch(VerifyException &e){
        QJsonObject error = makeVerifyError(1,1,"arduino",e.message);
        sendNotSuccess(jsonMessage,QJsonValue(error));
        qCritical()<<e.message;
        //QObject::disconnect(arduinoLocal, SIGNAL(verifyFinished(int)), this, SLOT(verificationFinished(int)));
        if (arduinoLocal) delete arduinoLocal;
    }catch(HexFileException &e){
        QJsonObject replyObject;
        replyObject.insert("stdErr",QJsonValue(e.message));
        replyObject.insert("title",e.errorType);
        sendNotSuccess(jsonMessage,QJsonValue(replyObject));
        qCritical()<<e.message;qCritical()<<e.message;
        if (arduinoLocal) delete arduinoLocal;
    }


}
