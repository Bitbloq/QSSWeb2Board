#include "bitbloqlibsupdater.h"
#include "unzipper.h"
#include "web2boardexceptions.h"


BitbloqLibsUpdater::BitbloqLibsUpdater(QString arduinoDir):
    __jsonFilePath{QCoreApplication::applicationDirPath() + "/res/versions.json"},
    __arduinoDir{arduinoDir},
    __tmpDir{(QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_TMP").isEmpty()) ?
                              QCoreApplication::applicationDirPath() + "/tmp/" :
                               QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_TMP")}

{

}

bool BitbloqLibsUpdater::existsNewVersion()
{
    //local versions are stored in local file
    QFile jsonFile;
    jsonFile.setFileName(__jsonFilePath);
    qDebug() << "Versions file " << __jsonFilePath;

    __remoteVersionInfo = __git.getLatestReleaseVersion("bitbloq","bitbloqLibs");

    //if local file does not exist returns existing new version
    if(!jsonFile.exists()){
        qDebug() << "No version.json file";
        return true; // new version
    }

    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString val = jsonFile.readAll();
    jsonFile.close();

    __localVersionInfo = QJsonDocument::fromJson(val.toUtf8()).object();


    qDebug() << "Current (remote) bitbloqLibs version " << __remoteVersionInfo["version"].toString();
    qDebug() << "Local bitbloqLibs version " << __localVersionInfo["version"].toString();

    return ( __remoteVersionInfo["version"] != __localVersionInfo["version"] ); //false if there is a new version
}

bool BitbloqLibsUpdater::update(){

    qInfo() << "Updating bitbloq libraries to new version";

    //CREATE TMP DIR IF IT DOES NOT EXIST
    qDebug()<< "Temp Dir: " << __tmpDir;
    if(!QDir(__tmpDir).exists()){
        QDir().mkdir(__tmpDir);
    }

    //DOWNLOAD ZIP FILE
    if(! __git.downloadFile(__remoteVersionInfo["zipball_url"].toString(),
            __tmpDir, //store in __tmpDir
            "bitbloqLibs.zip", //store as bitbloqlibs.zip
            20000) //20 seconds timeout
            )
    {
        qDebug() << "TimeOut Downloading Libs";
        throw GetTimeOutException("TimeOut Downloading Libs");
    }

    //UNCOMPRESS ZIP FILE
    QString zipfilename = __tmpDir + "bitbloqLibs.zip";

    if(UnZipper::unzip(zipfilename,__tmpDir) == 0){
        qDebug() << "Succesfully Unzipped";
    }else{
        throw CannotMoveTmpLibsException("Cannot unzip file");
    }; //unzip zipfile into __tmpDir




    //MOVE NEW LIBS TO ARDUINO LIBS DIRECTORY


    QString temp_LibsDir;

    QDirIterator it(__tmpDir);
    while (it.hasNext()) {
        QString foundDir = it.next();
        if(foundDir.contains("-bitbloqLibs-")){
            temp_LibsDir = foundDir;
        }
    }


    if(temp_LibsDir.isEmpty()){
        throw CannotMoveTmpLibsException("Cannot find unzipped dir");
    }else{
        qDebug() << "TEMP DIR " << temp_LibsDir;
    }

    //REMOVE FORMER BITBLOQLIBS
    QString arduinoLibrariesDir = __arduinoDir + "libraries";
    qDebug()  << "Removing former libs version " << arduinoLibrariesDir;
    QDir(arduinoLibrariesDir).removeRecursively();

    qDebug() << "Removed "  << arduinoLibrariesDir;


    QDir dir;
    if( !dir.rename(temp_LibsDir,arduinoLibrariesDir) ){
      throw CannotMoveTmpLibsException("Cannot move libraries to " + arduinoLibrariesDir);
    }

    qDebug() << "libraries saved to " << arduinoLibrariesDir;


#if (defined (Q_OS_LINUX))
    QProcess proc;
    QString command = "chmod -R 777 " + arduinoLibrariesDir;
    qInfo() << "permissions command: "<< command;
    proc.start(command);
    proc.waitForFinished();
    qInfo() << "changed premissions exit code: " << proc.exitCode();
#endif
    //If no errors, the upgrade is done. update local version info

    __localVersionInfo.insert("version",__remoteVersionInfo["version"].toString());
    __localVersionInfo.insert("zipball_url",__remoteVersionInfo["zipball_url"].toString());
    __localVersionInfo.insert("tarball_url",__remoteVersionInfo["tarball_url"].toString());

    QFile jsonFile;
    jsonFile.setFileName(__jsonFilePath);
    qDebug() << "Open " << __jsonFilePath;
    jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
    qDebug() << "Delete contents...";
    jsonFile.resize(0); //clear all contents
    qDebug() << "Write version info: " << QJsonDocument(__localVersionInfo).toJson();
    jsonFile.write(QJsonDocument(__localVersionInfo).toJson());
    qDebug() << "Close versions file";
    jsonFile.close();

    //REMOVE TMP FILES
    QFile(zipfilename).remove();
    QDir(temp_LibsDir).removeRecursively();

    return true;
}
