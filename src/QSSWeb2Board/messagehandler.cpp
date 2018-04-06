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

QString MessageHandler::extractMessage(QString msg, QString first, QString second){
    int begin = msg.indexOf(first) + first.length();
    int end = msg.indexOf(second) - begin;
    return msg.mid(begin,end).trimmed().simplified();

}


void MessageHandler::manageBodyMessage(){
    bodyMessage = bodyMessage.trimmed().simplified();
    if(bodyMessage.contains(CommsProtocol::SKETCH.first) && bodyMessage.contains(CommsProtocol::SKETCH.second) ){
        sketch = extractMessage(bodyMessage,CommsProtocol::SKETCH.first, CommsProtocol::SKETCH.second).trimmed().simplified();
        qDebug() << sketch;
    }

    if(bodyMessage.contains(CommsProtocol::BAUDRATE.first) && bodyMessage.contains(CommsProtocol::BAUDRATE.second) ){
        baudrate = extractMessage(bodyMessage,CommsProtocol::BAUDRATE.first, CommsProtocol::BAUDRATE.second).trimmed().simplified().toInt();
        qDebug() << baudrate;
    }

    if(bodyMessage.contains(CommsProtocol::BOARDID.first) && bodyMessage.contains(CommsProtocol::BOARDID.second) ){
        boardID = extractMessage(bodyMessage,CommsProtocol::BOARDID.first, CommsProtocol::BOARDID.second).trimmed().simplified();
        qDebug() << "board: " << boardID;
    }
}


void MessageHandler::manageFullMessage(){
    if( message.startsWith(CommsProtocol::VERIFY.first) && message.endsWith(CommsProtocol::VERIFY.second) ){
        qDebug()<< "Verify message received";
        bodyMessage = extractMessage(message,CommsProtocol::VERIFY.first,CommsProtocol::VERIFY.second);
        action = Action::VERIFY;
        manageBodyMessage();
    }else if( message.startsWith(CommsProtocol::UPLOAD.first) && message.endsWith(CommsProtocol::UPLOAD.second) ){
        qDebug()<< "Upload message received";
        bodyMessage = extractMessage(message,CommsProtocol::UPLOAD.first,CommsProtocol::UPLOAD.second);
        action = Action::UPLOAD;
        manageBodyMessage();
    }else if( message.startsWith(CommsProtocol::VERSION.first) && message.endsWith(CommsProtocol::VERSION.second) ){
        qDebug()<< "Version message received";
        bodyMessage = extractMessage(message,CommsProtocol::VERSION.first,CommsProtocol::VERSION.second);
        action = Action::VERSION;
        manageBodyMessage();
    }else if( message.startsWith(CommsProtocol::OPENSERIALMONITOR.first) && message.endsWith(CommsProtocol::OPENSERIALMONITOR.second) ){
        qDebug()<< "Open Serial Monitor";
        bodyMessage = extractMessage(message,CommsProtocol::OPENSERIALMONITOR.first,CommsProtocol::OPENSERIALMONITOR.second);
        action = Action::OPENSERIALMONITOR;
        manageBodyMessage();
    }else if( message.startsWith(CommsProtocol::CLOSESERIALMONITOR.first) && message.endsWith(CommsProtocol::CLOSESERIALMONITOR.second) ){
        qDebug()<< "Close Serial Monitor";
        bodyMessage = extractMessage(message,CommsProtocol::CLOSESERIALMONITOR.first,CommsProtocol::CLOSESERIALMONITOR.second);
        action = Action::CLOSESERIALMONITOR;
        manageBodyMessage();
    }else if( message.startsWith(CommsProtocol::SERIALMESSAGE.first) && message.endsWith(CommsProtocol::SERIALMESSAGE.second) ){
        qDebug()<< "Serial Message Received";
        bodyMessage = extractMessage(message,CommsProtocol::SERIALMESSAGE.first,CommsProtocol::SERIALMESSAGE.second);
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
    }else if (action == Action::VERSION){
        msg = CommsProtocol::VERSION.first;
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

    if (action != Action::VERSION){
        //SUCCESS
        msg+=CommsProtocol::SUCCESS.first;
        msg+=success;
        msg+=CommsProtocol::SUCCESS.second;
    }else{ //Set version
        msg+="1.0.0";
    }

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
    }else if (action == Action::VERSION){
        msg += CommsProtocol::VERSION.second;
    }else if (action == Action::UPLOAD){
        msg += CommsProtocol::UPLOAD.second;
    }else if (action == Action::OPENSERIALMONITOR){
        msg += CommsProtocol::OPENSERIALMONITOR.second;
    }else if (action == Action::CLOSESERIALMONITOR){
        msg += CommsProtocol::CLOSESERIALMONITOR.second;
    }

    return msg;
}
