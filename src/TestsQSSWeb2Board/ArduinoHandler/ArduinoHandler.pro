QT += testlib \
      serialport
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testarduinohandler.cpp \
    ../../QSSWeb2Board/arduinoexceptions.cpp \
    ../../QSSWeb2Board/arduinohandler.cpp \
    ../../QSSWeb2Board/knownboards.cpp \
    ../../QSSWeb2Board/commsprotocol.cpp \
    ../../QSSWeb2Board/arduinoserialmonitor.cpp

HEADERS += \
    ../../QSSWeb2Board/arduinoexceptions.h \
    ../../QSSWeb2Board/arduinohandler.h \
    ../../QSSWeb2Board/knownboards.h \
    ../../QSSWeb2Board/commsprotocol.h \
    ../../QSSWeb2Board/arduinoserialmonitor.h
