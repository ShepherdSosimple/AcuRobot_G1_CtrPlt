#include "motor.h"
#include "epos.h"
#include "sensor.h"
#include "easyusb_card_dll.h"
#include <windows.h>
#include <QTimer>


extern Photoelec photoelec1;
extern Photoelec photoelec2;
extern Photoelec photoelec3;

Motor::Motor()
{

}

Motor::~Motor()
{

}

/*********************************输入电机参数、编码器参数、PID参数、安全参数*****************************************/
void Motor::InputMotorData(WORD motorType, WORD nominalCurrent, WORD maxOutputCurrent, WORD thermalTimeConst,double reductionRatio, WORD nodeID)
{
    this->motorType = motorType;
    this->nominalCurrent = nominalCurrent;
    this->maxOutputCurrent = maxOutputCurrent;
    this->thermalTimeConst = thermalTimeConst;
    this->reductionRatio = reductionRatio;
    this->nodeID = nodeID;
}

void Motor::InputEncoderData(WORD encoderType, DWORD encoderResolution, BOOL invertPolarity)
{
    this->encoderType = encoderType;
    this->encoderResolution = encoderResolution;
    this->invertPolarity = invertPolarity;
}

void Motor::InputCurrentRegulationData(WORD p_inCurrent, WORD i_inCurrent)
{
    this->p_inCurrent =p_inCurrent;
    this->i_inCurrent = i_inCurrent;
}

void Motor::InputVelocityRegulationData(WORD p_inVelocity, WORD iV, WORD vV, WORD aFeedforward_inV)
{
    p_inVelocity = p_inVelocity; i_inVelocity = iV; vFeedforward_inV = vV; aFeedforward_inV = aFeedforward_inV;
}

void Motor::InputPositionRegulationData(WORD p_inPosition, WORD i_inPosition, WORD d_inPosition, WORD vFeedforward_inP, WORD aFeedforward_inP)
{
    this->p_inPosition = p_inPosition;
    this->i_inPosition = i_inPosition;
    this->d_inPosition = d_inPosition;
    this->vFeedforward_inP = vFeedforward_inP;
    this->aFeedforward_inP = aFeedforward_inP;
}

void Motor::InputSafetyData(DWORD maxFollowError, DWORD maxProfileVelocity, DWORD maxAcceleration)
{
    this->maxFollowError = maxFollowError;
    this->maxProfileVelocity = maxProfileVelocity;
    this->maxAcceleration = maxAcceleration;
}

/*********************************写入电机参数、编码器参数、PID参数、安全参数*****************************************/
void Motor::SetMotor()
{
    VCS_SetMotorType(motorHandle,nodeID,motorType,&errorInfo);
    VCS_SetDcMotorParameter(motorHandle,nodeID,nominalCurrent,maxOutputCurrent,thermalTimeConst,&errorInfo);
}

void Motor::SetEncoder()
{
    VCS_SetSensorType(motorHandle,nodeID,encoderType,&errorInfo);
    VCS_SetIncEncoderParameter(motorHandle,nodeID,encoderResolution,invertPolarity,&errorInfo);
}

void Motor::SetCurrentRegulation()
{
    VCS_SetCurrentRegulatorGain(motorHandle,nodeID,p_inCurrent,i_inCurrent,&errorInfo);
}

void Motor::SetVelocityRegulation()
{
    VCS_SetVelocityRegulatorGain(motorHandle,nodeID,p_inVelocity,i_inVelocity,&errorInfo);
    VCS_SetVelocityRegulatorFeedForward(motorHandle,nodeID,vFeedforward_inV,aFeedforward_inV,&errorInfo);
}

void Motor::SetPositionRegulation()
{
    VCS_SetPositionRegulatorGain(motorHandle,nodeID,p_inPosition,i_inPosition,d_inPosition,&errorInfo);
    VCS_SetPositionRegulatorFeedForward(motorHandle,nodeID,vFeedforward_inP,aFeedforward_inP,&errorInfo);
}

void Motor::SetSafety()
{
    VCS_SetMaxFollowingError(motorHandle,nodeID,maxFollowError,&errorInfo);
    VCS_SetMaxProfileVelocity(motorHandle,nodeID,maxProfileVelocity,&errorInfo);
    VCS_SetMaxAcceleration(motorHandle,nodeID,maxAcceleration,&errorInfo);
}
/*********************************Motor Method: 基本运动方式*****************************************/

bool Motor::MoveOnProfilePositionMode(long targetPosition, BOOL absolute, BOOL immediately, DWORD profileVelocity, DWORD profileAcceleration, DWORD profileDeceleration, bool mustFinish)
{
    if(VCS_SetOperationMode(motorHandle,nodeID,1,&errorInfo))
    {
        if(VCS_SetPositionProfile(motorHandle,nodeID,profileVelocity,profileAcceleration,profileDeceleration,&errorInfo))
        {
            if(VCS_ActivateProfilePositionMode(motorHandle,nodeID,&errorInfo))
            {
                if(VCS_MoveToPosition(motorHandle,nodeID,targetPosition,absolute,immediately,&errorInfo))
                {
                    if(mustFinish)
                    {
                        VCS_WaitForTargetReached(motorHandle,nodeID,60000,&errorInfo);
                        return true;
                    }
                    else
                        return true;
                }
                else
                    return false;
            }
            else
                return false;
        }
        else
            return false;
    }
    else
        return false;
}

bool Motor::MoveOnProfileVelocityMode(long targetVelocity,DWORD profileAcceleration,DWORD profileDeceleration)
{
    if(VCS_SetOperationMode(motorHandle,nodeID,3,&errorInfo))
    {
        if(VCS_SetVelocityProfile(motorHandle,nodeID,profileAcceleration,profileDeceleration,&errorInfo))
        {
            if(VCS_ActivateProfileVelocityMode(motorHandle,nodeID,&errorInfo))
            {
                if(VCS_MoveWithVelocity(motorHandle,nodeID,targetVelocity,&errorInfo))
                    return true;
                else
                    return false;
            }
            else
                return false;
        }
        else
            return false;
    }
    else
        return false;
}

bool Motor::SetQuickStop()
{
    if(VCS_SetQuickStopState(motorHandle,nodeID,&errorInfo))
        return true;
    else
        return false;
}


/*********************************Motor Method: 获取信息*****************************************/
long Motor::GetTargetPosition()
{
    long targetPosition;
    VCS_GetTargetPosition(motorHandle,nodeID,&targetPosition,&errorInfo);
    return targetPosition;
}

long Motor::GetPosition()
{
    long currentPosition;
    VCS_GetPositionIs(motorHandle,nodeID,&currentPosition,&errorInfo);
    return currentPosition;
}

long Motor::GetVelocity()
{
    long velocity;
    VCS_GetVelocityIs(motorHandle,nodeID,&velocity,&errorInfo);
    return velocity;
}

short Motor::GetCurrent()
{
    short current;
    VCS_GetCurrentIs(motorHandle,nodeID,&current,&errorInfo);
    return current;
}

BOOL Motor::IsTargetReached()
{
    BOOL targetReached;
    VCS_GetMovementState(motorHandle,nodeID,&targetReached,&errorInfo);
    return targetReached;
}


/*********************************UpdownMotor Method*****************************************/
/*********************************UpdownMotor Method*****************************************/


UpdownMotor::UpdownMotor()
{
    resetVelocity = 200;
    readyLenth = 50;
    photoelec1HasArrIS = false;
    originPosition = 0;
    originHasFound = false;
}

void UpdownMotor::InputMotorData(WORD motorType, WORD nominalCurrent, WORD maxOutputCurrent, WORD thermalTimeConst,double reductionRatio, WORD nodeID)
{
    this->motorType = motorType;
    this->nominalCurrent = nominalCurrent;
    this->maxOutputCurrent = maxOutputCurrent;
    this->thermalTimeConst = thermalTimeConst;
    this->reductionRatio = reductionRatio;
    this->nodeID = nodeID;

    mm_to_qc = 0.1*(this->reductionRatio)*encoderResolution*4; //RE40无减速器：值为200。
    mmps_to_rpm = 60*(this->reductionRatio)/10; //RE40无减速器：值为6。

}

void UpdownMotor::InputEncoderData(WORD encoderType, DWORD encoderResolution, WINBOOL invertPolarity)
{
    this->encoderType = encoderType;
    this->encoderResolution = encoderResolution;
    this->invertPolarity = invertPolarity;

    mm_to_qc = 0.1*reductionRatio*(this->encoderResolution*4);
    mmps_to_rpm = 60*reductionRatio/10;

}

UpdownMotor::~UpdownMotor()
{

}

bool UpdownMotor::UpResetMotion()
{
    if(MoveOnProfileVelocityMode(resetVelocity,resetAcceleration,resetDeceleration))
        return true;
    else
        return false;
}

bool UpdownMotor::DownResetMotion()
{
    if(MoveOnProfileVelocityMode(-resetVelocity,resetAcceleration,resetDeceleration))
        return true;
    else
        return false;
}

/***************************原点相关运动*****************************/
bool UpdownMotor::FindOrigin()//搜寻原点
{
    if(!photoelec1.arrive && !photoelec1HasArrIS){
        UpResetMotion();
        return false;
    }
    else if(photoelec1.arrive){
        DownResetMotion();
        photoelec1HasArrIS = true;
        return false;
    }
    else if(photoelec2.arrive && photoelec1HasArrIS){
        VCS_HaltVelocityMovement(motorHandle,nodeID,&errorInfo);
        VCS_GetPositionIs(motorHandle,nodeID,&originPosition,&errorInfo);
        originHasFound = true;
        return true;
    }
    else
        return false;
}
bool UpdownMotor::MoveToOrigin()//回到原点
{
    if(originHasFound){
        MoveOnProfilePositionMode(originPosition,TRUE,TRUE,resetVelocity,resetAcceleration,resetDeceleration,false);
        return true;
    }
    else
        return false;
}

/********************************************刺入相关动作********************************************/
bool UpdownMotor::ReadyPierce() //刺入时向上抬起动作
{
    if(originHasFound){
        MoveOnProfilePositionMode(originPosition+readyLenth*mm_to_qc,true,false,resetVelocity,resetAcceleration,resetDeceleration,false);
        return true;
    }
    return false;
}
bool UpdownMotor::Pierce()  //向下加速刺入
{
    pierceAcceleration = 3*pierceVelocity_mmps*pierceVelocity_mmps/(readyLenth-10);
    pierceDeceleration = 3*pierceVelocity_mmps*pierceVelocity_mmps/(pierceDepth_mm/1.5);
    if(originHasFound){
        MoveOnProfilePositionMode(originPosition-pierceDepth_mm*mm_to_qc,1,0,pierceVelocity_mmps*mmps_to_rpm,pierceAcceleration,pierceDeceleration,false);//可考虑插值位置模式来确保皮肤接触点的刺破速度为所需速度；pierceVelocity_mmps*mmps_to_rpm,pierceAcceleration,pierceDeceleration
        return true;
    }
    else
        return false;
}

/********************************************提插相关操作**********************************************/
bool UpdownMotor::Lift()
{
    DWORD peakVelocity = 4*2*amplitude_mm*frequency_tpm*mmps_to_rpm/60;//frequency_tpm最好是5的倍数才能得到整数的peakVelocity
    DWORD acceleration = peakVelocity*frequency_tpm/15;
    if(originHasFound){
        if(MoveOnProfilePositionMode(2*amplitude_mm*mm_to_qc,FALSE,FALSE,peakVelocity,acceleration,acceleration,false))
            return true;
        else
            return false;
    }
    else
        return false;
}

bool UpdownMotor::Thrust()
{
    DWORD peakVelocity = 4*2*amplitude_mm*frequency_tpm*mmps_to_rpm/60;//frequency_tpm最好是5的倍数才能得到整数的peakVelocity
    DWORD acceleration = peakVelocity*frequency_tpm/15;
    if(originHasFound){
        if(MoveOnProfilePositionMode(-2*amplitude_mm*mm_to_qc,FALSE,FALSE,peakVelocity,acceleration,acceleration,false))
            return true;
        else
            return false;
    }
    else
        return false;
}


/******************************************************ClampMotor Method**************************************************/
/******************************************************ClampMotor Method**************************************************/

ClampMotor::ClampMotor()
{
    tightPosition = -34000;
    isTight = false;
}

ClampMotor::~ClampMotor()
{

}

bool ClampMotor::Tighten()
{
    if(MoveOnProfilePositionMode(tightPosition,TRUE,TRUE,1500,5000,5000,false)){
        isTight = true;
        return true;
    }
    else
        return false;
}

bool ClampMotor::Loosen()
{
    if(MoveOnProfilePositionMode(0,TRUE,TRUE,1500,5000,5000,false)){
        isTight = false;
        return true;
    }
    else
        return false;
}


/******************************************************TwirlMotor Method**************************************************/
/******************************************************TwirlMotor Method**************************************************/
TwirlMotor::TwirlMotor()
{
    leftLimitation = 0; //左极限：靠近捻转电机侧
    rightLimitation = 49000; //右极限：靠近光电开关侧
}

TwirlMotor::~TwirlMotor()
{

}

void TwirlMotor::InputMotorData(WORD motorType, WORD nominalCurrent, WORD maxOutputCurrent, WORD thermalTimeConst,double reductionRatio, WORD nodeID)
{
    this->motorType = motorType;
    this->nominalCurrent = nominalCurrent;
    this->maxOutputCurrent = maxOutputCurrent;
    this->thermalTimeConst = thermalTimeConst;
    this->reductionRatio = reductionRatio;
    this->nodeID = nodeID;

    degree_to_qc = encoderResolution*4*reductionRatio/360;

}

void TwirlMotor::InputEncoderData(WORD encoderType, DWORD encoderResolution, WINBOOL invertPolarity)
{
    this->encoderType = encoderType;
    this->encoderResolution = encoderResolution;
    this->invertPolarity = invertPolarity;

    degree_to_qc = encoderResolution*4*reductionRatio/360;
}

bool TwirlMotor::GoRight()
{
    DWORD peakVelocity = reductionRatio*twirlAngle_degree*twirlFrequency_tpm/90;  //单位：r/min
    DWORD acceleration = peakVelocity*twirlFrequency_tpm/15; //单位：(r/min)/s
    if(MoveOnProfilePositionMode(twirlAngle_degree*degree_to_qc,FALSE,FALSE,peakVelocity,acceleration,acceleration,false))
        return true;
    else
        return false;
}

bool TwirlMotor::GoLeft()
{
    DWORD peakVelocity = reductionRatio*twirlAngle_degree*twirlFrequency_tpm/90;  //单位：r/min
    DWORD acceleration = peakVelocity*twirlFrequency_tpm/15; //单位：(r/min)/s
    if(MoveOnProfilePositionMode(-twirlAngle_degree*degree_to_qc,FALSE,FALSE,peakVelocity,acceleration,acceleration,false))
        return true;
    else
        return false;
}


