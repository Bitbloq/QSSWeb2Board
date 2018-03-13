#ifndef ARDUINOHANDLER_H
#define ARDUINOHANDLER_H

#include <QObject>

//forward declararion of classes
class QProcess;

class ArduinoHandler
{
public:
    ArduinoHandler();
    virtual ~ArduinoHandler();

    void setExecutableDir(QString s);
    void setSourceDir(QString s);
    void setFileName(QString s);
    void setBoardName(QString s);
    void setBoardPort(QString s);

    void verify();
    void load();

private:
    QString executableDir;
    QString sourdeDir;
    QString fileName;
    QString boardName;
    QString boardPort;

    QProcess *proc;
};

#endif // ARDUINOHANDLER_H
