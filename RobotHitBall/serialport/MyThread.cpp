//#include"stdafx.h"

#include"MyThread.h"
#include "Parameter.h"


#define KeepConditionTime 500 //100ms没有状态保持帧，状态置为reset


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

	int SeriPortNum = 6;

	THSerialPort.m_SerialPortType = THTheroVision;

	THSerialPort.InitSuccess = false;

	cout << "THSerialPort      初始化" << SeriPortNum << endl;

	if (THSerialPort.InitPort(SeriPortNum, 115200))
	{
		cout << "THSerialPort 初始化成功！" << endl;

		if (THSerialPort.OpenListenThread())
		{
			cout << "THSerialPort 打开监听线程成功！" << endl;

			if (THSerialPort.OpenSendThread())
			{
				cout << "THSerialPort 打开发送线程成功！" << endl;
			}
			else
			{
				cout << "THSerialPort 打开发送线程失败**********************！" << endl;
			}
		}
		else
		{
			cout << "THSerialPort 打开监听线程失败*******************！" << endl;
		}

	}
	else
	{
		cout << "THSerialPort 初始化失败******************！" << endl;
	}

	THSerialPort.InitSuccess = true;

	return 0;
}

DWORD WINAPI ContrlOfABCarThread(LPVOID p_lpVoid)
{
	KinectSerialPort.InitSuccess = false;



	cout << "初始化KinectSerialPort      7"<<endl;
	while (!KinectSerialPort.InitPort(7, 115200))
	{
		cout << "KinectSerialPort 初始化串口  失败***************" << endl;
		Sleep(5000);

		//发送串口没有成功机制
	}

	cout << "初始化串口成功" << endl;

	while (!KinectSerialPort.OpenListenThread())
	{
		cout << "KinectSerialPort打开监听线程失败**************" << endl;
		Sleep(5000);

		//发送串口没有成功机制
	}


	while (!KinectSerialPort.OpenSendThread())
	{
		cout << "KinectSerialPort打开发送线程失败***************！" << endl;
		Sleep(5000);

		//发送串口没有成功机制
	}




	KinectSerialPort.m_SerialPortType = KinectCar;


	//初始化与双目连接串口
	CreateThread(NULL, NULL, InitTHSerialPort,NULL,NULL,NULL);


	double Time;

	//只管给双目，B车发送位置
	while (1)
	{
		Sleep(15);

		Time = GETTIME();

		//决策机正常运行
		if (Time - KinectSerialPort.GetPositionTime < KeepConditionTime)
		{
			KinectSerialPort.InitSuccess = true;

			//决策没有挂的情况下发送小旗子状态
			//发送小旗子状态
			if (m_StateOfAB == AGetheBall)
			{
				CAN.Send_RobotAttackState(0x01, 1);
			}
			else
			{
				CAN.Send_RobotAttackState(0x01, 2);
			}



			////在2m范围内才开始躲
			//if (
			//	((KinectSerialPort.x - CAN.m_robotState.robot_xNow)*(KinectSerialPort.x - CAN.m_robotState.robot_xNow) +
			//	(KinectSerialPort.y - CAN.m_robotState.robot_yNow)*(KinectSerialPort.y - CAN.m_robotState.robot_yNow)) < 2000 * 2000
			//	)
			//{

			//	if (KinectSerialPort.Vx != 0 || KinectSerialPort.Vy != 0)
			//	{
			//		//避让
			//		if (m_StateOfAB == BGetheBall   || m_StateOfAB == AllWait)
			//		{

			//			double TempX = (CAN.m_robotState.robot_xNow - KinectSerialPort.x);
			//			double TempY = (CAN.m_robotState.robot_yNow - KinectSerialPort.y);

			//			//点到直线的距离
			//			double Theta111 = acos((KinectSerialPort.Vx *TempX + KinectSerialPort.Vy *TempY) / sqrt((KinectSerialPort.Vx*KinectSerialPort.Vx + KinectSerialPort.Vy * KinectSerialPort.Vy)*(TempX*TempX + TempY*TempY)));

			//			double S = sqrt(TempX*TempX + TempY*TempY);

			//			double Distance = S*sin(Theta111);

			//			if (Distance < 1500)
			//			{


			//				double TempVx = -KinectSerialPort.Vy;
			//				double TempVy = KinectSerialPort.Vx;
			//				if ((TempVx*TempX + TempVy*TempY) < 0)
			//				{
			//					TempVx = -TempVx;
			//					TempVy = -TempVy;
			//				}


			//				double AddS = 1500 - Distance;
			//				double K = sqrt(AddS*AddS / (TempVx*TempVx + TempVy*TempVy));

			//				double addX = K* TempVx;
			//				double addY = K* TempVy;


			//				cout << "躲避 ResetFrame" << endl;
			//				CAN.Send_RobotPositionReset_Frame(0x01, CAN.m_robotState.robot_xNow + addX, CAN.m_robotState.robot_yNow + addY, 1200);
			//			}



			//		}
			//	}
			//}
			
		}
		else
		{
			//输出决策错误信息
			//这个条件在guess中用到了
			KinectSerialPort.InitSuccess = false;

			//Sleep(100);

			//决策挂了发送小旗子状态
			//发送小旗子状态
			CAN.Send_RobotAttackState(0x01, 0);

			//cout << "决策机挂了" << endl;
		}

		


		////要写在外面
		//if (Time - GetActionTypeTime > KeepConditionTime&&m_ActionType != Wait)
		//{
		//	//置位小状态
		//	m_ActionType = Wait;
		//	//CAN.Send_RobotPositionReset_Frame(0x01, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow,1200);

		//	playBallLog << "置位小状态" << CAN.m_robotState.robot_xNow << "	" << CAN.m_robotState.robot_yNow << endl;

		//	cout<< "置位小状态" << CAN.m_robotState.robot_xNow << "	" << CAN.m_robotState.robot_yNow << endl;
		//}


		//要写在外面
		if (Time - GeStateOfABTime > KeepConditionTime&&m_StateOfAB != AllWait )
		{
			//置位大状态
			m_StateOfAB = AllWait;

			//置位小状态
			m_ActionType = Wait;


			if (!CAN.isFirstPlayBall)
			{
				cout << "置位状态Send_RobotPositionReset_Frame" << CAN.m_robotState.robot_xNow << "	" << CAN.m_robotState.robot_yNow << endl;
				CAN.Send_RobotPositionReset_Frame(0x01, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow, 1200);
			}

			/*cout << "置位仰角" << endl;
			KinectAngle = 55.0;
			CAN.Send_RobotHolder(0x03, KinectAngle, 0);*/

		}


		//给双目发送底盘状态位置
		THSerialPort.AddDataToSend(TellPosConditionFrame, m_StateOfAB, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow, 0);


		Sleep(15);//延时

		KinectSerialPort.AddDataToSend(TellPosConditionFrame, m_StateOfAB, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow, 0);



		//接第一个球判断
		if (CAN.isFirstPlayBall)
		{
			m_StateOfAB = AGetheBall;
			GeStateOfABTime = Time;

		}

	}


	return 0;

}
