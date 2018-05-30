#include "bitbloqlibsupdater.h"
#include "unzipper.h"
#include "arduinoexceptions.h"


BitbloqLibsUpdater::BitbloqLibsUpdater(QString arduinoDir):
    __jsonFilePath{QCoreApplication::applicationDirPath() + "/res/versions.json"},
    __arduinoDir{arduinoDir}
{

}

bool BitbloqLibsUpdater::existsNewVersion()
{
    //local versions are stored in local file
    QFile jsonFile;
    jsonFile.setFileName(__jsonFilePath);

    //if local file does not exist return existing new version
    if(!jsonFile.exists()){
        return true;
    }
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString val = jsonFile.readAll();
    jsonFile.close();

    __localVersionInfo = QJsonDocument::fromJson(val.toUtf8()).object();
    __remoteVersionInfo = __git.getLatestReleaseVersion("bq","bitbloqLibs");


    qInfo() << "Current bitbloqLibs version " << __remoteVersionInfo["version"].toString();
    qInfo() << "Local bitbloqLibs version " << __localVersionInfo["version"].toString();

    return ( __remoteVersionInfo["version"] != __localVersionInfo["version"] );
}

bool BitbloqLibsUpdater::update(){

    qInfo() << "Updating to new version";


    //DOWNLOAD ZIP FILE
    __git.downloadFile(__remoteVersionInfo["zipball_url"].toString(),
            QCoreApplication::applicationDirPath() + "/tmp",
            "bitbloqLibs.zip",
            10000);

    //UNCOMPRESS ZIP FILE

    QString zipfilename = QCoreApplication::applicationDirPath() + "/tmp/bitbloqLibs.zip";
    QString temp_targetDir = QCoreApplication::applicationDirPath() + "/tmp/";

    UnZipper::unzip(zipfilename,temp_targetDir);

    //REMOVE FORMER BITBLOQLIBS

    QString arduinoLibrariesDir = __arduinoDir + "libraries";
    QDir(arduinoLibrariesDir).removeRecursively();

    //MOVE NEW LIBS TO ARDUINO LIBS DIRECTORY
    QString temp_LibsDir = temp_targetDir + "/bitbloqLibs-" + __remoteVersionInfo["version"].toString();
    QDir dir;
    if( !dir.rename(temp_LibsDir,arduinoLibrariesDir) ){
      throw CannotMoveTmpLibsException("Cannot move libraries to " + arduinoLibrariesDir);
    }

    //If no errors is done

    __localVersionInfo.insert("version",__remoteVersionInfo["version"].toString());
    __localVersionInfo.insert("zipball_url",__remoteVersionInfo["zipball_url"].toString());
    __localVersionInfo.insert("tarball_url",__remoteVersionInfo["tarball_url"].toString());
    __localVersionInfo.insert("file",  QCoreApplication::applicationDirPath() + "/tmp/bitbloqLibs.zip");

    QFile jsonFile;
    jsonFile.setFileName(__jsonFilePath);
    jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
    jsonFile.resize(0); //clear all contents
    jsonFile.write(QJsonDocument(__localVersionInfo).toJson());
    jsonFile.close();


    return true;
}
