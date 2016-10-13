/////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, ���пƼ���ѧtickTick Group  ����Ȩ������
/// All rights reserved.
/// 
/// @file    SerialPort.cpp  
/// @brief   ����ͨ�����ʵ���ļ�
///
/// ���ļ�Ϊ����ͨ�����ʵ�ִ���
///
/// @version 1.0   
/// @author  ¬��  
/// @E-mail��lujun.hust@gmail.com
/// @date    2010/03/19
/// 
///
///  �޶�˵����
//////////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "SerialPort.h"
#include <process.h>
#include <iostream>
#include "Parameter.h"
#include "KinectSleep.h"

using namespace std;


const double R_Avoid = 2000;

#define Mysquare(x) ((x)*(x))


/** �߳��˳���־ */
bool CSerialPort::s_bExit = false;
/** ������������ʱ,sleep���´β�ѯ�����ʱ��,��λ:�� */
const UINT SLEEP_TIME_INTERVAL = 1;



unsigned int cal_crc(unsigned char *ptr, unsigned char len)
{
	unsigned int crc;
	uchar i;
	crc = 0;
	while (len-- != 0) {
		for (i = 0x80; i != 0; i /= 2) {
			if ((crc & 0x8000) != 0){
				crc *= 2; crc ^= 0x1021;
			}
			else crc *= 2;
			if ((*ptr&i) != 0)
				crc ^= 0x1021;
		}
		ptr++;
	}
	return(crc);
}



CSerialPort::CSerialPort(void)
:
m_hListenThread(INVALID_HANDLE_VALUE),
m_hSendDataThread(INVALID_HANDLE_VALUE)
{

	InitSuccess = false;

	m_hComm = INVALID_HANDLE_VALUE;
	m_hListenThread = INVALID_HANDLE_VALUE;
	m_hSendDataThread = INVALID_HANDLE_VALUE;

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

	/** ��ʱ����,���ƶ�����ת��Ϊ�ַ�����ʽ,�Թ���DCB�ṹ */
	char szDCBparam[50];
	sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);

	/** ��ָ������,�ú����ڲ��Ѿ����ٽ�������,�����벻Ҫ�ӱ��� */
	if (!openPort(portNo))
	{
		return false;
	}

	/** �����ٽ�� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �Ƿ��д����� */
	BOOL bIsSuccess = TRUE;

	/** �ڴ˿���������������Ļ�������С,���������,��ϵͳ������Ĭ��ֵ.
	*  �Լ����û�������Сʱ,Ҫע�������Դ�һЩ,���⻺�������
	*/
	/*if (bIsSuccess )
	{
	bIsSuccess = SetupComm(m_hComm,10,10);
	}*/

	/** ���ô��ڵĳ�ʱʱ��,����Ϊ0,��ʾ��ʹ�ó�ʱ���� */
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
		// ��ANSI�ַ���ת��ΪUNICODE�ַ���
		DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, NULL, 0);
		wchar_t *pwText = new wchar_t[dwNum];
		if (!MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, pwText, dwNum))
		{
			bIsSuccess = TRUE;
		}

		/** ��ȡ��ǰ�������ò���,���ҹ��촮��DCB���� */
		bIsSuccess = GetCommState(m_hComm, &dcb) && BuildCommDCB(pwText, &dcb);
		/** ����RTS flow���� */
		dcb.fRtsControl = RTS_CONTROL_ENABLE;

		/** �ͷ��ڴ�ռ� */
		delete[] pwText;
	}

	if (bIsSuccess)
	{
		/** ʹ��DCB�������ô���״̬ */
		bIsSuccess = SetCommState(m_hComm, &dcb);
	}

	/**  ��մ��ڻ����� */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/** �뿪�ٽ�� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return bIsSuccess == TRUE;
}

bool CSerialPort::InitPort(UINT portNo, const LPDCB& plDCB)
{
	/** ��ָ������,�ú����ڲ��Ѿ����ٽ�������,�����벻Ҫ�ӱ��� */
	if (!openPort(portNo))
	{
		return false;
	}

	/** �����ٽ�� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** ���ô��ڲ��� */
	if (!SetCommState(m_hComm, plDCB))
	{
		return false;
	}

	/**  ��մ��ڻ����� */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/** �뿪�ٽ�� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

void CSerialPort::ClosePort()
{
	/** ����д��ڱ��򿪣��ر��� */
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}
}

bool CSerialPort::openPort(UINT portNo)
{
	/** �����ٽ�� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �Ѵ��ڵı��ת��Ϊ�豸�� */
	char szPort[50];
	sprintf_s(szPort, "COM%d", portNo);

	/** ��ָ���Ĵ��� */
	m_hComm = CreateFileA(szPort,		                /** �豸��,COM1,COM2�� */
		GENERIC_READ | GENERIC_WRITE,  /** ����ģʽ,��ͬʱ��д */
		0,                             /** ����ģʽ,0��ʾ������ */
		NULL,							/** ��ȫ������,һ��ʹ��NULL */
		OPEN_EXISTING,					/** �ò�����ʾ�豸�������,���򴴽�ʧ�� */
		0,
		0);

	/** �����ʧ�ܣ��ͷ���Դ������ */
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		LeaveCriticalSection(&m_csCommunicationSync);
		return false;
	}

	/** �˳��ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

bool CSerialPort::OpenListenThread()
{
	/** ����߳��Ƿ��Ѿ������� */
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** �߳��Ѿ����� */
		return false;
	}

	s_bExit = false;
	/** �߳�ID */
	UINT threadId;
	/** �����������ݼ����߳� */
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);
	if (!m_hListenThread)
	{
		return false;
	}
	/** �����̵߳����ȼ�,������ͨ�߳� */
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
		/** ֪ͨ�߳��˳� */
		s_bExit = true;

		/** �ȴ��߳��˳� */
		Sleep(10);

		/** ���߳̾����Ч */
		CloseHandle(m_hListenThread);
		m_hListenThread = INVALID_HANDLE_VALUE;
	}
	return true;
}

UINT CSerialPort::GetBytesInCOM()
{
	DWORD dwError = 0;	/** ������ */
	COMSTAT  comstat;   /** COMSTAT�ṹ��,��¼ͨ���豸��״̬��Ϣ */
	memset(&comstat, 0, sizeof(COMSTAT));

	UINT BytesInQue = 0;
	/** �ڵ���ReadFile��WriteFile֮ǰ,ͨ�������������ǰ�����Ĵ����־ */
	if (ClearCommError(m_hComm, &dwError, &comstat))
	{
		BytesInQue = comstat.cbInQue; /** ��ȡ�����뻺�����е��ֽ��� */
	}

	return BytesInQue;
}

UINT WINAPI CSerialPort::ListenThread(void* pParam)
{
	//������ڣ�������̲߳���ʹ��static������
	/** �õ������ָ�� */
	CSerialPort *pSerialPort = reinterpret_cast<CSerialPort*>(pParam);

	std::cout << "Open thread to receive data from TH, Sleep time " << SLEEP_TIME_INTERVAL << " ms" << std::endl;

	pSerialPort->Counter = 0;

	pSerialPort->Vx = 0;
	pSerialPort->Vy = 0;

	pSerialPort->x = 0;
	pSerialPort->y = 0;


	// �߳�ѭ��,��ѯ��ʽ��ȡ��������
	while (!pSerialPort->s_bExit)
	{
		UINT BytesInQue = pSerialPort->GetBytesInCOM();
		/** ����������뻺������������,����Ϣһ���ٲ�ѯ */
		if (BytesInQue == 0)
		{
			Sleep(SLEEP_TIME_INTERVAL);

			//һ�����ڻῪһ���߳�
			//cout << "*" << endl;
			//Sleep(10000);

			continue;
		}

		/** ��ȡ���뻺�����е����ݲ������ʾ */
		char cRecved = 0x00;
		do
		{
			cRecved = 0x00;
			if (pSerialPort->ReadChar(cRecved) == true)
			{
				if ((0) == pSerialPort->Counter)
				{
					if ((char)0xFF == cRecved)//0xEE��windows���� 0x000000EE
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


				if (pSerialPort->Counter == 9)
				{
					if (pSerialPort->m_thReceiveDataUnion.charData[8] != (uchar)(cal_crc(pSerialPort->m_thReceiveDataUnion.charData, 8) & 0xff))
					{
						pSerialPort->Counter = 0;
						continue;
					}



					double ThisTime = GETTIME();


					pSerialPort->Counter = 0;

					//������־λ
					FramwType TypeOfFrame = (FramwType)((pSerialPort->m_thReceiveDataUnion.charData[1] & 0xf0) >> 4);


					switch (TypeOfFrame)
					{
					case TellPosConditionFrame://ֻ��kinect���Է�
					{
												   //cout << "TellThePosFrame";

												   //����B״̬,������ڷ�����λ��־  �򲩷�BGetheBall��˵���ڷ��򣬷���û���ڷ���״̬
												   if (BGetheBall == (StateOfAB)((pSerialPort->m_thReceiveDataUnion.charData[1] & 0x0f)))
												   {
													   m_StateOfAB = BGetheBall;
													   GeStateOfABTime = ThisTime;
												   }


												   pSerialPort->Vx = ((double)(pSerialPort->m_thReceiveDataUnion.shortData[1] - pSerialPort->x)) / (ThisTime - pSerialPort->GetPositionTime);
												   pSerialPort->Vy = ((double)(pSerialPort->m_thReceiveDataUnion.shortData[2] - pSerialPort->y)) / (ThisTime - pSerialPort->GetPositionTime);
												   pSerialPort->x = pSerialPort->m_thReceiveDataUnion.shortData[1];
												   pSerialPort->y = pSerialPort->m_thReceiveDataUnion.shortData[2];
												   pSerialPort->GetPositionTime = ThisTime;

												   //cout << "x" << pSerialPort->x << "y" << pSerialPort->y << endl;

					}
						break;



					case AimPosFrame:
					{	//�����ǽ���

										if (!CAN.isNeedTH && pSerialPort->m_SerialPortType == THTheroVision)
										{
											break;
										}


										if (CAN.isFirstPlayBall && (m_ActionType <= GetTheBallTH))
										{
											//cout << "TH ע�⣡��ԽҪ�ӵ�һ��������" << endl;
											break;
										}


										//������Ϣ
										AimPosFrameInfo AimPosFrameInfoTemp = (AimPosFrameInfo)(pSerialPort->m_thReceiveDataUnion.charData[1] & 0x0f);

										switch (AimPosFrameInfoTemp)
										{

										case AGet_HighBall:
										case AGet_LowBall:
										{
															 //cout << "AGet" << endl;
															 //�ݴ�
															 if (BGetheBall == m_StateOfAB) break;


															 /******************************************************/
															 //�Ƕ�ֻ�ܵ�һ��
															 //ֻ��˫Ŀ���ܵ��Ƕ�
															 if (AGet_HighBall == AimPosFrameInfoTemp&&m_StateOfAB == AllWait&&m_ActionType <= GetTheBallTH&&THTheroVision == pSerialPort->m_SerialPortType)
															 {
																/* cout << "TH����" << endl;
																 KinectEnable = 5;
																 if (KinectAngle < 69 || KinectAngle >71)
																 {
																	 KinectAngle = 70;
																	 cout << "70" << endl;
																	 CAN.Send_RobotHolder(0x03, KinectAngle, 0);
																 }*/

															 }

															 if (AGet_LowBall == AimPosFrameInfoTemp&&m_StateOfAB == AllWait&&m_ActionType <= GetTheBallTH&&THTheroVision == pSerialPort->m_SerialPortType)
															 {
																/* cout << "TH����" << endl;
																 KinectEnable = 5;

																 if (KinectAngle < 54 || KinectAngle >56)
																 {
																	 KinectAngle = 55;
																	 CAN.Send_RobotHolder(0x03, KinectAngle, 0);
																 }*/
															 }




															 /******************************************************/
															 if (AllWait == m_StateOfAB)//��ռ״̬
															 {
																 cout << "��ռA" << endl;
																 m_StateOfAB = AGetheBall;
															 }


															 GeStateOfABTime = ThisTime;



															 if (AGetheBall == m_StateOfAB)//�ȴ�״̬���Ա�����
															 {
																 //��һ̨kinect����
																 if (pSerialPort->m_SerialPortType == KinectCar && (m_ActionType <= GetTheBallKinect))
																 {
																	 //С״̬ά��
																	 GetActionTypeTime = ThisTime;
																	 m_ActionType = GetTheBallKinect;


																	 short tempX = CAN.m_robotState.robot_xNow;
																	 short tempY = CAN.m_robotState.robot_yNow;

																	 if (abs(pSerialPort->m_thReceiveDataUnion.shortData[1] - tempX) > 1000) tempX = pSerialPort->m_thReceiveDataUnion.shortData[1];
																	 if (abs(pSerialPort->m_thReceiveDataUnion.shortData[2] - tempY) > 500) tempY = pSerialPort->m_thReceiveDataUnion.shortData[2];


																	 if (abs(pSerialPort->m_thReceiveDataUnion.shortData[1] - CAN.m_robotState.robot_xNow) > 1500
																		 || abs(pSerialPort->m_thReceiveDataUnion.shortData[2] - CAN.m_robotState.robot_yNow) > 500)
																	 {
																		 cout << "Another Car ����" << tempX << "	" << tempY << "	" << pSerialPort->m_thReceiveDataUnion.shortData[3] << endl;

																		/* if (tempY ==0)
																		 {
																			 Sleep(1);
																		 }*/

																		 //CAN.Send_RobotPosition_Frame(0x01, tempX, tempY, pSerialPort->m_thReceiveDataUnion.shortData[3]);
																	 }
																 }

																 //˫Ŀ����
																 if (pSerialPort->m_SerialPortType == THTheroVision && (m_ActionType <= GetTheBallTH))
																 {
																	 //С״̬ά��
																	 GetActionTypeTime = ThisTime;
																	 m_ActionType = GetTheBallTH;

																	 /*	 short tempX = CAN.m_robotState.robot_xNow;
																		  short tempY = CAN.m_robotState.robot_yNow;

																		  if (abs(pSerialPort->m_thReceiveDataUnion.shortData[1] - tempX) > 1500) tempX = pSerialPort->m_thReceiveDataUnion.shortData[1];
																		  if (abs(pSerialPort->m_thReceiveDataUnion.shortData[2] - tempY) > 500) tempY = pSerialPort->m_thReceiveDataUnion.shortData[2];


																		  if (abs(pSerialPort->m_thReceiveDataUnion.shortData[1] - CAN.m_robotState.robot_xNow) > 1500
																		  || abs(pSerialPort->m_thReceiveDataUnion.shortData[2] - CAN.m_robotState.robot_yNow) > 500)
																		  {
																		  cout << "TH����" << tempX << "	" << tempY << "	" << pSerialPort->m_thReceiveDataUnion.shortData[3] << endl;
																		  CAN.Send_RobotPosition_Frame(0x01, tempX, tempY, pSerialPort->m_thReceiveDataUnion.shortData[3]);
																		  }*/


																	 double R_temp = sqrt(Mysquare(CAN.m_robotState.robot_xNow - pSerialPort->m_thReceiveDataUnion.shortData[1]) + Mysquare(CAN.m_robotState.robot_yNow - pSerialPort->m_thReceiveDataUnion.shortData[2]));

																	 //Ŀ��λ�þ������1.2m����
																	 if (R_temp > 1200 && R_temp < 2500)
																	 {
																		 //����1mʱ������Ŀ��λ��1mʱͣ��
																		 double K = 1200.0 / R_temp;

																		 short aimX = K*(CAN.m_robotState.robot_xNow - pSerialPort->m_thReceiveDataUnion.shortData[1]) + pSerialPort->m_thReceiveDataUnion.shortData[1];
																		 short ainY = K*(CAN.m_robotState.robot_yNow - pSerialPort->m_thReceiveDataUnion.shortData[2]) + pSerialPort->m_thReceiveDataUnion.shortData[2];

																		 cout << "TH����" << aimX << "	" << ainY << "	" << pSerialPort->m_thReceiveDataUnion.shortData[3] << endl;
																		 CAN.Send_RobotPosition_Frame(0x01, aimX, ainY, pSerialPort->m_thReceiveDataUnion.shortData[3]);
																	 }
																 }

																 //�Լ�����ʱ������

															 }
										}
											break;





										case BGet_HighBall:
										case BGet_LowBall:
										{
															 //cout << "BGet" << endl;
															 if (AGetheBall == m_StateOfAB) break;

															 if (AllWait == m_StateOfAB)//��ռ״̬
															 {
																 cout << "��ռB" << endl;
																 m_StateOfAB = BGetheBall;
															 }


															 GeStateOfABTime = ThisTime;
															 GetActionTypeTime = ThisTime;


															 //TH�����µĶ��
															 if (pSerialPort->m_SerialPortType == THTheroVision)
															 {
																 if (m_ActionType == AvoidCarKinect)
																 {
																	 break;
																 }
																 else
																 {
																	 m_ActionType = AvoidCarTH;
																 }

															 }


															 if (pSerialPort->m_SerialPortType == KinectCar)
															 {
																 m_ActionType = AvoidCarKinect;
															 }




															 ////��ܷ�ʽ1
															 //double TempX = (CAN.m_robotState.robot_xNow - KinectSerialPort.x);
															 //double TempY = (CAN.m_robotState.robot_yNow - KinectSerialPort.y);

															 //double A = (pSerialPort->m_thReceiveDataUnion.shortData[1] - KinectSerialPort.x) *TempX + (pSerialPort->m_thReceiveDataUnion.shortData[2] - KinectSerialPort.y) *TempY;
															 //double B = Mysquare(pSerialPort->m_thReceiveDataUnion.shortData[1] - KinectSerialPort.x) + Mysquare(pSerialPort->m_thReceiveDataUnion.shortData[2] - KinectSerialPort.y);
															 //double C = Mysquare(TempX) + Mysquare(TempY);

															 ////�㵽ֱ�ߵľ���
															 //double Theta111 = acos(A / sqrt(B*C));

															 //double S = sqrt(TempX*TempX + TempY*TempY);

															 //double Distance = S*sin(Theta111);

															 //if (Distance < R_Avoid)
															 //{
															 // double minL = (KinectSerialPort.x>pSerialPort->m_thReceiveDataUnion.shortData[1]) ? (pSerialPort->m_thReceiveDataUnion.shortData[1]) : (KinectSerialPort.x);
															 // minL = minL - R_Avoid;
															 // double maxL = (KinectSerialPort.x < pSerialPort->m_thReceiveDataUnion.shortData[1]) ? (pSerialPort->m_thReceiveDataUnion.shortData[1]) : (KinectSerialPort.x);
															 // maxL = maxL + R_Avoid;

															 // if ((CAN.m_robotState.robot_xNow - minL)*(CAN.m_robotState.robot_xNow - maxL) < 0)
															 // {
															 //	 double TempVx = -(pSerialPort->m_thReceiveDataUnion.shortData[2] - KinectSerialPort.y);
															 //	 double TempVy = (pSerialPort->m_thReceiveDataUnion.shortData[1] - KinectSerialPort.x);


															 //	 if ((TempVx*TempX + TempVy*TempY) < 0)
															 //	 {
															 //		 TempVx = -TempVx;
															 //		 TempVy = -TempVy;
															 //	 }


															 //	 double AddS = R_Avoid - Distance;
															 //	 double K = sqrt(AddS*AddS / (TempVx*TempVx + TempVy*TempVy));

															 //	 double addX = K* TempVx;
															 //	 double addY = K* TempVy;



															 //	 if (CAN.m_robotState.robot_yNow + addY > 0)//̫������������
															 //	 {
															 //		 //��������
															 //		 cout << "��������" << endl;

															 //		 TempVx = -TempVx;
															 //		 TempVy = -TempVy;

															 //		 AddS = R_Avoid - Distance;
															 //		 K = sqrt(AddS*AddS / (TempVx*TempVx + TempVy*TempVy));

															 //		 addX = K* TempVx;
															 //		 addY = K* TempVy;
															 //	 }



															 //	 cout << "��� ResetFrame" << CAN.m_robotState.robot_xNow + addX << "	" << CAN.m_robotState.robot_yNow + addY << endl;
															 //	 CAN.Send_RobotPositionReset_Frame(0x01, CAN.m_robotState.robot_xNow + addX, CAN.m_robotState.robot_yNow + addY, 1200);
															 // }
															 //}






															 //////��ܷ�ʽ2
															 //double VectorX = pSerialPort->m_thReceiveDataUnion.shortData[1] - KinectSerialPort.x;
															 //double VectorY = pSerialPort->m_thReceiveDataUnion.shortData[2] - KinectSerialPort.y;

															 //double VectortempX = -VectorY;
															 //double VectortempY = VectorX;
															 //double R_temp = sqrt(Mysquare(VectortempX) + Mysquare(VectortempY));

															 //double addX = 1500.0 / (R_temp)* (VectortempX);
															 //double addY = 1500.0 / (R_temp)* (VectortempY);



															 //double Point11X = KinectSerialPort.x + abs(addX);
															 //double Point11Y = KinectSerialPort.y + abs(addY);

															 //double Point12X = KinectSerialPort.x - abs(addX);
															 //double Point12Y = KinectSerialPort.y - abs(addY);

															 //double Point21X = pSerialPort->m_thReceiveDataUnion.shortData[1] + abs(addX);
															 //double Point21Y = pSerialPort->m_thReceiveDataUnion.shortData[2] + abs(addY);

															 //double Point22X = pSerialPort->m_thReceiveDataUnion.shortData[1] - abs(addX);
															 //double Point22Y = pSerialPort->m_thReceiveDataUnion.shortData[2] - abs(addY);


															 //double chaotiaoX = 1500.0 / R_temp *(VectorX)+pSerialPort->m_thReceiveDataUnion.shortData[1];
															 //double chaotiaoY = 1500.0 / R_temp *(VectorY)+pSerialPort->m_thReceiveDataUnion.shortData[2];

															 //double Point31X = chaotiaoX + abs(addX);
															 //double Point31Y = chaotiaoY + abs(addY);

															 //double Point32X = chaotiaoX - abs(addX);
															 //double Point32Y = chaotiaoY - abs(addY);



															 //if (CAN.m_robotState.robot_yNow < ((Point12Y - Point11Y) / (Point12X - Point11X)* (CAN.m_robotState.robot_xNow - Point11X)) + Point11Y
															 // &&CAN.m_robotState.robot_yNow > ((Point32Y - Point31Y) / (Point32X - Point31X)* (CAN.m_robotState.robot_xNow - Point31X)) + Point31Y
															 // &&CAN.m_robotState.robot_yNow < ((Point11Y - Point31Y) / (Point11X - Point31X)* (CAN.m_robotState.robot_xNow - Point11X)) + Point11Y
															 // &&CAN.m_robotState.robot_yNow < ((Point12Y - Point32Y) / (Point12X - Point32X)* (CAN.m_robotState.robot_xNow - Point12X)) + Point12Y
															 // )
															 //{
															 // //��Ҫ���

															 // double TempX = (CAN.m_robotState.robot_xNow - KinectSerialPort.x);
															 // double TempY = (CAN.m_robotState.robot_yNow - KinectSerialPort.y);

															 // double A = (pSerialPort->m_thReceiveDataUnion.shortData[1] - KinectSerialPort.x) *TempX + (pSerialPort->m_thReceiveDataUnion.shortData[2] - KinectSerialPort.y) *TempY;
															 // double B = Mysquare(pSerialPort->m_thReceiveDataUnion.shortData[1] - KinectSerialPort.x) + Mysquare(pSerialPort->m_thReceiveDataUnion.shortData[2] - KinectSerialPort.y);
															 // double C = Mysquare(TempX) + Mysquare(TempY);

															 // //�㵽ֱ�ߵľ���
															 // double Theta111 = acos(A / sqrt(B*C));

															 // double S = sqrt(TempX*TempX + TempY*TempY);

															 // double Distance = S*sin(Theta111);

															 //
															 // double TempVx = -(pSerialPort->m_thReceiveDataUnion.shortData[2] - KinectSerialPort.y);
															 // double TempVy = (pSerialPort->m_thReceiveDataUnion.shortData[1] - KinectSerialPort.x);


															 // if ((TempVx*TempX + TempVy*TempY) < 0)
															 // {
															 //	 TempVx = -TempVx;
															 //	 TempVy = -TempVy;
															 // }


															 // double AddS = R_Avoid - Distance;
															 // double K = sqrt(AddS*AddS / (TempVx*TempVx + TempVy*TempVy));

															 // double addX = K* TempVx;
															 // double addY = K* TempVy;



															 // if (CAN.m_robotState.robot_yNow + addY > 0)//̫������������
															 // {
															 //	 //��������
															 //	 cout << "��������" << endl;

															 //	 TempVx = -TempVx;
															 //	 TempVy = -TempVy;

															 //	 AddS = R_Avoid - Distance;
															 //	 K = sqrt(AddS*AddS / (TempVx*TempVx + TempVy*TempVy));

															 //	 addX = K* TempVx;
															 //	 addY = K* TempVy;
															 // }


															 // cout << "��� ResetFrame" << CAN.m_robotState.robot_xNow + addX << "	" << CAN.m_robotState.robot_yNow + addY << endl;
															 // CAN.Send_RobotPositionReset_Frame(0x01, CAN.m_robotState.robot_xNow + addX, CAN.m_robotState.robot_yNow + addY, 1200);

															 //}





															 //��ܷ�ʽ3
															 if (!((KinectSerialPort.x - pSerialPort->m_thReceiveDataUnion.shortData[1])*(KinectSerialPort.x - CAN.m_robotState.robot_xNow) < 0 
																 && (KinectSerialPort.y - pSerialPort->m_thReceiveDataUnion.shortData[2])*(KinectSerialPort.y - CAN.m_robotState.robot_yNow) < 0))//A��Զ��B���������
															 {
																 double BaimX;
																 double BaimY;



																 //��ܺ��� B����A��
																 //��ܷ�ʽ1
																 double TempX = (CAN.m_robotState.robot_xNow - KinectSerialPort.x);
																 double TempY = (CAN.m_robotState.robot_yNow - KinectSerialPort.y);

																 double A = (pSerialPort->m_thReceiveDataUnion.shortData[1] - KinectSerialPort.x) *TempX + (pSerialPort->m_thReceiveDataUnion.shortData[2] - KinectSerialPort.y) *TempY;
																 double B = Mysquare(pSerialPort->m_thReceiveDataUnion.shortData[1] - KinectSerialPort.x) + Mysquare(pSerialPort->m_thReceiveDataUnion.shortData[2] - KinectSerialPort.y);
																 double C = Mysquare(TempX) + Mysquare(TempY);

																 //�㵽ֱ�ߵľ���
																 double Theta111 = acos(A / sqrt(B*C));
																 double S = sqrt(TempX*TempX + TempY*TempY);
																 double Distance = S*sin(Theta111);

																 if (Distance < R_Avoid)
																 {

																	 double zhongjianPointX;
																	 double zhongjianPointY;

#define FENDUAN_NUM 8 

																	 //��������ֱ�ߵĵ�ľ���̫Զ����Ҫ���
																	 for (int i = 0; i <= FENDUAN_NUM; i++)
																	 {
																		 zhongjianPointX = KinectSerialPort.x + i*(pSerialPort->m_thReceiveDataUnion.shortData[1] - KinectSerialPort.x) / FENDUAN_NUM;
																		 zhongjianPointY = KinectSerialPort.y + i*(pSerialPort->m_thReceiveDataUnion.shortData[2] - KinectSerialPort.y) / FENDUAN_NUM;

																		 if (sqrt(Mysquare(zhongjianPointX - CAN.m_robotState.robot_xNow) + Mysquare(zhongjianPointY - CAN.m_robotState.robot_yNow)) < R_Avoid)
																		 {
																			 double TempVx = -(pSerialPort->m_thReceiveDataUnion.shortData[2] - KinectSerialPort.y);
																			 double TempVy = (pSerialPort->m_thReceiveDataUnion.shortData[1] - KinectSerialPort.x);


																			 if ((TempVx*TempX + TempVy*TempY) < 0)
																			 {
																				 TempVx = -TempVx;
																				 TempVy = -TempVy;
																			 }


																			 double AddS = R_Avoid - Distance;
																			 double K = sqrt(AddS*AddS / (TempVx*TempVx + TempVy*TempVy));

																			 double addX = K* TempVx;
																			 double addY = K* TempVy;



																			 if (CAN.m_robotState.robot_yNow + addY > 600)//̫������������
																			 {

																				 if ((KinectSerialPort.x - CAN.m_robotState.robot_xNow)*(KinectSerialPort.x - pSerialPort->m_thReceiveDataUnion.shortData[1]) < 0)
																				 {
																					 BaimX = KinectSerialPort.x - (abs(pSerialPort->m_thReceiveDataUnion.shortData[1] - KinectSerialPort.x) / (pSerialPort->m_thReceiveDataUnion.shortData[1] - KinectSerialPort.x)) * R_Avoid;
																				 }
																				 else
																				 {
																					 BaimX = pSerialPort->m_thReceiveDataUnion.shortData[1] + (abs(pSerialPort->m_thReceiveDataUnion.shortData[1] - KinectSerialPort.x) / (pSerialPort->m_thReceiveDataUnion.shortData[1] - KinectSerialPort.x)) * R_Avoid;
																				 }



																				 BaimY = 600;

																				 if (BaimX>3020)
																				 {
																					 BaimX = 3020;
																				 }


																				 if (BaimX<-3020)
																				 {
																					 BaimX = (-3020);
																				 }


																			 }
																			 else
																			 {

																				 cout << "��� ";

																				 BaimX = CAN.m_robotState.robot_xNow + addX;
																				 BaimY = CAN.m_robotState.robot_yNow + addY;
																			 }


																		



																			 cout << "BaimX	" << BaimX << "	BaimY	" << BaimY << endl;
																			// CAN.Send_RobotPositionReset_Frame(0x01, BaimX, BaimY, pSerialPort->m_thReceiveDataUnion.shortData[2]);

																			 CAN.Send_RobotPositionReset_Frame(0x01, BaimX, BaimY, pSerialPort->m_thReceiveDataUnion.shortData[2]);


																			 break;

																		 }

																	 }


																 }

															 }

										}
											break;




										default:
											cout << "AimPosFrame �������Ϣ" << endl;
											break;

										}


					}

						GeStateOfABTime = ThisTime;//��״̬ά��
						//cout << "״̬ά��" << endl;
						break;







					case ResetFrame:
						m_ActionType = Wait;
						m_StateOfAB = AllWait;
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
						gkout << "�����֡" << endl;
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

	/** �ٽ������� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �ӻ�������ȡһ���ֽڵ����� */
	bResult = ReadFile(m_hComm, &cRecved, 1, &BytesRead, NULL);
	if ((!bResult))
	{
		/** ��ȡ������,���Ը��ݸô�����������ԭ�� */
		DWORD dwError = GetLastError();

		/** ��մ��ڻ����� */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/** �뿪�ٽ��� */
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

	/** �ٽ������� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �򻺳���д��ָ���������� */
	bResult = WriteFile(m_hComm, pData, length, &BytesToSend, NULL);
	if (!bResult)
	{
		DWORD dwError = GetLastError();
		/** ��մ��ڻ����� */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/** �뿪�ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}





/************************�Լ�����ĺ���****************************************/
bool CSerialPort::OpenSendThread()
{

	/** ����߳��Ƿ��Ѿ������� */
	if (m_hSendDataThread != INVALID_HANDLE_VALUE)
	{
		/** �߳��Ѿ����� */
		return false;
	}



	/** �߳�ID */
	UINT threadId;
	/** �����������ݼ����߳� */
	m_hSendDataThread = (HANDLE)_beginthreadex(NULL, 0, SendThread, this, 0, &threadId);
	if (!m_hSendDataThread)
	{
		return false;
	}


	/** �����̵߳����ȼ�,������ͨ�߳� */
	if (!SetThreadPriority(m_hSendDataThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return false;
	}

	cout << "Open Send Data Thread success��" << endl;

	return true;
}

UINT WINAPI CSerialPort::SendThread(void* pParam)
{

	//������ڣ�������̲߳���ʹ��static������
	/** �õ������ָ�� */
	CSerialPort *pSerialPort = reinterpret_cast<CSerialPort*>(pParam);

	// �߳�ѭ��,��ѯ��ʽ��ȡ��������
	while (!pSerialPort->s_bExit)
	{
		while (!pSerialPort->SendData[pSerialPort->SendListTail][0])
		{
			Sleep(2);
			//cout << "�ȴ�����" << endl;
		}


		if (pSerialPort->WriteData(&pSerialPort->SendData[pSerialPort->SendListTail][1], 9))
		{

		}
		else
		{
			cout << "Send data error!" << endl;
		}


		//cout << "Send data" << pSerialPort->SendListTail << endl;

		pSerialPort->SendData[pSerialPort->SendListTail][0] = 0;


		pSerialPort->SendListTail++;

		if (pSerialPort->SendListTail == SendDataListNum) pSerialPort->SendListTail = 0;

		Sleep(1);

	}


	return 0;
}

bool CSerialPort::AddDataToSend(FramwType _FramwType, unsigned char Infor, short x, short y, short t)
{
	if (this == NULL)
	{
		cout << "*Seriport == NULL" << endl;
		return false;
	}


	unsigned char SendData[9];

	SendData[0] = 0xFF;
	SendData[1] = _FramwType << 4;
	SendData[1] += Infor;



	//�ȷ���λ���ٷ���λ
	SendData[2] = ((unsigned char)(x));
	SendData[3] = ((short)(x)) >> 8;

	SendData[4] = ((unsigned char)((y)));
	SendData[5] = ((short)((y))) >> 8;

	SendData[6] = ((unsigned char)((t)));
	SendData[7] = ((short)((t))) >> 8;



	SendData[8] = (unsigned char)(cal_crc(SendData, 8) & 0xff);



	while (this->SendData[this->SendListHead][0])
	{
		Sleep(1);
		cout << "����ӵ��" << endl;
	}


	//���������λ��־λ
	//�ӵ�0�Ǳ�־λ
	memcpy(&this->SendData[this->SendListHead][1], SendData, 9);


	//cout << "add data" << this->SendListHead << endl;


	//��λ��־λ
	this->SendData[this->SendListHead][0] = 1;


	this->SendListHead++;
	if (this->SendListHead == SendDataListNum) this->SendListHead = 0;

}
