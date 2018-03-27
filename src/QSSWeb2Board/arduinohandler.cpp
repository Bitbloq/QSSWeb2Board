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

ArduinoHandler::ArduinoHandler():
    serialMonitor(Q_NULLPTR),
    sketchesDefaultBaseDir(QCoreApplication::applicationDirPath() + "/res/sketches/"),
    arduinoDefaultDir(QCoreApplication::applicationDirPath() + "/res/arduino/"),
    buildDefaultDir(QCoreApplication::applicationDirPath() + "/res/build/"),
    proc(NULL),
    arduinoBoards("knownboards.json")
{
    setSketchesBaseDir(sketchesDefaultBaseDir);
    proc = new QProcess(); //this is to launch the arduino commands

    if(!QDir().exists(sketchesDefaultBaseDir)){
        QDir().mkdir(sketchesDefaultBaseDir);
    }

    if(!QDir().exists(arduinoDefaultDir)){
        QDir().mkdir(arduinoDefaultDir);
    }

    if(!QDir().exists(buildDefaultDir)){
        QDir().mkdir(buildDefaultDir);
    }

    eraseExistingSketches();
}

bool ArduinoHandler::openSerialMonitor(int baudrate){
    serialMonitor = new ArduinoSerialMonitor(getBoardPort(),baudrate);
    return serialMonitor->open();
}

bool ArduinoHandler::closeSerialMonitor(){

    if(serialMonitor){
        serialMonitor->close();
        delete serialMonitor;
        serialMonitor = Q_NULLPTR;
    }

    return true;
}

ArduinoHandler::~ArduinoHandler(){
    //free memory from pointers
    if(proc!=NULL){
        delete proc;
    }

    if(serialMonitor){
        delete serialMonitor;
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
/*    QString path = QCoreApplication::applicationDirPath() + "/res/sketches/";
    QDir dir(path);
    dir.setNameFilters(QStringList() << "*");
    dir.setFilter(QDir::Dirs);

    //remove files older than 1 day
    foreach(QString dirToRemove, dir.entryList())
    {

        QFileInfo fileInfo(path + dirToRemove);
        qDebug() << fileInfo.absoluteFilePath();
        //if older than one day, remove file
        if(fileInfo.created().daysTo(QDateTime().currentDateTime()) > 1 )
            QDir(dirToRemove).removeRecursively();
    }
    */
}

bool ArduinoHandler::writeSketch(QString _sketch, QString _sketchName){
    //remove tail white spaces and retrun cartridges
    _sketch = _sketch.trimmed().simplified();

    if(_sketchName.isEmpty()) sketchName = createRandomString();
    else sketchName = _sketchName;

    QString sketchFileName = sketchName + ".ino";
    QString sketchPath= sketchesBaseDir + sketchName + "/";
    QString sketchWithPath = sketchPath + sketchFileName;

    QDir().mkdir(sketchPath);

    QFile file(sketchWithPath);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << _sketch << endl;
        file.close();
    }else{
        throw FileNotCreatedException("Cannot create sketch file in " +sketchPath + " path");
    }

    return true;
}

bool ArduinoHandler::setArduinoPath(QString s){
    //if no param passed, set the default directory (whis is relative to the app dir)
    if (s == ""){
        //portable arduino ide MUST be in in ~/res/arduino relative to application path !!
        arduinoPath=arduinoDefaultDir;
    }else{
        arduinoPath=s;
    }

    qDebug() << "app: " << QCoreApplication::applicationDirPath();
    qDebug() << arduinoPath;

    return true;
}

bool ArduinoHandler::setBuildPath(QString s){
    //if no param passed, set the default path (which is relative to the app dir)
    if (s == ""){
        //build path MUST be in in ~/res/build relative to application path !!
        buildPath=buildDefaultDir;
    }else{
        buildPath=s;
    }

    qDebug() << buildPath;
    return true;
}

void ArduinoHandler::setSketchesBaseDir(QString s){
    //absolute path to the .ino file to verify
    sketchesBaseDir=s;
    if(!QDir().exists(sketchesBaseDir))
        QDir().mkdir(sketchesBaseDir);
}


bool ArduinoHandler::setSketchWithAbsolutePath(QString s){

    if ( QFile::exists(s)) {
        QString sketchFileName = s.right(s.length() - s.lastIndexOf("/") - 1);
        qDebug() << "sketchFileName: " << sketchFileName;
        sketchName = sketchFileName.left(sketchFileName.length()-4); //remove .ino
        qDebug() << "sketchName: " << sketchName;
        QString sketchPath = s.left(s.lastIndexOf("/") + 1);
        qDebug() << "sketchPath: " << sketchPath;
        sketchesBaseDir = sketchPath.left(sketchPath.length()-1);
        sketchesBaseDir = sketchesBaseDir.left(sketchesBaseDir.lastIndexOf("/") + 1);
        qDebug() << "sketchBasePath: " << sketchesBaseDir;
    }else{
        throw FileNotFoundException("File "+s+" does not exist");
    }

    return true;
}

bool ArduinoHandler::setBoardNameID(QString s){
    //check whether we know that board
    //remove tail spaces and tail carriadge returns
    s = s.trimmed().simplified();
    if (arduinoBoards[s].isUndefined()){
        throw BoardNotKnownException("BOARD NOT KNOWN: " + s);
        return false;
    }else{
        boardNameID=s;
        return true;
    }
}

bool ArduinoHandler::setBoardPort(QString _boardPort){
    QSerialPort port(_boardPort);
    QSerialPortInfo serialPort(port);
    if(serialPort.isNull()){
        throw BoardNotDetectedException("NO BOARD CONNECTED TO " + _boardPort);
    }else{
        boardPort = _boardPort;
    }
    return true;
}

bool ArduinoHandler::autoDetectBoardPort(){

    if(boardNameID.isEmpty()){
        throw BoardNotSetException("BOARDNAME NOT SET");
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
                QString _boardPort=serialPorts.at(i).systemLocation();
                qDebug() << boardNameID << " found at " << _boardPort;
                return setBoardPort(_boardPort);

            }
        }
    }

    throw BoardNotDetectedException("BOARD NOT DETECTED: " + boardNameID);
    //board name not connected to the computer
    qDebug() << boardNameID << "not connected to the computer";
    return false;
}

int ArduinoHandler::verify(){

    if(sketchName.isEmpty()) throw SketchNotSetException("Sketch not set");
    if(boardNameID.isEmpty()) throw BoardNotSetException("BoardNameID not set");

    setArduinoPath();
    setBuildPath();

    QString command = makeVerifyCommand();

    proc->start(command);
    proc->waitForFinished();

    QString output = QString(proc->readAllStandardError());
    int exitCode = proc->exitCode();
    switch(exitCode){
    case 0:
        qDebug()<<"Verify OK";
        break;
    case 1:
        throw VerifyException(extractErrorfromOutput(output), verifyErrorsList);
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

    return exitCode;


}

int ArduinoHandler::upload()
{
    if(sketchName.isEmpty()) throw SketchNotSetException("Sketch not set");
    if(boardNameID.isEmpty()) throw BoardNotSetException("BoardNameID not set");
    if(boardPort.isEmpty()) autoDetectBoardPort();

    verify();

    //makeLoadCommand creates the load command to execute
    proc->start(makeUploadCommand());
    proc->waitForFinished(-1);
    QString errorOutput(proc->readAllStandardError());
    int exitCode = proc->exitCode();
    switch(exitCode){
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

    return exitCode;
}




//This function, called recursively, allows to list the errors removing the local route to the file
QString ArduinoHandler::extractSingleError(QString s){

    QString match = sketchesBaseDir + sketchName + "/" + sketchName + "ino:";
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

QString ArduinoHandler::getBoardPort() const
{
    return boardPort;
}

//This function, together with extractSingleError creates a string with all the errors eliminating reference
//to local routes
QString ArduinoHandler::extractErrorfromOutput(QString s){
    qDebug() << s;
    QString errorsLine;
    QString match = sketchesBaseDir + sketchName + "/" + sketchName + "ino:";
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
                                    sketchesBaseDir + sketchName + "/" + sketchName + "ino");

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
                                    sketchesBaseDir + sketchName + "/" + sketchName + "ino");

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
                                    sketchesBaseDir + sketchName + "/" + sketchName + "ino");

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
                                    sketchesBaseDir + sketchName + "/" + sketchName + "ino");
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
                                    sketchesBaseDir + sketchName + "/" + sketchName + "ino");
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
                                    sketchesBaseDir + sketchName + "/" + sketchName + "ino");
    qDebug() << uploadCommand;
    return uploadCommand;
}
