#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QString>
#include <QPair>
#include <QMap>

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

    //received commands dicitionary
    QMap<QString,QPair<QString,QString> > command;
};

#endif // MESSAGEHANDLER_H
