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


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);



    QString sketch = QString("      void setup(){                           ")+
                     QString("          pinMode(13,OUTPUT);Serial.begin(9600);                 ")+
                     QString("          Serial.println(\"Hello World\");    ")+
                     QString("      }                                       ")+
                     QString("      void loop(){                            ")+
                     QString("          if(Serial.available()){ char in = Serial.read(); if(in=='A') digitalWrite(13,HIGH);    ")+
                     QString("          delay(10);}                        ")+
                     QString("      }                                       ");


    ArduinoHandler* arduino = Arduino();

    arduino->setBoardNameID("ZUMCore");
    arduino->autoDetectBoardPort();
    arduino->writeSketch(sketch);
    arduino->upload();

    //spy on lineReceived signal (emitted when something is received from serial)
    arduino->openSerialMonitor(9600);

    QTest::qSleep(3000);
    arduino->serialMonitor->sendToArduino("A");

    return a.exec();





    SSLServer server(1234);
    Q_UNUSED(server);
    return a.exec();
}
