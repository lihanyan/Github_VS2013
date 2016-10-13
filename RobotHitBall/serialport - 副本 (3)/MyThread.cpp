//#include"stdafx.h"

#include"MyThread.h"
#include "Parameter.h"


#define KeepConditionTime 200 //100ms没有状态保持帧，状态置为reset


CSerialPort KinectSerialPort;
CSerialPort THSerialPort;

StateOfAB m_StateOfAB;//大状态
double GeStateOfABTime;

ActionType m_ActionType;//小状态
double GetActionTypeTime;

int R = 2000;




extern HUST_CAN CAN;

extern int  RESETX;
extern int  RESETY;


DWORD WINAPI InitTHSerialPort(LPVOID p_lpVoid)
{
	cout << "初始化THSerialPort      7" << endl;
	while (!KinectSerialPort.InitPort(7, 115200))
	{
		cout << "初始化串口 THSerialPort 失败" << endl;
		Sleep(5000);
	}

	return 0;
}

DWORD WINAPI ContrlOfABCarThread(LPVOID p_lpVoid)
{
	cout << "初始化KinectSerialPort      6"<<endl;
	while (!KinectSerialPort.InitPort(6, 115200))
	{
		cout << "初始化串口 KinectSerialPort 失败" << endl;
		Sleep(5000);

		//发送串口没有成功机制
	}
	
	CreateThread(NULL, NULL, InitTHSerialPort,NULL,NULL,NULL);


	double Time;

	//只管给双目，B车发送位置
	while (1)
	{
		Time = GETTIME();

		Sleep(5);

		//决策机正常运行
		if (Time - KinectSerialPort.GetPositionTime < KeepConditionTime)
		{
			//避让
			if (m_StateOfAB == AllWait || m_StateOfAB == BGetheBall)
			{

				double TempX = (CAN.m_robotState.robot_xNow - KinectSerialPort.x);
				double TempY = (CAN.m_robotState.robot_yNow - KinectSerialPort.y);

				//点到直线的距离
				double Theta111 = acos((KinectSerialPort.Vx *TempX + KinectSerialPort.Vy *TempY) / sqrt((KinectSerialPort.Vx*KinectSerialPort.Vx + KinectSerialPort.Vy * KinectSerialPort.Vy)*(TempX*TempX + TempY*TempY)));

				double S = sqrt(TempX*TempX + TempY*TempY);

				double Distance = S*sin(Theta111);

				if (Distance < 1500)
				{


					double TempVx = -KinectSerialPort.Vy;
					double TempVy = KinectSerialPort.Vx;
					if ((TempVx*TempX + TempVy*TempY) < 0)
					{
						TempVx = -TempVx;
						TempVy = -TempVy;
					}


					double AddS = 1500 - Distance;
					double K = sqrt(AddS*AddS / (TempVx*TempVx + TempVy*TempVy));

					double addX = K* TempVx;
					double addY = K* TempVy;

					CAN.Send_RobotPositionReset_Frame(0x01, CAN.m_robotState.robot_xNow + addX, CAN.m_robotState.robot_yNow + addY, 1200);
				}



			}
		}
		else
		{
			//输出决策错误信息
			//这个条件在guess中用到了
			KinectSerialPort.InitSuccess = false;
		}

		


		//要写在外面
		if (Time - GetActionTypeTime > KeepConditionTime&&m_ActionType != Wait)
		{
			cout << "置位小状态" << endl;
			//置位小状态
			m_ActionType = Wait;
			CAN.Send_RobotPositionReset_Frame(0x01, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow,1200);
		}


		//要写在外面
		if (Time - GeStateOfABTime > KeepConditionTime&&m_StateOfAB != AllWait)
		{
			cout << "置位大状态" << endl;
			//置位大状态
			m_StateOfAB = AllWait;
			CAN.Send_RobotPositionReset_Frame(0x01, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow, 1200);
		}


		//给双目发送底盘状态位置
		THSerialPort.AddDataToSend(TellPosConditionFrame, m_StateOfAB, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow, 0);

		Sleep(5);//延时

		KinectSerialPort.AddDataToSend(TellPosConditionFrame, m_StateOfAB, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow, 0);

	}


	return 0;

}
