#include <QCoreApplication>
#include <QProcess>
#include <QDebug>

#include "arduinohandler.h"
#include "arduinoexceptions.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#if (defined (Q_OS_WIN))
    WindowsArduinoHandler arduino;
#elif (defined (Q_OS_LINUX))
    LinuxArduinoHandler arduino;
#elif (defined (Q_OS_MAC))
    MacArduinoHandler arduino;
#endif

    try{
        arduino.setFileWithFullPath("/home/avalero/arduino-1.8.5/examples/01.Basics/Blink/Blink.ino");
        arduino.setFileWithFullPath("/home/avalero/arduino-1.8.5/examples/error/error.ino");
        arduino.setBoardNameID("ZUMCore");
        arduino.setBoardPort();
        arduino.verify("ZUMCore");
        arduino.upload("ZUMCore");
    }catch(FileNotFoundException &e){
        qDebug() << e.message;
    }catch(BoardNotKnownException &e){
        qDebug() << e.getMessage();
    }catch(BoardNotDetectedException &e){
        qDebug() << e.getMessage();
    }catch(VerifyException &e){
        qDebug() << "Found " << e.errorsList.size() << " verification errors";
        for(int i=0;i<e.errorsList.size();i++){
            qDebug() << i + 1 << ".- " << e.errorsList.at(i);
        }
    }catch(UploadException &e){
        qDebug() << e.message;
    }

    return 1; //a.exec();
}
