#include <QtTest>
#include <QFileInfo>
#include <QList>

#include "../../QSSWeb2Board/arduinohandler.h"

// add necessary includes here

class TestArduinoHandler : public QObject
{
    Q_OBJECT

public:
    TestArduinoHandler();
    ~TestArduinoHandler();

    QList<ArduinoHandler*> arduinoList;

    ArduinoHandler* Arduino(){
        #if (defined (Q_OS_WIN))
            WindowsArduinoHandler *arduino = new WindowsArduinoHandler();
        #elif (defined (Q_OS_LINUX))
            LinuxArduinoHandler * arduino = new LinuxArduinoHandler();
        #elif (defined (Q_OS_MAC))
            MacArduinoHandler *arduino = new MacArduinoHandler();
        #endif
        arduinoList.append(arduino);
        return arduino;
}

private:
    QString testingBaseDirPath;
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
    void test_setSketchesBasePath();
    void test_setSketchWillFullPath();
    void test_writeSketch();
    void test_setBoardNameID();
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
    for(int i=0;i<arduinoList.size();i++){
        if(arduinoList[i] != NULL)
            //delete arduinoList[i];
    }
}

void TestArduinoHandler::initTestCase(){
    testingBaseDirPath = QCoreApplication::applicationDirPath() + "/temp/";
    QDir().mkdir(testingBaseDirPath);
    testingFileName = "test.ino";
    QDir().mkdir(testingBaseDirPath + "test/");
    testingFilePath = testingBaseDirPath + "test/" + testingFileName;
    testingFile.setFileName(testingFilePath);
    QVERIFY(testingFile.open(QIODevice::ReadWrite));
    testingFile.close();
}

void TestArduinoHandler::test_setSketchesBasePath(){
    ArduinoHandler* arduino = Arduino();

    QCOMPARE(arduino->sketchesBaseDir, arduino->sketchesDefaultBaseDir);
    arduino->setSketchesBaseDir(testingBaseDirPath);
    QCOMPARE(arduino->sketchesBaseDir, testingBaseDirPath);

    delete arduino; arduino=Q_NULLPTR;
}

void TestArduinoHandler::test_setBoardNameID(){
    ArduinoHandler *arduino = Arduino();
    arduino->writeSketch(workingSketch);
    QVERIFY_EXCEPTION_THROWN(arduino->verify(),BoardNotSetException);
    QVERIFY_EXCEPTION_THROWN(arduino->setBoardNameID("zzz"),BoardNotKnownException);
    QVERIFY(arduino->setBoardNameID("ArduinoUNO"));

    delete arduino; arduino=Q_NULLPTR;
}

void TestArduinoHandler::test_verify(){
    ArduinoHandler *arduino = Arduino();
    QVERIFY(arduino->writeSketch(workingSketch));
    QVERIFY(arduino->setBoardNameID("ArduinoUNO"));
    //it should work ok
    QCOMPARE(arduino->verify(),0);

    QVERIFY(arduino->writeSketch(oneErrorSketch));
    //check whether it throwns verify exception (wrong sketch)
    QVERIFY_EXCEPTION_THROWN(arduino->verify(),VerifyException);
    //check whether it throwns board not known exception (invented board ID)

    QVERIFY(arduino->setSketchWithAbsolutePath("/home/avalero/arduino-1.8.5/examples/01.Basics/Blink/Blink.ino"));
    QCOMPARE(arduino->verify(),0);

    delete arduino; arduino=Q_NULLPTR;
}

void TestArduinoHandler::test_writeSketch(){
    ArduinoHandler *arduino = Arduino();
    QVERIFY(arduino->writeSketch(workingSketch));

    QString sketchPath = arduino->sketchesBaseDir
            + arduino->sketchName + "/"
            + arduino->sketchName + ".ino";

    QFile sketchFile(sketchPath);
    QVERIFY(sketchFile.open(QIODevice::ReadOnly | QFile::Text));

    QString sketch = QString(sketchFile.readAll());
    QCOMPARE(sketch.simplified().trimmed(),workingSketch.simplified().trimmed());

    sketchFile.close();
    sketchFile.remove();

   //*************

    arduino->setSketchesBaseDir(testingBaseDirPath);
    QVERIFY(arduino->writeSketch(workingSketch));

    sketchPath = arduino->sketchesBaseDir
            + arduino->sketchName + "/"
            + arduino->sketchName + ".ino";

    sketchFile.setFileName(sketchPath);
    QVERIFY(sketchFile.open(QIODevice::ReadOnly | QFile::Text));

    sketch = QString(sketchFile.readAll());
    QCOMPARE(sketch.simplified().trimmed(),workingSketch.simplified().trimmed());

    sketchFile.close();
    sketchFile.remove();

    delete arduino; arduino=Q_NULLPTR;
}


void TestArduinoHandler::test_setArduinoPath()
{
    ArduinoHandler *arduino = Arduino();
    QString dirPath = testingBaseDirPath;

    QVERIFY(arduino->setArduinoPath(testingBaseDirPath));
    QCOMPARE(arduino->arduinoPath,dirPath);

    QVERIFY(arduino->setArduinoPath());
    QCOMPARE(arduino->arduinoPath,arduino->arduinoDefaultDir);

    delete arduino; arduino=Q_NULLPTR;
}

void TestArduinoHandler::test_setBuildPath()
{
    ArduinoHandler *arduino = Arduino();
    QString dirPath = testingBaseDirPath;

    QVERIFY(arduino->setBuildPath(dirPath));

    QCOMPARE(arduino->buildPath,dirPath);

    QVERIFY(arduino->setBuildPath());
    QCOMPARE(arduino->buildPath,arduino->buildDefaultDir);

    delete arduino; arduino=Q_NULLPTR;
}

void TestArduinoHandler::test_setSketchWillFullPath()
{
    ArduinoHandler *arduino = Arduino();
    arduino->setSketchWithAbsolutePath(testingFilePath);

    QString sketchWithPath=arduino->sketchesBaseDir
            + arduino->sketchName + "/"
            + arduino->sketchName + ".ino";

    QString sketchPath=arduino->sketchesBaseDir;

    QCOMPARE(sketchWithPath,testingFilePath);
    QCOMPARE(sketchPath,testingBaseDirPath);
    QCOMPARE(arduino->sketchName + ".ino",testingFileName);

    QString nonExistingPath = testingFilePath + "abc";
    QVERIFY_EXCEPTION_THROWN(arduino->setSketchWithAbsolutePath(nonExistingPath),FileNotFoundException);

    delete arduino; arduino=Q_NULLPTR;
}



void TestArduinoHandler::cleanupTestCase(){
    testingFile.remove();
    QDir().rmdir(testingBaseDirPath);

}

QTEST_GUILESS_MAIN(TestArduinoHandler)
#include "tst_testarduinohandler.moc"
