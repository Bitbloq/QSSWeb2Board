#include "messagehandler.h"
#include <QDebug>

MessageHandler::MessageHandler()
{
}

void MessageHandler::handle(QString _message){
    action = Action::NONSET;
    boardID ="";
    sketch = "";
    baudrate = 0;

    message = _message.trimmed().simplified();

    qDebug() << message;

    manageFullMessage();
    manageBodyMessage();
}

void MessageHandler::manageBodyMessage(){
    bodyMessage = bodyMessage.trimmed().simplified();
    if(bodyMessage.contains(CommsProtocol::SKETCH.first) && bodyMessage.contains(CommsProtocol::SKETCH.second) ){
        int begin = bodyMessage.indexOf(CommsProtocol::SKETCH.first) + CommsProtocol::SKETCH.first.length();
        int end = bodyMessage.indexOf(CommsProtocol::SKETCH.second) - begin;
        sketch = bodyMessage.mid(begin,end).trimmed().simplified();
        qDebug() << sketch;
    }

    if(bodyMessage.contains(CommsProtocol::BAUDRATE.first) && bodyMessage.contains(CommsProtocol::BAUDRATE.second) ){
        int begin = bodyMessage.indexOf(CommsProtocol::BAUDRATE.first) + CommsProtocol::BAUDRATE.first.length();
        int end = bodyMessage.indexOf(CommsProtocol::BAUDRATE.second) - begin;
        baudrate = bodyMessage.mid(begin,end).trimmed().simplified().toInt();
        qDebug() << baudrate;
    }

    if(bodyMessage.contains(CommsProtocol::BOARDID.first) && bodyMessage.contains(CommsProtocol::BOARDID.second) ){
        int begin = bodyMessage.indexOf(CommsProtocol::BOARDID.first) + CommsProtocol::BOARDID.first.length();
        int end = bodyMessage.indexOf(CommsProtocol::BOARDID.second) - begin;
        boardID = bodyMessage.mid(begin,end).trimmed().simplified();
        qDebug() << "board: " << boardID;
    }
}

void MessageHandler::manageFullMessage(){
    if( message.startsWith(CommsProtocol::VERIFY.first) && message.endsWith(CommsProtocol::VERIFY.second) ){
        qDebug()<< "Verify message received";
        int length = message.length() - CommsProtocol::VERIFY.first.length() - CommsProtocol::VERIFY.second.length();
        bodyMessage = message.mid( CommsProtocol::VERIFY.first.length(), length);
        action = Action::VERIFY;
    }else if( message.startsWith(CommsProtocol::UPLOAD.first) && message.endsWith(CommsProtocol::UPLOAD.second) ){
        qDebug()<< "Upload message received";
        int length = message.length() - CommsProtocol::UPLOAD.first.length() - CommsProtocol::UPLOAD.second.length();
        bodyMessage = message.mid( CommsProtocol::UPLOAD.first.length(), length);
        qDebug() << bodyMessage;
        action = Action::UPLOAD;
    }else if( message.startsWith(CommsProtocol::SERIALMONITOR.first) && message.endsWith(CommsProtocol::SERIALMONITOR.second) ){
        qDebug()<< "Serial Monitor";
        int length = message.length() - CommsProtocol::SERIALMONITOR.first.length() - CommsProtocol::SERIALMONITOR.second.length();
        bodyMessage = message.mid( CommsProtocol::SERIALMONITOR.first.length(), length);
        qDebug() << bodyMessage;
        action = Action::OPENSERIALMONITOR;
    }else{
        action = Action::UNKNOWN;
    }
}


QString ReturnMessage::makeReturnMessage(){
    QString msg;

    //ACTION
    if (action == Action::VERIFY){
        msg = CommsProtocol::VERIFY.first;
    }else if (action == Action::UPLOAD){
        msg = CommsProtocol::UPLOAD.first;
    }else{
        msg = "<UNKNOWN></UNKNOWN>";
        return msg;
    }

    //SUCCESS
    msg+=CommsProtocol::SUCCESS.first;
    msg+=success;
    msg+=CommsProtocol::SUCCESS.second;

    //In case of error
    if(success == "FALSE"){
        //ErrorType
        msg+=CommsProtocol::ERROR_TYPE.first;
        msg+=errorType;
        msg+=CommsProtocol::ERROR_TYPE.second;

        //ErrorDescripcion
        msg+=CommsProtocol::ERROR_DESC.first;
        msg+=errorDesc;
        msg+=CommsProtocol::ERROR_DESC.second;
    }

    //CLOSE ACTION
    if (action == Action::VERIFY){
        msg += CommsProtocol::VERIFY.second;
    }else if (action == Action::UPLOAD){
        msg += CommsProtocol::UPLOAD.second;
    }

    return msg;
}
