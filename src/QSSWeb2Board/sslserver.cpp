#include "sslserver.h"
#include "QtWebSockets/QWebSocket"
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>

QT_USE_NAMESPACE

Web2BoardSocketServer::Web2BoardSocketServer(QObject *parent):
                       QWebSocketServer(QStringLiteral("SSL Echo Server"),
                                        QWebSocketServer::SecureMode,
                                        parent),
                       pWeb2Board(Q_NULLPTR)
{
    pWeb2Board = new Web2Board();
}

Web2BoardSocketServer::~Web2BoardSocketServer(){
    if(pWeb2Board != Q_NULLPTR){
        delete pWeb2Board;
    }
}


//! [constructor]
SSLServer::SSLServer(quint16 port, QObject *parent) :
    QObject(parent),
    m_pWeb2BoardSocketServer(Q_NULLPTR),
    m_clients()
{
    m_pWeb2BoardSocketServer = new Web2BoardSocketServer(this);
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

    m_pWeb2BoardSocketServer->setSslConfiguration(sslConfiguration);

    if (m_pWeb2BoardSocketServer->listen(QHostAddress::Any, port))
    {
        qDebug() << "SSL Echo Server listening on port" << port;
        connect(m_pWeb2BoardSocketServer, &QWebSocketServer::newConnection,
                this, &SSLServer::onNewConnection);
        connect(m_pWeb2BoardSocketServer, &QWebSocketServer::sslErrors,
                this, &SSLServer::onSslErrors);
    }
}
//! [constructor]

SSLServer::~SSLServer()
{
    m_pWeb2BoardSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}


//! [onNewConnection]
void SSLServer::onNewConnection()
{
    QWebSocket *pSocket = m_pWeb2BoardSocketServer->nextPendingConnection();

    qDebug() << "Client connected:" << pSocket->peerName() << pSocket->origin();

    connect(pSocket, &QWebSocket::textMessageReceived, m_pWeb2BoardSocketServer->pWeb2Board, &Web2Board::processTextMessage);
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
