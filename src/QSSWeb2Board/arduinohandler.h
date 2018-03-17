#ifndef ARDUINOHANDLER_H
#define ARDUINOHANDLER_H

#include <QObject>

#include "knownboards.h"
#include "arduinoexceptions.h"

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
    bool setBoardNameID(QString s) throw(BoardNotKnownException);

    bool setBoardPort(QString s="") throw(BoardNotKnownException,
                                          BoardNotDetectedException);

    QString verify(QString _boardNameID) throw(BoardNotKnownException,
                                               BoardNotDetectedException,
                                               VerifyException);
    QString upload(QString _boardNameID);

private:

    QString extractErrorfromOutput(QString s);
    QString extractSingleError(QString s);

    QString executableDir;
    QString filePath;
    QString fileName;
    QString boardNameID;
    QString boardPort;
    QList<QString> verifyErrorsList;

    QString makeUploadCommand();
    QString makeVerifyCommand();

    QProcess *proc;

    KnownBoards arduinoBoards;

};

#endif // ARDUINOHANDLER_H
