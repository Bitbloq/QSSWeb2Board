#include <QCoreApplication>
#include <QProcess>
#include <QDebug>
#include <QtTest/QtTest>

#include "sslserver.h"

ArduinoHandler* Arduino(){
    #if (defined (Q_OS_WIN))
        WindowsArduinoHandler *arduino = new WindowsArduinoHandler();
    #elif (defined (Q_OS_LINUX))
        LinuxArduinoHandler * arduino = new LinuxArduinoHandler();
    #elif (defined (Q_OS_MAC))
        MacArduinoHandler *arduino = new MacArduinoHandler();
    #endif
    return arduino;
}


namespace Examples{
namespace ArduinoExample1{
int main(void){
    ArduinoHandler* arduino = Arduino();
    try{
        arduino->setBoardNameID("ZUMCore");
        QString blinkSketch{arduino->getArduinoDefaultDir() + "examples/01.Basics/Blink/Blink.ino"};
        arduino->setSketchWithAbsolutePath(blinkSketch);
        arduino->verify();
        arduino->autoDetectBoardPort();
        arduino->upload();
    }catch(SerialPortOpenException &e){
        qDebug()<<e.message;

    }catch(FileNotCreatedException &e){
        qDebug()<<e.message;

    }catch(BoardNotKnownException &e){
        qDebug()<<e.message;

    }catch(FileNotFoundException &e){
        qDebug()<<e.message;


    }catch(BoardNotDetectedException &e){
        qDebug()<<e.message;

    }catch(BoardNotSetException &e){
        qDebug()<<e.message;

    }catch(SketchNotSetException &e){
        qDebug()<<e.message;

    }catch(UploadException &e){
        qDebug()<<e.message;

    }catch(VerifyException &e){
        qDebug()<<e.message;
    }

    return 0;
}

}//end ArduinoExample1 namespace

namespace ArduinoExample2{
int main(void){
    ArduinoHandler* arduino = Arduino();
    try{
        arduino->setBoardNameID("ZUMCore");
        QString blinkSketch{"void setup(){pinMode(13,OUTPUT);} void loop(){digitalWrite(13,HIGH);}"};
        arduino->writeSketch(blinkSketch);
        arduino->verify();
        arduino->autoDetectBoardPort();
        arduino->upload();
    }catch(SerialPortOpenException &e){
        qDebug()<<e.message;

    }catch(FileNotCreatedException &e){
        qDebug()<<e.message;

    }catch(BoardNotKnownException &e){
        qDebug()<<e.message;

    }catch(FileNotFoundException &e){
        qDebug()<<e.message;


    }catch(BoardNotDetectedException &e){
        qDebug()<<e.message;

    }catch(BoardNotSetException &e){
        qDebug()<<e.message;

    }catch(SketchNotSetException &e){
        qDebug()<<e.message;

    }catch(UploadException &e){
        qDebug()<<e.message;

    }catch(VerifyException &e){
        qDebug()<<e.message;
    }

    return 0;
}

}//end ArduinoExample2 namespace

} // end examples namespace

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Examples::ArduinoExample1::main();
    Examples::ArduinoExample2::main();

    //SSLServer server(1234);
    //Q_UNUSED(server);
    //return a.exec();
}
