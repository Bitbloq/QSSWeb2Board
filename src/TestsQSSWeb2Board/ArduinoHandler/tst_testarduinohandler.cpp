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

    QString workingSketch;
    QString oneErrorSketch;
    QString twoErrorSketch;

private slots:
    void initTestCase();

    void test_setArduinoPath();
    void test_setBuildPath();
    void test_setSketchWillFullPath();
    void test_writeSketchInDefaultPath();

    void cleanupTestCase();



};


TestArduinoHandler::TestArduinoHandler()
{
    workingSketch = "void setup(){} void loop(){}";
    oneErrorSketch = "void setup(){x;} void loop(){}";
    twoErrorSketch = "void setup(){x;} void loop(){y;}";
}

TestArduinoHandler::~TestArduinoHandler()
{

}


void TestArduinoHandler::test_writeSketchInDefaultPath(){
    QVERIFY(arduino.writeSketchInDefaultPath(workingSketch));

    QFile sketchFile(arduino.sketchWithPath);
    QVERIFY(sketchFile.open(QIODevice::ReadOnly | QFile::Text));

    QString sketch = QString(sketchFile.readAll());
    QCOMPARE(sketch.simplified().trimmed(),workingSketch.simplified().trimmed());
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

    QVERIFY(arduino.setArduinoPath(testingDirPath));
    QCOMPARE(arduino.arduinoPath,dirPath);

    QVERIFY(arduino.setArduinoPath());
    QCOMPARE(arduino.arduinoPath,arduino.arduinoDefaultDir);
}

void TestArduinoHandler::test_setBuildPath()
{
    QString dirPath = testingDirPath;

    QVERIFY(arduino.setBuildPath(dirPath));

    QCOMPARE(arduino.buildPath,dirPath);

    QVERIFY(arduino.setBuildPath());
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
