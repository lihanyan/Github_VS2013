#pragma once
#include "RT_CAN.h"
#include "HUST_TYPE.h"

#define ROBOT_CONTROL_DEVICEID 0x01


class HUST_CAN :
	public RT_CAN{
public:
	
	MyCamState* pm_camState = NULL;				//��ǰ�ļ��״̬��ָ�룬���㴫������
	M_robotState m_robotState;
	unsigned char playDir = 1;
	unsigned char detectEnable = 1;
	float resetX = -1200;
	float resetY = -500;
	int m_motorPos;
	int tim1;
	int tim2;
	int time_count;
	bool isConnect = false;
	bool isPowerOff = false;
	char attackScope = 0;
	float* kinectPitch = NULL;
	char* kinectEnable = NULL;
	bool isFirstPlayBall =false;
	bool isFirstPlayBallChujie = false;
	char towerSwitch = 3;

	char isNeedTH = 0;
	


	unsigned char RobotAttackState = 3;//�����ж�״̬�����״̬��ͬ�򲻵���Send_RobotAttackState�ײ�ķ��ͺ�������ʡ��Դ
	int m_KinectAngle = 3;//�����ж�״̬�����״̬��ͬ�򲻵���Send_RobotAttackState�ײ�ķ��ͺ�������ʡ��Դ
	

	HUST_CAN(void);
	~HUST_CAN(void);

	void Send_Test_Frame(unsigned char TargetID, unsigned char LED, unsigned char Times);							//���ܲ���֡���ͺ���

	void Send_MotorCtlMode_Frame(unsigned char TargetID, unsigned char MotorCtlMode);						//�ջ����ͷ��ͺ���

	void Send_MotorCtlParams_Frame(unsigned char TargetID, unsigned int P, unsigned int I);					//���ñջ����Ʋ���

	void Send_MotorSpeedSet_Frame(unsigned char TargetID, int MotorSPD);

	void Send_MotorPositionSet_Frame(unsigned char TargetID, int MotorPOS);
	void Send_MotorPositionSet_Frame(unsigned char TargetID, int MotorPOS, int TimeRemain);			//���ͻ�����λ���Լ�
	void Send_MotorPositionSet_Frame(unsigned char TargetID, short MotorPOS_m4, short MotorPOS_m5, short MotorPOS_m7, short MotorPOS_m8);			//�����ϲ��ĸ����״̬
	bool Send_MotorPositionGet_Frame(unsigned char TargetID);										//���Ŀ������λ��

	void Send_MotorCtlEnable_Frame(unsigned char TargetID, unsigned char MotorEn);							//���ʹ��֡���ͺ���

	void Send_MotorInit_Frame(unsigned char TargetID, int posAim);												//�����ʼ�����ͺ���

	void Send_MotorSpeedTest_Frame(unsigned char TargetID, int SetSpeed);							//�趨���Եĵ���ٶȻ�Ŀ���ٶ�	

	void Send_Jiqiu_Frame(unsigned char TargetID, int ExceptPos, int Reserve);	
	void Send_Jiqiu_Frame(unsigned char TargetID, int ExceptPos, short Speed, short timeRemain);//���ͻ���
	void Send_Jiqiu_Frame(unsigned char TargetID, short HitBallPos, short StopPos, short Speed, short timeRemain);//���ͻ���

	void Send_Faqiu_Frame(unsigned char TargetID, int ExceptPos, int timeRemain);						//���ͷ���
	
	void Send_RobotPosition_Frame(unsigned char TargetID,short Robot_x,short Robot_y,short Robot_time);//����Ŀ��λ��

	void Send_RobotPositionReset_Frame(unsigned char TargetID, short Robot_x, short Robot_y, short Robot_time);

	void Send_RobotAngle_Frame(unsigned char TargetID, float Angle);//����Ŀ����̬

	void Send_RobotRelocalPosition_Frame(unsigned char TargetID, float x_shifting,float y_shifting);

	void Send_RobotRelocalAngle_Frame(unsigned char TargetID, float angle_shifting);

	void Send_CylindControl_Frame(unsigned char TargetID, char CtlMode);	//����A�������׿���

	void Send_RobotAttackState(unsigned char TargetID,unsigned char attackState);//���;�С����״̬



	void Send_MC_Parame_Frame(char TargetID, short p1, short p2, short p3, MC_Parame_Typedef  MC_Parame);//���վ��������Ϣ
	void Send_MC_Parame_Frame(char TargetID, float p1, float p2, float p3, MC_Parame_Typedef  MC_Parame);//���վ��������Ϣ

	void Send_RobotHolder(unsigned char TargetID, float pitch,float yaw);

	void Send_HeartPack(unsigned char TargetID);

	virtual void OnRecvPackage(const BYTE * byBuf, DWORD dwLen);			//CAN�����߳�	

	void Recv_RobotPosition();												//CAN����λ��
	void Recv_RobotState();												//CAN���յ���״̬

	void Recv_RobotAngle();													//CAN����ƫ��

	void Recv_ControlJiqiuMode();											//����ģʽ						

	void Recv_Information();												//CAN����������Ϣ

	void Recv_ResetPara();//����RESET��ֵ

	void Recv_KinectPitch();//����KinectPitch

	void Recv_TowerSwitch();

	void Recv_IsFirstPlayBall();
	
private:
	DWORD BuildId(unsigned char Permit, unsigned char Priority, unsigned char DeviceId, unsigned int Property);

	void InitData();							//��ʼ������
	int GetAimDeviceId(CAN_msg m_can_msg);		//����豸ID��

	bool isGetData;								//����Զ��֡���ж��Ƿ�õ�����
};