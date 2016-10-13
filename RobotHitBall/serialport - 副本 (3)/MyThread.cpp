//#include"stdafx.h"

#include"MyThread.h"
#include "Parameter.h"


#define KeepConditionTime 200 //100msû��״̬����֡��״̬��Ϊreset


CSerialPort KinectSerialPort;
CSerialPort THSerialPort;

StateOfAB m_StateOfAB;//��״̬
double GeStateOfABTime;

ActionType m_ActionType;//С״̬
double GetActionTypeTime;

int R = 2000;




extern HUST_CAN CAN;

extern int  RESETX;
extern int  RESETY;


DWORD WINAPI InitTHSerialPort(LPVOID p_lpVoid)
{
	cout << "��ʼ��THSerialPort      7" << endl;
	while (!KinectSerialPort.InitPort(7, 115200))
	{
		cout << "��ʼ������ THSerialPort ʧ��" << endl;
		Sleep(5000);
	}

	return 0;
}

DWORD WINAPI ContrlOfABCarThread(LPVOID p_lpVoid)
{
	cout << "��ʼ��KinectSerialPort      6"<<endl;
	while (!KinectSerialPort.InitPort(6, 115200))
	{
		cout << "��ʼ������ KinectSerialPort ʧ��" << endl;
		Sleep(5000);

		//���ʹ���û�гɹ�����
	}
	
	CreateThread(NULL, NULL, InitTHSerialPort,NULL,NULL,NULL);


	double Time;

	//ֻ�ܸ�˫Ŀ��B������λ��
	while (1)
	{
		Time = GETTIME();

		Sleep(5);

		//���߻���������
		if (Time - KinectSerialPort.GetPositionTime < KeepConditionTime)
		{
			//����
			if (m_StateOfAB == AllWait || m_StateOfAB == BGetheBall)
			{

				double TempX = (CAN.m_robotState.robot_xNow - KinectSerialPort.x);
				double TempY = (CAN.m_robotState.robot_yNow - KinectSerialPort.y);

				//�㵽ֱ�ߵľ���
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
			//������ߴ�����Ϣ
			//���������guess���õ���
			KinectSerialPort.InitSuccess = false;
		}

		


		//Ҫд������
		if (Time - GetActionTypeTime > KeepConditionTime&&m_ActionType != Wait)
		{
			cout << "��λС״̬" << endl;
			//��λС״̬
			m_ActionType = Wait;
			CAN.Send_RobotPositionReset_Frame(0x01, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow,1200);
		}


		//Ҫд������
		if (Time - GeStateOfABTime > KeepConditionTime&&m_StateOfAB != AllWait)
		{
			cout << "��λ��״̬" << endl;
			//��λ��״̬
			m_StateOfAB = AllWait;
			CAN.Send_RobotPositionReset_Frame(0x01, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow, 1200);
		}


		//��˫Ŀ���͵���״̬λ��
		THSerialPort.AddDataToSend(TellPosConditionFrame, m_StateOfAB, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow, 0);

		Sleep(5);//��ʱ

		KinectSerialPort.AddDataToSend(TellPosConditionFrame, m_StateOfAB, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow, 0);

	}


	return 0;

}
