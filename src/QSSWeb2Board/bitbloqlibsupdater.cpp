#include "bitbloqlibsupdater.h"
#include "unzipper.h"
#include "arduinoexceptions.h"


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
    qInfo() << "Versions file " << __jsonFilePath;

    __remoteVersionInfo = __git.getLatestReleaseVersion("bitbloq","bitbloqLibs");

    //if local file does not exist return existing new version
    if(!jsonFile.exists()){
        qInfo() << "No version.json file";
        return true;
    }

    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString val = jsonFile.readAll();
    jsonFile.close();

    __localVersionInfo = QJsonDocument::fromJson(val.toUtf8()).object();


    qInfo() << "Current bitbloqLibs version " << __remoteVersionInfo["version"].toString();
    qInfo() << "Local bitbloqLibs version " << __localVersionInfo["version"].toString();

    return ( __remoteVersionInfo["version"] != __localVersionInfo["version"] );
}

bool BitbloqLibsUpdater::update(){

    qInfo() << "Updating to new version";

    //REMOVE PREVIOUS TMP FILES
    QDir(__tmpDir).removeRecursively();

    //DOWNLOAD ZIP FILE
    __git.downloadFile(__remoteVersionInfo["zipball_url"].toString(),
            __tmpDir,
            "bitbloqLibs.zip",
            10000);

    //UNCOMPRESS ZIP FILE
    QString zipfilename = __tmpDir + "bitbloqLibs.zip";

    UnZipper::unzip(zipfilename,__tmpDir);

    //REMOVE FORMER BITBLOQLIBS
    QString arduinoLibrariesDir = __arduinoDir + "libraries";
    qInfo()  << "Removing former libs version " << arduinoLibrariesDir;
    QDir(arduinoLibrariesDir).removeRecursively();

    qInfo() << "Removed "  << arduinoLibrariesDir;



    //MOVE NEW LIBS TO ARDUINO LIBS DIRECTORY


    QString temp_LibsDir;

    QDirIterator it(__tmpDir);
    while (it.hasNext()) {
        QString foundDir = it.next();
        if(foundDir.contains("-bitbloqLibs-"))
            temp_LibsDir = foundDir;

    }

    qInfo() << "TEMP DIR " << temp_LibsDir;

    QDir dir;
    if( !dir.rename(temp_LibsDir,arduinoLibrariesDir) ){
      throw CannotMoveTmpLibsException("Cannot move libraries to " + arduinoLibrariesDir);
    }

    qInfo() << "libraries saved to " << arduinoLibrariesDir;

    //If no errors is done

    __localVersionInfo.insert("version",__remoteVersionInfo["version"].toString());
    __localVersionInfo.insert("zipball_url",__remoteVersionInfo["zipball_url"].toString());
    __localVersionInfo.insert("tarball_url",__remoteVersionInfo["tarball_url"].toString());

    QFile jsonFile;
    jsonFile.setFileName(__jsonFilePath);
    jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
    jsonFile.resize(0); //clear all contents
    jsonFile.write(QJsonDocument(__localVersionInfo).toJson());
    jsonFile.close();

    //REMOVE TMP FILES
    QDir(__tmpDir).removeRecursively();

    return true;
}
