#ifndef COMMSPROTOCOL_H
#define COMMSPROTOCOL_H

#include <QPair>
#include <QString>

class CommsProtocol
{
public:
    CommsProtocol();

    //commands dicitionary
    static const QPair<QString,QString>  VERIFY;
    static const QPair<QString,QString>  UPLOAD;
    static const QPair<QString,QString>  OPENSERIALMONITOR;
    static const QPair<QString,QString>  CLOSESERIALMONITOR;

    static const QPair<QString,QString>  SERIALMESSAGE;
    static const QPair<QString,QString>  SKETCH;
    static const QPair<QString,QString>  BOARDID;
    static const QPair<QString,QString>  BAUDRATE;


    static const QPair<QString,QString>  SUCCESS;
    static const QPair<QString,QString>  ERROR_TYPE;
    static const QPair<QString,QString>  ERROR_DESC;

    static const QString VERIFY_ERROR;
    static const QString UPLOAD_ERROR;
    static const QString FILE_NOT_FOUND_ERROR;
    static const QString BOARD_NOT_KNOWN_ERROR;
    static const QString BOARD_NOT_DETECTED_ERROR;
    static const QString FILE_NOT_CREATED_ERROR;
    static const QString SERIAL_PORT_NOT_OPEN;

};

#endif // COMMSPROTOCOL_H
