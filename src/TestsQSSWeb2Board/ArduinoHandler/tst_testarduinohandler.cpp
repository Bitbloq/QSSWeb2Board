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

#if (defined (Q_OS_WIN))
    WindowsArduinoHandler arduino;
#elif (defined (Q_OS_LINUX))
    LinuxArduinoHandler arduino;
#elif (defined (Q_OS_MAC))
    MacArduinoHandler arduino;
#endif


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
    void test_verify();

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

void TestArduinoHandler::test_verify(){
    QVERIFY(arduino.writeSketchInDefaultPath(workingSketch));
    //it should work ok
    QCOMPARE(arduino.verify("ArduinoUNO"),0);

    QVERIFY(arduino.writeSketchInDefaultPath(oneErrorSketch));
    //check whether it throwns verify exception (wrong sketch)
    QVERIFY_EXCEPTION_THROWN(arduino.verify("ArduinoUNO"),VerifyException);
    //check whether it throwns board not known exception (invented board ID)
    QVERIFY_EXCEPTION_THROWN(arduino.verify("xxx"),BoardNotKnownException);

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
