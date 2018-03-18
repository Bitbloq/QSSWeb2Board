#include "commsprotocol.h"


const QPair<QString, QString> CommsProtocol::VERIFY = QPair<QString,QString>(   "<VERIFY>"  ,   "</VERIFY>");
const QPair<QString, QString> CommsProtocol::UPLOAD = QPair<QString,QString>(   "<UPLOAD>"  ,   "</UPLOAD>");
const QPair<QString, QString> CommsProtocol::SKETCH  = QPair<QString,QString>(  "<SKETCH>"  ,   "</SKETCH>");
const QPair<QString, QString> CommsProtocol::BOARDID = QPair<QString,QString>(  "<BOARD>"   ,   "</BOARD>");

CommsProtocol::CommsProtocol()
{
}
