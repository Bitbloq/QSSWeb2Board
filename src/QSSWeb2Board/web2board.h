#ifndef WEB2BOARD_H
#define WEB2BOARD_H

#include <QObject>
#include <QMap>
#include <QPair>

#include "arduinohandler.h"
#include "messagehandler.h"

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
    void sendIncomingSerialToClient(QString message);

private:
    void processCommands();
    QWebSocket *m_pClient;

    MessageHandler messageHandler;


#if (defined (Q_OS_WIN))
    WindowsArduinoHandler arduino;
#elif (defined (Q_OS_LINUX))
    LinuxArduinoHandler arduino;
#elif (defined (Q_OS_MAC))
    MacArduinoHandler arduino;
#endif

};

#endif // WEB2BOARD_H
