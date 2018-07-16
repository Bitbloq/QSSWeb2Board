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
    Web2Board(QObject *parent = Q_NULLPTR);
    virtual ~Web2Board();


public Q_SLOTS:
    /**
     * @brief processTextMessage. Public slot to process messages received from client thorugh wss
     * @param message
     */
    void processTextMessage(QString message);
    void sendIncomingSerialToClient();
    void feedMessageFromArduinoToBitbloq(QString message);

private:
    void processCommands();

    void sendVerifying();
    void sendUploading();

    void sendSuccess(QJsonObject const & jsonObj, QJsonValue const & replyValue);
    void sendNotSuccess(QJsonObject const & jsonObj, QJsonValue const & replyValue);

    QJsonObject makeVerifyError(int column, int line, QString file, QString error);

    QWebSocket *m_pClient;
    QJsonObject jsonMessage;

    int __messageID;
    QString __messageFromArduinoToBitbloq;

    QTimer* __timer;
    int __timeout;

#if (defined (Q_OS_WIN))
    WindowsArduinoHandler arduino;
#elif (defined (Q_OS_LINUX))
    LinuxArduinoHandler arduino;
#elif (defined (Q_OS_MAC))
    MacArduinoHandler arduino;
#endif

};

#endif // WEB2BOARD_H
