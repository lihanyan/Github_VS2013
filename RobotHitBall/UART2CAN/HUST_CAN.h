#pragma once
#include "RT_CAN.h"
#include "HUST_TYPE.h"

#define ROBOT_CONTROL_DEVICEID 0x01


class HUST_CAN :
	public RT_CAN{
public:
	
	MyCamState* pm_camState = NULL;				//当前的检测状态，指针，方便传输数据
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
	


	unsigned char RobotAttackState = 3;//用于判断状态，如果状态相同则不调用Send_RobotAttackState底层的发送函数，节省资源
	int m_KinectAngle = 3;//用于判断状态，如果状态相同则不调用Send_RobotAttackState底层的发送函数，节省资源
	

	HUST_CAN(void);
	~HUST_CAN(void);

	void Send_Test_Frame(unsigned char TargetID, unsigned char LED, unsigned char Times);							//功能测试帧发送函数

	void Send_MotorCtlMode_Frame(unsigned char TargetID, unsigned char MotorCtlMode);						//闭环类型发送函数

	void Send_MotorCtlParams_Frame(unsigned char TargetID, unsigned int P, unsigned int I);					//设置闭环控制参数

	void Send_MotorSpeedSet_Frame(unsigned char TargetID, int MotorSPD);

	void Send_MotorPositionSet_Frame(unsigned char TargetID, int MotorPOS);
	void Send_MotorPositionSet_Frame(unsigned char TargetID, int MotorPOS, int TimeRemain);			//发送机器人位置以及
	void Send_MotorPositionSet_Frame(unsigned char TargetID, short MotorPOS_m4, short MotorPOS_m5, short MotorPOS_m7, short MotorPOS_m8);			//发送上层四个电机状态
	bool Send_MotorPositionGet_Frame(unsigned char TargetID);										//获得目标电机的位置

	void Send_MotorCtlEnable_Frame(unsigned char TargetID, unsigned char MotorEn);							//电机使能帧发送函数

	void Send_MotorInit_Frame(unsigned char TargetID, int posAim);												//电机初始化发送函数

	void Send_MotorSpeedTest_Frame(unsigned char TargetID, int SetSpeed);							//设定测试的电机速度环目标速度	

	void Send_Jiqiu_Frame(unsigned char TargetID, int ExceptPos, int Reserve);	
	void Send_Jiqiu_Frame(unsigned char TargetID, int ExceptPos, short Speed, short timeRemain);//发送击球
	void Send_Jiqiu_Frame(unsigned char TargetID, short HitBallPos, short StopPos, short Speed, short timeRemain);//发送击球

	void Send_Faqiu_Frame(unsigned char TargetID, int ExceptPos, int timeRemain);						//发送发球
	
	void Send_RobotPosition_Frame(unsigned char TargetID,short Robot_x,short Robot_y,short Robot_time);//发送目标位置

	void Send_RobotPositionReset_Frame(unsigned char TargetID, short Robot_x, short Robot_y, short Robot_time);

	void Send_RobotAngle_Frame(unsigned char TargetID, float Angle);//发送目标姿态

	void Send_RobotRelocalPosition_Frame(unsigned char TargetID, float x_shifting,float y_shifting);

	void Send_RobotRelocalAngle_Frame(unsigned char TargetID, float angle_shifting);

	void Send_CylindControl_Frame(unsigned char TargetID, char CtlMode);	//二代A车的气缸控制

	void Send_RobotAttackState(unsigned char TargetID,unsigned char attackState);//发送举小旗子状态



	void Send_MC_Parame_Frame(char TargetID, short p1, short p2, short p3, MC_Parame_Typedef  MC_Parame);//向从站发调试信息
	void Send_MC_Parame_Frame(char TargetID, float p1, float p2, float p3, MC_Parame_Typedef  MC_Parame);//向从站发调试信息

	void Send_RobotHolder(unsigned char TargetID, float pitch,float yaw);

	void Send_HeartPack(unsigned char TargetID);

	virtual void OnRecvPackage(const BYTE * byBuf, DWORD dwLen);			//CAN接收线程	

	void Recv_RobotPosition();												//CAN接收位置
	void Recv_RobotState();												//CAN接收底盘状态

	void Recv_RobotAngle();													//CAN接收偏角

	void Recv_ControlJiqiuMode();											//击球模式						

	void Recv_Information();												//CAN接收其他信息

	void Recv_ResetPara();//接收RESET的值

	void Recv_KinectPitch();//接收KinectPitch

	void Recv_TowerSwitch();

	void Recv_IsFirstPlayBall();
	
private:
	DWORD BuildId(unsigned char Permit, unsigned char Priority, unsigned char DeviceId, unsigned int Property);

	void InitData();							//初始化数据
	int GetAimDeviceId(CAN_msg m_can_msg);		//获得设备ID号

	bool isGetData;								//发送远程帧，判断是否得到数据
};