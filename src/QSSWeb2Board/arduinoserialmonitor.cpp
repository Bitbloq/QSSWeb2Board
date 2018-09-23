#include "arduinoserialmonitor.h"
#include <QtSerialPort>
#include "web2boardexceptions.h"
#include <cstdio>

ArduinoSerialMonitor::ArduinoSerialMonitor(QString portName, int baudrate, QObject* parent): QObject(parent)
{
    port.setPortName(portName);
    port.setBaudRate(baudrate);
    port.setDataBits(QSerialPort::Data8);
    port.setParity(QSerialPort::NoParity);
    port.setStopBits(QSerialPort::OneStop);
    port.setFlowControl(QSerialPort::NoFlowControl);

}

ArduinoSerialMonitor::~ArduinoSerialMonitor(){
    if(port.isOpen()){
        port.close();
    }
}

bool ArduinoSerialMonitor::open(){
    if (!port.open(QSerialPort::ReadWrite)){
        throw SerialPortOpenException("Cannot Open Serial Port " + port.portName());
    }
    return true;
}

void ArduinoSerialMonitor::close(){
    if(port.isOpen()){
        port.close();
    }
}

void ArduinoSerialMonitor::sendToArduino(QString msg){
    port.write(msg.toLatin1());
}

void ArduinoSerialMonitor::readArduino(){

    //this is called when readyRead() is emitted
    //qDebug() << "New data available: " << port.bytesAvailable();
    const QByteArray datas{port.readAll()};
    //qDebug() << datas;
    emit dataReceived(datas);


    /*
    while (port.canReadLine()){
        emit lineReceived(QString(port.readLine()));
    }
    */
}
