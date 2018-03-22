#ifndef ARDUINOSERIALMONITOR_H
#define ARDUINOSERIALMONITOR_H

#include <QObject>
#include <QtSerialPort>
#include <cstdio>
#include "arduinoexceptions.h"

class ArduinoSerialMonitor:public QObject
{
    Q_OBJECT

public:
    ArduinoSerialMonitor(QString portName, int baudrate, QObject* parent = NULL);
    virtual ~ArduinoSerialMonitor();

    void start() throw(SerialPortOpenException);
    void close();
    void sendToArduino(QString msg);

public slots:
    void readArduino();
    void writeString(QString str){qDebug()<<str;}

signals:
    void lineReceived(QString line);


protected:
    QSerialPort port;
};




#endif // ARDUINOSERIALMONITOR_H
