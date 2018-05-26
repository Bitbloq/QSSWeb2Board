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

    QMap<QString, QString> getLatestReleaseVersion(QString owner, QString project, int timeout = 5000);
    QMap<QString, QString> getLatestTagVersion(QString owner, QString project, int timeout = 5000);

private:
    QUrl _url;
    QNetworkRequest* _request;

    QJsonDocument get(QUrl url, int timeout);

    QTimer* _timer;
    bool _timeout;

public slots:
    void setTimeOut();
};

#endif // GITHUBRESTAPICLIENT_H
