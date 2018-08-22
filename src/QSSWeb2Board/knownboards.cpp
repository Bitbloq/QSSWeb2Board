
#include <QDebug>
#include <QCoreApplication>
#include <QJsonArray>
#include "knownboards.h"

KnownBoards::KnownBoards(QString jsonFileName)
{
    setKnownBoards(jsonFileName);
}

void KnownBoards::setKnownBoards(QString jsonFileName){
    qInfo() << "json FileName: " << jsonFileName;
    QFile jsonFile;
    jsonFile.setFileName(jsonFileName);
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString val = jsonFile.readAll();
    jsonFile.close();

    QJsonDocument jsonKnownBoards = QJsonDocument::fromJson(val.toUtf8());
    boardsJsonObject = jsonKnownBoards.object();
}

JsonBoard KnownBoards::operator [](const QString& boardName){
    return JsonBoard(boardsJsonObject.value(boardName));
}

JsonBoard::JsonBoard(const QJsonValue& q):QJsonValue(q){;}

JsonBoardID::JsonBoardID(const QJsonValue& q):QJsonValue(q){;}

JsonBoardID JsonBoard::operator [](const int& i){
    return JsonBoardID(this->toObject().value("id").toArray().at(i));
}

int JsonBoard::size() const{
    return this->toObject().value("id").toArray().size();
}

int JsonBoardID::operator [](const QString& id){
    return qint16(this->toObject().value(id).toInt());
}

