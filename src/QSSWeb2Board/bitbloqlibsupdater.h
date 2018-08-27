#ifndef BITBLOQLIBSUPDATER_H
#define BITBLOQLIBSUPDATER_H


#include "githubrestapiclient.h"

class BitbloqLibsUpdater
{
public:
    BitbloqLibsUpdater(QString arduinoDir);
    bool existsNewVersion();
    bool update();

private:
    GitHubRestApiClient __git; ///client of the Github API repository
    QJsonObject __localVersionInfo; ///info of bitbloq libs stored locally
    QJsonObject __remoteVersionInfo; ///info of bitbloqlibs stored remotelly
    QString __arduinoDir; ///absolute path to arduino installation
    QString __tmpDir; ///absolute path to temp files to store transient data.
    QString __arduinoLibrariesDir; ///absolut path to Arduino Libraries
    QString __jsonFilePath; ///absolute path to the json file storing local version info
};

#endif // BITBLOQLIBSUPDATER_H
