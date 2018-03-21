#include "commsprotocol.h"


const QPair<QString, QString> CommsProtocol::VERIFY = QPair<QString,QString>(   "<VERIFY>"  ,   "</VERIFY>");
const QPair<QString, QString> CommsProtocol::UPLOAD = QPair<QString,QString>(   "<UPLOAD>"  ,   "</UPLOAD>");
const QPair<QString, QString> CommsProtocol::SKETCH  = QPair<QString,QString>(  "<SKETCH>"  ,   "</SKETCH>");
const QPair<QString, QString> CommsProtocol::BOARDID = QPair<QString,QString>(  "<BOARD>"   ,   "</BOARD>");

const QPair<QString, QString> CommsProtocol::SUCCESS = QPair<QString,QString>(  "<SUCCESS>"   ,   "</SUCCESS>");
const QPair<QString, QString> CommsProtocol::ERROR_TYPE = QPair<QString,QString>(  "<ERROR_TYPE>"   ,   "</ERROR_TYPE>");
const QPair<QString, QString> CommsProtocol::ERROR_DESC = QPair<QString,QString>(  "<ERROR_DESC>"   ,   "</ERROR_DESC>");

const QString CommsProtocol::VERIFY_ERROR = "VERIFYERROR";
const QString CommsProtocol::UPLOAD_ERROR = "UPLOADERROR";
const QString CommsProtocol::FILE_NOT_FOUND_ERROR = "FILENOTFOUND";
const QString CommsProtocol::BOARD_NOT_KNOWN_ERROR = "BOARDNOTKNOWN";
const QString CommsProtocol::BOARD_NOT_DETECTED_ERROR = "BOARDNOTDETECTED";
const QString CommsProtocol::FILE_NOT_CREATED_ERROR ="FILENOTCREATED";


CommsProtocol::CommsProtocol()
{
}
