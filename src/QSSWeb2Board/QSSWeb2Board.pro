VERSION_MAJOR=3
VERSION_MINOR=0
VERSION_REVISION=0

message("QSSWeb2Board $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_REVISION}")

QT += core \
      serialport \
      websockets \
      network \

CONFIG += c++11
CONFIG += console

TARGET = QSSWeb2Board
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# App Version
DEFINES += APP_VERSION=\\\"$${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_REVISION}\\\"

#Check wether is online or local compiler
if(!isEmpty(ONLINE_COMPILER)){
  message("Building for online compiler")
  RESOURCES += onlinesslcertificates.qrc
  DEFINES += ONLINE_COMPILER
  QT -= gui
} else {
  message("Building for local compiler")
    RESOURCES += localsslcertificates.qrc

  QT += widgets
}



SOURCES += main.cpp \
    arduinohandler.cpp \
    sslserver.cpp \
    web2board.cpp \
    arduinoserialmonitor.cpp \
    runguard.cpp \
    knownboards.cpp \
    githubrestapiclient.cpp \
    bitbloqlibsupdater.cpp \
    unzipper.cpp \
    web2boardexceptions.cpp \
    literals.cpp


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    arduinohandler.h \
    knownboards.h \
    sslserver.h \
    web2board.h \
    arduinoserialmonitor.h \
    runguard.h \
    githubrestapiclient.h \
    bitbloqlibsupdater.h \
    unzipper.h \
    web2boardexceptions.h \
    literals.h




DISTFILES += \
    res/knownboards.json


