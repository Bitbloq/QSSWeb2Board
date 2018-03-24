#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QString>
#include <QPair>
#include <QMap>

#include "commsprotocol.h"

/**
 * @brief The MessageHandler class. Utility to parse de messages received/to send from/to client.
 */
class MessageHandler
{
public:
    MessageHandler();

    void handle(QString _message);

    /**
     * @brief The Action enum. Type of actions than can be performed.
     */
    enum class Action{
        NONSET,
        VERIFY,
        UPLOAD,
        OPENSERIALMONITOR,
        CLOSESERIALMONITOR,
        SENDSERIAL,
        UNKNOWN
    };


    QString sketch; ///String to store the sketch to verify/upload
    QString boardID; ///String to store the boardID (withing the knownboards)
    QString serialMessage; ///String to store de messageto send through serial port
    Action action; ///action to perform
    int baudrate; ///baudrate of the serial communications.


private:
    QString message; /// Internal. Full received message from client.
    QString bodyMessage; /// Internal. Body message onche the beginnign and tail actions are interpreted.


    void manageBodyMessage(); ///Parses the body message
    void manageFullMessage(); ///Parses the full received message


};

class ReturnMessage
{

public:

    ReturnMessage(){};

    enum class Action{
        NONSET,
        VERIFY,
        UPLOAD,
        OPENSERIALMONITOR,
        CLOSESERIALMONITOR,
        UNKNOWN
    };

    Action action;
    QString success;
    QString errorType;
    QString errorDesc;


    QString makeReturnMessage();

};

#endif // MESSAGEHANDLER_H
