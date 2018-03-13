#ifndef ARDUINOHANDLER_H
#define ARDUINOHANDLER_H

#include <QObject>

//forward declararion of classes
class QProcess;

class ArduinoHandler
{
public:
    ArduinoHandler();
    virtual ~ArduinoHandler();

    void setExecutableDir(QString s="");
    void setFilePath(QString s);
    void setFileName(QString s);
    void setBoardNameID(QString s);
    bool setBoardPort(QString s="");

    QString verify();
    QString load();

private:
    QString executableDir;
    QString filePath;
    QString fileName;
    QString boardNameID;
    QString boardPort;

    QString makeLoadCommand();
    QString makeVerifyCommand();

    QProcess *proc;

    struct ArduinoBoards{
        ArduinoBoards(QString n, qint16 p, qint16 v):nameID(n),productID(p),vendorID(v){}
        QString nameID;
        qint16 productID;
        qint16 vendorID;
    };

    QList<ArduinoBoards> arduinoBoards;
    int boardIndexAtList;
};

#endif // ARDUINOHANDLER_H
