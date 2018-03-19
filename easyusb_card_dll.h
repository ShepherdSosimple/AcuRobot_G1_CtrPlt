
#ifndef __EASYUSB_CARD_DLL__
#define __EASYUSB_CARD_DLL__


//#pragma comment(lib,"easyusb_card_dll.lib")
 
#ifdef __cplusplus
extern "C" {
#endif


// *********** 设备打开函数，成功返回0 ***********
	int __stdcall OpenUsbV20(void);

// *********** 设备关闭函数，成功返回0 ***********
	int __stdcall CloseUsbV20(void);

// *********** USB设备重新挂载函数，成功返回0，然后需要重新打开设备 ***********
	int __stdcall ResetUsbV20(void);

// *********** 复位设备函数，成功返回0 ***********
	int __stdcall ResetUsbPipeV20(void);

// *********** 获取设备数量函数，成功返回0 ***********
	int __stdcall GetDeviceCountV20(void);

// *********** 切换设备函数，成功返回0 ***********
	int __stdcall SetCurDeviceV20(int Devicenum);



// *********** 单次获取AD采集结果，成功返回0 ***********
	int __stdcall ADSingleV20(int chan,float* adResult);

// *********** 单通道获取AD采集结果，成功返回0 ***********
	int __stdcall ADContinuV20(int chan,int Num_Sample,int Frequency,float  *databuf);

// *********** 获取多通道AD采集结果，成功返回0 ***********
	int __stdcall MADContinuV20(int chan_first,int chan_last,int Num_Sample,int Frequency,float  *mad_data);

// *********** DA通道单值输出，成功返回0 ***********
	int __stdcall DASingleOutV20(int chan,int value);

// *********** DA通道扫描数据发送，成功返回0 ***********
	int __stdcall DADataSendV20(int chan,int Num,int *databuf);

// *********** DA通道扫描输出设置，成功返回0 ***********
	int __stdcall DAScanOutV20(int chan,int Freq,int scan_Num);

// *********** PWM输出设置，成功返回0 ***********
	int __stdcall PWMOutSetV20(int chan,int Freq,float DutyCycle);

// *********** PWM输入设置，成功返回0 ***********
	int __stdcall PWMInSetV20(int mod);

// *********** PWM输入结果获取，成功返回0 ***********
	int __stdcall PWMInReadV20(float* Freq, int* DutyCycle);

// *********** 计数器输入设置，成功返回0 ***********
	int __stdcall CountSetV20(int mod);

// *********** 计数器结果读取，成功返回0 ***********
	int __stdcall CountReadV20(int* count);

// *********** 开关量输出设定，成功返回0 ***********
	int __stdcall DoSetV20(unsigned char chan,unsigned char state);

// *********** 开关量输入获取，成功返回0 ***********
	int __stdcall DiReadV20(unsigned char *value);

// *********** 获取设备ID号，成功返回0 ***********
	unsigned int __stdcall GetCardIdV20(void);
#ifdef __cplusplus
}
#endif



#endif


