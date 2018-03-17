#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <QSerialPortInfo>
#include <QJsonArray>
#include <QSerialPort>

#include "arduinoexceptions.h"
#include "arduinohandler.h"

ArduinoHandler::ArduinoHandler():proc(NULL),arduinoBoards("knownboards.json")
{
    proc = new QProcess(); //this is to launch the arduino commands
}

ArduinoHandler::~ArduinoHandler(){
    //free memory from pointers
    if(proc!=NULL){
        delete proc;
    }
}

void ArduinoHandler::setExecutableDir(QString s){
    //if no param passed, set the default directory (whis is relative to the app dir)
    if (s == ""){
        //portable arduino ide MUST be in in ~/res/arduino relative to application path !!
        executableDir=QCoreApplication::applicationDirPath() + "/res/arduino/";
    }else{
        executableDir=s;
    }

    qDebug() << executableDir;
}

void ArduinoHandler::setFilePath(QString s){
    //absolute path to the .ino file to verify
    filePath=s;
    fileWithPath = filePath + fileName;
}

void ArduinoHandler::setFileName(QString s){
    //name of the .ino file
    fileName=s;
    fileWithPath = filePath + fileName;
}

void ArduinoHandler::setFileWithFullPath(QString s) throw (FileNotFoundException){

    if ( QFile::exists(s)) {
        fileWithPath=s;
    }else{
        throw FileNotFoundException("File "+s+" does not exist");
    }
}

bool ArduinoHandler::setBoardNameID(QString s) throw(BoardNotKnownException){
    //check whether we know that board
    if (arduinoBoards[s].isUndefined()){
        throw BoardNotKnownException("BOARD NOT KNOWN: " + s);
        return false;
    }else{
        boardNameID=s;
        return true;
    }
}

bool ArduinoHandler::setBoardPort(QString s) throw(BoardNotDetectedException, BoardNotKnownException){

    //if serial port is passed fix with it.
    if(s!=""){
        QSerialPort port(s);
        QSerialPortInfo serialPort(port);
        if(serialPort.isNull()){
            throw BoardNotDetectedException("NO BOARD CONNECTED TO " + s);
        }else{
            //We know there is something connected to port s, but we do not know what it is
            boardPort=s;
            return true;
        }
    }


    //before going on let's check whether we know the board or not
    // only way boardNameID has a value is that is on the known boards
    if(boardNameID.isEmpty()){
        throw BoardNotKnownException("BOARDNAME NOT DEFINED");
    }


    //we are sure the board is in our known boards list

    // if no port introduced check for available serial ports
    // and check whether the desired board is connected to one of them
    // usign vendorID and product ID
    QSerialPortInfo serialinfo;

    //gets list of all available serial ports
    QList<QSerialPortInfo> serialPorts = serialinfo.availablePorts();
    qDebug() << "Available ports";

    //get all productID and vendorID and check if any is equal to boardNameID
    for(int i=0; i< serialPorts.size(); i++){
        qDebug() << serialPorts.at(i).systemLocation();
        //qDebug() << "Product Id " <<serialPorts.at(i).productIdentifier();
        //qDebug() << "Vendor Id " <<serialPorts.at(i).vendorIdentifier();

        //loop over all vendorID and productID of selected board to check whether the board is connected (compare con vendor and product of connected serial ports)
        for (int j=0; j<arduinoBoards[boardNameID].size(); j++){
            //qDebug() << idArray.at(j).toObject().value("productID").toInt();
            //qDebug() << idArray.at(j).toObject().value("vendorID").toInt();

            if( (arduinoBoards[boardNameID][j]["productID"] == serialPorts.at(i).productIdentifier()) &&
                    (arduinoBoards[boardNameID][j]["vendorID"] == serialPorts.at(i).vendorIdentifier()) ){
                //Yay found, save board port
                boardPort=serialPorts.at(i).systemLocation();
                qDebug() << boardNameID << " found at " << boardPort;
                return true;
            }
        }
    }

    throw BoardNotDetectedException("BOARD NOT DETECTED: " + boardNameID);
    //board name not connected to the computer
    qDebug() << boardNameID << "not connected to the computer";
    return false;
}

QString ArduinoHandler::verify(QString _boardNameID) throw(BoardNotKnownException,
                                                           BoardNotDetectedException,
                                                           VerifyException){

    //throws BoardNotKnowException if _boardNameID is not among the known boards
    setBoardNameID(_boardNameID);

    setExecutableDir();

    proc->start(makeVerifyCommand());
    proc->waitForFinished();

    QString errorOuput = QString(proc->readAllStandardError());

    //Check how compilation has gone
    if(errorOuput.endsWith("Verifying...\n")){
        qDebug()<<"Verify OK";
    }else{
        throw VerifyException("Verify Error: " + extractErrorfromOutput(errorOuput), verifyErrorsList);
    }

    //return the output of the verification
    QString output(proc->readAllStandardOutput());
    //qDebug() << output;
    return output;


}

QString ArduinoHandler::upload(QString _boardNameID){
    setBoardNameID(_boardNameID);

    if(!setBoardPort()) return QString("Board ") + boardNameID + QString(" not connected to the computer");

    //makeLoadCommand creates the load command to execute
    proc->start(makeUploadCommand());
    proc->waitForFinished();
    //return the output of the verification
    QString output(proc->readAllStandardOutput());
    //qDebug() << output;
    return output;
}

QString ArduinoHandler::makeVerifyCommand(){
    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    qDebug() << executableDir + "arduino --verify " + "--board " +boardCommand + " " + fileWithPath;
    return QString(executableDir + "arduino --verify " + "--board " +boardCommand + " " + fileWithPath);
}

QString ArduinoHandler::makeUploadCommand(){
    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    qDebug() << executableDir + "arduino --upload " + "--board " +boardCommand + " --port " + boardPort + " " + fileWithPath;
    return QString(executableDir + "arduino --upload " + "--board " +boardCommand + " --port " + boardPort + " " + fileWithPath);
}


//This function, called recursively, allows to list the errors removing the local route to the file
QString ArduinoHandler::extractSingleError(QString s){

    QString match = filePath + fileName +":";
    int pos = s.indexOf(match);
    if ( pos >= 0 )
    {
        QString leftOf = s.left(pos);
        verifyErrorsList.append(leftOf.simplified());
        return (leftOf.simplified() + extractSingleError(s.mid (pos + match.length())));
    }else{
        verifyErrorsList.append(s.simplified());
        return s.simplified();
    }
}

//This function, together with extractSingleError creates a string with all the errors eliminating reference
//to local routes
QString ArduinoHandler::extractErrorfromOutput(QString s){
    QString errorsLine;
    QString errorReturn;
    QString match = filePath + fileName + ":";
    int pos = s.indexOf(match);
    if ( pos >= 0 )
    {
        errorsLine = s.mid (pos + match.length());
    }

    return extractSingleError(errorsLine);
}
