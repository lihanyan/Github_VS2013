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





/** 线程退出标志 */
bool CSerialPort::s_bExit = false;
/** 当串口无数据时,sleep至下次查询间隔的时间,单位:秒 */
const UINT SLEEP_TIME_INTERVAL = 1;


CSerialPort::CSerialPort(void)
	: m_hListenThread(INVALID_HANDLE_VALUE)
{

	InitSuccess = false;

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
					double ThisTime = GETTIME();


					pSerialPort->Counter = 0;

					//解析标志位
					FramwType TypeOfFrame = (FramwType)((pSerialPort->m_thReceiveDataUnion.charData[1] & 0xf0) >> 4);



					switch (TypeOfFrame)
					{


					case TellPosConditionFrame://只有kinect可以发
					{
						//cout << "TellThePosFrame";

						//解析A状态
						//m_StateOfAB = (StateOfAB)((pSerialPort->m_thReceiveDataUnion.charData[1] & 0x0f));


						pSerialPort->Vx = ((double)(pSerialPort->m_thReceiveDataUnion.shortData[1] - pSerialPort->x)) / (ThisTime - pSerialPort->GetPositionTime);
						pSerialPort->Vy = ((double)(pSerialPort->m_thReceiveDataUnion.shortData[2] - pSerialPort->y)) / (ThisTime - pSerialPort->GetPositionTime);
						pSerialPort->x = pSerialPort->m_thReceiveDataUnion.shortData[1];
						pSerialPort->y = pSerialPort->m_thReceiveDataUnion.shortData[2];
						pSerialPort->GetPositionTime = ThisTime;

						cout << "x" << pSerialPort->x << "y" << pSerialPort->y << endl;

					}
					break;



					case SeeTheBallFrame:
						//标志位抢占
						if (m_StateOfAB == AllWait)
						{
							//1 外部裁定A接
							if ((pSerialPort->m_thReceiveDataUnion.charData[1] & 0x0f))
							{
								m_StateOfAB = AGetheBall;
							}
							else//0 外部裁定B接
							{
								m_StateOfAB = BGetheBall;
							}
						}


						if (m_StateOfAB == Wait || m_StateOfAB == AGetheBall)//等待状态可以被控制
						{
							//另一台kinect控制
							if (pSerialPort->m_SerialPortType == KinectCar && (m_ActionType <= GetTheBallKinect))
							{
								m_ActionType = GetTheBallKinect;

								short tempX = CAN.m_robotState.robot_xNow;
								short tempY = CAN.m_robotState.robot_yNow;

								if (abs(pSerialPort->m_thReceiveDataUnion.shortData[1] - tempX) > 500) tempX = pSerialPort->m_thReceiveDataUnion.shortData[1];
								if (abs(pSerialPort->m_thReceiveDataUnion.shortData[2] - tempY) > 500) tempY = pSerialPort->m_thReceiveDataUnion.shortData[2];

								CAN.Send_RobotPosition_Frame(0x01, tempX, tempY, pSerialPort->m_thReceiveDataUnion.shortData[3]);

							}


							//双目控制
							if (pSerialPort->m_SerialPortType == THTheroVision && (m_ActionType <= GetTheBallTH))
							{
								m_ActionType = GetTheBallTH;

								short tempX = CAN.m_robotState.robot_xNow;
								short tempY = CAN.m_robotState.robot_yNow;

								if (abs(pSerialPort->m_thReceiveDataUnion.shortData[1] - tempX) > 500) tempX = pSerialPort->m_thReceiveDataUnion.shortData[1];
								if (abs(pSerialPort->m_thReceiveDataUnion.shortData[2] - tempY) > 500) tempY = pSerialPort->m_thReceiveDataUnion.shortData[2];

								CAN.Send_RobotPosition_Frame(0x01, tempX, tempY, pSerialPort->m_thReceiveDataUnion.shortData[3]);

							}

							//自己控制时，跳出

						}
						break;

					case ResetFrame:
						m_ActionType = Wait;
						//pSerialPort->m_CAN->detectEnable = true;
						//pSerialPort->m_Me->m_CAN->Send_RobotPositionReset_Frame(1, RESETX, RESETY, 1200);
						gkout << "ResetFrame" << endl;
						break;



					case AskTypeFrame:
						gkout << "AskTypeFrame" << endl;
						pSerialPort->AddDataToSend(AnswerTypeFrame, (unsigned char)pSerialPort->m_SerialPortType, 0, 0, 0);
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
bool CSerialPort::CreatSendThread()
{
	return true;
}

UINT WINAPI CSerialPort::SendThread(void* pParam)
{
	return 0;
}

bool CSerialPort::AddDataToSend(FramwType _FramwType, unsigned char Infor, short x, short y, short t)
{
	//int TempNum = this->SendListHead;
	//int tempH = 0;

	//TempNum++;

	//if (TempNum == SendDataListNum)
	//{
	//	tempH = 1;
	//	TempNum = 0;
	//}

	//this->SendData[TempNum][0] = 0xFF;
	//this->SendData[TempNum][1] = _FramwType << 4;
	//this->SendData[TempNum][1] += Infor;

	////先发地位，再发高位
	//this->SendData[TempNum][2] = ((unsigned char)(x));
	//this->SendData[TempNum][3] = ((short)(x)) >> 8;

	//this->SendData[TempNum][4] = ((unsigned char)((y)));
	//this->SendData[TempNum][5] = ((short)((y))) >> 8;

	//this->SendData[TempNum][6] = ((unsigned char)((t)));
	//this->SendData[TempNum][7] = ((short)((t))) >> 8;

	//this->SendListHeadHigh;

	if (this == NULL)
	{
		cout << "*Seriport == NULL" << endl;
		return false;
	}


	unsigned char SendData[10];

	SendData[0] = 0xFF;
	SendData[1] = _FramwType << 4;
	SendData[1] += Infor;

	//先发地位，再发高位
	SendData[2] = ((unsigned char)(x));
	SendData[3] = ((short)(x)) >> 8;

	SendData[4] = ((unsigned char)((y)));
	SendData[5] = ((short)((y))) >> 8;

	SendData[6] = ((unsigned char)((t)));
	SendData[7] = ((short)((t))) >> 8;

	if(this->WriteData(SendData, 10))
	{
		return true;
	}
	else
	{
		cout << "Send data error!" << endl;
		return false;
	}

}