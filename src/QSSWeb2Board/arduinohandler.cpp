#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <QSerialPortInfo>
#include <QJsonArray>
#include <QSerialPort>
#include <QDir>
#include <QDateTime>

#include "web2boardexceptions.h"
#include "arduinohandler.h"

ArduinoHandler::ArduinoHandler():
    QObject(),
    serialMonitor{Q_NULLPTR},

    sketchesDefaultBaseDir{QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_SKETCHES").isEmpty() ?
                               QCoreApplication::applicationDirPath() + "/tmp/sketches/" :
                               QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_SKETCHES")},

    arduinoDefaultDir{QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_ARDUINO").isEmpty() ?
                               QCoreApplication::applicationDirPath() + "/res/arduino/" :
                               QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_ARDUINO")},

    buildDefaultDir{(QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_BUILD").isEmpty()) ?
                               QCoreApplication::applicationDirPath() + "/tmp/build/" :
                               QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_BUILD")},

    arduinoBoards{QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_KNOWNBOARDS").isEmpty() ?
                               QCoreApplication::applicationDirPath() + "/res/arduino/libraries/knownboards.json" :
                               QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_KNOWNBOARDS")},

    tmpDir{(QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_TMP").isEmpty()) ?
                               QCoreApplication::applicationDirPath() + "/tmp/" :
                               QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_TMP")}

{
    qsrand(uint(QDateTime::currentMSecsSinceEpoch())); //seed for initializing randomstrings
    setSketchesBaseDir(sketchesDefaultBaseDir);

   //qDebug() << "ARDUINO HANDLER CONSTRUCTOR";
   if(!QDir().exists(tmpDir)){
       QDir().mkdir(tmpDir);

   }
    if(!QDir().exists(sketchesDefaultBaseDir)){
        QDir().mkdir(sketchesDefaultBaseDir);

    }

    if(!QDir().exists(buildDefaultDir)){
        QDir().mkdir(buildDefaultDir);

    }
}

void ArduinoHandler::updateArduinoBoards(){
    QString jsonFile{ QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_KNOWNBOARDS").isEmpty() ?
                                   QCoreApplication::applicationDirPath() + "/res/arduino/libraries/knownboards.json" :
                                   QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_KNOWNBOARDS") };

    arduinoBoards.setKnownBoards(jsonFile);
}

bool ArduinoHandler::openSerialMonitor(int baudrate){
   if(serialMonitor){
       serialMonitor->close();
       delete serialMonitor;
    }

    if(boardNameID.isEmpty()) throw BoardNotSetException("BoardNameID not set");
    if(boardPort.isEmpty()) autoDetectBoardPort();



    serialMonitor = new ArduinoSerialMonitor(boardPort,baudrate);

    QObject::connect(&serialMonitor->port, SIGNAL(readyRead()), serialMonitor, SLOT(readArduino()));
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

    if(serialMonitor){
        delete serialMonitor;
    }
}

QString ArduinoHandler::createRandomString() const{

    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    const int randomStringLength = 12; // assuming you want random strings of 12 characters


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
    QDir dirToClean(sketchesDefaultBaseDir);

    dirToClean.setNameFilters(QStringList() << "*");
    dirToClean.setFilter(QDir::Dirs);
    foreach(const QString &subDir, dirToClean.entryList())
    {
        if (subDir != "." && subDir != ".."){
            QFileInfo sketchInfo(sketchesDefaultBaseDir + subDir + "/" + subDir + ".ino");
            //remove sketches older than one day
            if(sketchInfo.created().addDays(1) <= QDateTime::currentDateTime()){
                //qDebug() << "erasing " + sketchesDefaultBaseDir + subDir + "/";
                QDir(sketchesDefaultBaseDir + subDir + "/").removeRecursively();
            }
        }
    }
    setPermissions(tmpDir, "777");
    setPermissions(sketchesDefaultBaseDir, "777");
    setPermissions(buildDefaultDir, "777");

}

void ArduinoHandler::eraseExistingBuildFiles() const {
//TODO
}


bool ArduinoHandler::writeSketch(QString _sketch, QString _sketchName){
    //remove tail white spaces and retrun cartridges

    if(_sketchName.isEmpty()) sketchName = createRandomString();
    else sketchName = _sketchName;

    if(sketchesBaseDir.isEmpty()) setSketchesBaseDir(sketchesDefaultBaseDir);

    QString sketchFileName = sketchName + ".ino";
    QString sketchPath= sketchesBaseDir + sketchName + "/";
    QString sketchWithPath = sketchPath + sketchFileName;


    //if there is a sketch with the same name remove
    if(QFile().exists(sketchWithPath)){
        QFile().remove(sketchWithPath);
    }

    //if sketch dir does not exist create
    if(!QDir().exists(sketchPath)){
        if(!QDir().mkdir(sketchPath))
            throw DirNotCreatedException("Cannot create sketch dir in " + sketchPath);
    }

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

bool ArduinoHandler::resetPathsToDefault(){
    return (setArduinoPath() &&
            setBuildPath() &&
            setSketchesBaseDir()
            );
}

bool ArduinoHandler::setArduinoPath(QString s){
    //if no param passed, set the default directory (whis is relative to the app dir)
    if (s == ""){
        arduinoPath=arduinoDefaultDir;
    }else{
        arduinoPath=s;
    }
    qDebug() << "Arduino Path: " << arduinoPath << "arduino";

    if(! checkArduinoPath(arduinoPath) ) throw(ArduinoNotFoundException("Arduino not found at " + arduinoPath));

    //we must not create this directory because it should already exist

    return true;
}

bool ArduinoHandler::setBuildPath(QString s){
    //if no param passed, set the default path (which is relative to the app dir)
    if (s == ""){
        //build path MUST be in in ~/res/build relative to application path !!
        buildPath=buildDefaultDir;
        qDebug() << "Build Path: " << buildPath;
    }else{
        buildPath=s;
    }

    if(!QDir().exists(buildPath)){
        qDebug() << "Path does not exist";
        return QDir().mkdir(buildPath);
    }

    return true;
}

bool ArduinoHandler::setSketchesBaseDir(QString s){
    if(s==""){
        sketchesBaseDir = sketchesDefaultBaseDir;
    }else{
        //absolute path to the .ino file to verify
        sketchesBaseDir=s;
    }

    if(!QDir().exists(sketchesBaseDir))
        return QDir().mkdir(sketchesBaseDir);

    return true;
}


bool ArduinoHandler::setSketchWithAbsolutePath(QString s){

    if ( QFile::exists(s)) {

        QString sketchFileName = s.right(s.length() - s.lastIndexOf("/") - 1);
        sketchName = sketchFileName.left(sketchFileName.length()-4); //remove .ino

        QString sketchPath = s.left(s.lastIndexOf("/") + 1);
        sketchesBaseDir = sketchPath.left(sketchPath.length()-1);
        sketchesBaseDir = sketchesBaseDir.left(sketchesBaseDir.lastIndexOf("/") + 1);
    }else{
        throw FileNotFoundException("File " + s + " does not exist");
    }

    return true;
}

bool ArduinoHandler::setBoardNameID(QString s){
    //check whether we know that board
    //remove tail spaces and tail carriadge returns
    s = s.trimmed().simplified();
    if (arduinoBoards[s].isUndefined()){
        throw BoardNotKnownException("BOARD NOT KNOWN: " + s);

    }else{
        boardNameID=s;
        return true;
    }
}

bool ArduinoHandler::setBoardPort(QString _boardPort){
    QSerialPort port(_boardPort);
    QSerialPortInfo serialPort(port);

    //Check whether there is anything connected to that port
    //it does not imply that there is an Arduino Board there
    if(serialPort.isNull()){
        throw BoardNotDetectedException("NO BOARD CONNECTED TO " + _boardPort);
    }else{
        boardPort = _boardPort;
    }

    return true;
}

QString ArduinoHandler::getHex(){

    QString hexfilename = buildPath + sketchName + ".ino.hex";
    //qDebug() << "Hex filename: " << hexfilename;

    QFile f(hexfilename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) throw HexFileException("HEX FILE NOT FOUND");
    QTextStream in(&f);
    QString ret = in.readAll();
    f.close();

    return ret;
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

    //get all productID and vendorID and check if any is equal to boardNameID

    foreach(const QSerialPortInfo & port,serialPorts){
        //loop over all vendorID and productID of selected board to check whether the board is connected (compare con vendor and product of connected serial ports)
        for (int j=0; j<arduinoBoards[boardNameID].size(); j++){
            if( (arduinoBoards[boardNameID][j]["productID"] == qint16(port.productIdentifier())) &&
                    (arduinoBoards[boardNameID][j]["vendorID"] == qint16(port.vendorIdentifier())) ){
                //Yay found, save board port
                QString _boardPort=port.systemLocation();
                return setBoardPort(_boardPort); //this is not optimal because we are making extra checking inside the function (already done here)
            }
        }
    }

    throw BoardNotDetectedException("BOARD NOT DETECTED: " + boardNameID);
    //board name not connected to the computer

}


int ArduinoHandler::verify(){

    qInfo() << "Verifying sketch...";
    QString command = makeVerifyCommand();

    QProcess *proc = new QProcess();
    //let's verify the sketch
    proc->start(command);

    //blocker instruction, it blocks program until proc is finished.
    proc->waitForFinished();

    int exitCode = proc->exitCode();
    QString output = QString(proc->readAllStandardError());
    proc->close();

    delete proc;

    switch(exitCode){
    case 0:
        qDebug()<<"Verify OK";
        break;
    case 1:
        throw VerifyException(output, verifyErrorsList);
    case 2:
        throw VerifyException("Sketch not found");
    case 3:
        throw VerifyException("Invalid (argument for) commandline option");
    case 4:
        throw VerifyException("Preference passed to --get-pref does not exist");
    }

    return exitCode;
}



int ArduinoHandler::asyncVerify(int buildPathCounter){

    qInfo() << "Verifying sketch...";

    setBuildPath( tmpDir + "build" + QString::number(buildPathCounter) + "/");

    qDebug()<< "buildPath (async): "  << buildPath;
    QString command = makeVerifyCommand();

    QProcess* proc = new QProcess;
    //let's verify the sketch
    proc->start(command);

    //non blocker envent loop
    QEventLoop eventLoop;
    QObject::connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    int exitCode = proc->exitCode();
    QString output = QString(proc->readAllStandardError());
    proc->close();

    delete proc;

    switch(exitCode){
    case 0:
        qDebug()<<"Verify OK";
        break;
    case 1:
        throw VerifyException(output, verifyErrorsList);
    case 2:
        throw VerifyException("Sketch not found");
    case 3:
        throw VerifyException("Invalid (argument for) commandline option");
    case 4:
        throw VerifyException("Preference passed to --get-pref does not exist");
    }

    return exitCode;
}

int ArduinoHandler::upload()
{

    qInfo() << "Uploading sketch to board...";

    QProcess *proc = new QProcess();
    //makeUploadCommand creates the load command to execute
    proc->start(makeUploadCommand());
    proc->waitForFinished(30000); //-1 means there is no time out

    QString errorOutput(proc->readAllStandardError());
    int exitCode = proc->exitCode();
    proc->close();

    delete proc;

    switch(exitCode){
    case 0:
        qDebug()<<"Upload OK";
        break;
    case 1:
        throw UploadException(errorOutput);
    case 2:
        throw UploadException("Sketch not found");
    case 3:
        throw UploadException("Invalid (argument for) commandline option");
    case 4:
        throw UploadException("Preference passed to --get-pref does not exist");
    }

    return exitCode;
}


//This function, called recursively, allows to list the errors removing the local route to the file
QString ArduinoHandler::extractSingleError(QString s){

    QJsonObject error;

    QString match = sketchesBaseDir + sketchName + "/" + sketchName + "ino:";
    int pos = s.indexOf(match);
    if ( pos >= 0 )
    {
        QString leftOf = s.left(pos);
        //qDebug() << "leftOf: " << leftOf;
        verifyErrorsList.append(leftOf.simplified());
        return (leftOf.simplified() + extractSingleError(s.mid (pos + match.length())));
    }else{
        verifyErrorsList.append(s.simplified());
        return s.simplified();
    }
}

QString ArduinoHandler::getArduinoDefaultDir() const
{
    return arduinoDefaultDir;
}

QString ArduinoHandler::getBoardPort() const
{
    return boardPort;
}

//This function, together with extractSingleError creates a string with all the errors eliminating reference
//to local routes
QString ArduinoHandler::extractErrorfromOutput(QString s){
    QString errorsLine;
    QString match = sketchesBaseDir + sketchName + "/" + sketchName + ".ino:";
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
    if(sketchName.isEmpty()) throw SketchNotSetException("Sketch not set");
    if(boardNameID.isEmpty()) throw BoardNotSetException("BoardNameID not set");



    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    QString verifyCommand = QString(arduinoPath +
                                    "arduino "+
                                    "--verify " +
                                    "--pref build.path=" + buildPath + " " +
                                    "--board " +boardCommand + " " +
                                    sketchesBaseDir + sketchName + "/" + sketchName + ".ino");

    qDebug() << "Verify command: " << verifyCommand;

    return verifyCommand;
}

QString MacArduinoHandler::makeVerifyCommand(){

    if(arduinoPath.isEmpty()) setArduinoPath();
    if(buildPath.isEmpty()) setBuildPath();
    if(sketchName.isEmpty()) throw SketchNotSetException("Sketch not set");
    if(boardNameID.isEmpty()) throw BoardNotSetException("BoardNameID not set");

    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    QString verifyCommand = QString(arduinoPath +
                                    "Arduino.app/Contents/MacOS/Arduino "+
                                    "--verify " +
                                    "--pref build.path=" + buildPath + " " +
                                    "--board " +boardCommand + " " +
                                    sketchesBaseDir + sketchName + "/" + sketchName + ".ino");

    return verifyCommand;
}



QString WindowsArduinoHandler::makeVerifyCommand(){

    if(arduinoPath.isEmpty()) setArduinoPath();
    if(buildPath.isEmpty()) setBuildPath();
    if(sketchName.isEmpty()) throw SketchNotSetException("Sketch not set");
    if(boardNameID.isEmpty()) throw BoardNotSetException("BoardNameID not set");

    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    QString verifyCommand = QString(arduinoPath +
                                    "arduino_debug.exe "+
                                    "--verify " +
                                    "--pref build.path=" + buildPath + " " +
                                    "--board " +boardCommand + " " +
                                    sketchesBaseDir + sketchName + "/" + sketchName + ".ino");

    return verifyCommand;
}

QString LinuxArduinoHandler::makeUploadCommand(){

    if(boardNameID.isEmpty()) throw BoardNotSetException("BoardNameID not set");
    if(boardPort.isEmpty()) autoDetectBoardPort();
    if(arduinoPath.isEmpty()) setArduinoPath();
    if(buildPath.isEmpty()) setBuildPath();
    if(sketchName.isEmpty()) throw SketchNotSetException("Sketch not set");

    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    QString uploadCommand = QString(arduinoPath +
                                    "arduino "+
                                    "--upload " +
                                    "--board " +boardCommand +
                                    " --port " + boardPort + " " +
                                    "--pref build.path=" + buildPath + " " +
                                    sketchesBaseDir + sketchName + "/" + sketchName + ".ino");
    return uploadCommand;
}

void LinuxArduinoHandler::setPermissions(QString dir, QString permissions) const{
   QProcess proc;
   QString command = "chmod -R " + permissions + " " + dir;
   // qDebug() << "permissions command: "<< command;
   proc.start(command);
   proc.waitForFinished();
   // qDebug() << "changed premissions exit code: " << proc.exitCode();
}

QString MacArduinoHandler::makeUploadCommand(){

    if(boardNameID.isEmpty()) throw BoardNotSetException("BoardNameID not set");
    if(boardPort.isEmpty()) autoDetectBoardPort();
    if(arduinoPath.isEmpty()) setArduinoPath();
    if(buildPath.isEmpty()) setBuildPath();
    if(sketchName.isEmpty()) throw SketchNotSetException("Sketch not set");

    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    QString uploadCommand = QString(arduinoPath +
                                    "Arduino.app/Contents/MacOS/Arduino "+
                                    "--upload " +
                                    "--board " +boardCommand +
                                    " --port " + boardPort + " " +
                                    "--pref build.path=" + buildPath + " " +
                                    sketchesBaseDir + sketchName + "/" + sketchName + ".ino");
    return uploadCommand;
}

QString WindowsArduinoHandler::makeUploadCommand(){

    if(boardNameID.isEmpty()) throw BoardNotSetException("BoardNameID not set");
    if(boardPort.isEmpty()) autoDetectBoardPort();
    if(arduinoPath.isEmpty()) setArduinoPath();
    if(buildPath.isEmpty()) setBuildPath();
    if(sketchName.isEmpty()) throw SketchNotSetException("Sketch not set");

    QString boardCommand = arduinoBoards[boardNameID].toObject().value("board").toString();
    QString uploadCommand = QString(arduinoPath +
                                    "arduino_debug.exe "+
                                    "--upload " +
                                    "--board " +boardCommand +
                                    " --port " + boardPort + " " +
                                    "--pref build.path=" + buildPath + " " +
                                    sketchesBaseDir + sketchName + "/" + sketchName + ".ino");
    return uploadCommand;
}

bool WindowsArduinoHandler::checkArduinoPath(QString arduinoPath){
    return (QFile(arduinoPath + "arduino_debug.exe").exists());
}

bool MacArduinoHandler::checkArduinoPath(QString arduinoPath){
    return (QFile(arduinoPath + "Arduino.app/Contents/MacOS/Arduino").exists());
}

bool LinuxArduinoHandler::checkArduinoPath(QString arduinoPath){
    return (QFile(arduinoPath + "arduino").exists());
}

