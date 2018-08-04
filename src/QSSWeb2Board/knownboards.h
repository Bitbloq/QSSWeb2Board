#ifndef KNOWNBOARDS_H
#define KNOWNBOARDS_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>


/**
 * @brief The JsonBoardID class. Parses de vendorID and productID attributes of a board
 */
class JsonBoardID: public QJsonValue{
public:
    JsonBoardID(const QJsonValue& q);
    virtual ~JsonBoardID(){};

    /**
     * @brief operator [] It allows to access de productID and vendorID attributes by string
     * @param id: "vendorID" or "productID"
     * @return the value of the vendor or product ID
     */
    int operator[](const QString& id);
};


class JsonBoard: public QJsonValue{
public:
    JsonBoard(const QJsonValue& q);
    virtual ~JsonBoard(){};

    JsonBoardID operator[](const int& i);
    int size() const;
};

/**
 * @brief The KnownBoards class. Parses the knownboards.json file to get information about vendorID
 * productID and verify/upload commands to send Arduino.
 */
class KnownBoards
{
public:
    KnownBoards(QString jsonFileName);
    void setKnownBoards(QString jsonFilename);
    JsonBoard operator[](const QString& boardName);

private:

    QJsonObject boardsJsonObject;

};





#endif // KNOWNBOARDS_H
