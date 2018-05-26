#include <QCoreApplication>
#include <QApplication>
#include <QProcess>
#include <QDebug>
#include <QSystemTrayIcon>
#include <QMenu>

#include "githubrestapiclient.h"

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
        arduino->setBoardNameID("zumcore");
        QString blinkSketch{/*arduino->getArduinoDefaultDir() + */ "/home/avalero/arduino1.8.5/examples/01.Basics/Blink/Blink.ino"};
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
        arduino->setBoardNameID("zumcore");
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

namespace ArduinoExample3{
int main(void){
    ArduinoHandler* arduino = Arduino();
    qInfo() << "jsonFile" + QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_KNOWNBOARDS");
    try{
        arduino->setBoardNameID("zumcore");
        QString blinkSketch{/*arduino->getArduinoDefaultDir() + */ "/home/avalero/arduino-1.8.5/examples/01.Basics/Error/Error.ino"};
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
}}



} // end examples namespace

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    GitHubRestApiClient git;
    QMap<QString,QString> versionInfo = git.getLatestTagVersion("bq","bitbloqLibs");
    qInfo() << "BitbloqLibs version: " << versionInfo;
    return app.exec();
/*
    RunGuard guard( "asdfghjkl" );
    if( !guard.tryToRun() ){
        qInfo() << "Another instance of QSSWeb2Board is running";
        return 0;
    }

    QApplication a(argc, argv);


    //System Tray Icon

    QSystemTrayIcon trayIcon(&a);
    trayIcon.setIcon(QIcon(QCoreApplication::applicationDirPath() + "/res/qssweb2board.ico"));
    trayIcon.setToolTip("Web2Board");

    QMenu * menu = new QMenu();
    QAction * quitAction = new QAction("Close Web2Board", menu);
    QObject::connect(quitAction, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));

    menu->addAction(quitAction);
    trayIcon.setContextMenu(menu);
    trayIcon.show();

    SSLServer server(9867);
    Q_UNUSED(server);
    return a.exec();
    */
}
