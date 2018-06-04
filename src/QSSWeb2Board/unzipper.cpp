#include "unzipper.h"
#include <QProcess>
#include <QDebug>

UnZipper::UnZipper()
{

}

void UnZipper::unzip(QString filename, QString targetdir){
#if (defined (Q_OS_WIN))

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
