//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, 华中科技大学tickTick Group  （版权声明）
/// All rights reserved.
/// 
/// @file    SerialPort.h  
/// @brief   串口通信类头文件
///
/// 本文件完成串口通信类的声明
///
/// @version 1.0   
/// @author  卢俊 
/// @E-mail：lujun.hust@gmail.com
/// @date    2010/03/19
///
///  修订说明：
//////////////////////////////////////////////////////////////////////////

#ifndef SERIALPORT_H_
#define SERIALPORT_H_

//#define BCar
#define ACar







#include <Windows.h>

#include"..\UART2CAN\HUST_CAN.h"
#include"..\Gettime.h"
#include"TCPSocket.h"



/******************************TH的数据区*************************************/
union THReceiveDataUnion
{
	unsigned char charData[8];
	short shortData[4];
	float floatData[2];
	double doubleData[1];
};


//标识串口种类 
typedef enum tagSerialPortType
{
	UnKnow = 0,//未知的串口
	THTheroVision = 1,      
	KinectCarA = 2,      //A的优先级高于B，A具有最高决策权
	KinectCarB = 3,
} SerialPortType;


//动作标识 
typedef enum tagActionType
{
	Wait = 0, 
	zijiwan = 1,
	Moving = 2,
	GetTheBallTH = 3,
	GetTheBallLA = 4,
	AvoidTheCar = 5,
} ActionType;


//帧定义
//{0XFF , 帧头，决定 ，x，y，t}
//kinect的决策写在guess中
typedef enum tagFramwType
{
	TellPosFrame = 0,//只有kinect可以发
	ToMoveFrame = 1,//控制移动帧，只有Kinect可以发（双目发送的一定是决策帧）（帧定义 ：{0XFF , （帧头，0） ，x，y，t} ）
	DecisionFrame = 2,//命令决策帧，只有双目才可以发（帧定义 ：{0XFF , 帧头，（决定（0，1）） ，x，y，t} ）
	AskTypeFrame = 3,
	AnswerTypeFrame = 4, //帧定义 ：{ 0XFF, 帧头，归属 ，x，y，t } ）
	ResetFrame = 5,

} FramwType;
/*******************************************************************/






/** 串口通信类
*
*  本类实现了对串口的基本操作
*  例如监听发到指定串口的数据、发送指定数据到串口
*/
class CSerialPort
{
public:
	CSerialPort(void);
	~CSerialPort(void);

	int Counter;
	float LastNum;//用于查看误码
	int flag;//标志AB车
	THReceiveDataUnion m_thReceiveDataUnion;//收到的数据标志


	int portNum;

	double DistanceToDropPoint;//车距离计算的落点的距离，用于判断哪个车接球哪个车避让


	short x;
	short y;

	double GetPositionTime;//记录上次得到车位置的时间，用于差值计算速度
	double Vx;
	double Vy;


	CSerialPort *m_Me;

	double GetActionTypeTime;
	ActionType m_ActionType;

	//标识串口归属
	SerialPortType m_SerialPortType;//一个监听线程只对应一对蓝牙

	CSerialPort *AkinectSerialPort;
	CSerialPort *BkinectSerialPort;
	CSerialPort *THSerialPort;


	bool LAsend;

	bool THsend;


	HUST_CAN *m_CAN;//用于接收线程发送

public:

	/** 初始化串口函数
	*
	*  @param:  UINT portNo 串口编号,默认值为1,即COM1,注意,尽量不要大于9
	*  @param:  UINT baud   波特率,默认为9600
	*  @param:  char parity 是否进行奇偶校验,'Y'表示需要奇偶校验,'N'表示不需要奇偶校验
	*  @param:  UINT databits 数据位的个数,默认值为8个数据位
	*  @param:  UINT stopsbits 停止位使用格式,默认值为1
	*  @param:  DWORD dwCommEvents 默认为EV_RXCHAR,即只要收发任意一个字符,则产生一个事件
	*  @return: bool  初始化是否成功
	*  @note:   在使用其他本类提供的函数前,请先调用本函数进行串口的初始化
	*　　　　　 \n本函数提供了一些常用的串口参数设置,若需要自行设置详细的DCB参数,可使用重载函数
	*           \n本串口类析构时会自动关闭串口,无需额外执行关闭串口
	*  @see:
	*/
	bool InitPort(UINT  portNo = 1, UINT  baud = CBR_9600, char  parity = 'N', UINT  databits = 8,
		UINT  stopsbits = 1, DWORD dwCommEvents = EV_RXCHAR);

	/** 串口初始化函数
	*
	*  本函数提供直接根据DCB参数设置串口参数
	*  @param:  UINT portNo
	*  @param:  const LPDCB & plDCB
	*  @return: bool  初始化是否成功
	*  @note:   本函数提供用户自定义地串口初始化参数
	*  @see:
	*/
	bool InitPort(UINT  portNo, const LPDCB& plDCB);

	/** 开启监听线程
	*
	*  本监听线程完成对串口数据的监听,并将接收到的数据打印到屏幕输出
	*  @return: bool  操作是否成功
	*  @note:   当线程已经处于开启状态时,返回flase
	*  @see:
	*/
	bool OpenListenThread();

	/** 关闭监听线程
	*
	*
	*  @return: bool  操作是否成功
	*  @note:   调用本函数后,监听串口的线程将会被关闭
	*  @see:
	*/
	bool CloseListenThread();


	bool OpenSendThread();
	//这个函数把后台数据添加到发送线程
	bool ToSendData(unsigned char* pData, unsigned int length);



	/** 向串口写数据
	*
	*  将缓冲区中的数据写入到串口
	*  @param:  unsigned char * pData 指向需要写入串口的数据缓冲区
	*  @param:  unsigned int length 需要写入的数据长度
	*  @return: bool  操作是否成功
	*  @note:   length不要大于pData所指向缓冲区的大小
	*  @see:
	*/
	bool WriteData(unsigned char* pData, unsigned int length);

	/** 获取串口缓冲区中的字节数
	*
	*
	*  @return: UINT  操作是否成功
	*  @note:   当串口缓冲区中无数据时,返回0
	*  @see:
	*/
	UINT GetBytesInCOM();

	/** 读取串口接收缓冲区中一个字节的数据
	*
	*
	*  @param:  char & cRecved 存放读取数据的字符变量
	*  @return: bool  读取是否成功
	*  @note:
	*  @see:
	*/
	bool ReadChar(char &cRecved);



	/************************自己定义的函数****************************************/
	bool SendTellPosFrame(short x, short y);

	bool SendToMovingFrame(bool MeOrYou, short x, short y, short t);
	bool SendDecisionFrame(bool _GET_NO, short x, short y, ushort t);

	bool SendAskTypeFrame();
	bool SendAnswerTypeFrame(SerialPortType _SerialPortType);
	bool SendResetFrame();

private:

	/** 打开串口
	*
	*
	*  @param:  UINT portNo 串口设备号
	*  @return: bool  打开是否成功
	*  @note:
	*  @see:
	*/
	bool openPort(UINT  portNo);

	/** 关闭串口
	*
	*
	*  @return: void  操作是否成功
	*  @note:
	*  @see:
	*/
	void ClosePort();

	/** 串口监听线程
	*
	*  监听来自串口的数据和信息
	*  @param:  void * pParam 线程参数
	*  @return: UINT WINAPI 线程返回值
	*  @note:
	*  @see:
	*/
	static UINT WINAPI ListenThread(void* pParam);

private:

	/** 串口句柄 */
	HANDLE  m_hComm;

	/** 线程退出标志变量 */
	static bool s_bExit;

	/** 线程句柄 */
	volatile HANDLE    m_hListenThread;

	/** 同步互斥,临界区保护 */
	CRITICAL_SECTION   m_csCommunicationSync;       //!< 互斥操作串口

};

#endif //SERIALPORT_H_