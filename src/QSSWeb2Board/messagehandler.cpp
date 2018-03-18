#include "messagehandler.h"

MessageHandler::MessageHandler()
{
    command["VERIFY"] =     QPair<QString,QString>( "<VERIFY>"  , "</VERIFY>");
    command["UPLOAD"] =     QPair<QString,QString>( "<UPLOAD>"  , "</UPLOAD>");
    command["SKETCH"] =     QPair<QString,QString>( "<SKETCH>"  , "</SKETCH>");
    command["BOARDID"] =    QPair<QString,QString>( "<BOARD>"   , "</BOARD>");


}

void MessageHandler::handle(QString _message){
    action = Action::NONSET;
    boardID ="";
    sketch = "";

    message = _message;

    manageFullMessage();
    manageBodyMessage();
}

void MessageHandler::manageBodyMessage(){
    if(bodyMessage.contains(command["SKETCH"].first) && bodyMessage.contains(command["SKETCH"].second) ){
        int begin = bodyMessage.indexOf(command["SKETCH"].first) + command["SKETCH"].first.length();
        int end = bodyMessage.indexOf(command["SKETCH"].second) - begin;
        sketch = bodyMessage.mid(begin,end);
    }

    if(bodyMessage.contains(command["BOARDID"].first) && bodyMessage.contains(command["BOARDID"].second) ){
        int begin = bodyMessage.indexOf(command["BOARDID"].first) + command["BOARDID"].first.length();
        int end = bodyMessage.indexOf(command["BOARDID"].second) - begin;
        boardID = bodyMessage.mid(begin,end);
    }
}

void MessageHandler::manageFullMessage(){
    if( message.startsWith(command["VERIFY"].first) && message.endsWith(command["VERIFY"].second) ){
        int length = message.length() - command["VERIFY"].first.length() - command["VERIFY"].second.length();
        bodyMessage = message.mid( command["VERIFY"].first.length(), length);
        action = Action::VERIFY;
    }else if( message.startsWith(command["UPLOAD"].first) && message.endsWith(command["UPLOAD"].second) ){
        int length = message.length() - command["UPLOAD"].first.length() - command["UPLOAD"].second.length();
        bodyMessage = message.mid( command["UPLOAD"].first.length(), length);
        action = Action::UPLOAD;
    }else{
        action = Action::UNKNOWN;
    }
}
