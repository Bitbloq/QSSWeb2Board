#ifndef WEB2BOARD_H
#define WEB2BOARD_H

#include <QObject>
#include <QMap>
#include <QPair>

#include "arduinohandler.h"
#include "messagehandler.h"
#include "arduinoserialmonitor.h"

QT_FORWARD_DECLARE_CLASS(QWebSocket)

class Web2Board: public QObject
{
    Q_OBJECT

public:
    Web2Board(QObject *parent = Q_NULLPTR);
    virtual ~Web2Board();
    void processVerify(QString message);
    void processUpload(QString message);


public Q_SLOTS:
    void processTextMessage(QString message);
    void sendIncomingSerialToClient(QString message);

private:
    void processCommands();
    QWebSocket *m_pClient;

    MessageHandler messageHandler;
    ArduinoSerialMonitor* serialMonitor;

#if (defined (Q_OS_WIN))
    WindowsArduinoHandler arduino;
#elif (defined (Q_OS_LINUX))
    LinuxArduinoHandler arduino;
#elif (defined (Q_OS_MAC))
    MacArduinoHandler arduino;
#endif

};

#endif // WEB2BOARD_H
