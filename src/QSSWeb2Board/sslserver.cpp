#include "sslserver.h"
#include "QtWebSockets/QWebSocketServer"
#include "QtWebSockets/QWebSocket"
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>

QT_USE_NAMESPACE

//! [constructor]
SSLServer::SSLServer(quint16 port, Web2Board *w2b, QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(Q_NULLPTR),
    m_clients(),
    web2board(w2b)
{

    m_pWebSocketServer = new QWebSocketServer(QStringLiteral("SSL Echo Server"),
                                              QWebSocketServer::SecureMode,
                                              this);
    QSslConfiguration sslConfiguration;
    QFile certFile(QStringLiteral(":/res/localhost.cert"));
    QFile keyFile(QStringLiteral(":/res/localhost.key"));
    certFile.open(QIODevice::ReadOnly);
    keyFile.open(QIODevice::ReadOnly);
    QSslCertificate certificate(&certFile, QSsl::Pem);
    QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
    certFile.close();
    keyFile.close();
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfiguration.setLocalCertificate(certificate);
    sslConfiguration.setPrivateKey(sslKey);
    sslConfiguration.setProtocol(QSsl::TlsV1SslV3);

    m_pWebSocketServer->setSslConfiguration(sslConfiguration);

    if (m_pWebSocketServer->listen(QHostAddress::Any, port))
    {
        qDebug() << "SSL Echo Server listening on port" << port;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &SSLServer::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::sslErrors,
                this, &SSLServer::onSslErrors);
    }
}
//! [constructor]

SSLServer::~SSLServer()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}


//! [onNewConnection]
void SSLServer::onNewConnection()
{
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

    qDebug() << "Client connected:" << pSocket->peerName() << pSocket->origin();

    connect(pSocket, &QWebSocket::textMessageReceived, web2board, &Web2Board::processTextMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &SSLServer::socketDisconnected);

    m_clients << pSocket;
}
//! [onNewConnection]

//! [socketDisconnected]

void SSLServer::socketDisconnected()
{
    qDebug() << "Client disconnected";
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient)
    {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}

void SSLServer::onSslErrors(const QList<QSslError> &)
{
    qDebug() << "Ssl errors occurred";
}
//! [socketDisconnected]
