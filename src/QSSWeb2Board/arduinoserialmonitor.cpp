#include "arduinoserialmonitor.h"
#include <QtSerialPort>
#include "arduinoexceptions.h"
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

void ArduinoSerialMonitor::start() throw(SerialPortOpenException){
    if (!port.open(QSerialPort::ReadWrite)){
        throw SerialPortOpenException("Cannot Open Serial Port " + port.portName());
    }

    QObject::connect(&port, SIGNAL(readyRead()), this, SLOT(readArduino()));
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
    while (port.canReadLine()){
        emit lineReceived(QString(port.readLine()));
    }
}


