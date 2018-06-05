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
    GitHubRestApiClient __git;
    QJsonObject __localVersionInfo;
    QJsonObject __remoteVersionInfo;
    QString __jsonFilePath;
    QString __arduinoDir;
    QString __tmpDir;
};

#endif // BITBLOQLIBSUPDATER_H
