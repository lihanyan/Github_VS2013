//#include"stdafx.h"

#include"MyThread.h"
#include "Parameter.h"


#define KeepConditionTime 500 //100msû��״̬����֡��״̬��Ϊreset


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

	int SeriPortNum = 6;

	THSerialPort.m_SerialPortType = THTheroVision;

	THSerialPort.InitSuccess = false;

	cout << "THSerialPort      ��ʼ��" << SeriPortNum << endl;

	if (THSerialPort.InitPort(SeriPortNum, 115200))
	{
		cout << "THSerialPort ��ʼ���ɹ���" << endl;

		if (THSerialPort.OpenListenThread())
		{
			cout << "THSerialPort �򿪼����̳߳ɹ���" << endl;

			if (THSerialPort.OpenSendThread())
			{
				cout << "THSerialPort �򿪷����̳߳ɹ���" << endl;
			}
			else
			{
				cout << "THSerialPort �򿪷����߳�ʧ��**********************��" << endl;
			}
		}
		else
		{
			cout << "THSerialPort �򿪼����߳�ʧ��*******************��" << endl;
		}

	}
	else
	{
		cout << "THSerialPort ��ʼ��ʧ��******************��" << endl;
	}

	THSerialPort.InitSuccess = true;

	return 0;
}

DWORD WINAPI ContrlOfABCarThread(LPVOID p_lpVoid)
{
	KinectSerialPort.InitSuccess = false;



	cout << "��ʼ��KinectSerialPort      7"<<endl;
	while (!KinectSerialPort.InitPort(7, 115200))
	{
		cout << "KinectSerialPort ��ʼ������  ʧ��***************" << endl;
		Sleep(5000);

		//���ʹ���û�гɹ�����
	}

	cout << "��ʼ�����ڳɹ�" << endl;

	while (!KinectSerialPort.OpenListenThread())
	{
		cout << "KinectSerialPort�򿪼����߳�ʧ��**************" << endl;
		Sleep(5000);

		//���ʹ���û�гɹ�����
	}


	while (!KinectSerialPort.OpenSendThread())
	{
		cout << "KinectSerialPort�򿪷����߳�ʧ��***************��" << endl;
		Sleep(5000);

		//���ʹ���û�гɹ�����
	}




	KinectSerialPort.m_SerialPortType = KinectCar;


	//��ʼ����˫Ŀ���Ӵ���
	CreateThread(NULL, NULL, InitTHSerialPort,NULL,NULL,NULL);


	double Time;

	//ֻ�ܸ�˫Ŀ��B������λ��
	while (1)
	{
		Sleep(15);

		Time = GETTIME();

		//���߻���������
		if (Time - KinectSerialPort.GetPositionTime < KeepConditionTime)
		{
			KinectSerialPort.InitSuccess = true;

			//����û�йҵ�����·���С����״̬
			//����С����״̬
			if (m_StateOfAB == AGetheBall)
			{
				CAN.Send_RobotAttackState(0x01, 1);
			}
			else
			{
				CAN.Send_RobotAttackState(0x01, 2);
			}



			////��2m��Χ�ڲſ�ʼ��
			//if (
			//	((KinectSerialPort.x - CAN.m_robotState.robot_xNow)*(KinectSerialPort.x - CAN.m_robotState.robot_xNow) +
			//	(KinectSerialPort.y - CAN.m_robotState.robot_yNow)*(KinectSerialPort.y - CAN.m_robotState.robot_yNow)) < 2000 * 2000
			//	)
			//{

			//	if (KinectSerialPort.Vx != 0 || KinectSerialPort.Vy != 0)
			//	{
			//		//����
			//		if (m_StateOfAB == BGetheBall   || m_StateOfAB == AllWait)
			//		{

			//			double TempX = (CAN.m_robotState.robot_xNow - KinectSerialPort.x);
			//			double TempY = (CAN.m_robotState.robot_yNow - KinectSerialPort.y);

			//			//�㵽ֱ�ߵľ���
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


			//				cout << "��� ResetFrame" << endl;
			//				CAN.Send_RobotPositionReset_Frame(0x01, CAN.m_robotState.robot_xNow + addX, CAN.m_robotState.robot_yNow + addY, 1200);
			//			}



			//		}
			//	}
			//}
			
		}
		else
		{
			//������ߴ�����Ϣ
			//���������guess���õ���
			KinectSerialPort.InitSuccess = false;

			//Sleep(100);

			//���߹��˷���С����״̬
			//����С����״̬
			CAN.Send_RobotAttackState(0x01, 0);

			//cout << "���߻�����" << endl;
		}

		


		////Ҫд������
		//if (Time - GetActionTypeTime > KeepConditionTime&&m_ActionType != Wait)
		//{
		//	//��λС״̬
		//	m_ActionType = Wait;
		//	//CAN.Send_RobotPositionReset_Frame(0x01, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow,1200);

		//	playBallLog << "��λС״̬" << CAN.m_robotState.robot_xNow << "	" << CAN.m_robotState.robot_yNow << endl;

		//	cout<< "��λС״̬" << CAN.m_robotState.robot_xNow << "	" << CAN.m_robotState.robot_yNow << endl;
		//}


		//Ҫд������
		if (Time - GeStateOfABTime > KeepConditionTime&&m_StateOfAB != AllWait )
		{
			//��λ��״̬
			m_StateOfAB = AllWait;

			//��λС״̬
			m_ActionType = Wait;


			if (!CAN.isFirstPlayBall)
			{
				cout << "��λ״̬Send_RobotPositionReset_Frame" << CAN.m_robotState.robot_xNow << "	" << CAN.m_robotState.robot_yNow << endl;
				CAN.Send_RobotPositionReset_Frame(0x01, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow, 1200);
			}

			/*cout << "��λ����" << endl;
			KinectAngle = 55.0;
			CAN.Send_RobotHolder(0x03, KinectAngle, 0);*/

		}


		//��˫Ŀ���͵���״̬λ��
		THSerialPort.AddDataToSend(TellPosConditionFrame, m_StateOfAB, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow, 0);


		Sleep(15);//��ʱ

		KinectSerialPort.AddDataToSend(TellPosConditionFrame, m_StateOfAB, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow, 0);



		//�ӵ�һ�����ж�
		if (CAN.isFirstPlayBall)
		{
			m_StateOfAB = AGetheBall;
			GeStateOfABTime = Time;

		}

	}


	return 0;

}
