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
#include "ReverseTheta.h"
#include "SerialPort.h"
#include <process.h>
#include <iostream>

using namespace std;


int AnotherCarX = -2000;
int AnotherCarY = 0;





/** �߳��˳���־ */
bool CSerialPort::s_bExit = false;
/** ������������ʱ,sleep���´β�ѯ�����ʱ��,��λ:�� */
const UINT SLEEP_TIME_INTERVAL = 1;


CSerialPort::CSerialPort(void)
	: m_hListenThread(INVALID_HANDLE_VALUE)
{

	//������Ŀ���ǣ���������ʱ��LAsend��־λΪtrue������Ҫ���
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
	sprintf_s(szPort, "\\\\.\\COM%d", portNo);

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

	pSerialPort->Counter = -2;

	pSerialPort->Vx = 0;
	pSerialPort->Vy = 0;

	pSerialPort->x = 0;
	pSerialPort->y = 0;


	static int numPos = 0;
	int temp;

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


				if (pSerialPort->Counter == 8)
				{
					pSerialPort->Counter = 0;

					//������־λ
					FramwType TypeOfFrame = (FramwType)((pSerialPort->m_thReceiveDataUnion.charData[1] & 0xf0) >> 4);



					switch (TypeOfFrame)
					{


					case TellPosFrame://ֻ��kinect���Է�
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



					case ToMoveFrame://ֻ��kinect���ܷ���
					{
						gkout << "ToMoveFrame" << endl;

						if (pSerialPort->m_Me->m_ActionType == Wait || pSerialPort->m_Me->m_ActionType == Moving)//˫Ŀ�ľ�����û��׷�ӽ���
						{
							pSerialPort->m_Me->m_ActionType = Moving;

							pSerialPort->m_Me->GetActionTypeTime = GETTIME();

							//���͵���λ��,

							gkout << "����KInectָ��λ��" << endl;

							//����ҪԤ����
							//pSerialPort->m_CAN->Send_RobotPosition_Frame(0x01, pSerialPort->m_thReceiveDataUnion.shortData[1], pSerialPort->m_thReceiveDataUnion.shortData[2], pSerialPort->m_thReceiveDataUnion.shortData[3]);

							//DipanAimx = pSerialPort->m_thReceiveDataUnion.shortData[1];
							//DipanAimy = pSerialPort->m_thReceiveDataUnion.shortData[2];
						}
					}
					break;




					case DecisionFrame://ֻ��˫Ŀ���ܷ���
					{
						//cout << "DecisionFrame" << endl;


						//��������
						bool Gettemp = (pSerialPort->m_thReceiveDataUnion.charData[1] & 0x0f);


						if (Gettemp)
						{
							gkout << "Get the ball" << endl;

							if (pSerialPort->m_Me->m_ActionType == GetTheBallLA)//LA��ʼ�س�,˫Ŀ����
							{
								break;
							}
							else
							{
								//ͬ����̨�����򣬵ȴ�LA�س�
								pSerialPort->m_Me->m_ActionType = GetTheBallTH;
								pSerialPort->m_Me->GetActionTypeTime = GETTIME();

								//DipanAimx = pSerialPort->m_thReceiveDataUnion.shortData[1];
								//DipanAimy = pSerialPort->m_thReceiveDataUnion.shortData[2];


								gkout << "����˫Ŀλ��" << pSerialPort->m_thReceiveDataUnion.shortData[1] << "	" << pSerialPort->m_thReceiveDataUnion.shortData[2] << endl;
								//���͵���λ��
								//����ҪԤ����
								//pSerialPort->m_CAN->Send_RobotPosition_Frame(0x01, pSerialPort->m_thReceiveDataUnion.shortData[1], pSerialPort->m_thReceiveDataUnion.shortData[2], pSerialPort->m_thReceiveDataUnion.shortData[3]);
							}
						}
						else
						{

							gkout << "Avoid The Car" << endl;

							//�����̨������ֱ�ӷ��
							//˫Ŀ����ֱ�ӷ����̨���Ľ��򣬽�����
							pSerialPort->m_Me->m_ActionType = AvoidTheCar;
							pSerialPort->m_Me->m_CAN->detectEnable = false;
							pSerialPort->m_Me->GetActionTypeTime = GETTIME();
						}
					}
					break;


					case ResetFrame:
						pSerialPort->m_Me->m_ActionType = Wait;
						pSerialPort->m_Me->m_CAN->detectEnable = true;
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


					case LAPoseFrame:
						
						temp = GetYawSpeed(pSerialPort->m_thReceiveDataUnion.shortData[1], pSerialPort->m_thReceiveDataUnion.shortData[2], CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow);
						//cout << "LAPose" << pSerialPort->m_thReceiveDataUnion.shortData[1] << "	" << pSerialPort->m_thReceiveDataUnion.shortData[2] << endl;
						yawThresholdLA[numPos][0] = (temp & 0xFF);
						yawThresholdLA[numPos][1] = (temp >> 8 & 0xFF);
						playBallSpeed = (temp >> 16 & 0xFF) * 100;
						//cout << "jiexi" << "		" << (int)yawThresholdLA[numPos][0] << "		" << (int)yawThresholdLA[numPos][1] << "		" << (int)playBallSpeed * 100 << endl;
						numPos++;
						numPos = numPos % 2;
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
bool CSerialPort::SendAskTypeFrame()
{
	if (this == NULL)
	{
		cout << "Send data error" << endl;
		return false;
	}

	//kinect �����ѯ��֡
	unsigned char SerialPortSendData[8] = { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	//���֡ͷ
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
		cout << "Send data error" << endl;
		return false;
	}

	unsigned char SerialPortSendData[8] = { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	//���֡ͷ
	SerialPortSendData[1] = AnswerTypeFrame << 4;

	//�������
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
		cout << "Send data error" << endl;
		return false;
	}


	unsigned char SerialPortSendData[8] = { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	//���֡ͷ
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
		cout << "Send data error" << endl;
		return false;
	}


	unsigned char SerialPortSendData[8] = { 0xFF, 0x00, 0, 0, 0, 0, 0, 0 };

	//���֡ͷ
	SerialPortSendData[1] = TellPosFrame << 4;

	//�ȷ���λ���ٷ���λ
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
		return false;
	}
}


bool CSerialPort::SendToMovingFrame(bool MeOrYou ,short x, short y,short t)//1 A���� 0 B����
{


	if (this == NULL)
	{
		cout << "Send data error" << endl;
		return false;
	}


	unsigned char SerialPortSendData[8] = { 0xFF, 0x00, 0, 0, 0, 0, 0, 0 };

	//���֡ͷ
	SerialPortSendData[1] = ToMoveFrame << 4;

	SerialPortSendData[1] += MeOrYou;

	//�ȷ���λ���ٷ���λ
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
		cout << "Send data error!" << endl;
		return false;
	}
}

bool CSerialPort::SendDecisionFrame(bool _GET_NO, short x, short y, ushort t)
{
	if (this == NULL)
	{
		cout << "Send data error" << endl;
		return false;
	}


	unsigned char SerialPortSendData[8] = { 0xFF, 0x00, 0, 0, 0, 0, 0, 0 };

	//���֡ͷ
	SerialPortSendData[1] = DecisionFrame << 4;
	SerialPortSendData[1] += _GET_NO ;

	//�ȷ���λ���ٷ���λ
	SerialPortSendData[2] = ((unsigned char)(x));
	SerialPortSendData[3] = ((short)(x)) >> 8;

	SerialPortSendData[4] = ((unsigned char)((y)));
	SerialPortSendData[5] = ((short)((y))) >> 8;

	//�ȷ���λ���ٷ���λ
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