#include "unzipper.h"
#include <QProcess>
#include <QDebug>
#include <QCoreApplication>

UnZipper::UnZipper()
{

}

void UnZipper::unzip(QString filename, QString targetdir){
#if (defined (Q_OS_WIN))
    QString command = QCoreApplication::applicationDirPath() + "/unzip/bin/unzip.exe -o " + filename + " -d " + targetdir;
#elif (defined (Q_OS_LINUX))
    QString command = "unzip -o " + filename + " -d " + targetdir;
#elif (defined (Q_OS_MAC))

#endif

    qInfo() << command;
    QProcess proc;
    proc.start(command);
    proc.waitForFinished();
    qInfo() << proc.exitCode();
    proc.close();
}
