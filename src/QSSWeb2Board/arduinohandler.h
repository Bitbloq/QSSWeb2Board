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

    void setArduinoPath(QString s="");
    void setBuildPath(QString s="");
    void setFilePath(QString s);
    void setFileName(QString s);

    void setFileWithFullPath(QString s) throw (FileNotFoundException);


    QString verify(QString _boardNameID) throw(BoardNotKnownException,
                                               BoardNotDetectedException,
                                               VerifyException);
    QString upload(QString _boardNameID)throw(BoardNotKnownException,
                                                   BoardNotDetectedException,
                                                   VerifyException,
                                                   UploadException);

protected:

    bool setBoardNameID(QString s) throw(BoardNotKnownException);
    bool setBoardPort(QString s="") throw(BoardNotKnownException,
                                          BoardNotDetectedException);


    QString extractErrorfromOutput(QString s);
    QString extractSingleError(QString s);

    QString arduinoPath;
    QString buildPath;
    QString filePath;
    QString fileName;
    QString fileWithPath;
    QString boardNameID;
    QString boardPort;
    QList<QString> verifyErrorsList;

    virtual QString makeUploadCommand(){return "";};
    virtual QString makeVerifyCommand(){return "";};

    QProcess *proc;

    KnownBoards arduinoBoards;

};

class LinuxArduinoHandler : public ArduinoHandler{
public:
    LinuxArduinoHandler():ArduinoHandler(){}
    virtual ~LinuxArduinoHandler(){};
    virtual QString makeUploadCommand();
    virtual QString makeVerifyCommand();
};

class WindowsArduinoHandler : public ArduinoHandler{
public:
    WindowsArduinoHandler():ArduinoHandler(){}
    virtual ~WindowsArduinoHandler(){};
    virtual QString makeUploadCommand();
    virtual QString makeVerifyCommand();
};

class MacArduinoHandler : public ArduinoHandler{
public:
    MacArduinoHandler():ArduinoHandler(){}
    virtual ~MacArduinoHandler(){};
    virtual QString makeUploadCommand();
    virtual QString makeVerifyCommand();
};

#endif // ARDUINOHANDLER_H
