#include "motorthreads.h"
#include "motor.h"
#include "mainwindow.h"

extern UpdownMotor updownMotor;
extern TwirlMotor twirlMotor;
extern PierceOnlyTd pierceOnlyTd;
extern LiftThrustOnlyTd liftThrustOnlyTd;
extern TwirlTd twirlTd;

/**********************************************Threads: updownMotor*********************************************/
/**********************************************Threads: updownMotor*********************************************/

/**************************************************MoveToOriginTd*********************************************/
void MoveToOriginTd::run()
{
    BOOL targetReached = FALSE;
    if(!stopped){
        if(updownMotor.originHasFound){
            updownMotor.MoveToOrigin();
            while( !targetReached ){
                targetReached = updownMotor.IsTargetReached();
                if(stopped){
                    updownMotor.SetQuickStop();
                    break;
                }
            }
        }
        stopped = false;
    }
}

void MoveToOriginTd::stop()
{
    stopped = true;
}

void MoveToOriginTd::renew()
{
    stopped = false;
}

/**************************************************ReplaceNeedleManulTd*********************************************/

void ReplaceNeedleManulTd::run()
{
    BOOL targetReached = FALSE;
    if(!stopped){
        if(updownMotor.originHasFound){
            updownMotor.ReadyPierce();
            while( !targetReached ){
                targetReached = updownMotor.IsTargetReached();
                if(stopped){
                    updownMotor.SetQuickStop();
                    break;
                }
            }
        }
        stopped = false;
    }
}

void ReplaceNeedleManulTd::stop()
{
    stopped = true;
}

void ReplaceNeedleManulTd::renew()
{
    stopped = false;
}


/**************************************************PierceOnlyTd***********************************************/
void PierceOnlyTd::run()
{
    BOOL targetReached = FALSE;
    if(!stopped){
        if(updownMotor.originHasFound){
            updownMotor.ReadyPierce();
            while(!targetReached){
                targetReached = updownMotor.IsTargetReached();
                if(stopped){
                    updownMotor.SetQuickStop();
                    break;
                }
            }
            targetReached = FALSE;
            if(!stopped){
                updownMotor.Pierce();
                while(!targetReached){
                    targetReached = updownMotor.IsTargetReached();
                    if(stopped){
                        updownMotor.SetQuickStop();
                        break;
                    }
                }
            targetReached = FALSE;
            }
        }
        stopped = false;
    }
}

void PierceOnlyTd::stop()
{
    stopped = true;
}

void PierceOnlyTd::renew()
{
    stopped = false;
}

/*************************************************LiftThrustOnlyTd***********************************************/
void LiftThrustOnlyTd::run()
{
    BOOL targetReached = FALSE;
    int i;
    int totaltimes = updownMotor.frequency_tpm*updownMotor.operationTime_s/60; //frequency_tpm*time_s乘积应是60的倍数
    if(!stopped && updownMotor.originHasFound){
        updownMotor.MoveOnProfilePositionMode(-updownMotor.amplitude_mm*updownMotor.mm_to_qc,FALSE,TRUE,50,1000,1000,true);//提插准备工作：向下插一个amplify
        for(i=0;i<totaltimes && !stopped;i++){
            updownMotor.Lift();
            while(!targetReached){
                targetReached = updownMotor.IsTargetReached();
                if(stopped){
                    updownMotor.SetQuickStop();
                    break;
                }
            }
            targetReached = FALSE;
            if(!stopped){
                updownMotor.Thrust();
                while(!targetReached){
                    targetReached = updownMotor.IsTargetReached();
                    if(stopped){
                        updownMotor.SetQuickStop();
                        break;
                    }
                }
            targetReached = FALSE;
            }
            //qDebug("%d",j++);
        }
        updownMotor.MoveOnProfilePositionMode(updownMotor.amplitude_mm*updownMotor.mm_to_qc,FALSE,TRUE,40,1000,1000,true);//提插收尾工作：回到刺入点
    stopped = false;
    }
}

void LiftThrustOnlyTd::stop()
{
    stopped = true;
}

void LiftThrustOnlyTd::renew()
{
    stopped = false;
}

/*************************************************PierceAndLiftThrustTd************************************************/
void PierceAndLiftThrustTd::run()
{
    if(!stopped){
        pierceOnlyTd.start();
        while(pierceOnlyTd.isRunning()){
            if(stopped){
                pierceOnlyTd.stop();
                break;
            }
        }
        if(!stopped){
            liftThrustOnlyTd.start();
            while(liftThrustOnlyTd.isRunning()){
                if(stopped){
                    liftThrustOnlyTd.stop();
                    break;
                }
            }
        }
        stopped = false;
    }
}

void PierceAndLiftThrustTd::stop()
{
    stopped = true;
}

void PierceAndLiftThrustTd::renew()
{
    stopped = false;
}



/**********************************************Thread: twirlMotor*********************************************/
/**********************************************Thread: twirlMotor*********************************************/

/***************************************************TwirlTd***************************************************/
void TwirlTd::run()
{
    BOOL targetReached = FALSE;
    int i;
    int totaltimes = twirlMotor.twirlFrequency_tpm*twirlMotor.twirlTime_s/60; //twirlFrequency_tpm*twirlTime_s乘积应是60的倍数
    if(!stopped){
        for(i=0;i<totaltimes && !stopped;i++){
            twirlMotor.GoRight();
            while(!targetReached){
                targetReached = twirlMotor.IsTargetReached();
                if(stopped){
                    twirlMotor.SetQuickStop();
                    break;
                }
            }
            targetReached = FALSE;
            if(!stopped){
                twirlMotor.GoLeft();
                while(!targetReached){
                    targetReached = twirlMotor.IsTargetReached();
                    if(stopped){
                        twirlMotor.SetQuickStop();
                        break;
                    }
                }
            targetReached = FALSE;
            }
        }
    stopped = false;
    }
}

void TwirlTd::stop()
{
    stopped = true;
}

void TwirlTd::renew()
{
    stopped = false;
}

/**********************************************Threads: Combo*********************************************/
/**********************************************Threads: Combo*********************************************/

/**********************************************Combo1Td:刺入后提插捻转*********************************************/
void Combo1Td::run()
{
    if(!stopped){
        pierceOnlyTd.start();
        while(pierceOnlyTd.isRunning()){
            if(stopped){
                pierceOnlyTd.stop();
                break;
            }
        }
        if(!stopped){
            liftThrustOnlyTd.start();
            twirlTd.start();
            while(liftThrustOnlyTd.isRunning() || twirlTd.isRunning()){
                if(stopped){
                    liftThrustOnlyTd.stop();
                    twirlTd.stop();
                    break;
                }
            }
        }
        stopped = false;
    }
}

void Combo1Td::stop()
{
    stopped = true;
}

void Combo1Td::renew()
{
    stopped = false;
}

/**********************************************Combo2Td:刺入后仅捻转*********************************************/
void Combo2Td::run()
{
    if(!stopped){
        pierceOnlyTd.start();
        while(pierceOnlyTd.isRunning()){
            if(stopped){
                pierceOnlyTd.stop();
                break;
            }
        }
        if(!stopped){
            twirlTd.start();
            while(twirlTd.isRunning()){
                if(stopped){
                    twirlTd.stop();
                    break;
                }
            }
        }
        stopped = false;
    }
}

void Combo2Td::stop()
{
    stopped = true;
}

void Combo2Td::renew()
{
    stopped = false;
}

/**********************************************Combo3Td:仅提插捻转*********************************************/
void Combo3Td::run()
{
    if(!stopped){
        liftThrustOnlyTd.start();
        twirlTd.start();
        while(liftThrustOnlyTd.isRunning() || twirlTd.isRunning()){
            if(stopped){
                liftThrustOnlyTd.stop();
                twirlTd.stop();
                break;
                }
            }
        }
    stopped = false;
}

void Combo3Td::stop()
{
    stopped = true;
}

void Combo3Td::renew()
{
    stopped = false;
}
