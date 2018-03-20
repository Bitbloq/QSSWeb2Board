#include "messagehandler.h"
#include <QDebug>

MessageHandler::MessageHandler()
{
}

void MessageHandler::handle(QString _message){
    action = Action::NONSET;
    boardID ="";
    sketch = "";

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
    }else{
        action = Action::UNKNOWN;
    }
}
