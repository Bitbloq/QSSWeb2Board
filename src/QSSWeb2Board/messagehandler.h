#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QString>
#include <QPair>
#include <QMap>

#include "commsprotocol.h"

class MessageHandler
{
public:
    MessageHandler();

    void handle(QString _message);

    enum class Action{
        NONSET,
        VERIFY,
        UPLOAD,
        UNKNOWN
    };


    QString sketch;
    QString boardID;
    Action action;


private:
    QString message;
    QString bodyMessage;

    void manageBodyMessage();
    void manageFullMessage();


};

#endif // MESSAGEHANDLER_H
