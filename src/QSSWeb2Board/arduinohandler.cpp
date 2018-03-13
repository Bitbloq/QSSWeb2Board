#include <QProcess>

#include "arduinohandler.h"

ArduinoHandler::ArduinoHandler():proc(NULL)
{
    proc = new QProcess();

}

ArduinoHandler::~ArduinoHandler(){
    //free memory from pointers
    if(proc!=NULL){
        delete proc;
    }
}

void ArduinoHandler::setExecutableDir(QString s){
    executableDir=s;
}

void ArduinoHandler::setSourceDir(QString s){
    sourdeDir=s;
}

void ArduinoHandler::setFileName(QString s){
    fileName=s;
}

void ArduinoHandler::setBoardName(QString s){
    boardName=s;
}

void ArduinoHandler::setBoardPort(QString s){
    boardPort=s;
}

void ArduinoHandler::verify(){
    proc->start("geany");
}
