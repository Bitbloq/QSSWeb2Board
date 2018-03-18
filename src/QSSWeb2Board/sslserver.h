#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtNetwork/QSslError>
#include "QtWebSockets/QWebSocket"

#include "web2board.h"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)

/**
 * @brief The Web2BoardSocketServer class handles connections from clients.
 * It is the standard QWebSocketServer with the addtion of Web2Board
 * Web2Board handles the messages and controls the ArduinoHandler
 */
class Web2BoardSocket: public QObject{
    Q_OBJECT

public:
    explicit Web2BoardSocket(QWebSocket *ws);
    virtual ~Web2BoardSocket();
    Web2Board* m_pWeb2Board; //for managing messages
    QWebSocket* m_pWebSocket;
};

/**
 * @brief The SSLServer class is a SSL Server for web secure sockets
 */
class SSLServer : public QObject
{
    Q_OBJECT
public:
    explicit SSLServer(quint16 port, QObject *parent = Q_NULLPTR);
    virtual ~SSLServer();

private Q_SLOTS:
    void onNewConnection();
    void socketDisconnected();
    void onSslErrors(const QList<QSslError> &errors);

private:
    QWebSocketServer *m_pWebSocketServer;
    QList<Web2BoardSocket *> m_web2BoardSocketClients; //list of connected clients
};




#endif // SSLSERVER_H
