//#include"stdafx.h"

#include"MyThread.h"
#include"TCPSocket.h"
#include "Parameter.h"


#define KeepConditionTime 200 //100msû��״̬����֡��״̬��Ϊreset kinect�����ʱ�䳤��


//����ͨѶ����
CSerialPort m_SeriPort[2];

CSerialPort Me;//���״̬

int R = 2000;

extern HUST_CAN CAN;

extern int  RESETX;
extern int  RESETY;



extern int AnotherCarX;
extern int AnotherCarY;


extern bool isPlayBall;

//extern double DipanAimx;
//extern double DipanAimy;


bool TH_InitSeriPort(CSerialPort *_SerialPort, int SerialPortNum)
{
	/****************************��ʼ����������************************************/
	if (!_SerialPort->InitPort(SerialPortNum, 115200))
	{
		gkout << "��ʼ������ʧ�ܣ�" << endl;
		return false;
	}
	else
	{
		if (_SerialPort->OpenListenThread())
		{
			gkout << "�򿪼����̳߳ɹ�!" << endl;
			_SerialPort->m_CAN = &CAN;
			_SerialPort->m_SerialPortType = UnKnow;
			_SerialPort->m_Me = &Me;
			return true;
		}
		else
		{
			gkout << "�򿪼����߳�ʧ�ܣ�" << endl;
			return false;
		}
	}
}

DWORD WINAPI ContrlOfABCarThread(LPVOID p_lpVoid)
{

	while ((Me.BkinectSerialPort == NULL || Me.THSerialPort == NULL))
	{
		//��ʼ����������+�������ڼ����ɹ�
		//��ʼѯ�ʴ�������,�������ڲ�����Ӧ����
		while (m_SeriPort[0].m_SerialPortType == UnKnow)
		{
			m_SeriPort[0].SendAskTypeFrame();
			Sleep(500);
			gkout << "ѯ�ʴ�������0" << endl;
		}
		while (m_SeriPort[1].m_SerialPortType == UnKnow)
		{
			m_SeriPort[1].SendAskTypeFrame();
			Sleep(500);
			gkout << "ѯ�ʴ�������1" << endl;
		}


		if (m_SeriPort[0].m_SerialPortType == KinectCarA)
		{
			Me.AkinectSerialPort = &m_SeriPort[0];
		}
		if (m_SeriPort[0].m_SerialPortType == KinectCarB)
		{
			Me.BkinectSerialPort = &m_SeriPort[0];
		}
		if (m_SeriPort[0].m_SerialPortType == THTheroVision)
		{
			Me.THSerialPort = &m_SeriPort[0];
		}


		if (m_SeriPort[1].m_SerialPortType == KinectCarA)
		{
			Me.AkinectSerialPort = &m_SeriPort[1];
		}
		if (m_SeriPort[1].m_SerialPortType == KinectCarB)
		{
			Me.BkinectSerialPort = &m_SeriPort[1];
		}
		if (m_SeriPort[1].m_SerialPortType == THTheroVision)
		{
			Me.THSerialPort = &m_SeriPort[1];
		}
	} 




	gkout << "���ں���" << endl;

	//ֻ�ܸ�˫Ŀ��B������λ��
	while (1)
	{

		Sleep(5);

		if (GETTIME() - Me.GetActionTypeTime > KeepConditionTime)
		{
			if (Me.m_ActionType != Wait)
			{
				Me.m_ActionType = Wait;
				gkout << "Time To Long!" << endl;
			}
			
			//isPlayBall = true;
		}


#define Siqu 50
#define AddS 120

		//����
		if (Me.m_ActionType == AvoidTheCar)//|| Me.m_ActionType == Wait)
		{
			////���Ҫײ���ˣ���Զ�복��һ����  //Զ��
			//if (Mysquare(CAN.m_robotState.robot_xNow - AnotherCarX) + Mysquare(CAN.m_robotState.robot_yNow - AnotherCarY) < Mysquare(R))
			//{

			//	CAN.Send_RobotPositionReset_Frame(0x01,2000,0,1200);
			//	cout << "Che" << endl;
			//}
			//else
			//{
			//	CAN.Send_RobotPositionReset_Frame(0x01, RESETX, RESETY, 1200);
			//	cout << "Reset" << endl;
			//}



			/**********************************************************************************/
			////���Ҫײ���ˣ���Զ�복��һ����  //Զ��
			if (abs(AnotherCarX - CAN.m_robotState.robot_xNow)<R)
			{

				//Ҫ��reset֡��
				CAN.Send_RobotPositionReset_Frame(0x01, AnotherCarX + R, 0, 1200);
				gkout << ">>>>" << endl;
			}
			

			///**********************************************************************************/
			//////���Ҫײ���ˣ���Զ�복��һ����  //Զ��
			//if (Mysquare(CAN.m_robotState.robot_xNow - AnotherCarX) + Mysquare(CAN.m_robotState.robot_yNow - AnotherCarY) < Mysquare(R + 200))
			//{
			//	CAN.Send_RobotPositionReset_Frame(
			//		0x01,
			//		CAN.m_robotState.robot_xNow + AddS * abs(CAN.m_robotState.robot_xNow - AnotherCarX) / (CAN.m_robotState.robot_xNow - AnotherCarX),
			//		CAN.m_robotState.robot_yNow,
			//		1200);

			//	cout << "Che" << endl;
			//}
			//else//û��ײ������reset����  //����
			//{
			//	short TempX = CAN.m_robotState.robot_xNow;
			//	short TempY = CAN.m_robotState.robot_yNow;

			//	if (abs(CAN.m_robotState.robot_xNow - RESETX) > Siqu)
			//	{
			//		TempX = CAN.m_robotState.robot_xNow - AddS * abs(CAN.m_robotState.robot_xNow - RESETX) / (CAN.m_robotState.robot_xNow - RESETX);
			//	}

			//	if (abs(CAN.m_robotState.robot_yNow - RESETY) > Siqu)
			//	{
			//		TempY = CAN.m_robotState.robot_yNow - AddS * abs(CAN.m_robotState.robot_yNow - RESETY) / (CAN.m_robotState.robot_yNow - RESETY);
			//	}

			//	CAN.Send_RobotPositionReset_Frame(0x01, TempX, TempY, 1200);

			//	cout << "Reset" << endl;
			//}

			/**********************************************************************************/
			////����ѡ��reset�ص㣬����ܵ�resetλ�û�ײ��������ѡ��һ����
			////��ǰ���ܵ�resetλ�õĻ�����ֱ�ߣ���һ̨����Ӧ�ó�����ֱ���ܣ����򣬳�����ֱ���ܣ�����λ�ò�Ӧ��С�ڱ����뾶
			//if (
			//	Mysquare(RESETX - AnotherCarX) + Mysquare(RESETY - AnotherCarY) >  Mysquare(R)
			//	&& 

			//	)
			//{
			//	//DipanAimx = RESETX;
			//	//DipanAimy = RESETY;


			//	//CAN.Send_RobotPosition_Frame(0x01, RESETX, RESETY,600);
			//	//CAN.Send_RobotPositionReset_Frame(0x01, RESETX, RESETY, 1500);

			//	CAN.Send_RobotPosition_Frame(0x01, RESETX, RESETY, 1500);

			//}
			//else//��Ҫ�������һ����reset�ĵ�
			//{
			//	//cout <<"TH"<< CAN.m_robotState.robot_xNow << "	" << CAN.m_robotState.robot_yNow << "	" << AnotherCarX << "	" << AnotherCarY << "	R	" << sqrt(Mysquare(CAN.m_robotState.robot_xNow - AnotherCarX) + Mysquare(CAN.m_robotState.robot_yNow - AnotherCarY)) << endl;

			//	short DipanAimxTemp = (double)R / sqrt(Mysquare(CAN.m_robotState.robot_xNow - AnotherCarX) + Mysquare(CAN.m_robotState.robot_yNow - AnotherCarY)) *(CAN.m_robotState.robot_xNow - AnotherCarX);
			//	short DipanAimyTemp = (double)R / sqrt(Mysquare(CAN.m_robotState.robot_xNow - AnotherCarX) + Mysquare(CAN.m_robotState.robot_yNow - AnotherCarY)) *(CAN.m_robotState.robot_yNow - AnotherCarY);

			//	//CAN.Send_RobotPosition_Frame(0x01, DipanAimxTemp, DipanAimyTemp, 600);
			//	CAN.Send_RobotPositionReset_Frame(0x01, DipanAimxTemp, DipanAimyTemp, 600);

			//	//cout << "ans	" << DipanAimxTemp << "	" << DipanAimyTemp << endl;
			//}




		}
		else
		{
			//gkout << "���ڽ���" << endl;
		}





		//��˫Ŀ���͵���״̬λ��
		Me.THSerialPort->SendTellPosFrame(CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow);

		Sleep(5);//��ʱ

		//��B�����͵���λ��
		Me.BkinectSerialPort->SendTellPosFrame(CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow);

	}


	return 0;

}

//����̸߳����ͳ���λ��
//����resetc��
//�������״̬
void ContrlOfABCar()
{
	
	TH_InitSeriPort(&m_SeriPort[0], 6);//TH
	TH_InitSeriPort(&m_SeriPort[1], 7);//B


	//��ʼ��״̬
	Me.LAsend = true;
	Me.THsend = true;
	Me.m_ActionType = Wait;
	Me.m_CAN = &CAN;

	Me.m_SerialPortType = KinectCarA;

	



	CreateThread(NULL, NULL, ContrlOfABCarThread, NULL, NULL, NULL);
}