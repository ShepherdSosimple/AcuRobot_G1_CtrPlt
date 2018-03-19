/********************Node1：updownMotor RE30***********************/
/********************Node2: TwirlMotor RE25***********************/
/********************Node3: clampMotor RE13***********************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "epos.h"
#include "motor.h"
#include "sensor.h"
#include "easyusb_card_dll.h"
#include "motorthreads.h"
#include <QTimer>
#include <QMessageBox>
#include <QThread>
#include <QDebug>
#include <windows.h>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QQueue>
#include "hd.h"

#define  Rad_to_deg  57.2957795130823

extern Epos updownEpos;
extern Epos twirlEpos;
extern Epos clampEpos;

extern UpdownMotor updownMotor;
extern TwirlMotor twirlMotor;
extern ClampMotor clampMotor;

//声明光电开关
Photoelec photoelec1(0);
Photoelec photoelec2(1);
Photoelec photoelec3(2);
//声明线程
MoveToOriginTd moveToOriginTd;
PierceAndLiftThrustTd pierceAndLiftThrustTd;
PierceOnlyTd pierceOnlyTd;
LiftThrustOnlyTd liftThrustOnlyTd;
ReplaceNeedleManulTd replaceNeedleManulTd;

TwirlTd twirlTd;
Combo1Td combo1Td;
Combo2Td combo2Td;
Combo3Td combo3Td;
//输出文件
QFile updownInfoFile("updownInfoFile.txt");
QFile twirlInfoFile("twirlInfoFile.txt");
QFile comboExecInfoFile("comboExecInfoFile.txt");
//touch
HHD hHD;
QQueue<double> angleQueue; //队列用于保存上一次和当前gimbal角度值



struct TouchState
{
    HDdouble velocity[3];
    HDdouble position[3];
    HDdouble gimbalAngle[3];
    HDint button = 0;
};
TouchState touchInfo;
HDCallbackCode HDCALLBACK DeviceStateCallback(void *data);


//GeomagicTouch 调度程序
HDCallbackCode HDCALLBACK DeviceStateCallback(void *data)
{
    HHD hHD = hdGetCurrentDevice();
    hdBeginFrame(hHD);
    hdGetDoublev(HD_CURRENT_GIMBAL_ANGLES,touchInfo.gimbalAngle);
    hdGetDoublev(HD_CURRENT_VELOCITY,touchInfo.velocity);
    hdGetDoublev(HD_CURRENT_POSITION,touchInfo.position);
    hdGetIntegerv(HD_CURRENT_BUTTONS,&touchInfo.button);


    //hdGetIntegerv(HD_CURRENT_BUTTONS,&touchInfo.button1);
    hdEndFrame(hHD);
    return HD_CALLBACK_CONTINUE;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->auto_widget->hide();
    ui->manu_widget->hide();

    connect(&replaceNeedleManulTd,SIGNAL(started()),this,SLOT(LockInfo1()));
    connect(&replaceNeedleManulTd,SIGNAL(finished()),this,SLOT(UnlockInfo1()));
    connect(&pierceAndLiftThrustTd,SIGNAL(started()),this,SLOT(LockInfo1()));
    connect(&pierceOnlyTd,SIGNAL(started()),this,SLOT(LockInfo1()));
    connect(&liftThrustOnlyTd,SIGNAL(started()),this,SLOT(LockInfo1()));
    connect(&moveToOriginTd,SIGNAL(started()),this,SLOT(LockInfo1()));
    connect(&pierceAndLiftThrustTd,SIGNAL(finished()),this,SLOT(UnlockInfo1()));
    connect(&pierceOnlyTd,SIGNAL(finished()),this,SLOT(UnlockInfo1()));
    connect(&liftThrustOnlyTd,SIGNAL(finished()),this,SLOT(UnlockInfo1()));
    connect(&moveToOriginTd,SIGNAL(finished()),this,SLOT(UnlockInfo1()));
    connect(&twirlTd,SIGNAL(started()),this,SLOT(LockInfo2()));
    connect(&twirlTd,SIGNAL(finished()),this,SLOT(UnlockInfo2()));

    updownResetTimer = 0;
    /*hHD = hdInitDevice(HD_DEFAULT_DEVICE) ;
    hdScheduleSynchronous(DeviceStateCallback,(void*)0,HD_DEFAULT_SCHEDULER_PRIORITY);
    hdStartScheduler();*/
    //开启光电开关定时器
    photoelecTimer = startTimer(20,Qt::PreciseTimer);
}

MainWindow::~MainWindow()
{
    //hdStopScheduler();
    delete ui;
}

void MainWindow::LockInfo1()//锁住部分输入界面
{
    ui->execute_pushButton->setEnabled(false);
    ui->executeAll_pushButton->setEnabled(false);
    ui->updwonMotorSettingBox->setEnabled(false);
}

void MainWindow::UnlockInfo1()//解锁部分输入界面
{
    ui->execute_pushButton->setEnabled(true);
    ui->executeAll_pushButton->setEnabled(true);
    ui->updwonMotorSettingBox->setEnabled(true);
}

void MainWindow::LockInfo2()//锁住部分输入界面
{
    ui->executeTwirl_pushButton->setEnabled(false);
    ui->executeAll_pushButton->setEnabled(false);
    ui->twirlMotorSettingBox->setEnabled(false);
}

void MainWindow::UnlockInfo2()//解锁部分输入界面
{
    ui->executeTwirl_pushButton->setEnabled(true);
    ui->executeAll_pushButton->setEnabled(true);
    ui->twirlMotorSettingBox->setEnabled(true);
}

/*************************************定时器事件******************************************/
void MainWindow::timerEvent(QTimerEvent *event)
{
   //定时器：采集光电开关数据 并实现限位保护
   if(event->timerId() == photoelecTimer)
   {
    if(photoelec1.Arrive() && updownMotor.originHasFound){
        moveToOriginTd.stop();
        liftThrustOnlyTd.stop();
        pierceAndLiftThrustTd.stop();
        pierceOnlyTd.stop();
        updownMotor.SetQuickStop();
        updownEpos.DisableEpos();
    }
    photoelec2.Arrive();
    if(photoelec3.Arrive() && updownMotor.originHasFound){
        moveToOriginTd.stop();
        liftThrustOnlyTd.stop();
        pierceAndLiftThrustTd.stop();
        pierceOnlyTd.stop();
        updownMotor.SetQuickStop();
    }
   }
   //定时器：搜寻原点
   else if(event->timerId() == updownResetTimer)
   {
       if(!updownMotor.FindOrigin()){
       }
       else{
           killTimer(updownResetTimer);
           QMessageBox::warning(this, tr("提示！"),
                                      tr("光电开关限位保护已开启！"),
                                     QMessageBox::Yes);
           updownResetTimer = 0;
       }
   }
   //定时器：单独执行升降操作时，获取updownMotor信息
   else if(event->timerId() == execUpdownInfoAcquiTimer)
   {
       if(pierceOnlyTd.isRunning() || liftThrustOnlyTd.isRunning() || pierceAndLiftThrustTd.isRunning()){
           updownInfoFile.open(QIODevice::Append | QIODevice::Text);
           QTextStream out(&updownInfoFile);
           out<<qSetFieldWidth(30)<<left<<updownMotor.GetPosition()<<updownMotor.GetVelocity()<<updownMotor.GetCurrent()<<qSetFieldWidth(0)<<endl;
           updownInfoFile.close();
       }
       else{
           UnlockInfo1();
           killTimer(execUpdownInfoAcquiTimer);
           execUpdownInfoAcquiTimer = 0;
       }
   }
   //定时器：单独执行捻转操作时，获取并输出twirlMotor电机信息
   else if(event->timerId() == execTwirlInfoAcquiTimer)
   {
       if(twirlTd.isRunning()){
           LockInfo2();
           twirlInfoFile.open(QIODevice::Append | QIODevice::Text);
           QTextStream out(&twirlInfoFile);
           out<<qSetFieldWidth(30)<<left<<twirlMotor.GetPosition()<<twirlMotor.GetVelocity()<<twirlMotor.GetCurrent()<<qSetFieldWidth(0)<<endl;
           twirlInfoFile.close();
       }
       else{
           UnlockInfo2();
           killTimer(execTwirlInfoAcquiTimer);
           execTwirlInfoAcquiTimer = 0;
       }
   }
   //定时器：组合执行时，获取并输出updownMotor和twirlMotor信息
   else if(event->timerId() == comboExecInfoAcquiTimer)
   {
       if(combo1Td.isRunning() || combo2Td.isRunning() || combo3Td.isRunning()){
           LockInfo1();
           LockInfo2();
           comboExecInfoFile.open(QIODevice::Append | QIODevice::Text);
           QTextStream out(&comboExecInfoFile);
           out<<qSetFieldWidth(30)<<left<<updownMotor.GetPosition()<<updownMotor.GetVelocity()<<updownMotor.GetCurrent();
           out<<twirlMotor.GetPosition()<<twirlMotor.GetVelocity()<<twirlMotor.GetCurrent()<<qSetFieldWidth(0)<<endl;
           comboExecInfoFile.close();
       }
       else{
           UnlockInfo1();
           UnlockInfo2();
           killTimer(comboExecInfoAcquiTimer);
           comboExecInfoAcquiTimer = 0;
       }
   }
   //定时器：手动模式，激活提插功能
   else if(event->timerId() == actLiftThrustTimer)
   {
       float liftThrustScale;
       liftThrustScale = 0.01*ui->liftThrustScale_spinBox->value();
       if(!touchInfo.button){
           updownMotor.MoveOnProfileVelocityMode(liftThrustScale*updownMotor.mmps_to_rpm*touchInfo.velocity[0],7000,7000);
       }
       else
           updownMotor.MoveOnProfileVelocityMode(0,7000,7000);
   }
   //定时器：手动模式，激活捻转功能
   else if(event->timerId() == actTwirlTimer)
   {
        float twirlScale = 1;
        double angleToMove = 0;

        twirlScale = 0.01*ui->twirlScale_spinBox->value();
        if(!touchInfo.button){
            angleQueue.dequeue();
            angleQueue.enqueue(touchInfo.gimbalAngle[2]);
            angleToMove = angleQueue.last() - angleQueue.first();
            //判断队列第一位是否为零，为零意味着没有足够数据做差
            if(angleQueue.first()){
                twirlMotor.MoveOnProfilePositionMode(twirlScale*angleToMove*Rad_to_deg*twirlMotor.degree_to_qc,
                                                     FALSE,FALSE,3000,5000,5000,false);
            }
        } else{
            angleQueue.clear();
            angleQueue.enqueue(0);
            angleQueue.enqueue(0);
        }
   }

}



//连接设备
void MainWindow::on_actionConnect_device_triggered()
{
    if(OpenUsbV20() == 0){
        if(photoelec1.Arrive() && photoelec2.Arrive() && photoelec3.Arrive()){
            QMessageBox::warning(this, tr("警告！"),
                                       tr("请接通光电开关电源后重新连接，否则会发生意外！"),
                                      QMessageBox::Yes);
        }
        else{
            if(updownEpos.ConnectEpos() && twirlEpos.ConnectEpos() /*&& clampEpos.ConnectEpos()*/){
                //统一handle
                updownMotor.motorHandle = updownEpos.keyHandle;
                twirlMotor.motorHandle = twirlEpos.keyHandle;
       //         clampMotor.motorHandle = clampEpos.keyHandle;
                //写入电机参数
                updownMotor.SetMotor(); updownMotor.SetEncoder(); updownMotor.SetSafety();
                twirlMotor.SetMotor(); twirlMotor.SetEncoder(); twirlMotor.SetSafety();
      //          clampMotor.SetMotor(); clampMotor.SetEncoder(); clampMotor.SetSafety();
                //设置epos状态
                updownEpos.EnableEpos();
                twirlEpos.EnableEpos();
      //          clampEpos.EnableEpos();

            }
            else
                QMessageBox::warning(this, tr("警告！"),
                                           tr("连接Epos失败，请检查Epos设置和连接！"),
                                          QMessageBox::Yes);
        }
    }
    else
        QMessageBox::warning(this, tr("警告！"),
                                   tr("打开采集卡失败，请检查USB采集卡连接！"),
                                  QMessageBox::Yes);
}

void MainWindow::on_actionDisconnect_device_triggered()
{
    if(updownEpos.DisableEpos() && twirlEpos.DisableEpos() /* && clampEpos.DisableEpos() */){
        updownEpos.DisconnectEpos();
        twirlEpos.DisconnectEpos();
  //      clampEpos.DisconnectEpos();
        CloseUsbV20();
    }
}

void MainWindow::on_actionExit_triggered()
{
    if(updownEpos.DisableEpos() && twirlEpos.DisableEpos()){
        updownEpos.DisconnectEpos();
        twirlEpos.DisconnectEpos();
 //       clampEpos.DisconnectEpos();
        CloseUsbV20();
    }
    close();
}

void MainWindow::on_execute_pushButton_clicked()
{
    if(updownEpos.IsEnable()){
        updownMotor.pierceVelocity_mmps = ui->pierceVelocity_spinBox->value();
        updownMotor.pierceDepth_mm = ui->pierceDepth_doubleSpinBox->value();
        updownMotor.frequency_tpm = ui->frequency_spinBox->value();
        updownMotor.amplitude_mm = ui->amplitude_doubleSpinBox->value();
        updownMotor.operationTime_s = ui->operationTime_spinBox->value();
        switch (ui->operationSelec_comboBox->currentIndex()) {
            case 0:
                if(updownMotor.originHasFound)
                    moveToOriginTd.start();
                //开启搜寻原点定时器
                else{
                    updownMotor.photoelec1HasArrIS = false;
                    updownResetTimer = startTimer(20,Qt::PreciseTimer);
                }
                break;
            case 1:
                if(updownMotor.originHasFound){
                    pierceAndLiftThrustTd.start();
                    if(updownInfoFile.open(QFile::Append | QFile::Text)){
                        QTextStream out(&updownInfoFile);
                        out<<"***********************************Mode: PierceAndLiftThrust***********************************"<<endl;
                        out<<qSetFieldWidth(30)<<left<<"DataTime:"<<QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz ddd")<<qSetFieldWidth(0)<<endl<<endl;
                        out<<qSetFieldWidth(30)<<left<<"originPostion"<<updownMotor.originPosition<<qSetFieldWidth(0)<<endl;
                        out<<qSetFieldWidth(30)<<left<<"pierceVelocity_mmps:"<<updownMotor.pierceVelocity_mmps<<qSetFieldWidth(0)<<endl;
                        out<<qSetFieldWidth(30)<<left<<"pierceDepth_mm:"<<updownMotor.pierceDepth_mm<<qSetFieldWidth(0)<<endl;
                        out<<qSetFieldWidth(30)<<left<<"frequency_tpm:"<<updownMotor.frequency_tpm<<qSetFieldWidth(0)<<endl;
                        out<<qSetFieldWidth(30)<<left<<"amplitude_mm:"<<updownMotor.amplitude_mm<<qSetFieldWidth(0)<<endl;
                        out<<qSetFieldWidth(30)<<left<<"operationTime_s:"<<updownMotor.operationTime_s<<qSetFieldWidth(0)<<endl<<endl;
                        out<<qSetFieldWidth(30)<<left<<"POSITION"<<"VELOCITY"<<"CURRENT"<<qSetFieldWidth(0)<<endl;
                        updownInfoFile.close();
                        execUpdownInfoAcquiTimer = startTimer(20,Qt::PreciseTimer);
                    }
                }
                else
                    QMessageBox::warning(this, tr("警告！"),
                                               tr("开机后必须至少复位一次！"),
                                              QMessageBox::Yes);
                break;
            case 2:
                if(updownMotor.originHasFound){
                    pierceOnlyTd.start();
                    if(updownInfoFile.open(QFile::Append | QFile::Text)){
                        QTextStream out(&updownInfoFile);
                        out<<"***********************************Mode: PierceOnly***********************************"<<endl;
                        out<<qSetFieldWidth(30)<<left<<"DataTime:"<<QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz ddd")<<qSetFieldWidth(0)<<endl<<endl;
                        out<<qSetFieldWidth(30)<<left<<"originPostion"<<updownMotor.originPosition<<qSetFieldWidth(0)<<endl;
                        out<<qSetFieldWidth(30)<<left<<"pierceVelocity_mmps:"<<updownMotor.pierceVelocity_mmps<<qSetFieldWidth(0)<<endl;
                        out<<qSetFieldWidth(30)<<left<<"pierceDepth_mm:"<<updownMotor.pierceDepth_mm<<qSetFieldWidth(0)<<endl;
                        out<<qSetFieldWidth(30)<<left<<"frequency_tpm:"<<updownMotor.frequency_tpm<<qSetFieldWidth(0)<<endl;
                        out<<qSetFieldWidth(30)<<left<<"amplitude_mm:"<<updownMotor.amplitude_mm<<qSetFieldWidth(0)<<endl;
                        out<<qSetFieldWidth(30)<<left<<"operationTime_s:"<<updownMotor.operationTime_s<<qSetFieldWidth(0)<<endl<<endl;
                        out<<qSetFieldWidth(30)<<left<<"POSITION"<<"VELOCITY"<<"CURRENT"<<qSetFieldWidth(0)<<endl;
                        updownInfoFile.close();
                        execUpdownInfoAcquiTimer = startTimer(20,Qt::PreciseTimer);
                    }
                }
                else
                    QMessageBox::warning(this, tr("警告！"),
                                               tr("开机后必须至少复位一次！"),
                                              QMessageBox::Yes);
                break;
            case 3:
                if(updownMotor.originHasFound){
                    liftThrustOnlyTd.start();
                    if(updownInfoFile.open(QFile::Append | QFile::Text)){
                        QTextStream out(&updownInfoFile);
                        out<<"***********************************Mode: LiftThrustOnly***********************************"<<endl;
                        out<<qSetFieldWidth(30)<<left<<"DataTime:"<<QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz ddd")<<qSetFieldWidth(0)<<endl<<endl;
                        out<<qSetFieldWidth(30)<<left<<"originPostion"<<updownMotor.originPosition<<qSetFieldWidth(0)<<endl;
                        out<<qSetFieldWidth(30)<<left<<"pierceVelocity_mmps:"<<updownMotor.pierceVelocity_mmps<<qSetFieldWidth(0)<<endl;
                        out<<qSetFieldWidth(30)<<left<<"pierceDepth_mm:"<<updownMotor.pierceDepth_mm<<qSetFieldWidth(0)<<endl;
                        out<<qSetFieldWidth(30)<<left<<"frequency_tpm:"<<updownMotor.frequency_tpm<<qSetFieldWidth(0)<<endl;
                        out<<qSetFieldWidth(30)<<left<<"amplitude_mm:"<<updownMotor.amplitude_mm<<qSetFieldWidth(0)<<endl;
                        out<<qSetFieldWidth(30)<<left<<"operationTime_s:"<<updownMotor.operationTime_s<<qSetFieldWidth(0)<<endl<<endl;
                        out<<qSetFieldWidth(30)<<left<<"POSITION"<<"VELOCITY"<<"CURRENT"<<qSetFieldWidth(0)<<endl;
                        updownInfoFile.close();
                        execUpdownInfoAcquiTimer = startTimer(20,Qt::PreciseTimer);
                    }
                }
                else
                    QMessageBox::warning(this, tr("警告！"),
                                               tr("开机后必须至少复位一次！"),
                                              QMessageBox::Yes);
                break;
            case 4:
            if(updownMotor.originHasFound)
                replaceNeedleManulTd.start();
            else
                QMessageBox::warning(this, tr("警告！"),
                                           tr("开机后必须至少复位一次！"),
                                          QMessageBox::Yes);
            break;
            default:
                break;
        }
    }
    else
        QMessageBox::warning(this, tr("警告！"),
                                   tr("执行失败，请检查连接！"),
                                  QMessageBox::Yes);
}



void MainWindow::on_quickStop_pushButton_toggled(bool checked)
{
    if(checked){
        if(updownResetTimer){ //判断为真值表明updownMotor正在搜寻原点
            killTimer(updownResetTimer);
            updownResetTimer = 0;
            updownMotor.SetQuickStop();
            updownMotor.photoelec1HasArrIS = false; //很重要。必须还原搜寻过程的数据
        }
        else{
            switch (ui->operationSelec_comboBox->currentIndex()) {
            case 0:
                moveToOriginTd.stop();
                break;
            case 1:
                pierceAndLiftThrustTd.stop();
            case 2:
                pierceOnlyTd.stop();
            case 3:
                liftThrustOnlyTd.stop();
            case 4:
                replaceNeedleManulTd.stop();
            default:
                break;
            }
        }
    }
    else{
        updownEpos.EnableEpos();
        replaceNeedleManulTd.renew();
        moveToOriginTd.renew();
        pierceAndLiftThrustTd.renew();
        pierceOnlyTd.renew();
        liftThrustOnlyTd.renew();
    }
}

void MainWindow::on_tightenButton_clicked()
{
    clampMotor.Tighten();
}

void MainWindow::on_loosenButton_clicked()
{
    clampMotor.Loosen();
}

void MainWindow::on_executeTwirl_pushButton_clicked()
{
    if(twirlEpos.IsEnable()){
        twirlMotor.twirlAngle_degree = ui->twirlAngle_spinBox->value();
        twirlMotor.twirlFrequency_tpm = ui->twirlFrequency_spinBox->value();
        twirlMotor.twirlTime_s = ui->twirlTime_spinBox->value();
        twirlTd.start();
        if(twirlInfoFile.open(QFile::Append | QFile::Text)){
            QTextStream out(&twirlInfoFile);
            out<<"***********************************Mode: TwirlOnly***********************************"<<endl;
            out<<qSetFieldWidth(30)<<left<<"DataTime:"<<QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz ddd")<<qSetFieldWidth(0)<<endl<<endl;
            out<<qSetFieldWidth(30)<<left<<"twirlAngle_degree:"<<twirlMotor.twirlAngle_degree<<qSetFieldWidth(0)<<endl;
            out<<qSetFieldWidth(30)<<left<<"twirlFrequency_tpm:"<<twirlMotor.twirlFrequency_tpm<<qSetFieldWidth(0)<<endl;
            out<<qSetFieldWidth(30)<<left<<"twirlTime_s:"<<twirlMotor.twirlTime_s<<qSetFieldWidth(0)<<endl<<endl;
            out<<qSetFieldWidth(30)<<left<<"POSITION"<<"VELOCITY"<<"CURRENT"<<qSetFieldWidth(0)<<endl;
            twirlInfoFile.close();
            execTwirlInfoAcquiTimer = startTimer(20,Qt::PreciseTimer);
        }
    }
    else
        QMessageBox::warning(this, tr("警告！"),
                                   tr("执行失败，请检查连接！"),
                                  QMessageBox::Yes);
}


void MainWindow::on_stopTwirl_pushButton_toggled(bool checked)
{
    if(checked){
        twirlTd.stop();
    }
    else{
        twirlEpos.EnableEpos();
        twirlTd.renew();
    }
}

void MainWindow::on_executeAll_pushButton_clicked()
{
    if(updownEpos.IsEnable() && twirlEpos.IsEnable()){
        updownMotor.pierceVelocity_mmps = ui->pierceVelocity_spinBox->value();
        updownMotor.pierceDepth_mm = ui->pierceDepth_doubleSpinBox->value();
        updownMotor.frequency_tpm = ui->frequency_spinBox->value();
        updownMotor.amplitude_mm = ui->amplitude_doubleSpinBox->value();
        updownMotor.operationTime_s = ui->operationTime_spinBox->value();

        twirlMotor.twirlAngle_degree = ui->twirlAngle_spinBox->value();
        twirlMotor.twirlFrequency_tpm = ui->twirlFrequency_spinBox->value();
        twirlMotor.twirlTime_s = ui->twirlTime_spinBox->value();



        switch (ui->operationSelec_comboBox->currentIndex()) {
            case 0:
                QMessageBox::warning(this, tr("警告！"),
                                       tr("选择的执行选项无法组合执行！"),
                                      QMessageBox::Yes);
                break;
            case 1:
                if(updownMotor.originHasFound){
                    if(twirlEpos.IsEnable() && updownEpos.IsEnable()){
                        LockInfo1();
                        LockInfo2();
                        combo1Td.start();
                        if(comboExecInfoFile.open(QFile::Append | QFile::Text)){
                            QTextStream out(&comboExecInfoFile);
                            out<<"***************************************************************************";
                            out<<"Mode: Combo1(PierceThenLiftThrustAndTwirl)***************************************************************************"<<endl;
                            out<<qSetFieldWidth(30)<<left<<"DataTime:"<<QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz ddd")<<qSetFieldWidth(0)<<endl<<endl;
                            out<<qSetFieldWidth(30)<<left<<"originPostion"<<updownMotor.originPosition<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"pierceVelocity_mmps:"<<updownMotor.pierceVelocity_mmps<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"pierceDepth_mm:"<<updownMotor.pierceDepth_mm<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"LiftThrustFrequency_tpm:"<<updownMotor.frequency_tpm<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"LiftThrustAmplitude_mm:"<<updownMotor.amplitude_mm<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"LiftThrustOperationTime_s:"<<updownMotor.operationTime_s<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"twirlAngle_degree:"<<twirlMotor.twirlAngle_degree<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"twirlFrequency_tpm:"<<twirlMotor.twirlFrequency_tpm<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"twirlTime_s:"<<twirlMotor.twirlTime_s<<qSetFieldWidth(0)<<endl<<endl;
                            out<<qSetFieldWidth(30)<<left<<"updownMotorPOSITION"<<"updownMotorVELOCITY"<<"updownMotorCURRENT";
                            out<<"twirlMotorPOSITION"<<"twirlMotorVELOCITY"<<"twirlMotorCURRENT"<<qSetFieldWidth(0)<<endl;
                            comboExecInfoFile.close();
                            comboExecInfoAcquiTimer = startTimer(20,Qt::PreciseTimer);
                        }
                    } else{
                        QMessageBox::warning(this, tr("警告！"),
                                                   tr("执行失败，请检查连接！"),
                                                  QMessageBox::Yes);
                    }

                }

                else
                    QMessageBox::warning(this, tr("警告！"),
                                               tr("开机后必须至少复位一次！"),
                                              QMessageBox::Yes);
                break;
            case 2:
                if(updownMotor.originHasFound){
                    if(twirlEpos.IsEnable() && updownEpos.IsEnable()){
                        LockInfo1();
                        LockInfo2();
                        combo2Td.start();
                        if(comboExecInfoFile.open(QFile::Append | QFile::Text)){
                            QTextStream out(&comboExecInfoFile);
                            out<<"***************************************************************************";
                            out<<"Mode: Combo2(PierceThenTwirl)***************************************************************************"<<endl;
                            out<<qSetFieldWidth(30)<<left<<"DataTime:"<<QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz ddd")<<qSetFieldWidth(0)<<endl<<endl;
                            out<<qSetFieldWidth(30)<<left<<"originPostion"<<updownMotor.originPosition<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"pierceVelocity_mmps:"<<updownMotor.pierceVelocity_mmps<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"pierceDepth_mm:"<<updownMotor.pierceDepth_mm<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"LiftThrustFrequency_tpm:"<<updownMotor.frequency_tpm<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"LiftThrustAmplitude_mm:"<<updownMotor.amplitude_mm<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"LiftThrustOperationTime_s:"<<updownMotor.operationTime_s<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"twirlAngle_degree:"<<twirlMotor.twirlAngle_degree<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"twirlFrequency_tpm:"<<twirlMotor.twirlFrequency_tpm<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"twirlTime_s:"<<twirlMotor.twirlTime_s<<qSetFieldWidth(0)<<endl<<endl;
                            out<<qSetFieldWidth(30)<<left<<"updownMotorPosition"<<"updownMotorVelocity"<<"updownMotorCurrent";
                            out<<"twirlMotorPosition"<<"twirlMotorVelocity"<<"twirlMotorCurrent"<<qSetFieldWidth(0)<<endl;
                            comboExecInfoFile.close();
                            comboExecInfoAcquiTimer = startTimer(20,Qt::PreciseTimer);
                        }
                    } else{
                        QMessageBox::warning(this, tr("警告！"),
                                                tr("执行失败，请检查连接！"),
                                                QMessageBox::Yes);
                    }

                }

                else
                    QMessageBox::warning(this, tr("警告！"),
                                           tr("开机后必须至少复位一次！"),
                                          QMessageBox::Yes);
                break;
            case 3:
                if(updownMotor.originHasFound){
                    if(twirlEpos.IsEnable() && updownEpos.IsEnable()){
                        LockInfo1();
                        LockInfo2();
                        combo3Td.start();
                        if(comboExecInfoFile.open(QFile::Append | QFile::Text)){
                            QTextStream out(&comboExecInfoFile);
                            out<<"***************************************************************************";
                            out<<"Mode: Combo3(LiftThrustAndTwirl)***************************************************************************"<<endl;
                            out<<qSetFieldWidth(30)<<left<<"DataTime:"<<QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz ddd")<<qSetFieldWidth(0)<<endl<<endl;
                            out<<qSetFieldWidth(30)<<left<<"originPostion"<<updownMotor.originPosition<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"pierceVelocity_mmps:"<<updownMotor.pierceVelocity_mmps<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"pierceDepth_mm:"<<updownMotor.pierceDepth_mm<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"LiftThrustFrequency_tpm:"<<updownMotor.frequency_tpm<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"LiftThrustAmplitude_mm:"<<updownMotor.amplitude_mm<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"LiftThrustOperationTime_s:"<<updownMotor.operationTime_s<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"twirlAngle_degree:"<<twirlMotor.twirlAngle_degree<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"twirlFrequency_tpm:"<<twirlMotor.twirlFrequency_tpm<<qSetFieldWidth(0)<<endl;
                            out<<qSetFieldWidth(30)<<left<<"twirlTime_s:"<<twirlMotor.twirlTime_s<<qSetFieldWidth(0)<<endl<<endl;
                            out<<qSetFieldWidth(30)<<left<<"updownMotorPOSITION"<<"updownMotorVELOCITY"<<"updownMotorCURRENT";
                            out<<"twirlMotorPOSITION"<<"twirlMotorVELOCITY"<<"twirlMotorCURRENT"<<qSetFieldWidth(0)<<endl;
                            comboExecInfoFile.close();
                            comboExecInfoAcquiTimer = startTimer(20,Qt::PreciseTimer);
                        }
                    } else{
                        QMessageBox::warning(this, tr("警告！"),
                                                tr("执行失败，请检查连接！"),
                                                QMessageBox::Yes);
                    }

                }

                else
                    QMessageBox::warning(this, tr("警告！"),
                                           tr("开机后必须至少复位一次！"),
                                          QMessageBox::Yes);
                break;
            case 4:
                QMessageBox::warning(this, tr("警告！"),
                                       tr("选择的执行选项无法组合执行！"),
                                      QMessageBox::Yes);
                break;
            default:
                break;
        }
    }
    else
        QMessageBox::warning(this, tr("警告！"),
                                   tr("执行失败，请检查连接！"),
                                  QMessageBox::Yes);
}

void MainWindow::on_actionAutomatic_triggered()
{
    ui->manu_widget->hide();
    ui->auto_widget->show();
}

void MainWindow::on_actionManul_triggered()
{
    ui->auto_widget->hide();
    ui->manu_widget->show();
}


void MainWindow::on_actLiftThrust_pushButton_clicked()
{
    actLiftThrustTimer = startTimer(15,Qt::PreciseTimer);
    ui->actLiftThrust_pushButton->setEnabled(false);
    ui->freezLiftThrust_pushButton->setEnabled(true);
}

void MainWindow::on_freezLiftThrust_pushButton_clicked()
{
    killTimer(actLiftThrustTimer);
    actLiftThrustTimer = 0;
    updownMotor.MoveOnProfileVelocityMode(0,20000,20000);
    ui->actLiftThrust_pushButton->setEnabled(true);
    ui->freezLiftThrust_pushButton->setEnabled(false);
}

void MainWindow::on_actTwirl_pushButton_clicked()
{
    angleQueue.clear();
    angleQueue.enqueue(0);
    angleQueue.enqueue(0);
    actTwirlTimer = startTimer(15,Qt::PreciseTimer);
    ui->actTwirl_pushButton->setEnabled(false);
    ui->freezTwirl_pushButton->setEnabled(true);
}

void MainWindow::on_freezTwirl_pushButton_clicked()
{
    killTimer(actTwirlTimer);
    actTwirlTimer = 0;
    twirlMotor.MoveOnProfileVelocityMode(0,20000,20000);
    ui->actTwirl_pushButton->setEnabled(true);
    ui->freezTwirl_pushButton->setEnabled(false);
}
