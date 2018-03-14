
#include <QDebug>
#include <QCoreApplication>
#include "knownboards.h"

KnownBoards::KnownBoards(QString jsonFileName)
{
    QFile jsonFile;
    QString applicationDir=QCoreApplication::applicationDirPath() + "/res/";
    //qDebug() << applicationDir + jsonFileName;
    jsonFile.setFileName(applicationDir + jsonFileName);
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString val = jsonFile.readAll();
    jsonFile.close();
   // qDebug() << val;

    QJsonDocument jsonKnownBoards = QJsonDocument::fromJson(val.toUtf8());
    jsonObject = jsonKnownBoards.object();
}

QJsonValue KnownBoards::operator [](const QString& boardName){
    return jsonObject.value(boardName);
}

