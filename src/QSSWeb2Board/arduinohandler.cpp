#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <QSerialPortInfo>
#include <QJsonArray>

#include "arduinohandler.h"

ArduinoHandler::ArduinoHandler():proc(NULL),arduinoBoards("knownboards.json")
{

    qDebug() << arduinoBoards["ZUMCore"].toObject().value("board").toString();
    qDebug() << arduinoBoards["ArduinoUNO"].toObject().value("id");

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

void ArduinoHandler::setBoardNameID(QString s){
    boardNameID=s;
}

bool ArduinoHandler::setBoardPort(QString s){

    //if serial port is passed fix with it.
    if(s!=""){
        boardPort=s;
        return true;
    }

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
        qDebug() << arduinoBoards[boardNameID].toObject().value("id");

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
    setBoardNameID(_boardNameID);
    setExecutableDir();
    if(setBoardPort()){
        proc->start(makeVerifyCommand());
        proc->waitForFinished();
        //return the output of the verification
        QString output(proc->readAllStandardOutput());
        qDebug() << output;
        return output;
    }
    return QString("Board not connected");

}

QString ArduinoHandler::load(QString _boardNameID){
    setBoardNameID(_boardNameID);
    //makeLoadCommand creates the load command to execute
    proc->start(makeLoadCommand());

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

QString ArduinoHandler::makeLoadCommand(){
    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    qDebug() << executableDir + "arduino --upload " + "--board " +boardCommand + " --port " + boardPort + " " + filePath + fileName;
    return QString(executableDir + "arduino --upload " + "--board " +boardCommand + " --port " + boardPort + " " + filePath + fileName);
}

