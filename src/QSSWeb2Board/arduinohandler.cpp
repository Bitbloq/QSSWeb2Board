#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <QSerialPortInfo>

#include "arduinohandler.h"

ArduinoHandler::ArduinoHandler():proc(NULL)
{

    boardIndexAtList = -1; //before all happens we do not know where of if the desired board is at the arduinoBoards List

    //list of known arduino boards with boardNameID, producID and VendorID
    arduinoBoards.append(ArduinoBoards("ZUMCore",qint16(24577),qint16(1027)));
    arduinoBoards.append(ArduinoBoards("ZUMJunior",qint16(60000),qint16(4292)));
    arduinoBoards.append(ArduinoBoards("ZUMCore2",qint16(60000),qint16(4292)));
    arduinoBoards.append(ArduinoBoards("ArduinoUNO",qint16(0x0001),qint16(0x2341)));
    arduinoBoards.append(ArduinoBoards("FreaduinoUNO",qint16(1),qint16(9025)));

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

    //once we know the desired boardNameID, loof for it on the known arduinoBoards

    for (int i=0 ; i < arduinoBoards.size() ; i++ ){
        if(arduinoBoards.at(i).nameID == boardNameID){
            //if found set the index at the list and stop searching
            boardIndexAtList = i;
            qDebug() << s << "is a known board";
            return;
        }
    }

    //if not found it must be -1 (to be able to know it when needed
    boardIndexAtList = -1;
}

bool ArduinoHandler::setBoardPort(QString s){

    //if serial port is passed fix with it.
    if(s!=""){
        boardPort=s;
        return true;
    }

    //we have introduced no port, and board name is not known (not in the list)
    if(boardIndexAtList == -1)
        return false;

    //if not lets check for available serial ports
    // and check whether the desired board is connected to one of them
    // usign vendorID and product ID
    QSerialPortInfo serialinfo;
    //gets list of all available serial ports
    QList<QSerialPortInfo> serialPorts = serialinfo.availablePorts();
    qDebug() << "Available ports";

    //get all productID and vendorID and check if any is equal to boardNameID
    for(int i=0; i< serialPorts.size(); i++){
        qDebug() << serialPorts.at(i).systemLocation();
        qDebug() << "Product Id " <<serialPorts.at(i).productIdentifier();
        qDebug() << "Vendor Id " <<serialPorts.at(i).vendorIdentifier();
        if( (qint16(serialPorts.at(i).productIdentifier()) == arduinoBoards.at(boardIndexAtList).productID) &&
             (qint16(serialPorts.at(i).vendorIdentifier()) == arduinoBoards.at(boardIndexAtList).vendorID) ){
            //Yay found, save board port
            boardPort=serialPorts.at(i).systemLocation();
            qDebug() << boardNameID << " found at " << boardPort;
            return true;
        }
    }

    //board name not connected to the computer
    qDebug() << boardNameID << "not connected to the computer";
    return false;
}

QString ArduinoHandler::verify(){
    //makeVerifyCommand() creates the command to execute
    proc->start(makeVerifyCommand());

    proc->waitForFinished();

    //return the output of the verification
    QString output(proc->readAllStandardOutput());
    qDebug() << output;
    return output;
}

QString ArduinoHandler::load(){

    //makeLoadCommand creates the load command to execute
    proc->start(makeLoadCommand());

    proc->waitForFinished();

    //return the output of the verification
    QString output(proc->readAllStandardOutput());
    qDebug() << output;
    return output;
}

QString ArduinoHandler::makeVerifyCommand(){
    qDebug() << executableDir + "arduino --verify " + filePath + fileName;
    return QString(executableDir + "arduino --verify " + filePath + fileName);
}

QString ArduinoHandler::makeLoadCommand(){
    qDebug() << executableDir + "arduino --upload --port " + boardPort + " " + filePath + fileName;
    return QString(executableDir + "arduino --upload --port " + boardPort + " " + filePath + fileName);
}

