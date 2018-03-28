#include <QCoreApplication>
#include <QProcess>
#include <QDebug>

#include "sslserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    SSLServer server(1234);
    Q_UNUSED(server);
    return a.exec();
}
