#ifndef BITBLOQLIBSUPDATER_H
#define BITBLOQLIBSUPDATER_H


#include "githubrestapiclient.h"

class BitbloqLibsUpdater
{
public:
    BitbloqLibsUpdater();
    bool existsNewVersion();
    bool update();

private:
    GitHubRestApiClient __git;
    QJsonObject __localVersionInfo;
    QJsonObject __remoteVersionInfo;
    QString __jsonFilePath;
};

#endif // BITBLOQLIBSUPDATER_H
