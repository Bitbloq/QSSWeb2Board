#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtNetwork/QSslError>

#include "web2board.h"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class SSLServer : public QObject
{
    Q_OBJECT
public:
    explicit SSLServer(quint16 port, Web2Board* w2b, QObject *parent = Q_NULLPTR);
    virtual ~SSLServer();

private Q_SLOTS:
    void onNewConnection();
    void socketDisconnected();
    void onSslErrors(const QList<QSslError> &errors);

private:
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
    Web2Board* web2board;
};


#endif // SSLSERVER_H
