#ifndef ARDUINOHANDLER_H
#define ARDUINOHANDLER_H

#include <QObject>
#include "knownboards.h"

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
    bool setBoardNameID(QString s);
    bool setBoardPort(QString s="");

    QString verify(QString _boardNameID);
    QString upload(QString _boardNameID);

private:


    QString executableDir;
    QString filePath;
    QString fileName;
    QString boardNameID;
    QString boardPort;

    QString makeUploadCommand();
    QString makeVerifyCommand();

    QProcess *proc;

    KnownBoards arduinoBoards;

};

#endif // ARDUINOHANDLER_H
