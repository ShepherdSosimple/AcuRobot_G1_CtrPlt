/********************Node1：updownMotor RE30***********************/
/********************Node2: TwirlMotor RE25***********************/
/********************Node3: clampMotor RE13***********************/


#include "mainwindow.h"
#include <QApplication>
#include "epos.h"
#include "motor.h"
#include "sensor.h"
#include "easyusb_card_dll.h"

Epos updownEpos;
Epos twirlEpos;
Epos clampEpos;


UpdownMotor updownMotor;
TwirlMotor twirlMotor;
ClampMotor clampMotor;

int main(int argc, char *argv[])
{

    updownEpos.SetEposData("EPOS2","MAXON SERIAL V2","USB","USB0",1000000,500,1);
    twirlEpos.SetEposData("EPOS2","MAXON SERIAL V2","USB","USB0",1000000,500,2);
 //   clampEpos.SetEposData("EPOS2","MAXON SERIAL V2","USB","USB0",1000000,500,3);

    //Incremental Encoder 1 with index      (3-channel)      1
    //Incremental Encoder 1 without inde    (2-channel)      2
    updownMotor.InputMotorData(1,6000,6000*2,41,1,1);
    updownMotor.InputEncoderData(1,500,0);
    updownMotor.InputSafetyData(2000,7580,100000);

    twirlMotor.InputMotorData(1,3470,3470*2,16,4,2);
    twirlMotor.InputEncoderData(2,500,0);
    twirlMotor.InputSafetyData(2000,8810,50000);

    clampMotor.InputMotorData(1,319,319*2,4,67,3);
    clampMotor.InputEncoderData(2,256,0);
    clampMotor.InputSafetyData(2000,13300,50000);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
