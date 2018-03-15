#ifndef KNOWNBOARDS_H
#define KNOWNBOARDS_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>


class JsonBoardID: public QJsonValue{
public:
    JsonBoardID(const QJsonValue& q);
    virtual ~JsonBoardID(){};

    int operator[](const QString& id);
};


class JsonBoard: public QJsonValue{
public:
    JsonBoard(const QJsonValue& q);
    virtual ~JsonBoard(){};

    JsonBoardID operator[](const int& i);
};


class KnownBoards
{
public:
    KnownBoards(QString jsonFileName);
    JsonBoard operator[](const QString& boardName);

private:

    QJsonObject boardsJsonObject;

};





#endif // KNOWNBOARDS_H
