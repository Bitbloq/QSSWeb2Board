
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
    boardsJsonObject = jsonKnownBoards.object();
}

JsonBoard KnownBoards::operator [](const QString& boardName){
    return JsonBoard(boardsJsonObject.value(boardName));
}

JsonBoard::JsonBoard(QJsonValue q):QJsonValue(q){;}

JsonBoardID::JsonBoardID():QJsonValue(){;}

QJsonBoardID JsonBoard::operator [](const int& i){
    return QJsonBoardID(this->toObject().value("id").toArray().at(i));
}

int JsonBoardID::operator [](const QString& id){
    return this->toObject().value(id).toInt();
}

