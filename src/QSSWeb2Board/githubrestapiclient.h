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

    QString downloadFile(QString url, int timeout=5000);

private:
    QUrl _url;
    QNetworkRequest* _request;

    QJsonDocument get(QUrl url, QString header, int timeout);
    bool saveToDisk(const QString &filename, QIODevice *data);

    QTimer* _timer;
    bool _timeout;

public slots:
    void setTimeOut();
};

#endif // GITHUBRESTAPICLIENT_H
