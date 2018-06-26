#include "githubrestapiclient.h"
#include "web2boardexceptions.h"

GitHubRestApiClient::GitHubRestApiClient():QObject()
{
    _timer = new QTimer(this);
    QObject::connect(_timer , SIGNAL(timeout()), this ,SLOT(setTimeOut()));
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


QJsonDocument GitHubRestApiClient::get(const QUrl &url, const QString &header, int timeout){
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, header);
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
        qInfo() << "Time to get data from " << url.toString()  << " exceeded";
        throw GetTimeOutException("Time to get data from " +  url.toString()  + " exceeded");
    }
}


bool GitHubRestApiClient::saveToDisk(const QString & dir, QString &filename, QIODevice *data)
{
    qInfo() << QString ("Saving to " + dir + "/" + filename);
    if(!QDir().exists(dir)){
        QDir().mkdir(dir);
    }

    QString targetFileName = dir + "/" + filename;

    QFile file(targetFileName);

    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Could not open %s for writing: %s\n",
                qPrintable(targetFileName),
                qPrintable(file.errorString()));
        return false;
    }

    file.write(data->readAll());
    file.close();

    filename = targetFileName;

    qInfo()<< "Done";
    return true;
}



QString GitHubRestApiClient::downloadFile(QString url, QString path, QString filename, int timeout){

    qInfo() << "Downloading " << url;
    const QUrl qurl(url);
    qInfo() << url;

    QNetworkRequest request(qurl);
    //github redirects the request, so this attribute must be set to true, otherwise retuns nothing
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    QNetworkAccessManager manager;
    QNetworkReply * reply = manager.get(request);


    _timeout=false;
    _timer->start(timeout);

    //if download takes more time that set on timeout cancel download
    while(!_timeout){
        qApp->processEvents();
        if(reply->isFinished()) break;
    }

    _timeout = false;

    if(reply->isFinished()){
        if (reply->error()) {
                fprintf(stderr, "Download of %s failed: %s\n",
                        qurl.toEncoded().constData(),
                        qPrintable(reply->errorString()));
                return "";
        }else{
            saveToDisk(path, filename,reply);
            return filename;
        }
    }else{
        return "";
    }
}

QJsonObject GitHubRestApiClient::getLatestTagVersion(QString owner, QString project, int timeout)
{
    QUrl url("https://api.github.com/repos/" + owner +"/" + project + "/tags");
    qInfo() << url.toString();
    QJsonDocument json = get(url, "application/json", timeout);
    if(json.isEmpty()){
        QJsonObject r_value;
        r_value.insert("Error","Timeout");
        return r_value;
    }else{
        QJsonObject r_value;
        r_value.insert("version",json.array().at(0).toObject().value("name").toString());
        r_value.insert("zipball_url" , json.array().at(0).toObject().value("zipball_url").toString());
        r_value.insert("tarball_url", json.array().at(0).toObject().value("tarball_url").toString());
        return r_value;
    }
}


QJsonObject GitHubRestApiClient::getLatestReleaseVersion(QString owner, QString project, int timeout)
{
    QUrl url("https://api.github.com/repos/" + owner +"/" + project + "/releases/latest");
    qInfo() << url.toString();
    QJsonDocument json = get(url,"application/json", timeout);
    if(json.isEmpty()){
        QJsonObject r_value;
        r_value.insert("Error","Timeout");
        return r_value;
    }else{
        QJsonObject r_value;
        r_value.insert("version",json.object().value("tag_name").toString());
        r_value.insert("zipball_url" , json.object().value("zipball_url").toString());
        r_value.insert("tarball_url", json.object().value("tarball_url").toString());
        return r_value;
    }
}
