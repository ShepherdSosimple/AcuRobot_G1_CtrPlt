#ifndef MOTOR_H
#define MOTOR_H

#include "Definitions.h"
#include <QMainWindow>

class Motor
{    
protected:
    /*******************************************电机**************************************************/
    WORD motorType;  //电机型号
    WORD nominalCurrent;  //名义电流
    WORD maxOutputCurrent; //最大输出电流
    WORD thermalTimeConst; //热时间常数


    /*******************************************编码器*************************************************/
    WORD encoderType; // 编码器型号
    /*Incremental Encoder 1 with index(3-channel) 1 ST_INC_ENCODER_3CHANNEL
      Incremental Encoder 1 without index(2-channel) 2 ST_INC_ENCODER_2CHANNEL */
    DWORD encoderResolution; //编码器分辨率pulse per turn
    bool invertPolarity;

    /*******************************************安全参数*************************************************/
    DWORD maxFollowError;//最大跟随误差
    DWORD maxProfileVelocity; //最大轮廓速度
    DWORD maxAcceleration; //最大加速度

public:
    double reductionRatio;
    HANDLE motorHandle;
    WORD nodeID; //节点编号
    DWORD errorInfo;

    WORD status; //状态机
    /*Position Profile Mode 1 OMD_PROFILE_POSITION_MODE
      Position Velocity Mode 3 OMD_PROFILE_VELOCITY_MODE
      Homing Mode 6 OMD_HOMING_MODE
      Interpolated Position Mode 7 OMD_INTERPOLATED_POSITION_MODE
      Position Mode −1 OMD_POSITION_MODE
      Velocity Mode −2 OMD_VELOCITY_MODE
      Current Mode −3 OMD_CURRENT_MODE
      Master Encoder Mode −5 OMD_MASTER_ENCODER_MODE
      Step Direction Mode −6 OMD_STEP_DIRECTION_MODE */

    /******************************************电机PID参数***********************************************/
    //电流调节
    WORD p_inCurrent;
    WORD i_inCurrent;
    //速度调节
    WORD p_inVelocity;
    WORD i_inVelocity;
    WORD vFeedforward_inV; //速度调节中的速度前馈因子
    WORD aFeedforward_inV; //速度调节中的加速度前馈因子
    //位置调节
    WORD p_inPosition;
    WORD i_inPosition;
    WORD d_inPosition;
    WORD vFeedforward_inP; //速度调节中的速度前馈因子
    WORD aFeedforward_inP; //速度调节中的加速度前馈因子

    Motor();
    ~Motor();
    /*******************************************输入电机参数********************************************/
    void InputMotorData(WORD motorType, WORD nominalCurrent, WORD maxOutputCurrent, WORD thermalTimeConst,double reductionRatio, WORD nodeID);
    void InputEncoderData(WORD encoderType, DWORD encoderResolution, WINBOOL invertPolarity);
    void InputCurrentRegulationData(WORD p_inCurrent, WORD i_inCurrent);
    void InputVelocityRegulationData(WORD p_inVelocity, WORD iV, WORD vV, WORD aFeedforward_inV);
    void InputPositionRegulationData(WORD p_inPosition, WORD i_inPosition, WORD d_inPosition, WORD vFeedforward_inP, WORD aFeedforward_inP);
    void InputSafetyData(DWORD maxFollowError, DWORD maxProfileVelocity, DWORD maxAcceleration);

    /*******************************************写入电机参数********************************************/
    void SetMotor();
    void SetEncoder();
    void SetCurrentRegulation();
    void SetVelocityRegulation();
    void SetPositionRegulation();
    void SetSafety();

    /********************************************基本运动模式*********************************************/
    bool MoveOnProfilePositionMode(long targetPosition,BOOL absolute,BOOL immediately,DWORD profileVelocity,DWORD profileAcceleration,DWORD profileDeceleration,bool mustFinish);
    bool MoveOnProfileVelocityMode(long targetVelocity, DWORD profileAcceleration, DWORD profileDeceleration); //轮廓速度模式
    bool SetQuickStop();
    /*********************************Motor Method: 获取信息*****************************************/
    long GetTargetPosition();
    long GetPosition();
    long GetVelocity();
    short GetCurrent();

    WINBOOL IsTargetReached();
};



/*******************************************派生电机**************************************************/
/*******************************************派生电机**************************************************/

/*******************************************升降电机**************************************************/
class UpdownMotor: public Motor
{
public:
    double mm_to_qc;
    double mmps_to_rpm;

    long resetVelocity; //  单位r/min
    const DWORD resetAcceleration = 10000;
    const DWORD resetDeceleration = 18000;

    bool UpResetMotion();
    bool DownResetMotion();

    bool photoelec1HasArrIS;
    long originPosition;
    bool originHasFound;
    DWORD pierceAcceleration;
    DWORD pierceDeceleration;
    long readyLenth;//刺入向上抬起行程 单位 mm
    //刺入速度pierceVelocity_mmps-单位毫米每秒，刺入深度pierceDepth_mm-单位毫米
    DWORD pierceVelocity_mmps;
    double pierceDepth_mm;
    //频率frequency_tpm-单位次每分，幅度amplitude_mm-单位毫米，持续时长time_s单位秒
    DWORD frequency_tpm;
    double amplitude_mm;
    DWORD operationTime_s;


    UpdownMotor();
    ~UpdownMotor();

    void InputMotorData(WORD motorType, WORD nominalCurrent, WORD maxOutputCurrent, WORD thermalTimeConst,double reductionRatio, WORD nodeID);
    void InputEncoderData(WORD encoderType, DWORD encoderResolution, WINBOOL invertPolarity);

    //运动分解
    bool FindOrigin(); //搜寻原点
    bool MoveToOrigin(); //回原点
    bool ReadyPierce(); //刺入时向上抬起动作
    bool Pierce(); //加速刺入动作
    bool Lift();
    bool Thrust();
};

/*******************************************夹子电机**************************************************/
class ClampMotor: public Motor
{
private:
    long tightPosition;
public:
    bool isTight;
    ClampMotor();
    ~ClampMotor();
    bool Tighten();
    bool Loosen();
};

/*******************************************捻转电机**************************************************/
class TwirlMotor: public Motor
{
public:
    long leftLimitation; //左极限：靠近捻转电机侧
    long rightLimitation; //右极限：靠近光电开关侧
    double degree_to_qc;   //年转角度 转 qc

    long twirlAngle_degree;  //捻转角度：度
    long twirlFrequency_tpm; //捻转频率：次每分
    long twirlTime_s;        //捻转时间：秒

    TwirlMotor();
    ~TwirlMotor();

    void InputMotorData(WORD motorType, WORD nominalCurrent, WORD maxOutputCurrent, WORD thermalTimeConst,double reductionRatio, WORD nodeID);
    void InputEncoderData(WORD encoderType, DWORD encoderResolution, WINBOOL invertPolarity);

    //运动分解
    bool GoRight();
    bool GoLeft();


};

//换针电机
class RotateMotor: public Motor
{

};

#endif // MOTOR_H
