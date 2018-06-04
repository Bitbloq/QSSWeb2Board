#ifndef ARDUINOEXCEPTIONS_H
#define ARDUINOEXCEPTIONS_H

#include <QObject>
#include <QException>

#include "commsprotocol.h"


/**
 * @brief The VerifyException class
 * Exception thrwon when the verification process of a sketch fails (compilation errors)
 */
class VerifyException: public QException{
    public:
        VerifyException(const QString& s, const QList<QString>& l = QList<QString>()):
            message(s),errorType(CommsProtocol::VERIFY_ERROR),errorsList(l){}
        void raise() const { throw *this; }
        VerifyException *clone() const { return new VerifyException(*this); }
        QString getErrorsLists();

    public:
        const QString message;
        const QString errorType;
        const QList<QString> errorsList;
};

/**
 * @brief The GetTimeOutException class
 * Exception thrown when the HTTP GET exceeds defined timeout
 */
class GetTimeOutException: public QException{
    public:
        GetTimeOutException(const QString& s):
            errorType(CommsProtocol::GET_TIMEOUT),message(s){}
        void raise() const { throw *this; }
        GetTimeOutException *clone() const { return new GetTimeOutException(*this); }

        const QString errorType;
        const QString message;
};


/**
 * @brief The UploadException class
 * Exception thrown when the upload process of a compiled sketch fails.
 */
class UploadException: public QException{
    public:
        UploadException(const QString& s):
            errorType(CommsProtocol::UPLOAD_ERROR),message(s){}
        void raise() const { throw *this; }
        UploadException *clone() const { return new UploadException(*this); }

        const QString errorType;
        const QString message;
};

/**
 * @brief The FileNotFoundException class
 * Exception thrown when the Skecth file is not found (or you do not have read access).
 */
class FileNotFoundException: public QException{
    public:
        FileNotFoundException(const QString& s):
            message(s),errorType(CommsProtocol::FILE_NOT_FOUND_ERROR){}
        void raise() const { throw *this; }
        FileNotFoundException *clone() const { return new FileNotFoundException(*this); }

        const QString message;
        const QString errorType;
};

class ArduinoNotFoundException: public QException{
    public:
        ArduinoNotFoundException(const QString& s):
            message(s),errorType(CommsProtocol::ARDUINO_NOT_FOUND_ERROR){}
        void raise() const { throw *this; }
        ArduinoNotFoundException *clone() const { return new ArduinoNotFoundException(*this); }

        const QString message;
        const QString errorType;
};

/**
 * @brief The BoardNotKnownException class
 * Exception thrown when the Board is not among the know boards (defined in knownboards json file)
 */
class BoardNotKnownException: public QException{
    public:
        BoardNotKnownException(const QString& s)
            :message(s),errorType(CommsProtocol::BOARD_NOT_KNOWN_ERROR){}
        void raise() const { throw *this; }
        BoardNotKnownException *clone() const { return new BoardNotKnownException(*this); }

        const QString message;
        const QString errorType;
};

/**
 * @brief The BoardNotDetectedException class
 * Exception thrown when the board is not connected to any serial port.
 * This means there is no usb device connected with the know vendorid and productid
 * as described in the knownboards.json file
 */
class BoardNotDetectedException: public QException{
    public:
        BoardNotDetectedException(const QString& s):
            message(s),errorType(CommsProtocol::BOARD_NOT_DETECTED_ERROR){}
        void raise() const { throw *this; }
        BoardNotDetectedException *clone() const { return new BoardNotDetectedException(*this); }

        const QString message;
        const QString errorType;
};

class BoardNotSetException: public QException{
    public:
        BoardNotSetException(const QString& s):
            message(s),errorType(CommsProtocol::BOARD_NOT_SET_ERROR){}
        void raise() const { throw *this; }
        BoardNotSetException *clone() const { return new BoardNotSetException(*this); }

        const QString message;
        const QString errorType;
};

class SketchNotSetException: public QException{
    public:
        SketchNotSetException(const QString& s):
            message(s),errorType(CommsProtocol::SKETCH_NOT_SET_ERROR){}
        void raise() const { throw *this; }
        SketchNotSetException *clone() const { return new SketchNotSetException(*this); }

        const QString message;
        const QString errorType;
};


/**
 * @brief The FileNotCreatedException class
 * Exception thrown when the sketch file cannot be created (ususaly you have no write permissions)
 */
class FileNotCreatedException: public QException{
    public:
        FileNotCreatedException(const QString& s):
            message(s),errorType(CommsProtocol::FILE_NOT_CREATED_ERROR){}
        void raise() const { throw *this; }
        FileNotCreatedException *clone() const { return new FileNotCreatedException(*this); }

        const QString message;
        const QString errorType;
};

/**
 * @brief The DirNotCreatedException class
 * Exception thrown when the directory cannot be created (ususaly you have no write permissions)
 */
class DirNotCreatedException: public QException{
    public:
        DirNotCreatedException(const QString& s):
            message(s),errorType(CommsProtocol::DIR_NOT_CREATED_ERROR){}
        void raise() const { throw *this; }
        DirNotCreatedException *clone() const { return new DirNotCreatedException(*this); }

        const QString message;
        const QString errorType;
};

/**
 * @brief The FileNotCreatedException class
 * Exception thrown when the sketch file cannot be created (ususaly you have no write permissions)
 */
class SerialPortOpenException: public QException{
    public:
        SerialPortOpenException(const QString& s):
            message(s),errorType(CommsProtocol::SERIAL_PORT_NOT_OPEN){}
        void raise() const { throw *this; }
        SerialPortOpenException *clone() const { return new SerialPortOpenException(*this); }

        const QString message;
        const QString errorType;
};

/**
 * @brief The CannotMoveLibraryFiles class
 * Exception thrown when the library files cannot be moved from tmp to arduino dir (ususaly you have no write permissions)
 */
class CannotMoveTmpLibsException: public QException{
    public:
        CannotMoveTmpLibsException(const QString& s):
            message(s),errorType(CommsProtocol::SERIAL_PORT_NOT_OPEN){}
        void raise() const { throw *this; }
        CannotMoveTmpLibsException *clone() const { return new CannotMoveTmpLibsException(*this); }

        const QString message;
        const QString errorType;
};

#endif // ARDUINOEXCEPTIONS_H
