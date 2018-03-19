#include "epos.h"
#include "motor.h"
#include <cstring>

/**********构造函数************/

void Epos::SetEposData(char* deviceName,char* protocolStackName,char* interfaceName,char* portName,DWORD baudRate,DWORD timeOut,WORD nodeID)
{
    this->deviceName = deviceName;
    this->protocolStackName = protocolStackName;
    this->interfaceName = interfaceName;
    this->portName = portName;
    this->baudRate = baudRate ;
    this->timeOut = timeOut;
    this->nodeID = nodeID;
}


/******************************Method: 操作**************************************/
//连接Epos
bool Epos::ConnectEpos()
{
    keyHandle = VCS_OpenDevice(deviceName,protocolStackName,interfaceName,portName,&errorInfo);
    VCS_ClearFault(keyHandle,nodeID,&errorInfo);
    if(keyHandle && VCS_SetProtocolStackSettings(keyHandle,baudRate,timeOut,&errorInfo))
        return true;
    else
        return false;
}
//断开Epos
bool Epos::DisconnectEpos()
{
    if(VCS_CloseDevice(keyHandle,&errorInfo))
        return true;
    else
        return false;
}

bool Epos::EnableEpos()
{
    if(VCS_SetEnableState(keyHandle,nodeID,&errorInfo)){
        //VCS_EnablePositionWindow(keyHandle,nodeID,20,5,&errorInfo);
        return true;
    }
    else
        return false;
}

bool Epos::DisableEpos()
{ 
    //VCS_DisablePositionWindow(keyHandle,nodeID,&errorInfo);
    if(VCS_SetDisableState(keyHandle,nodeID,&errorInfo))
        return true;
    else
        return false;
}

/******************************Method: 获取信息**************************************/
BOOL Epos::IsEnable()
{
    BOOL isEnable;
    VCS_GetEnableState(keyHandle,nodeID,&isEnable,&errorInfo);
    return isEnable;
}




