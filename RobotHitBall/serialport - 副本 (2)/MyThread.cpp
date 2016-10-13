//#include"stdafx.h"

#include"MyThread.h"
#include "Parameter.h"


#define KeepConditionTime 200 //100ms没有状态保持帧，状态置为reset


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
		//cout << "初始化串口失败！" << endl;
		gkout << "InitPort fail!" << endl;
		return false;
	}
	else
	{
		if (_SerialPort->OpenListenThread())
		{
			gkout << "OpenListenThread success!" << endl;
			_SerialPort->m_CAN = &CAN;
			_SerialPort->m_SerialPortType = UnKnow;
			_SerialPort->m_Me = &Me;
			_SerialPort->portNum = SerialPortNum;
			return true;
		}
		else
		{
			gkout << "OpenListenThread fail！" << endl;
			return false;
		}
	}
}

DWORD WINAPI ContrlOfABCarThread(LPVOID p_lpVoid)
{

	while (Me.THSerialPort == NULL || Me.BkinectSerialPort ==NULL)
	{
		//初始化两个串口+两个串口监听成功
		//开始询问串口类型,这样串口才能相应返回
		while (m_SeriPort[0].m_SerialPortType == UnKnow)
		{
			m_SeriPort[0].SendAskTypeFrame();
			Sleep(500);
			gkout << "询问串口类型0" << endl;
			cout << "询问串口类型0" << endl;
		}
		while (m_SeriPort[1].m_SerialPortType == UnKnow)
		{
			m_SeriPort[1].SendAskTypeFrame();
			Sleep(500);
			gkout << "询问串口类型1" << endl;
			cout << "询问串口类型1" << endl;
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

	
	gkout << "Init THCom success ！" << endl;
	cout << "Init THCom success ！" << endl;



	//只管给双目，B车发送位置
	while (1)
	{

		Sleep(5);


		//要写在外面
		if (GETTIME() - Me.GetActionTypeTime > KeepConditionTime)
		{
			Me.m_ActionType = Wait;
			//isPlayBall = true;
			//Me.m_CAN->detectEnable = true;
		}

#define Siqu 50
//#define AddS 50


		//避让
		if (Me.m_ActionType == AvoidTheCar || Me.m_ActionType == Wait)
		{


			short TempX;
			short TempY = CAN.m_robotState.robot_yNow;


			if (abs(AnotherCarX - CAN.m_robotState.robot_xNow) < R)
			{
				//靠左边的车向左边走
				TempX = AnotherCarX - abs(AnotherCarX - CAN.m_robotState.robot_xNow) / (AnotherCarX - CAN.m_robotState.robot_xNow)* R;
				//cout << "X" << TempX << endl;
				CAN.Send_RobotPositionReset_Frame(0x01, TempX, TempY, 1200);
			}


			//else
			//{
			//	//没有撞车可能
			//	TempX = RESETX;

			//	//车到位了就不发了
			//	if (abs(TempX - CAN.m_robotState.robot_xNow) > Siqu || abs(TempY - CAN.m_robotState.robot_yNow) > Siqu)
			//	{
			//		CAN.Send_RobotPositionReset_Frame(0x01, TempX, TempY, 1200);
			//	}
			//}






			////如果要撞车了，向远离车的一侧跑  //远离
			//if (Mysquare(CAN.m_robotState.robot_xNow - AnotherCarX) + Mysquare(CAN.m_robotState.robot_yNow - AnotherCarY) < Mysquare(R))
			//{

			//	CAN.Send_RobotPositionReset_Frame(0x01, -2000, 0, 1200);
			//	cout << "Che" << endl;
			//}
			//else
			//{
			//	CAN.Send_RobotPositionReset_Frame(0x01, RESETX, RESETY, 1200);
			//	cout << "Reset" << endl;
			//}




			/*****************************************************************/
			////如果要撞车了，向远离车的一侧跑  //远离
			//if (Mysquare(CAN.m_robotState.robot_xNow - AnotherCarX) + Mysquare(CAN.m_robotState.robot_yNow - AnotherCarY) < Mysquare(R))
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
			//	if (Mysquare(CAN.m_robotState.robot_xNow - RESETX) + Mysquare(CAN.m_robotState.robot_yNow - RESETY) > Mysquare(Siqu))
			//	{
			//		CAN.Send_RobotPositionReset_Frame(
			//			0x01,
			//			CAN.m_robotState.robot_xNow - AddS * abs(CAN.m_robotState.robot_xNow - RESETX) / (CAN.m_robotState.robot_xNow - RESETX),
			//			CAN.m_robotState.robot_yNow - AddS * abs(CAN.m_robotState.robot_yNow - RESETY) / (CAN.m_robotState.robot_yNow - RESETY),
			//			1200);
			//		cout << "Reset" << endl;
			//	}

			//}

			/*****************************************************************/
			////优先选用reset地点，如果跑到reset位置会撞车，另外选用一个点
			////当前点跑到reset位置的画出的直线，另一台车不应该朝这条直线跑，或则，朝这条直线跑，但是位置不应该小于保护半径
			//if (
			//	Mysquare(RESETX - AnotherCarX) + Mysquare(RESETY - AnotherCarY) >  Mysquare(R)
			//	&& Mysquare(CAN.m_robotState.robot_xNow - AnotherCarX) + Mysquare(CAN.m_robotState.robot_yNow - AnotherCarY) >  Mysquare(R)

			//	)
			//{
			//	//DipanAimx = RESETX;
			//	//DipanAimy = RESETY;


			//	//CAN.Send_RobotPosition_Frame(0x01, RESETX, RESETY,600);

			//	CAN.Send_RobotPositionReset_Frame(0x01, RESETX, RESETY, 600);

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
			//cout << "正在接球" << endl;

			//cout << "*" ;
		}


		//给双目发送底盘状态位置
		Me.THSerialPort->SendTellPosFrame(CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow);

		Sleep(5);//延时


#ifdef BCar
		//给A车发送底盘位置
		Me.AkinectSerialPort->SendTellPosFrame(CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow);
#else
		//给B车发送底盘位置
		Me.BkinectSerialPort->SendTellPosFrame(CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow);
#endif



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

	

#ifdef BCar
	cout << "这是B车******************" << endl;
	Me.m_SerialPortType = KinectCarB;
#else
	cout << "这是A车******************" << endl;
	Me.m_SerialPortType = KinectCarA;
#endif

	CreateThread(NULL, NULL, ContrlOfABCarThread, NULL, NULL, NULL);
}