#ifndef ARDUINOHANDLER_H
#define ARDUINOHANDLER_H

#include <QObject>

#include "knownboards.h"
#include "arduinoexceptions.h"

//forward declararion of classes
class QProcess;

QT_FORWARD_DECLARE_CLASS(TestArduinoHandler)

/**
 * @brief The ArduinoHandler class
 * It handles all the  actions to be made with Arduino: create sketch, verify, upload, etc.
 */
class ArduinoHandler
{
public:
    friend TestArduinoHandler;

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
     * @returns True if everything has gone OK (for automated tests purpose)
     */
    bool setArduinoPath(QString s="");
    /**
     * @brief Sets the absolute path to the result of verifyin a sketch (built files).
     * @param s The path. When left empty it computes it relatively to this application exectuable.
     * @returns True if everything has gone OK (for automated tests purpose)
     */
    bool setBuildPath(QString s="");

    /**
     * @brief Sets the absolute path to the Arduino sketch file (.ino)
     * @param s The path.
     */
    void setSketchPath(QString s);

    /**
     * @brief Sets the sketch file name (.ino)
     * @param s The file name
     */
    void setSketchName(QString s);

    /**
     * @brief Sets the absolute path to the Arduino sketch file (.ino) including the file name
     * @param s The path+filename
     * @returns True if everything has gone OK (for automated tests purpose)
     * @throws FileNotFoundException
     */
    bool setSketchWithFullPath(QString s);

    /**
     * @brief verify. Verifies a sketch in Arduino for the selected board.
     * @param _boardNameID . The Arduino Board. if left empty it takes the board set on the boardID member.
     * @throws BoardNotKnownException -> the board is not among the known boards
     * @throws BoardNotDetectedException -> the board is not connected to the serial port
     * @throws VerifyException -> Verification (compiling) error
     * @return The output of the verification process
     */
    QString verify(QString _boardNameID="");
    /**
     * @brief Upload. Verifies and uploads a sketch in Arduino for the selected board.
     * @param _boardNameID . The Arduino Board. if left empty it takes the board set on the boardID member.
     * @throws BoardNotKnownException -> the board is not among the known boards
     * @throws BoardNotDetectedException -> the board is not connected to the serial port
     * @throws VerifyException -> Verification (compiling) error
     * @throws UploadException -> Compiled Sketch cannot be uploaded to the board
     * @return The output of the upload process
     */
    QString upload(QString _boardNameID="");
    /**
     * @brief writeSketch -> creates a Sketch file (.ino) whith a random name.
     * @param sketch -> The Arduino code
     * @throws FileNotCreatedException -> The file cannot be created
     */
    bool writeSketchInDefaultPath(QString sketch);

    /**
     * @brief setBoardNameID Sets the board to verify/upload
     * @param s The id of the board as in KnownBoards.json
     * @return true if board is known, false otherwise
     * @throws BoardNotKnownException -> When the board is not among the  know boards
     */
    bool setBoardNameID(QString s);


    /**
     * @brief setBoardPort Sets the serial port where the board is connected
     * @param s The serial port. If left empty it looks for it (using board vendorID and productID)
     * @return true if port exists and corresponds to the board, false otherwise
     * @throws BoardNotKnownException -> When the board is not among the  know boards
     * @throws BoardNotDetectedExcpetion -> When the board is not connected (or not found) to any serial port
     */
    bool setBoardPort(QString s="");


    /**
     * @brief getBoardPort returns board port
     * @return board port
     */
    QString getBoardPort() const;


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
    QString sketchPath; /// the absolut path to the sketch file.
    QString sketchName; /// the name of the sketch file (.ino)
    QString sketchWithPath; /// sketchPath + sketchName
    QString boardNameID; /// the name of the board as listed on the known boards json file
    QString boardPort; /// the serial port where the board is connected
    QList<QString> verifyErrorsList; /// List of Strings containing verification errors

    const QString sketchesDefaultDir;
    const QString arduinoDefaultDir;
    const QString buildDefaultDir;

    /**
     * @brief makeUploadCommand -> makes the upload command. OS dependant. Virtualized
     * @return upload command
     */
    virtual QString makeUploadCommand(){return "";};
    /**
     * @brief makeVerifyCommand -> makes the verify command. OS dependant. Virtualized
     * @return verify command
     */
    virtual QString makeVerifyCommand(){return "";};

    QProcess *proc; ///variable to handle command line commands.

    KnownBoards arduinoBoards; ///object holding known boards with their vendorID and productID

    /**
     * @brief createRandomString. Creates a random string of 12 characters
     * @return A random string of 12 characters.
     */
    QString createRandomString() const;

    /**
     * @brief eraseExistingSketches Erases all saved Sketchs with more than one day old
     */
    void eraseExistingSketches() const;

};

/**
 * @brief The LinuxArduinoHandler class for specific functions for linux based systems
 */
class LinuxArduinoHandler : public ArduinoHandler{
public:
    LinuxArduinoHandler():ArduinoHandler(){}
    virtual ~LinuxArduinoHandler(){};
    virtual QString makeUploadCommand();
    virtual QString makeVerifyCommand();
};

/**
 * @brief The WindowsArduinoHandler class for specific functions for windows based systems
 */
class WindowsArduinoHandler : public ArduinoHandler{
public:
    WindowsArduinoHandler():ArduinoHandler(){}
    virtual ~WindowsArduinoHandler(){};
    virtual QString makeUploadCommand();
    virtual QString makeVerifyCommand();
};

/**
 * @brief The MacArduinoHandler class for specific functions for Mac OS/X based systems
 */
class MacArduinoHandler : public ArduinoHandler{
public:
    MacArduinoHandler():ArduinoHandler(){}
    virtual ~MacArduinoHandler(){};
    virtual QString makeUploadCommand();
    virtual QString makeVerifyCommand();
};

#endif // ARDUINOHANDLER_H
