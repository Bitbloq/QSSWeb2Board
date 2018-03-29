#include <QtTest>
#include <QList>
// add necessary includes here

#include <QFileInfo>

#include "../../QSSWeb2Board/arduinohandler.h"


class TestBoards : public QObject
{
    Q_OBJECT

public:
    TestBoards();
    ~TestBoards();

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
    QString wrongSketch;



private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_SetPort();
    void test_upload();
    void test_serialMonitor();

};

TestBoards::TestBoards()
{
    workingSketch="void setup(){pinMode(13,OUTPUT);} void loop(){digitalWrite(13,HIGH);delay(500);digitalWrite(13,LOW);delay(500);}";
    wrongSketch = "void setup(){zzz;} void loop(){}";
}

TestBoards::~TestBoards()
{
    for(int i=0;i<arduinoList.size();i++){
        if(arduinoList[i]) {}
            //delete arduinoList[i];
    }

}

void TestBoards::initTestCase()
{
    testingBaseDirPath = QCoreApplication::applicationDirPath() + "/temp/";
    QDir().mkdir(testingBaseDirPath);
    testingFileName = "test.ino";
    QDir().mkdir(testingBaseDirPath + "test/");
    testingFilePath = testingBaseDirPath + "test/" + testingFileName;
    testingFile.setFileName(testingFilePath);
    QVERIFY(testingFile.open(QIODevice::ReadWrite));
    testingFile.close();
    QTest::qSleep(10);
}

void TestBoards::cleanupTestCase()
{
    testingFile.remove();
    QDir().rmdir(testingBaseDirPath);
}

void TestBoards::test_serialMonitor(){
    QString sketch = QString("      void setup(){                                               ")+
                     QString("          Serial.begin(9600);                                     ")+
                     QString("          Serial.println(\"Hello World\");                        ")+
                     QString("      }                                                           ")+
                     QString("      void loop(){                                                ")+
                     QString("          if(Serial.available()){                                 ")+
                     QString("              char in = Serial.read();                            ")+
                     QString("              Serial.print(\"Received \");Serial.println(in);     ")+
                     QString("              delay(10); }                                        ")+
                     QString("      }                                                           ");


    ArduinoHandler* arduino = Arduino();

    arduino->setBoardNameID("ZUMCore");
    arduino->autoDetectBoardPort();
    arduino->writeSketch(sketch);
    arduino->upload();

    //spy on lineReceived signal (emitted when something is received from serial)
    QVERIFY(arduino->openSerialMonitor(9600));


    QSignalSpy signalSpy(arduino->serialMonitor,SIGNAL(lineReceived(QString)));
    //wait one second to be sure the message is sent
    QVERIFY(signalSpy.wait(10000));
    arduino->serialMonitor->sendToArduino("A");
    QVERIFY(signalSpy.wait(10000));
    //check that signal has been emitted once
    QCOMPARE(signalSpy.count(),2);

    QList<QVariant> arguments1 = signalSpy.takeFirst(); // take the first signal
    QList<QVariant> arguments2 = signalSpy.takeLast(); // take the second signal
    QCOMPARE(arguments1.at(0).toString().simplified(),QString("Hello World")); // verify the first argument
    QCOMPARE(arguments2.at(0).toString().simplified(),QString("Received A")); // verify the first argument


    QVERIFY(arduino->closeSerialMonitor());

    delete arduino; arduino=Q_NULLPTR;
    QTest::qSleep(10);
}

void TestBoards::test_SetPort()
{
    ArduinoHandler * arduino = Arduino();

    arduino->setBoardNameID("ZUMCore");
    QVERIFY(arduino->autoDetectBoardPort());

    arduino->setBoardNameID("ArduinoUNO");
    QVERIFY_EXCEPTION_THROWN(arduino->autoDetectBoardPort(),BoardNotDetectedException);

    delete arduino;
    arduino = Q_NULLPTR;
    QTest::qSleep(10);

}

void TestBoards::test_upload(){
    ArduinoHandler *arduino = Arduino();


    QVERIFY_EXCEPTION_THROWN(arduino->upload(),SketchNotSetException);
    QVERIFY(arduino->writeSketch(workingSketch));

    QVERIFY_EXCEPTION_THROWN(arduino->upload(),BoardNotSetException);
    QVERIFY(arduino->setBoardNameID("ZUMCore"));


    //it should work ok
    QCOMPARE(arduino->upload(),0);

    QVERIFY(arduino->writeSketch(wrongSketch));
    //check whether it throwns verify exception (wrong sketch)
    QVERIFY_EXCEPTION_THROWN(arduino->upload(),VerifyException);
    //check whether it throwns board not known exception (invented board ID)

    QVERIFY(arduino->setSketchWithAbsolutePath(arduino->arduinoDefaultDir + "examples/01.Basics/Blink/Blink.ino"));
    QCOMPARE(arduino->upload(),0);

    arduino->boardPort="";
    arduino->setBoardNameID("ArduinoUNO");
    QVERIFY_EXCEPTION_THROWN(arduino->upload(),BoardNotDetectedException);

    delete arduino; arduino=Q_NULLPTR;
    QTest::qSleep(10);
}

QTEST_GUILESS_MAIN(TestBoards)
#include "tst_testboards.moc"
