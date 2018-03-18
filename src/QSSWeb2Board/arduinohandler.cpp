#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <QSerialPortInfo>
#include <QJsonArray>
#include <QSerialPort>
#include <QDir>
#include <QDateTime>

#include "arduinoexceptions.h"
#include "arduinohandler.h"

ArduinoHandler::ArduinoHandler():proc(NULL),arduinoBoards("knownboards.json")
{
    eraseExistingSketches();
    proc = new QProcess(); //this is to launch the arduino commands
}

ArduinoHandler::~ArduinoHandler(){
    //free memory from pointers
    if(proc!=NULL){
        delete proc;
    }
}

QString ArduinoHandler::createRandomString() const{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    const int randomStringLength = 12; // assuming you want random strings of 12 characters
    qsrand(QDateTime::currentMSecsSinceEpoch() / 1000);
    QString randomString;
    for(int i=0; i<randomStringLength; ++i)
    {
       int index = qrand() % possibleCharacters.length();
       QChar nextChar = possibleCharacters.at(index);
       randomString.append(nextChar);
    }
    return randomString;
}


void ArduinoHandler::eraseExistingSketches() const {
    QString path = QCoreApplication::applicationDirPath() + "/res/sketches/";
    QDir dir(path);
    dir.setNameFilters(QStringList() << "*");
    dir.setFilter(QDir::Dirs);
    foreach(QString dirDir, dir.entryList())
    {
        dir.remove(dirDir);
    }
}

void ArduinoHandler::writeSketch(QString sketch){
    QString randString = createRandomString();

    fileName = randString + ".ino";
    filePath=QCoreApplication::applicationDirPath() + "/res/sketches/" + randString + "/";
    fileWithPath = filePath + fileName;

    QDir().mkdir(filePath);

    QFile file(fileWithPath);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << sketch << endl;
        file.close();
    }
}

void ArduinoHandler::setArduinoPath(QString s){
    //if no param passed, set the default directory (whis is relative to the app dir)
    if (s == ""){
        //portable arduino ide MUST be in in ~/res/arduino relative to application path !!
        arduinoPath=QCoreApplication::applicationDirPath() + "/res/arduino/";
    }else{
        arduinoPath=s;
    }

    qDebug() << arduinoPath;
}

void ArduinoHandler::setBuildPath(QString s){
    //if no param passed, set the default path (which is relative to the app dir)
    if (s == ""){
        //build path MUST be in in ~/res/build relative to application path !!
        buildPath=QCoreApplication::applicationDirPath() + "/res/build/";
    }else{
        buildPath=s;
    }

    qDebug() << buildPath;
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
        fileName = fileWithPath.right(fileWithPath.length() - fileWithPath.lastIndexOf("/") - 1);
        filePath = fileWithPath.left(fileWithPath.lastIndexOf("/") + 1);
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
    //qDebug() << "Available ports";

    //get all productID and vendorID and check if any is equal to boardNameID
    for(int i=0; i< serialPorts.size(); i++){
        //qDebug() << serialPorts.at(i).systemLocation();
        //qDebug() << "Product Id " <<serialPorts.at(i).productIdentifier();
        //qDebug() << "Vendor Id " <<serialPorts.at(i).vendorIdentifier();

        //loop over all vendorID and productID of selected board to check whether the board is connected (compare con vendor and product of connected serial ports)
        for (int j=0; j<arduinoBoards[boardNameID].size(); j++){
            //qDebug() << arduinoBoards[boardNameID][j]["productID"];
            //qDebug() << arduinoBoards[boardNameID][j]["vendorID"];

            if( (arduinoBoards[boardNameID][j]["productID"] == qint16(serialPorts.at(i).productIdentifier())) &&
                    (arduinoBoards[boardNameID][j]["vendorID"] == qint16(serialPorts.at(i).vendorIdentifier())) ){
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

    setArduinoPath();
    setBuildPath();

    proc->start(makeVerifyCommand());
    proc->waitForFinished();

    QString errorOuput = QString(proc->readAllStandardError());

    switch(proc->exitCode()){
    case 0:
        qDebug()<<"Verify OK";
        break;
    case 1:
        throw VerifyException(extractErrorfromOutput(errorOuput), verifyErrorsList);
        break;
    case 2:
        throw VerifyException("Sketch not found");
        break;
    case 3:
        throw VerifyException("Invalid (argument for) commandline option");
        break;
    case 4:
        throw VerifyException("Preference passed to --get-pref does not exist");
        break;
    }

    //return the output of the verification
    QString output(proc->readAllStandardOutput());
    //qDebug() << output;
    return output;


}

QString ArduinoHandler::upload(QString _boardNameID)throw(BoardNotKnownException,
                                                          BoardNotDetectedException,
                                                          VerifyException,
                                                          UploadException)
{
    //check whether de boardName is on the known boards list
    verify(_boardNameID);

    //throws Exception if board not connect to computer
    setBoardPort();

    //makeLoadCommand creates the load command to execute
    proc->start(makeUploadCommand());
    proc->waitForFinished(-1);
    QString errorOutput(proc->readAllStandardError());

    switch(proc->exitCode()){
    case 0:
        qDebug()<<"Upload OK";
        break;
    case 1:
        throw UploadException(errorOutput);
        break;
    case 2:
        throw UploadException("Sketch not found");
        break;
    case 3:
        throw UploadException("Invalid (argument for) commandline option");
        break;
    case 4:
        throw UploadException("Preference passed to --get-pref does not exist");
        break;
    }

    return errorOutput;
}




//This function, called recursively, allows to list the errors removing the local route to the file
QString ArduinoHandler::extractSingleError(QString s){

    QString match = fileWithPath +":";
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
    qDebug() << s;
    QString errorsLine;
    QString match = fileWithPath + ":";
    int pos = s.indexOf(match);
    if ( pos >= 0 )
    {
        errorsLine = s.mid (pos + match.length());
    }

    return extractSingleError(errorsLine);
}


QString LinuxArduinoHandler::makeVerifyCommand(){

    if(arduinoPath.isEmpty()) setArduinoPath();
    if(buildPath.isEmpty()) setBuildPath();

    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    QString verifyCommand = QString(arduinoPath +
                                    "arduino "+
                                    "--verify " +
                                    "--pref build.path=" + buildPath + " " +
                                    "--board " +boardCommand + " " +
                                    fileWithPath);

    qDebug() << verifyCommand;
    return verifyCommand;
}

QString MacArduinoHandler::makeVerifyCommand(){

    if(arduinoPath.isEmpty()) setArduinoPath();
    if(buildPath.isEmpty()) setBuildPath();

    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    QString verifyCommand = QString(arduinoPath +
                                    "Arduino.app/Contents/MacOS/Arduino "+
                                    "--verify " +
                                    "--pref build.path=" + buildPath + " " +
                                    "--board " +boardCommand + " " +
                                    fileWithPath);

    qDebug() << verifyCommand;
    return verifyCommand;
}



QString WindowsArduinoHandler::makeVerifyCommand(){

    if(arduinoPath.isEmpty()) setArduinoPath();
    if(buildPath.isEmpty()) setBuildPath();

    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    QString verifyCommand = QString(arduinoPath +
                                    "arduino_debug.exe "+
                                    "--verify " +
                                    "--pref build.path=" + buildPath + " " +
                                    "--board " +boardCommand + " " +
                                    fileWithPath);

    qDebug() << verifyCommand;
    return verifyCommand;
}

QString LinuxArduinoHandler::makeUploadCommand(){

    if(arduinoPath.isEmpty()) setArduinoPath();
    if(buildPath.isEmpty()) setBuildPath();

    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    QString uploadCommand = QString(arduinoPath +
                                    "arduino "+
                                    "--upload " +
                                    "--board " +boardCommand +
                                    " --port " + boardPort + " " +
                                    "--pref build.path=" + buildPath + " " +
                                    fileWithPath);
    qDebug() << uploadCommand;
    return uploadCommand;
}

QString MacArduinoHandler::makeUploadCommand(){

    if(arduinoPath.isEmpty()) setArduinoPath();
    if(buildPath.isEmpty()) setBuildPath();

    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    QString uploadCommand = QString(arduinoPath +
                                    "Arduino.app/Contents/MacOS/Arduino "+
                                    "--upload " +
                                    "--board " +boardCommand +
                                    " --port " + boardPort + " " +
                                    "--pref build.path=" + buildPath + " " +
                                    fileWithPath);
    qDebug() << uploadCommand;
    return uploadCommand;
}

QString WindowsArduinoHandler::makeUploadCommand(){

    if(arduinoPath.isEmpty()) setArduinoPath();
    if(buildPath.isEmpty()) setBuildPath();

    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    QString uploadCommand = QString(arduinoPath +
                                    "arduino_debug.exe "+
                                    "--upload " +
                                    "--board " +boardCommand +
                                    " --port " + boardPort + " " +
                                    "--pref build.path=" + buildPath + " " +
                                    fileWithPath);
    qDebug() << uploadCommand;
    return uploadCommand;
}
