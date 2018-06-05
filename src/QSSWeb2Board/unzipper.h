#ifndef UNZIPPER_H
#define UNZIPPER_H

#include <QString>

class UnZipper
{
public:
    UnZipper();
    static void unzip(QString filename, QString targetdir);
};

#endif // UNZIPPER_H
