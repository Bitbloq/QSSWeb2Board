#include <QCoreApplication>
#include <QApplication>
#include <QProcess>
#include <QDebug>
#include <QSystemTrayIcon>
#include <QMenu>

#include "bitbloqlibsupdater.h"

#include "runguard.h"
#include "sslserver.h"
#include "unzipper.h"


int main(int argc, char *argv[])
{

    qInfo() << "QSSWebToBoard version: " << APP_VERSION;
    RunGuard guard( "asdfghjkl" );
    if( !guard.tryToRun() ){
        qInfo() << "Another instance of QSSWeb2Board is running";
        return 0;
    }


    /************* ONLY FOR ONLINE COMPILER ****************/
#ifdef ONLINE_COMPILER

    qInfo() << "Online Compiler QSSWeb2Board" ;
    QCoreApplication a(argc, argv);

#if (defined (Q_OS_WIN))
    WindowsArduinoHandler arduino;
#elif (defined (Q_OS_LINUX))
    LinuxArduinoHandler arduino;
#elif (defined (Q_OS_MAC))
    MacArduinoHandler arduino;
#endif

    BitbloqLibsUpdater libs(arduino.getArduinoDefaultDir());
    if (libs.existsNewVersion()){
        //qInfo() << "Updating bitbloqLibs";
        libs.update();
    }
    /************* END OF ONLINE COMPILER ****************/

#else
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
#endif


    SSLServer server(9867);
    Q_UNUSED(server);

    return a.exec();
}
