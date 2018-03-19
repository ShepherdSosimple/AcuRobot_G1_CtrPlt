#ifndef EPOS_H
#define EPOS_H
#include "Definitions.h"

class Epos
{
public:
    char* deviceName;  //设备名称：EPOS or EPOS2
    char* protocolStackName;//通讯协议：MAXON_RS232、MAXON SERIAL V2、CANopen
    char* interfaceName;    //RS232，USB,......
    char* portName;  //COM1, COM2, … USB0, USB1, … CAN0, CAN1, …
    DWORD baudRate;
    DWORD timeOut;
    DWORD errorInfo;
    HANDLE keyHandle;
    WORD nodeID;

    void SetEposData(char* deviceName,char* protocolStackName,char* interfaceName,char* portName,DWORD baudRate,DWORD timeOut,WORD nodeID);
    bool ConnectEpos(); //连接EPOS
    bool DisconnectEpos();  //断开EPOS
    bool EnableEpos();
    bool DisableEpos();

    //获取Epos状态
    WINBOOL IsEnable();

};

#endif // EPOS_H
