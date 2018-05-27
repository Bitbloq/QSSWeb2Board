#ifndef GITHUBRESTAPICLIENT_H
#define GITHUBRESTAPICLIENT_H

#include <QtNetwork>
#include <QObject>

class GitHubRestApiClient : public QObject
{
    Q_OBJECT
public:
    GitHubRestApiClient();
    virtual ~GitHubRestApiClient();

    QJsonObject getLatestReleaseVersion(QString owner, QString project, int timeout = 5000);
    QJsonObject getLatestTagVersion(QString owner, QString project, int timeout = 5000);

    QString downloadFile(QString url, QString path, QString filename, int timeout=5000);

private:
    QUrl _url;
    QNetworkRequest* _request;

    QJsonDocument get(const QUrl & url, const QString & header, int timeout);
    bool saveToDisk(const QString &dir, QString &filename, QIODevice *data);

    QTimer* _timer;
    bool _timeout;

public slots:
    void setTimeOut();
};

#endif // GITHUBRESTAPICLIENT_H
