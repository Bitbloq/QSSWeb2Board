#ifndef KNOWNBOARDS_H
#define KNOWNBOARDS_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

class KnownBoards
{
public:
    KnownBoards(QString jsonFileName);
    QJsonValue operator[](const QString& boardName);

private:

    QJsonObject jsonObject;

};



#endif // KNOWNBOARDS_H
