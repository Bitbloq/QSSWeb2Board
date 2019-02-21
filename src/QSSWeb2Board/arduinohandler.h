#ifndef ARDUINOHANDLER_H
#define ARDUINOHANDLER_H

#include <QObject>

#include "knownboards.h"
#include "web2boardexceptions.h"
#include "arduinoserialmonitor.h"

//forward declararion of classes
QT_FORWARD_DECLARE_CLASS(TestArduinoHandler)
QT_FORWARD_DECLARE_CLASS(TestBoards)
QT_FORWARD_DECLARE_CLASS(Web2Board)

/**
 * @brief The ArduinoHandler class
 * It handles all the  actions to be made with Arduino: create sketch, verify, upload, serial communication, etc.
 */
class ArduinoHandler: public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Public default ArduinoHandler Constructor
     */
    ArduinoHandler();

    /**
     * @brief Default virtual ~ArduinoHandler Destructor
     */
    virtual ~ArduinoHandler();

    void updateArduinoBoards();
    /**
     * @brief getHex returns a string with the hex file resulting of the arduino file compilation
     * @return a string with the hex file resulting of the arduino file compilation
     */
    QString getHex();

    /**
     * @brief resetPathsToDefault Sets all paths to their default values
     */
    bool resetPathsToDefault();

    /**
     * @brief Sets the absolute path to the arduino executable.
     * @param s The path. When left empty it sets the default path
     * @returns True if everything has gone OK (for automated tests purpose)
     */
    bool setArduinoPath(QString s="");

    /**
     * @brief Sets the absolute path to the result of verifyin a sketch (built files).
     * @param s The path. When left empty it computes its default path.
     * @returns True if everything has gone OK (for automated tests purpose)
     */
    bool setBuildPath(QString s="");

    /**
     * @brief Sets the absolute path to the Arduino sketches base dir
     * @param s The absolute path, if left empty it sets the default path
     */
    bool setSketchesBaseDir(QString s="");

    /**
     * @brief writeSketch -> creates a Sketch file (.ino) whith a random name.
     * @param _sketch -> The Arduino code
     * @param _skecthName -> desired name for the sketch, if left empty it creates random name
     * @throws FileNotCreatedException -> The file cannot be created
     */
    bool writeSketch(QString _sketch, QString _sketchName="");

    /**
     * @brief setSketchWithAbsolutePath Absolute Path to the sketch file (.ino)
     * @param s Absolute path to the sketch file
     * @return True if file exists
     */
    bool setSketchWithAbsolutePath(QString s);


    /**
     * @brief asyncVerify initiates the compilation of a ino file.
     * Async verification of a ino file. The process is launched but it does not waits until if finishes.
     * When verfication is finished verifyFinished signal is triggered.
     * Build files are stored in a dir "build + buildPathCounter"
     * @param buildPathCounter
     */
    int asyncVerify(int buildPathCounter);

    /**
     * @brief verify. Verifies a sketch in Arduino for the selected board.
     * @return The exitCode of the verification process
     */
    int verify();


    /**
     * @brief Upload. Verifies and uploads a sketch in Arduino for the selected board.
     * @return The exitCode of the upload process
     */
    int upload();



    /**
     * @brief setBoardPort Sets the serial port where the board is connected
     * @param _boardPort The boardPort.
     * @return true if port exists
     * @throws BoardNotDetectedExcpetion -> When the board is not connected (or not found) to any serial port
     */
    bool setBoardPort(QString _boardPort);

    /**
     * @brief autoDetectBoardPort Detects the serial port where the board is connected
     * If left empty it takes the member variable
     * @return true if port exists and corresponds to the board, false otherwise
     */
    bool autoDetectBoardPort();

    /**
     * @brief getBoardPort returns board port
     * @return board port
     */
    QString getBoardPort() const;

    /**
     * @brief setBoardNameID Sets the board to verify/upload
     * @param s The id of the board as in KnownBoards.json
     * @return true if board is known, false otherwise
     * @throws BoardNotKnownException -> When the board is not among the  know boards
     */
    bool setBoardNameID(QString s);

    /**
     * @brief openSerialMonitor Opens Serial port with the Arduino Board
     * @param baudrate baudrate for communication
     * @return  true if serial port is opened
     */
    bool openSerialMonitor(int baudrate);

    /**
     * @brief closeSerialMonitor Closes serial port for communication
     * @return true if serial port is closed
     */
    bool closeSerialMonitor();


    ArduinoSerialMonitor* serialMonitor; ///pointer to serial monitor handler

    /**
     * @brief getArduinoDefaultDir the path where arduino executable should be
     * @return the path where arduino executable should be
     */
    QString getArduinoDefaultDir() const;

protected:



    /**
     * @brief extractErrorfromOutput. Analyzes the verify error output and get the full list of erros
     * @param s The error output of the verify command
     * @return The list of errors
     */
    QString extractErrorfromOutput(QString s);

    /**
     * @brief extractSingleError. Decoposes the full error into single errors
     * @param s String containing several errors.
     * @return a single error string.
     */
    QString extractSingleError(QString s);

    QString arduinoPath; /// the absolute path to the arduino executable file
    QString buildPath; /// the absolute path to the placement of the build process resulting files.
    QString sketchesBaseDir; /// the abslute path to the sketches base diretory
    QString sketchName; /// the name of the sketch without .ino
    QString boardNameID; /// the name of the board as listed on the known boards json file
    QString boardPort; /// the serial port where the board is connected
    QList<QString> verifyErrorsList; /// List of Strings containing verification errors

    const QString sketchesDefaultBaseDir; /// the absolute path where the sketches to compile are stored
    const QString arduinoDefaultDir; /// the abslute path where arduino program is located
    const QString buildDefaultDir; /// the absolute path where the resulting building files are stored

    /**
     * @brief makeUploadCommand -> makes the upload command. OS dependant. Virtualized
     * @return upload command
     */
    virtual QString makeUploadCommand()  = 0;
    /**
     * @brief makeVerifyCommand -> makes the verify command. OS dependant. Virtualized
     * @return verify command
     */
    virtual QString makeVerifyCommand()  = 0;

    virtual void setPermissions(QString, QString) const = 0;


    /**
     * @brief checkArduinoPath virtual function (implemented on children)
     * @param arduinoPath
     * @return
     */
    virtual bool checkArduinoPath(QString arduinoPath){arduinoPath=""; return false;}

    KnownBoards arduinoBoards; ///object holding known boards with their vendorID and productID
    const QString tmpDir;

    /**
     * @brief createRandomString. Creates a random string of 12 characters
     * @return A random string of 12 characters.
     */
    QString createRandomString() const;

    /**
     * @brief eraseExistingSketches Erases all saved Sketchs with more than one day old
     */
    void eraseExistingSketches() const;

    /**
     * @brief eraseExistingBuildFiles Erases all saved BuildFiles with more than one day old
     */
    void eraseExistingBuildFiles() const;

    /**
     * @brief eraseTempDir
     */
    void eraseTempDir() const;

};

/**
 * @brief The LinuxArduinoHandler class for specific functions for linux based systems
 */
class LinuxArduinoHandler : public ArduinoHandler{

    Q_OBJECT

public:
    LinuxArduinoHandler():ArduinoHandler(){
        setPermissions(tmpDir, "777");
        setPermissions(sketchesDefaultBaseDir, "777");
        setPermissions(buildDefaultDir, "777");
    }
    virtual ~LinuxArduinoHandler(){}
    virtual QString makeUploadCommand() ;
    virtual QString makeVerifyCommand() ;
    virtual bool checkArduinoPath(QString arduinoPath);
    virtual void setPermissions(QString dir, QString permissions) const;
};

/**
 * @brief The WindowsArduinoHandler class for specific functions for windows based systems
 */
class WindowsArduinoHandler : public ArduinoHandler{

    Q_OBJECT

public:
    WindowsArduinoHandler():ArduinoHandler(){
        setPermissions(tmpDir, "777");
        setPermissions(sketchesDefaultBaseDir, "777");
        setPermissions(buildDefaultDir, "777");
    }
    virtual ~WindowsArduinoHandler(){}
    virtual QString makeUploadCommand() ;
    virtual QString makeVerifyCommand() ;
    virtual bool checkArduinoPath(QString arduinoPath);
    virtual void setPermissions(QString, QString) const{
        qDebug() << "No Need to set permissions";
    }
};

/**
 * @brief The MacArduinoHandler class for specific functions for Mac OS/X based systems
 */
class MacArduinoHandler : public ArduinoHandler{

    Q_OBJECT

public:
    MacArduinoHandler():ArduinoHandler(){
        setPermissions(tmpDir, "777");
        setPermissions(sketchesDefaultBaseDir, "777");
        setPermissions(buildDefaultDir, "777");
    }
    virtual ~MacArduinoHandler(){}
    virtual QString makeUploadCommand() ;
    virtual QString makeVerifyCommand() ;
    virtual bool checkArduinoPath(QString arduinoPath);
    virtual void setPermissions(QString, QString) const{
        qDebug() << "No Need to set permissions";
    }
};

#endif // ARDUINOHANDLER_H
