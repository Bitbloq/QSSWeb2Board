#include "githubrestapiclient.h"

GitHubRestApiClient::GitHubRestApiClient():QObject()
{
    _timer = new QTimer(this);
    QObject::connect(_timer , SIGNAL(_timeout()), this ,SLOT(setTimeOut()));
}

GitHubRestApiClient::~GitHubRestApiClient()
{
    if(_timer){
        delete _timer;
        _timer = NULL;
    }
}

void GitHubRestApiClient::setTimeOut()
{
    _timeout = true;
}


QJsonDocument GitHubRestApiClient::get(QUrl url, int timeout){
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkAccessManager nam;
    QNetworkReply * reply = nam.get(request);

    _timeout=false;
    _timer->start(timeout);

    while(!_timeout){
        qApp->processEvents();
        if(reply->isFinished()) break;
    }

    _timeout = false;

    if(reply->isFinished()){
        QByteArray response_data = reply->readAll();
        QJsonDocument json = QJsonDocument::fromJson(response_data);
        return json;
    }else{
        return QJsonDocument();
    }
}

QMap<QString,QString> GitHubRestApiClient::getLatestTagVersion(QString owner, QString project, int timeout)
{
    QUrl url("https://api.github.com/repos/" + owner +"/" + project + "/tags");
    qInfo() << url.toString();
    QJsonDocument json = get(url,timeout);
    if(json.isEmpty()){
        QMap<QString, QString> r_value;
        r_value.insert("Error","Timeout");
        return r_value;
    }else{
        QMap<QString, QString> r_value;
        r_value.insert("version",json[0]["name"].toString());
        r_value.insert("zip" , json[0]["zipball_url"].toString());
        r_value.insert("tar", json[0]["tarball_url"].toString());
        return r_value;
    }
}


QMap<QString,QString> GitHubRestApiClient::getLatestReleaseVersion(QString owner, QString project, int timeout)
{
    QUrl url("https://api.github.com/repos/" + owner +"/" + project + "/releases/latest");
    qInfo() << url.toString();
    QJsonDocument json = get(url,timeout);
    if(json.isEmpty()){
        QMap<QString, QString> r_value;
        r_value.insert("Error","Timeout");
        return r_value;
    }else{
        QMap<QString, QString> r_value;
        r_value.insert("version",json["tag_name"].toString());
        r_value.insert("zip" , json["zipball_url"].toString());
        r_value.insert("tar", json["tarball_url"].toString());
        return r_value;
    }
}
