#include "commsprotocol.h"

const QString CommsProtocol::VERSION = "get_version";
const QString CommsProtocol::LIBVERSION = "set_lib_version";

const QString CommsProtocol::VERIFY ="compile";
const QString CommsProtocol::VERIFYING ="is_compiling";


const QString CommsProtocol::UPLOAD = "upload";
const QString CommsProtocol::UPLOADING = "is_uploading";

const QString CommsProtocol::FINDBOARDPORT = "find_board_port";


const QString CommsProtocol::SUBSCRIBETOPORT= "subscribe_to_port";
const QString CommsProtocol::OPENSERIALMONITOR = "start_connection";
const QString CommsProtocol::CLOSESERIALMONITOR = "unsubscribe_from_port";
const QString CommsProtocol::CHANGEBAUDRATE = "change_baudrate";

const QString CommsProtocol::SENDSERIAL = "write";


const QString CommsProtocol::VERIFY_ERROR = "VERIFYERROR";
const QString CommsProtocol::UPLOAD_ERROR = "UPLOADERROR";
const QString CommsProtocol::FILE_NOT_FOUND_ERROR = "FILENOTFOUND";
const QString CommsProtocol::ARDUINO_NOT_FOUND_ERROR = "ARDUINONOTFOUND";
const QString CommsProtocol::BOARD_NOT_KNOWN_ERROR = "BOARDNOTKNOWN";
const QString CommsProtocol::BOARD_NOT_DETECTED_ERROR = "BOARDNOTDETECTED";
const QString CommsProtocol::BOARD_NOT_SET_ERROR = "BOARDNOTSET";
const QString CommsProtocol::SKETCH_NOT_SET_ERROR = "SKETCHNOTSET";
const QString CommsProtocol::FILE_NOT_CREATED_ERROR ="FILENOTCREATED";
const QString CommsProtocol::DIR_NOT_CREATED_ERROR ="DIRNOTCREATED";
const QString CommsProtocol::SERIAL_PORT_NOT_OPEN ="PORTNOTOPENED";

const QString CommsProtocol::CANNOT_MOVE_ARDUINO_LIBS = "CANNOTMOVELIBS";
const QString CommsProtocol::GET_TIMEOUT = "GETTIMEOUT";

const QString CommsProtocol::STATUS_FAIL ="FAIL";
const QString CommsProtocol::STATUS_WORKING ="WORKING";
const QString CommsProtocol::STATUS_SUCCESS ="SUCCESS";


CommsProtocol::CommsProtocol()
{
}
