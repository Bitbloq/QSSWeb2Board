#include "literals.h"

const QString Literals::VERSION = "get_version";
const QString Literals::LIBVERSION = "set_lib_version";

const QString Literals::VERIFY ="compile";
const QString Literals::VERIFYING ="is_compiling";


const QString Literals::UPLOAD = "upload";
const QString Literals::UPLOADING = "is_uploading";

const QString Literals::FINDBOARDPORT = "find_board_port";


const QString Literals::SUBSCRIBETOPORT= "subscribe_to_port";
const QString Literals::OPENSERIALMONITOR = "start_connection";
const QString Literals::CLOSESERIALMONITOR = "unsubscribe_from_port";
const QString Literals::CHANGEBAUDRATE = "change_baudrate";

const QString Literals::SENDSERIAL = "write";


const QString Literals::VERIFY_ERROR = "VERIFYERROR";
const QString Literals::UPLOAD_ERROR = "UPLOADERROR";
const QString Literals::FILE_NOT_FOUND_ERROR = "FILENOTFOUND";
const QString Literals::ARDUINO_NOT_FOUND_ERROR = "ARDUINONOTFOUND";
const QString Literals::BOARD_NOT_KNOWN_ERROR = "BOARDNOTKNOWN";
const QString Literals::BOARD_NOT_DETECTED_ERROR = "BOARDNOTDETECTED";
const QString Literals::BOARD_NOT_SET_ERROR = "BOARDNOTSET";
const QString Literals::SKETCH_NOT_SET_ERROR = "SKETCHNOTSET";
const QString Literals::FILE_NOT_CREATED_ERROR ="FILENOTCREATED";
const QString Literals::DIR_NOT_CREATED_ERROR ="DIRNOTCREATED";
const QString Literals::SERIAL_PORT_NOT_OPEN ="PORTNOTOPENED";
const QString Literals::CANNOT_GET_HEXFILE = "CANNOTGETHEXFILE";

const QString Literals::CANNOT_MOVE_ARDUINO_LIBS = "CANNOTMOVELIBS";
const QString Literals::GET_TIMEOUT = "GETTIMEOUT";

//const QString Literals::STATUS_FAIL ="FAIL";
//const QString Literals::STATUS_WORKING ="WORKING";
//const QString Literals::STATUS_SUCCESS ="SUCCESS";


Literals::Literals()
{
}
