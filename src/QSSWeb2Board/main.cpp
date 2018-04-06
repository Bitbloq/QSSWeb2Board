#include <QCoreApplication>
#include <QProcess>
#include <QDebug>
#include <QtTest/QtTest>
#include "runguard.h"

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
    qInfo() << "jsonFile" + QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_KNOWNBOARDS");
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
    RunGuard guard( "asdfghjkl" );
    if( !guard.tryToRun() ){
        qInfo() << "Another instance of QSSWeb2Board is running";
        //guard.killQSSWeb2Board();
        //qInfo() << "Killed";
        return 0;
    }

    QCoreApplication a(argc, argv);

    //qInfo() << "Hola";
    //Examples::ArduinoExample1::main();
    //Examples::ArduinoExample2::main();


    SSLServer server(1234);
    Q_UNUSED(server);
    return a.exec();
}
