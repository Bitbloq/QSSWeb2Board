#ifndef ARDUINOHANDLER_H
#define ARDUINOHANDLER_H

#include <QObject>

#include "knownboards.h"
#include "arduinoexceptions.h"

//forward declararion of classes
class QProcess;

/**
 * @brief The ArduinoHandler class
 * It handles all the  actions to be made with Arduino: create sketch, verify, upload, etc.
 */
class ArduinoHandler
{
public:

    /**
     * @brief Public default ArduinoHandler Constructor
     */
    ArduinoHandler();
    /**
     * @brief Default virtual ~ArduinoHandler Destructor
     */
    virtual ~ArduinoHandler();

    /**
     * @brief Sets the absolute path to the arduino executable.
     * @param s The path. When left empty it computes it relatively to this application exectuable.
     */
    void setArduinoPath(QString s="");
    /**
     * @brief Sets the absolute path to the result of verifyin a sketch (built files).
     * @param s The path. When left empty it computes it relatively to this application exectuable.
     */
    void setBuildPath(QString s="");

    /**
     * @brief Sets the absolute path to the Arduino sketch file (.ino)
     * @param s The path.
     */
    void setSketchPath(QString s);
    void setSketchName(QString s);

    void setSketchWithFullPath(QString s) throw (FileNotFoundException);


    QString verify(QString _boardNameID="") throw(BoardNotKnownException,
                                               BoardNotDetectedException,
                                               VerifyException);
    QString upload(QString _boardNameID="")throw(BoardNotKnownException,
                                                   BoardNotDetectedException,
                                                   VerifyException,
                                                   UploadException);
    void writeSketch(QString sketch) throw(FileNotCreatedException);

protected:

    bool setBoardNameID(QString s) throw(BoardNotKnownException);
    bool setBoardPort(QString s="") throw(BoardNotKnownException,
                                          BoardNotDetectedException);


    QString extractErrorfromOutput(QString s);
    QString extractSingleError(QString s);

    QString arduinoPath;
    QString buildPath;
    QString sketchPath;
    QString sketchName;
    QString sketchWithPath;
    QString boardNameID;
    QString boardPort;
    QList<QString> verifyErrorsList;

    virtual QString makeUploadCommand(){return "";};
    virtual QString makeVerifyCommand(){return "";};

    QProcess *proc;

    KnownBoards arduinoBoards;

    QString createRandomString() const;
    void eraseExistingSketches() const;

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
