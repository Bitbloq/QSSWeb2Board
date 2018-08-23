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
}
