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
        manageBodyMessage();
    }else if( message.startsWith(CommsProtocol::UPLOAD.first) && message.endsWith(CommsProtocol::UPLOAD.second) ){
        qDebug()<< "Upload message received";
        int length = message.length() - CommsProtocol::UPLOAD.first.length() - CommsProtocol::UPLOAD.second.length();
        bodyMessage = message.mid( CommsProtocol::UPLOAD.first.length(), length);
        qDebug() << bodyMessage;
        action = Action::UPLOAD;
        manageBodyMessage();
    }else if( message.startsWith(CommsProtocol::OPENSERIALMONITOR.first) && message.endsWith(CommsProtocol::OPENSERIALMONITOR.second) ){
        qDebug()<< "Open Serial Monitor";
        int length = message.length() - CommsProtocol::OPENSERIALMONITOR.first.length() - CommsProtocol::OPENSERIALMONITOR.second.length();
        bodyMessage = message.mid( CommsProtocol::OPENSERIALMONITOR.first.length(), length);
        qDebug() << bodyMessage;
        action = Action::OPENSERIALMONITOR;
        manageBodyMessage();
    }else if( message.startsWith(CommsProtocol::CLOSESERIALMONITOR.first) && message.endsWith(CommsProtocol::CLOSESERIALMONITOR.second) ){
        qDebug()<< "Close Serial Monitor";
        int length = message.length() - CommsProtocol::CLOSESERIALMONITOR.first.length() - CommsProtocol::CLOSESERIALMONITOR.second.length();
        bodyMessage = message.mid( CommsProtocol::CLOSESERIALMONITOR.first.length(), length);
        qDebug() << bodyMessage;
        action = Action::CLOSESERIALMONITOR;
        manageBodyMessage();
    }else if( message.startsWith(CommsProtocol::SERIALMESSAGE.first) && message.endsWith(CommsProtocol::SERIALMESSAGE.second) ){
        qDebug()<< "Serial Message Received";
        int length = message.length() - CommsProtocol::SERIALMESSAGE.first.length() - CommsProtocol::SERIALMESSAGE.second.length();
        serialMessage = message.mid( CommsProtocol::SERIALMESSAGE.first.length(), length);
        qDebug() << serialMessage;
        action = Action::SENDSERIAL;
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
    }else if (action == Action::OPENSERIALMONITOR){
        msg = CommsProtocol::OPENSERIALMONITOR.first;
    }else if (action == Action::CLOSESERIALMONITOR){
        msg = CommsProtocol::CLOSESERIALMONITOR.first;
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
    }else if (action == Action::OPENSERIALMONITOR){
        msg += CommsProtocol::OPENSERIALMONITOR.second;
    }else if (action == Action::CLOSESERIALMONITOR){
        msg += CommsProtocol::CLOSESERIALMONITOR.second;
    }


    return msg;
}
