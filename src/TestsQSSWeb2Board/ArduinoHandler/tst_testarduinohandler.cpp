#include <QtTest>
#include <QFileInfo>

#include "../../QSSWeb2Board/arduinohandler.h"

// add necessary includes here

class TestArduinoHandler : public QObject
{
    Q_OBJECT

public:
    TestArduinoHandler();
    ~TestArduinoHandler();

    ArduinoHandler arduino;

private slots:
    void test_setArduinoPath();
    void test_setBuildPath();



};

TestArduinoHandler::TestArduinoHandler()
{

}

TestArduinoHandler::~TestArduinoHandler()
{

}

void TestArduinoHandler::test_setArduinoPath()
{
    QString filePath = "/home/avalero/arduino-1.8.5/";

    arduino.setArduinoPath(filePath);
    QCOMPARE(arduino.arduinoPath,filePath);

    arduino.setArduinoPath();
    QCOMPARE(arduino.arduinoPath,QCoreApplication::applicationDirPath() + "/res/arduino/");
}

void TestArduinoHandler::test_setBuildPath()
{
    QString filePath = "/home/avalero/arduino-1.8.5/";
    arduino.setBuildPath(filePath);

    QCOMPARE(arduino.buildPath,filePath);

    arduino.setBuildPath();
    QCOMPARE(arduino.buildPath,QCoreApplication::applicationDirPath() + "/res/build/");
}

QTEST_GUILESS_MAIN(TestArduinoHandler)
#include "tst_testarduinohandler.moc"
