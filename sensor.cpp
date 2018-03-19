#include "sensor.h"
#include "easyusb_card_dll.h"
/************传感器基类*************/
Sensor::Sensor()
{
    num = -1;
    voltage = -1.0;
}

Sensor::Sensor(int n)
{
    num = n;
    voltage = -1.0;
}

Sensor::~Sensor()
{

}
//获取传感器电压值
void Sensor::GetVoltage()
{
    float adResult;
    ADSingleV20(num,&adResult);
    voltage = adResult;
}

/*************光电开关类**************/
Photoelec::Photoelec()
{

}

Photoelec::Photoelec(int n) : Sensor(n)
{

}

Photoelec::~Photoelec()
{

}
//判断是否到达
bool Photoelec::Arrive()
{
    GetVoltage();
    if(voltage<max_v && voltage>min_v)
    {
        arrive  = true;
        return true;
    }
    else
    {
        arrive = false;
        return false;
    }

}
