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


private:
    QString testingDirPath;
    QString testingFileName;
    QString testingFilePath;
    QFile testingFile;

private slots:
    void test_setArduinoPath();
    void test_setBuildPath();
    void test_setSketchWillFullPath();
    void initTestCase();
    void cleanupTestCase();



};

TestArduinoHandler::TestArduinoHandler()
{

}

TestArduinoHandler::~TestArduinoHandler()
{

}

void TestArduinoHandler::initTestCase(){
    testingDirPath = QCoreApplication::applicationDirPath() + "/temp/";
    QDir().mkdir(testingDirPath);
    testingFileName = "test.tst";
    testingFilePath = testingDirPath + testingFileName;
    testingFile.setFileName(testingFilePath);
    testingFile.open(QIODevice::ReadWrite);
    testingFile.close();
}

void TestArduinoHandler::test_setArduinoPath()
{
    QString dirPath = testingDirPath;

    arduino.setArduinoPath(testingDirPath);
    QCOMPARE(arduino.arduinoPath,dirPath);

    arduino.setArduinoPath();
    QCOMPARE(arduino.arduinoPath,arduino.arduinoDefaultDir);
}

void TestArduinoHandler::test_setBuildPath()
{
    QString dirPath = testingDirPath;
    arduino.setBuildPath(dirPath);

    QCOMPARE(arduino.buildPath,dirPath);

    arduino.setBuildPath();
    QCOMPARE(arduino.buildPath,arduino.buildDefaultDir);
}

void TestArduinoHandler::test_setSketchWillFullPath()
{
    arduino.setSketchWithFullPath(testingFilePath);

    QCOMPARE(arduino.sketchWithPath,testingFilePath);
    QCOMPARE(arduino.sketchPath,testingDirPath);
    QCOMPARE(arduino.sketchName,testingFileName);

    QString nonExistingPath = testingFilePath + "abc";
    QVERIFY_EXCEPTION_THROWN(arduino.setSketchWithFullPath(nonExistingPath),FileNotFoundException);

}



void TestArduinoHandler::cleanupTestCase(){
    testingFile.remove();
    QDir().rmdir(testingDirPath);
}

QTEST_GUILESS_MAIN(TestArduinoHandler)
#include "tst_testarduinohandler.moc"
