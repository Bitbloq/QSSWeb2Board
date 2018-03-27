#ifndef COMMSPROTOCOL_H
#define COMMSPROTOCOL_H

#include <QPair>
#include <QString>

class CommsProtocol
{
public:
    CommsProtocol();

    //commands dicitionary
    static const QPair<QString,QString>  VERIFY; ///Verify command
    static const QPair<QString,QString>  UPLOAD; ///Uplad command

    static const QPair<QString,QString>  SKETCH; ///Sketch to verify
    static const QPair<QString,QString>  BOARDID; ///ID of the board on the knownboards.json file

    static const QPair<QString,QString>  OPENSERIALMONITOR; ///Command to Open Serial Monitor
    static const QPair<QString,QString>  CLOSESERIALMONITOR; /// Command to Close Serial Monitor
    static const QPair<QString,QString>  BAUDRATE; ///Serial Port communications baudrate
    static const QPair<QString,QString>  SERIALMESSAGE; ///Serial Message received/sent from/to Arduino


    static const QPair<QString,QString>  SUCCESS; ///Informing whether the performed action has succeed or not
    static const QPair<QString,QString>  ERROR_TYPE; ///If error, error type
    static const QPair<QString,QString>  ERROR_DESC; ///If error, error description

    static const QString VERIFY_ERROR; ///String to define Verify Error (on Error Type)
    static const QString UPLOAD_ERROR; ///String to define Upload Error (on Error Type)
    static const QString FILE_NOT_FOUND_ERROR; ///String to define File not found Error (on Error Type)
    static const QString BOARD_NOT_KNOWN_ERROR; ///String to define Board unknown Error (on Error Type)
    static const QString BOARD_NOT_DETECTED_ERROR; ///String to define Board not detected Error (on Error Type)
    static const QString BOARD_NOT_SET_ERROR; ///String to define Board not set Error (on Error Type)
    static const QString FILE_NOT_CREATED_ERROR; ///String to define File not created Error (on Error Type)
    static const QString SERIAL_PORT_NOT_OPEN; ///String to define Serial Port not opened Error (on Error Type)

};

#endif // COMMSPROTOCOL_H
