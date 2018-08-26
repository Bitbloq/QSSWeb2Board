#ifdef ONLINE_COMPILER
#include <QCoreApplication>
#else
#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#endif

#include <QProcess>
#include <QDebug>


#include "bitbloqlibsupdater.h"

#include "runguard.h"
#include "sslserver.h"
#include "unzipper.h"


int main(int argc, char *argv[])
{

    qInfo() << "============================================";
    qInfo() << "  QSSWebToBoard version: " << QString(APP_VERSION);

    //Avoid two instances to be running at the same time
    RunGuard guard( "asdfghjklasfdsfdsfdsa" );
    if( !guard.tryToRun() ){
        qInfo() << "  Another instance of QSSWeb2Board is running";
        return 0;
    }

    //Clean tmp file


    /************* ONLY FOR ONLINE COMPILER ****************/
#ifdef ONLINE_COMPILER

    qInfo() << "Online Compiler QSSWeb2Board" ;
    QCoreApplication a(argc, argv);

    //Update libraries every time QSSWeb2Board is launched

#if (defined (Q_OS_WIN))
    WindowsArduinoHandler arduino;
#elif (defined (Q_OS_LINUX))
    LinuxArduinoHandler arduino;
#elif (defined (Q_OS_MAC))
    MacArduinoHandler arduino;
#endif

    BitbloqLibsUpdater libs(arduino.getArduinoDefaultDir());
    if (libs.existsNewVersion()){
        qInfo() << "Updating bitbloqLibs";
        libs.update();
    }

    /************* END OF ONLINE COMPILER ****************/

#else
    QApplication a(argc, argv);

    //System Tray Icon

    QSystemTrayIcon trayIcon(&a);
    trayIcon.setIcon(QIcon(QCoreApplication::applicationDirPath() + "/res/qssweb2board.ico"));
    trayIcon.setToolTip(QString("QSSWeb2Board ") + QString(APP_VERSION));

    QMenu * menu = new QMenu();
    QAction * quitAction = new QAction("Close QSSWeb2Board", menu);
    QObject::connect(quitAction, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));

    menu->addAction(quitAction);
    trayIcon.setContextMenu(menu);
    trayIcon.show();
#endif


    //Clean temp files

    QString tmpDirPath{(QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_TMP").isEmpty()) ?
                    QCoreApplication::applicationDirPath() + "/tmp/" :
                    QProcessEnvironment::systemEnvironment().value("QSSWEB2BOARD_TMP")};

    QDir tmpDir{tmpDirPath};
    if(tmpDir.exists()){
        qInfo() << "Cleaning temp dir: " << tmpDirPath;
        tmpDir.removeRecursively();
        QDir().mkdir(tmpDirPath);
    }

    SSLServer server(9867);
    Q_UNUSED(server);

    return a.exec();
}
