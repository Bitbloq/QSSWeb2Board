#ifndef WEB2BOARD_H
#define WEB2BOARD_H

#include <QObject>
#include <QMap>
#include <QPair>

#include <QJsonObject>

#include "arduinohandler.h"

QT_FORWARD_DECLARE_CLASS(QWebSocket)

class Web2Board: public QObject
{
    Q_OBJECT

public:
    Web2Board(int clientID, QObject *parent = Q_NULLPTR);
    virtual ~Web2Board();


public Q_SLOTS:
    /**
     * @brief processTextMessage. Public slot to process messages received from client thorugh wss
     * @param message
     */
    void processTextMessage(QString message);
    /**
     * @brief sendIncomingSerialToClient Send messages coming from Arduino to Bitbloq client
     */
    void sendIncomingSerialToClient();
    /**
     * @brief feedMessageFromArduinoToBitbloq Buffer of messages to be sent.
     * Messages are not sent as they are received. They are stored in __messageFromArduinoToBitbloq and are sent every __timeout miliseconds
     * @param message the new message to add to the buffer
     */
    void feedMessageFromArduinoToBitbloq(QString message);

private:
    void processCommands(QJsonObject msg);

    /**
     * @brief sendVerifying Sends to Bitbloq the message that the requested verification is in process
     */
    void sendVerifying(QJsonObject msg);
    /**
     * @brief sendUploading Sends to Bitbloq the message that the requested upload is in proccess
     */
    void sendUploading(QJsonObject msg);

    /**
     * @brief sendSuccess Sends to Arduino the message that the last requested action has been a success
     * @param jsonObj Info with the requested action
     * @param replyValue success reply
     */
    void sendSuccess(QJsonObject msg, QJsonValue const & replyValue);
    void sendNotSuccess(QJsonObject msg, QJsonValue const & replyValue);

    QJsonObject makeVerifyError(int column, int line, QString file, QString error);

    QWebSocket *m_pClient;

    int __messageID;
    QString __messageFromArduinoToBitbloq;

    QTimer* __timer;
    int __timeout;

    int __clientID;

#if (defined (Q_OS_WIN))
    WindowsArduinoHandler arduino;
#elif (defined (Q_OS_LINUX))
    LinuxArduinoHandler arduino;
#elif (defined (Q_OS_MAC))
    MacArduinoHandler arduino;
#endif

};

#endif // WEB2BOARD_H
