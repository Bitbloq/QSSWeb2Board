#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <QSerialPortInfo>
#include <QJsonArray>

#include "arduinohandler.h"

ArduinoHandler::ArduinoHandler():proc(NULL),arduinoBoards("knownboards.json")
{
    //qDebug() << arduinoBoards["ZUMCore"].toObject().value("board").toString();
    //qDebug() << arduinoBoards["ArduinoUNO"].toObject().value("id");

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
}

void ArduinoHandler::setFileName(QString s){
    //name of the .ino file
    fileName=s;
}

bool ArduinoHandler::setBoardNameID(QString s){
    boardNameID=s;

    //check whether we know that board
    if (arduinoBoards[s].isNull()){
        return false;
    }else{
        return true;
    }
}

bool ArduinoHandler::setBoardPort(QString s){

    //if serial port is passed fix with it.
    if(s!=""){
        boardPort=s;
        return true;
    }

    //before going on let's check whether we know the board or not
    if(arduinoBoards[boardNameID].isNull()) return false;

    // if no port introduced check for available serial ports
    // and check whether the desired board is connected to one of them
    // usign vendorID and product ID
    QSerialPortInfo serialinfo;

    //gets list of all available serial ports
    QList<QSerialPortInfo> serialPorts = serialinfo.availablePorts();
    qDebug() << "Available ports";

    //array of vendorID and productID of selected board (if in  the list)
    QJsonArray idArray = arduinoBoards[boardNameID].toObject().value("id").toArray();

    //get all productID and vendorID and check if any is equal to boardNameID
    for(int i=0; i< serialPorts.size(); i++){
        qDebug() << serialPorts.at(i).systemLocation();
        qDebug() << "Product Id " <<serialPorts.at(i).productIdentifier();
        qDebug() << "Vendor Id " <<serialPorts.at(i).vendorIdentifier();

        //loop over all vendorID and productID of selected board to check whether the board is connected (compare con vendor and product of connected serial ports)
        for (int j=0; j<idArray.size(); j++){
            qDebug() << idArray.at(j).toObject().value("productID").toInt();
            qDebug() << idArray.at(j).toObject().value("vendorID").toInt();

            if( (qint16(idArray.at(j).toObject().value("productID").toInt()) == qint16(serialPorts.at(i).productIdentifier())) &&
                    (qint16(idArray.at(j).toObject().value("vendorID").toInt()) == qint16(serialPorts.at(i).vendorIdentifier())) ){
                //Yay found, save board port
                boardPort=serialPorts.at(i).systemLocation();
                qDebug() << boardNameID << " found at " << boardPort;
                return true;
            }
        }
    }

    //board name not connected to the computer
    qDebug() << boardNameID << "not connected to the computer";
    return false;
}

QString ArduinoHandler::verify(QString _boardNameID){
    if(!setBoardNameID(_boardNameID)){
        return QString("Board Name not recognized");
    }

    setExecutableDir();

    proc->start(makeVerifyCommand());
    proc->waitForFinished();
    //return the output of the verification
    QString output(proc->readAllStandardOutput());
    qDebug() << output;
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
    qDebug() << output;
    return output;
}

QString ArduinoHandler::makeVerifyCommand(){
    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    qDebug() << executableDir + "arduino --verify " + "--board " +boardCommand + " " + filePath + fileName;
    return QString(executableDir + "arduino --verify " + "--board " +boardCommand + " " + filePath + fileName);
}

QString ArduinoHandler::makeUploadCommand(){
    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    qDebug() << executableDir + "arduino --upload " + "--board " +boardCommand + " --port " + boardPort + " " + filePath + fileName;
    return QString(executableDir + "arduino --upload " + "--board " +boardCommand + " --port " + boardPort + " " + filePath + fileName);
}

