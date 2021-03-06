/////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, 华中科技大学tickTick Group  （版权声明）
/// All rights reserved.
/// 
/// @file    SerialPort.cpp  
/// @brief   串口通信类的实现文件
///
/// 本文件为串口通信类的实现代码
///
/// @version 1.0   
/// @author  卢俊  
/// @E-mail：lujun.hust@gmail.com
/// @date    2010/03/19
/// 
///
///  修订说明：
//////////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "SerialPort.h"
#include <process.h>
#include <iostream>
#include "Parameter.h"

using namespace std;


int AnotherCarX = 2000;
int AnotherCarY = 0;


extern int RESETX;
extern int RESETY;




/** 线程退出标志 */
bool CSerialPort::s_bExit = false;
/** 当串口无数据时,sleep至下次查询间隔的时间,单位:秒 */
const UINT SLEEP_TIME_INTERVAL = 1;


CSerialPort::CSerialPort(void)
	: m_hListenThread(INVALID_HANDLE_VALUE)
{

	//加这句的目的是：单独调试时，LAsend标志位为true，不需要大改
	LAsend = true;
	THsend = true;
	m_ActionType = Wait;
	m_SerialPortType = UnKnow;

	AkinectSerialPort = NULL;
	BkinectSerialPort = NULL;
	THSerialPort = NULL;



	m_hComm = INVALID_HANDLE_VALUE;
	m_hListenThread = INVALID_HANDLE_VALUE;

	InitializeCriticalSection(&m_csCommunicationSync);

}

CSerialPort::~CSerialPort(void)
{
	CloseListenThread();
	ClosePort();
	DeleteCriticalSection(&m_csCommunicationSync);
}

bool CSerialPort::InitPort(UINT portNo /*= 1*/, UINT baud /*= CBR_9600*/, char parity /*= 'N'*/,
	UINT databits /*= 8*/, UINT stopsbits /*= 1*/, DWORD dwCommEvents /*= EV_RXCHAR*/)
{

	/** 临时变量,将制定参数转化为字符串形式,以构造DCB结构 */
	char szDCBparam[50];
	sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);

	/** 打开指定串口,该函数内部已经有临界区保护,上面请不要加保护 */
	if (!openPort(portNo))
	{
		return false;
	}

	/** 进入临界段 */
	EnterCriticalSection(&m_csCommunicationSync);

	/** 是否有错误发生 */
	BOOL bIsSuccess = TRUE;

	/** 在此可以设置输入输出的缓冲区大小,如果不设置,则系统会设置默认值.
	*  自己设置缓冲区大小时,要注意设置稍大一些,避免缓冲区溢出
	*/
	/*if (bIsSuccess )
	{
	bIsSuccess = SetupComm(m_hComm,10,10);
	}*/

	/** 设置串口的超时时间,均设为0,表示不使用超时限制 */
	COMMTIMEOUTS  CommTimeouts;
	CommTimeouts.ReadIntervalTimeout = 0;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 0;
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	CommTimeouts.WriteTotalTimeoutConstant = 0;
	if (bIsSuccess)
	{
		bIsSuccess = SetCommTimeouts(m_hComm, &CommTimeouts);
	}

	DCB  dcb;
	if (bIsSuccess)
	{
		// 将ANSI字符串转换为UNICODE字符串
		DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, NULL, 0);
		wchar_t *pwText = new wchar_t[dwNum];
		if (!MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, pwText, dwNum))
		{
			bIsSuccess = TRUE;
		}

		/** 获取当前串口配置参数,并且构造串口DCB参数 */
		bIsSuccess = GetCommState(m_hComm, &dcb) && BuildCommDCB(pwText, &dcb);
		/** 开启RTS flow控制 */
		dcb.fRtsControl = RTS_CONTROL_ENABLE;

		/** 释放内存空间 */
		delete[] pwText;
	}

	if (bIsSuccess)
	{
		/** 使用DCB参数配置串口状态 */
		bIsSuccess = SetCommState(m_hComm, &dcb);
	}

	/**  清空串口缓冲区 */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/** 离开临界段 */
	LeaveCriticalSection(&m_csCommunicationSync);

	return bIsSuccess == TRUE;
}

bool CSerialPort::InitPort(UINT portNo, const LPDCB& plDCB)
{
	/** 打开指定串口,该函数内部已经有临界区保护,上面请不要加保护 */
	if (!openPort(portNo))
	{
		return false;
	}

	/** 进入临界段 */
	EnterCriticalSection(&m_csCommunicationSync);

	/** 配置串口参数 */
	if (!SetCommState(m_hComm, plDCB))
	{
		return false;
	}

	/**  清空串口缓冲区 */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/** 离开临界段 */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

void CSerialPort::ClosePort()
{
	/** 如果有串口被打开，关闭它 */
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}
}

bool CSerialPort::openPort(UINT portNo)
{
	/** 进入临界段 */
	EnterCriticalSection(&m_csCommunicationSync);

	/** 把串口的编号转换为设备名 */
	char szPort[50];
	sprintf_s(szPort, "COM%d", portNo);

	/** 打开指定的串口 */
	m_hComm = CreateFileA(szPort,		                /** 设备名,COM1,COM2等 */
		GENERIC_READ | GENERIC_WRITE,  /** 访问模式,可同时读写 */
		0,                             /** 共享模式,0表示不共享 */
		NULL,							/** 安全性设置,一般使用NULL */
		OPEN_EXISTING,					/** 该参数表示设备必须存在,否则创建失败 */
		0,
		0);

	/** 如果打开失败，释放资源并返回 */
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		LeaveCriticalSection(&m_csCommunicationSync);
		return false;
	}

	/** 退出临界区 */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

bool CSerialPort::OpenListenThread()
{
	/** 检测线程是否已经开启了 */
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** 线程已经开启 */
		return false;
	}

	s_bExit = false;
	/** 线程ID */
	UINT threadId;
	/** 开启串口数据监听线程 */
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);
	if (!m_hListenThread)
	{
		return false;
	}
	/** 设置线程的优先级,高于普通线程 */
	if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return false;
	}

	return true;
}


bool CSerialPort::CloseListenThread()
{
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** 通知线程退出 */
		s_bExit = true;

		/** 等待线程退出 */
		Sleep(10);

		/** 置线程句柄无效 */
		CloseHandle(m_hListenThread);
		m_hListenThread = INVALID_HANDLE_VALUE;
	}
	return true;
}

UINT CSerialPort::GetBytesInCOM()
{
	DWORD dwError = 0;	/** 错误码 */
	COMSTAT  comstat;   /** COMSTAT结构体,记录通信设备的状态信息 */
	memset(&comstat, 0, sizeof(COMSTAT));

	UINT BytesInQue = 0;
	/** 在调用ReadFile和WriteFile之前,通过本函数清除以前遗留的错误标志 */
	if (ClearCommError(m_hComm, &dwError, &comstat))
	{
		BytesInQue = comstat.cbInQue; /** 获取在输入缓冲区中的字节数 */
	}

	return BytesInQue;
}

UINT WINAPI CSerialPort::ListenThread(void* pParam)
{
	//多个串口，开多个线程不能使用static变量！
	/** 得到本类的指针 */
	CSerialPort *pSerialPort = reinterpret_cast<CSerialPort*>(pParam);

	std::cout << "Open thread to receive data from TH, Sleep time " << SLEEP_TIME_INTERVAL << " ms" << std::endl;

	pSerialPort->Counter = -2;

	pSerialPort->Vx = 0;
	pSerialPort->Vy = 0;

	pSerialPort->x = 0;
	pSerialPort->y = 0;

	// 线程循环,轮询方式读取串口数据
	while (!pSerialPort->s_bExit)
	{
		UINT BytesInQue = pSerialPort->GetBytesInCOM();
		/** 如果串口输入缓冲区中无数据,则休息一会再查询 */
		if (BytesInQue == 0)
		{
			Sleep(SLEEP_TIME_INTERVAL);

			//一个串口会开一个线程
			//cout << "*" << endl;
			//Sleep(10000);

			continue;
		}

		/** 读取输入缓冲区中的数据并输出显示 */
		char cRecved = 0x00;
		do
		{
			cRecved = 0x00;
			if (pSerialPort->ReadChar(cRecved) == true)
			{
				if ((0) == pSerialPort->Counter)
				{
					if ((char)0xFF == cRecved)//0xEE在windows中是 0x000000EE
					{
						pSerialPort->m_thReceiveDataUnion.charData[pSerialPort->Counter] = cRecved;
						pSerialPort->Counter++;
						continue;
					}
					else
					{
						pSerialPort->Counter = 0;
						continue;
					}
				}



				pSerialPort->m_thReceiveDataUnion.charData[pSerialPort->Counter] = cRecved;
				pSerialPort->Counter++;


				if (pSerialPort->Counter == 8)
				{
					pSerialPort->Counter = 0;

					//解析标志位
					FramwType TypeOfFrame = (FramwType)((pSerialPort->m_thReceiveDataUnion.charData[1] & 0xf0) >> 4);



					switch (TypeOfFrame)
					{


					case TellPosFrame://只有kinect可以发
					{
						//cout << "TellThePosFrame";


						AnotherCarX = pSerialPort->m_thReceiveDataUnion.shortData[1];
						AnotherCarY = pSerialPort->m_thReceiveDataUnion.shortData[2];


						double ThisTime = GETTIME();
						pSerialPort->Vx = ((double)(pSerialPort->m_thReceiveDataUnion.shortData[1] - pSerialPort->x)) / (ThisTime - pSerialPort->GetPositionTime);
						pSerialPort->Vy = ((double)(pSerialPort->m_thReceiveDataUnion.shortData[2] - pSerialPort->y)) / (ThisTime - pSerialPort->GetPositionTime);
						pSerialPort->x = pSerialPort->m_thReceiveDataUnion.shortData[1];
						pSerialPort->y = pSerialPort->m_thReceiveDataUnion.shortData[2];
						pSerialPort->GetPositionTime = ThisTime;

						//cout << "x" << pSerialPort->x << "y" << pSerialPort->y << endl;

					}
					break;



					case ToMoveFrame://只有kinect才能发送
						//gkout << "ToMoveFrame" << endl;

						if (pSerialPort->m_Me->m_ActionType == Wait || pSerialPort->m_Me->m_ActionType == Moving)//双目的决定还没有追加进来
						{
							pSerialPort->m_Me->m_ActionType = Moving;

							pSerialPort->m_Me->GetActionTypeTime = GETTIME();

							//发送底盘位置
							//pSerialPort->m_CAN->Send_RobotPosition_Frame(0x01, pSerialPort->m_thReceiveDataUnion.shortData[1], pSerialPort->m_thReceiveDataUnion.shortData[2], pSerialPort->m_thReceiveDataUnion.shortData[3]);

							//gkout << "kinect" << pSerialPort->m_thReceiveDataUnion.shortData[1] << "	" << pSerialPort->m_thReceiveDataUnion.shortData[2] << endl;

							//DipanAimx = pSerialPort->m_thReceiveDataUnion.shortData[1];
							//DipanAimy = pSerialPort->m_thReceiveDataUnion.shortData[2];
						}

						break;




					case DecisionFrame://只有双目才能发送


						gkout << "DecisionFrame" << endl;
						if (pSerialPort->m_SerialPortType == KinectCarA || pSerialPort->m_SerialPortType == KinectCarB)
						{
							gkout << "hao qi guai" << endl;
						}


						//1表示双目同意这台车接球 0表示拒绝这台车接球
						//同意接球
						if ((pSerialPort->m_thReceiveDataUnion.charData[1] & 0x0f))
						{
							gkout << "-TH TO  GetTheBall" << endl;
							if (pSerialPort->m_Me->m_ActionType == GetTheBallLA)//LA开始控车,双目不管
							{

								break;
							}
							else
							{
								//同意这台车接球，等待LA控车

								pSerialPort->m_Me->m_ActionType = GetTheBallTH;
								pSerialPort->m_Me->GetActionTypeTime = GETTIME();

								//DipanAimx = pSerialPort->m_thReceiveDataUnion.shortData[1];
								//DipanAimy = pSerialPort->m_thReceiveDataUnion.shortData[2];

								gkout << "TH Pos" << pSerialPort->m_thReceiveDataUnion.shortData[1] << "	" << pSerialPort->m_thReceiveDataUnion.shortData[2] << endl;

								//发送底盘位置
								//pSerialPort->m_CAN->Send_RobotPosition_Frame(0x01, pSerialPort->m_thReceiveDataUnion.shortData[1], pSerialPort->m_thReceiveDataUnion.shortData[2], pSerialPort->m_thReceiveDataUnion.shortData[3]);
							}
						}
						else
						{
							gkout << "-TH TO  AvoidTheCar" << endl;
							//否决这台车接球，直接否决
							//双目可以直接否决这台车的接球，进入躲避
							pSerialPort->m_CAN->detectEnable = false;
							pSerialPort->m_Me->m_ActionType = AvoidTheCar;
							pSerialPort->m_Me->GetActionTypeTime = GETTIME();
						}
						break;







					case ResetFrame:
						pSerialPort->m_Me->m_ActionType = Wait;
						pSerialPort->m_CAN->detectEnable = true;
						//pSerialPort->m_Me->m_CAN->Send_RobotPositionReset_Frame(1, RESETX, RESETY, 1200);
						gkout << "ResetFrame" << endl;
						break;



					case AskTypeFrame:
						gkout << "AskTypeFrame" << endl;
						pSerialPort->SendAnswerTypeFrame(pSerialPort->m_Me->m_SerialPortType);
						break;

					case AnswerTypeFrame:
						gkout << "AnswerTypeFrame" << endl;
						pSerialPort->m_SerialPortType = (SerialPortType)((pSerialPort->m_thReceiveDataUnion.charData[1] & 0x0f));
						break;

					default:
						gkout << "错误的帧" << endl;
						break;
					}

				}

				continue;
			}
		} while (--BytesInQue);
	}

	return 0;
}

bool CSerialPort::ReadChar(char &cRecved)
{
	BOOL  bResult = TRUE;
	DWORD BytesRead = 0;
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/** 临界区保护 */
	EnterCriticalSection(&m_csCommunicationSync);

	/** 从缓冲区读取一个字节的数据 */
	bResult = ReadFile(m_hComm, &cRecved, 1, &BytesRead, NULL);
	if ((!bResult))
	{
		/** 获取错误码,可以根据该错误码查出错误原因 */
		DWORD dwError = GetLastError();

		/** 清空串口缓冲区 */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/** 离开临界区 */
	LeaveCriticalSection(&m_csCommunicationSync);

	return (BytesRead == 1);

}

bool CSerialPort::WriteData(unsigned char* pData, unsigned int length)
{
	BOOL   bResult = TRUE;
	DWORD  BytesToSend = 0;
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/** 临界区保护 */
	EnterCriticalSection(&m_csCommunicationSync);

	/** 向缓冲区写入指定量的数据 */
	bResult = WriteFile(m_hComm, pData, length, &BytesToSend, NULL);
	if (!bResult)
	{
		DWORD dwError = GetLastError();
		/** 清空串口缓冲区 */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/** 离开临界区 */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}





/************************自己定义的函数****************************************/
bool CSerialPort::SendAskTypeFrame()
{
	if (this == NULL)
	{
		cout << "send data error" << endl;
		return false;
	}

	//kinect 发这个询问帧
	unsigned char SerialPortSendData[8] = { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	//添加帧头
	SerialPortSendData[1] = AskTypeFrame << 4;


	if (this->WriteData(SerialPortSendData, 8))
	{
		//std::cout << "send  OK	";// << x << "    " << y << "	" << t << endl;
		return true;
	}
	else
	{
		cout << "Send ask frame data error!" << endl;
		return false;
	}

}

bool CSerialPort::SendAnswerTypeFrame(SerialPortType _SerialPortType)
{
	if (this == NULL)
	{
		cout << "send data error" << endl;
		return false;
	}

	unsigned char SerialPortSendData[8] = { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	//添加帧头
	SerialPortSendData[1] = AnswerTypeFrame << 4;

	//添加数据
	SerialPortSendData[1] = SerialPortSendData[1] + _SerialPortType;

	if (this->WriteData(SerialPortSendData, 8))
	{
		//std::cout << "send  OK	";// << x << "    " << y << "	" << t << endl;
		return true;
	}
	else
	{
		cout << "Send answer frame data error!" << endl;
		return false;
	}

}


bool CSerialPort::SendResetFrame()
{
	if (this == NULL)
	{
		cout << "send data error" << endl;
		return false;
	}


	unsigned char SerialPortSendData[8] = { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	//添加帧头
	SerialPortSendData[1] = ResetFrame << 4;

	if (this->WriteData(SerialPortSendData, 8))
	{
		//std::cout << "send  OK	";// << x << "    " << y << "	" << t << endl;
		return true;
	}
	else
	{
		cout << "Send answer frame data error!" << endl;
		return false;
	}
}


bool CSerialPort::SendTellPosFrame(short x, short y)
{
	if (this == NULL)
	{
		cout << "send data error" << endl;
		return false;
	}


	unsigned char SerialPortSendData[8] = { 0xFF, 0x00, 0, 0, 0, 0, 0, 0 };

	//添加帧头
	SerialPortSendData[1] = TellPosFrame << 4;

	//先发地位，再发高位
	SerialPortSendData[2] = ((unsigned char)(x));
	SerialPortSendData[3] = ((short)(x)) >> 8;

	SerialPortSendData[4] = ((unsigned char)((y)));
	SerialPortSendData[5] = ((short)((y))) >> 8;

	if (this->WriteData(SerialPortSendData, 8))
	{
		//std::cout << "send  OK	";// << x << "    " << y << "	" << t << endl;
		return true;
	}
	else
	{
		cout << "Send data error!" << endl;

		//初始化串口
		if (this->InitPort(this->portNum, 115200))
		{
			cout << "重新初始化串口成功" << endl;
		}
		else
		{
			cout << "重新初始化串口失败" << endl;
		}

		return false;
	}
}


bool CSerialPort::SendToMovingFrame(bool MeOrYou, short x, short y,short t)//0 B车接 1A车接
{
	if (this == NULL)
	{
		cout << "send data error NULL" << endl;
		return false;
	}


	unsigned char SerialPortSendData[8] = { 0xFF, 0x00, 0, 0, 0, 0, 0, 0 };

	//添加帧头
	SerialPortSendData[1] = ToMoveFrame << 4;

	SerialPortSendData[1] += MeOrYou;

	//先发地位，再发高位
	SerialPortSendData[2] = ((unsigned char)(x));
	SerialPortSendData[3] = ((short)(x)) >> 8;

	SerialPortSendData[4] = ((unsigned char)((y)));
	SerialPortSendData[5] = ((short)((y))) >> 8;

	SerialPortSendData[6] = ((unsigned char)((t)));
	SerialPortSendData[7] = ((short)((t))) >> 8;

	if (this->WriteData(SerialPortSendData, 8))
	{
		//std::cout << "send  OK	";// << x << "    " << y << "	" << t << endl;
		return true;
	}
	else
	{
		cout << "Send data error!  **** " << endl;
		return false;
	}
}


//0表示收到帧的这台车 拒绝    1表示收到这个帧的车接球
bool CSerialPort::SendDecisionFrame(bool _GET_NO, short x, short y, ushort t)
{
	if (this == NULL)
	{
		cout << "send data error" << endl;
		return false;
	}


	unsigned char SerialPortSendData[8] = { 0xFF, 0x00, 0, 0, 0, 0, 0, 0 };

	//添加帧头
	SerialPortSendData[1] = DecisionFrame << 4;
	SerialPortSendData[1] += _GET_NO ;

	//先发地位，再发高位
	SerialPortSendData[2] = ((unsigned char)(x));
	SerialPortSendData[3] = ((short)(x)) >> 8;

	SerialPortSendData[4] = ((unsigned char)((y)));
	SerialPortSendData[5] = ((short)((y))) >> 8;

	//先发地位，再发高位
	SerialPortSendData[6] = ((unsigned char)(t));
	SerialPortSendData[7] = ((short)(t)) >> 8;

	if (this->WriteData(SerialPortSendData, 8))
	{
		//std::cout << "send  OK	";// << x << "    " << y << "	" << t << endl;
		return true;
	}
	else
	{
		cout << "Send data error!" << endl;
		return false;
	}
}