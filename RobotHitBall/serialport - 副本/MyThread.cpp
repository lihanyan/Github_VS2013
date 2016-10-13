//#include"stdafx.h"

#include"MyThread.h"
#include"TCPSocket.h"
#include "Parameter.h"


#define KeepConditionTime 200 //100ms没有状态保持帧，状态置为reset kinect的这个时间长点


//两个通讯串口
CSerialPort m_SeriPort[2];

CSerialPort Me;//存放状态

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
	/****************************初始化两个串口************************************/
	if (!_SerialPort->InitPort(SerialPortNum, 115200))
	{
		gkout << "初始化串口失败！" << endl;
		return false;
	}
	else
	{
		if (_SerialPort->OpenListenThread())
		{
			gkout << "打开监听线程成功!" << endl;
			_SerialPort->m_CAN = &CAN;
			_SerialPort->m_SerialPortType = UnKnow;
			_SerialPort->m_Me = &Me;
			return true;
		}
		else
		{
			gkout << "打开监听线程失败！" << endl;
			return false;
		}
	}
}

DWORD WINAPI ContrlOfABCarThread(LPVOID p_lpVoid)
{

	while ((Me.BkinectSerialPort == NULL || Me.THSerialPort == NULL))
	{
		//初始化两个串口+两个串口监听成功
		//开始询问串口类型,这样串口才能相应返回
		while (m_SeriPort[0].m_SerialPortType == UnKnow)
		{
			m_SeriPort[0].SendAskTypeFrame();
			Sleep(500);
			gkout << "询问串口类型0" << endl;
		}
		while (m_SeriPort[1].m_SerialPortType == UnKnow)
		{
			m_SeriPort[1].SendAskTypeFrame();
			Sleep(500);
			gkout << "询问串口类型1" << endl;
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




	gkout << "串口好了" << endl;

	//只管给双目，B车发送位置
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

		//避让
		if (Me.m_ActionType == AvoidTheCar)//|| Me.m_ActionType == Wait)
		{
			////如果要撞车了，向远离车的一侧跑  //远离
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
			////如果要撞车了，向远离车的一侧跑  //远离
			if (abs(AnotherCarX - CAN.m_robotState.robot_xNow)<R)
			{

				//要用reset帧！
				CAN.Send_RobotPositionReset_Frame(0x01, AnotherCarX + R, 0, 1200);
				gkout << ">>>>" << endl;
			}
			

			///**********************************************************************************/
			//////如果要撞车了，向远离车的一侧跑  //远离
			//if (Mysquare(CAN.m_robotState.robot_xNow - AnotherCarX) + Mysquare(CAN.m_robotState.robot_yNow - AnotherCarY) < Mysquare(R + 200))
			//{
			//	CAN.Send_RobotPositionReset_Frame(
			//		0x01,
			//		CAN.m_robotState.robot_xNow + AddS * abs(CAN.m_robotState.robot_xNow - AnotherCarX) / (CAN.m_robotState.robot_xNow - AnotherCarX),
			//		CAN.m_robotState.robot_yNow,
			//		1200);

			//	cout << "Che" << endl;
			//}
			//else//没有撞车，向reset点跑  //靠进
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
			////优先选用reset地点，如果跑到reset位置会撞车，另外选用一个点
			////当前点跑到reset位置的画出的直线，另一台车不应该朝这条直线跑，或则，朝这条直线跑，但是位置不应该小于保护半径
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
			//else//需要另外计算一个非reset的点
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
			//gkout << "正在接球" << endl;
		}





		//给双目发送底盘状态位置
		Me.THSerialPort->SendTellPosFrame(CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow);

		Sleep(5);//延时

		//给B车发送底盘位置
		Me.BkinectSerialPort->SendTellPosFrame(CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow);

	}


	return 0;

}

//这个线程负责发送车的位置
//负责resetc车
//负责清除状态
void ContrlOfABCar()
{
	
	TH_InitSeriPort(&m_SeriPort[0], 6);//TH
	TH_InitSeriPort(&m_SeriPort[1], 7);//B


	//初始化状态
	Me.LAsend = true;
	Me.THsend = true;
	Me.m_ActionType = Wait;
	Me.m_CAN = &CAN;

	Me.m_SerialPortType = KinectCarA;

	



	CreateThread(NULL, NULL, ContrlOfABCarThread, NULL, NULL, NULL);
}