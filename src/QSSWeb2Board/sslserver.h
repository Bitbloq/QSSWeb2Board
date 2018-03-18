#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtNetwork/QSslError>
#include "QtWebSockets/QWebSocketServer"

#include "web2board.h"

QT_FORWARD_DECLARE_CLASS(QWebSocket)

class Web2BoardSocketServer : public QWebSocketServer{
    Q_OBJECT

public:
    explicit Web2BoardSocketServer(QObject *parent = Q_NULLPTR);
    virtual ~Web2BoardSocketServer();
    Web2Board* pWeb2Board;
};

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
    Web2BoardSocketServer *m_pWeb2BoardSocketServer;
    QList<QWebSocket *> m_clients;
};




#endif // SSLSERVER_H
