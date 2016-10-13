#include "stdafx.h"
#include "HUST_CAN.h"
#include "RobotCAN_Proplist.h"
#include <iostream>
#include <time.h>
#include "HUST_TYPE.h"
#include "gettime.h"
using namespace std;



HUST_CAN::HUST_CAN(void)
{
	InitData();
}
HUST_CAN::~HUST_CAN(void)
{

}

void HUST_CAN::InitData()							//初始化数据
{
	m_robotState.robot_x = 0;
	m_robotState.robot_y = 0;
	m_robotState.angle = 0.0;
	tim1 = 0;
	tim2 = 0;
	time_count = 0;
	isGetData = true;
	m_motorPos = 0;
}
void HUST_CAN::Send_Test_Frame(unsigned char TargetID, unsigned char LED, unsigned char Times)
{
	CAN_msg msg;
	msg.id = BuildId(0, DEFAULT_PRIO, TargetID, TEST);
	msg.len = 2;
	*((unsigned char*)msg.data) = LED;
	*((unsigned char*)(msg.data + 1)) = Times;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}

void HUST_CAN::Send_MotorCtlMode_Frame(unsigned char TargetID, unsigned char MotorCtlMode)						//闭环类型发送函数
{

}

/*******************************************************************************
* Function Name  : Send_MotorCtlParams_Frame
* Description    : 发送PI参数 D默认为0，不发，这里由于CAN通信数据位限制所以不发
* Input          : TargetID: 目标设备ID号
P: 比例系数
I: 积分系数
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_MotorCtlParams_Frame(unsigned char TargetID, unsigned int P, unsigned int I)		//设置闭环控制参数
{
	CAN_msg msg;
	msg.id = BuildId(0, MOTOR_CTLPARAMS_PRIO, TargetID, MOTOR_CTLSET);
	msg.len = 8;
	*((DWORD*)msg.data) = P;
	*((DWORD*)(msg.data + 1)) = I;	//是不是+1并没有测试，使用的时候注意是+1还是+4
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}

/*******************************************************************************
* Function Name  : Send_MotorSpeedSet_Frame
* Description    : 发送速度设置函数
* Input          : TargetID: 目标设备ID号
				   MotorSPD: 目标速度
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_MotorSpeedSet_Frame(unsigned char TargetID, int MotorSPD)
{
	CAN_msg msg;
	msg.id = BuildId(0, MOTOR_SPEED_PRIO, TargetID, MOTOR_CTLSPD);
	msg.len = 4;
	*((DWORD*)msg.data) = MotorSPD;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);

}

/*******************************************************************************
* Function Name  : Send_MotorSpeedSet_Frame
* Description    : 发送位置设置函数
* Input          : TargetID: 目标设备ID号
MotorPOS: 目标位置
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_MotorPositionSet_Frame(unsigned char TargetID, int MotorPOS)
{
	CAN_msg msg;
	msg.id = BuildId(0, MOTOR_POSTION_PRIO, TargetID, MOTOR_CTLPOS);
	msg.len = 8;
	*((DWORD*)msg.data) = MotorPOS;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);

}
/*******************************************************************************
* Function Name  : Send_MotorSpeedSet_Frame
* Description    : 发送位置设置函数
* Input          : TargetID: 目标设备ID号
				   MotorPOS: 目标位置
				   TimeRemain:剩余时间
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_MotorPositionSet_Frame(unsigned char TargetID, int MotorPOS,int TimeRemain)
{
	CAN_msg msg;
	msg.id = BuildId(0, MOTOR_POSTION_PRIO, TargetID, MOTOR_CTLPOS);
	msg.len = 8;
	*((DWORD*)msg.data) = MotorPOS;
	*((DWORD*)(msg.data + 4)) = TimeRemain;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}


void HUST_CAN::Send_MotorPositionSet_Frame(unsigned char TargetID, short MotorPOS_m4, short MotorPOS_m5, short MotorPOS_m7, short MotorPOS_m8)			//发送上层四个电机状态
{
	CAN_msg msg;
	msg.id = BuildId(0, MOTOR_POSTION_PRIO, TargetID, MOTOR_CTLPOS);
	msg.len = 8;
	*((short*)(msg.data + 0)) = MotorPOS_m4;
	*((short*)(msg.data + 2)) = MotorPOS_m5;
	*((short*)(msg.data + 4)) = MotorPOS_m7;
	*((short*)(msg.data + 6)) = MotorPOS_m8;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}
/*******************************************************************************
* Function Name  : Send_MotorPositionGet_Frame
* Description    : 获得当前电机的状态(发送远程帧)
* Input          : TargetID: 目标设备ID号
* Output         : 得到的位置数据
* Return         : void
*******************************************************************************/
bool HUST_CAN::Send_MotorPositionGet_Frame(unsigned char TargetID)
{
	CAN_msg msg;
	msg.id = BuildId(0, MOTOR_POSTION_PRIO, TargetID, MOTOR_CTLPOS);
	msg.len = 0;
	msg.type = REMOTE_FRAME;
	msg.format = EXTENDED_FORMAT;
	if (!isGetData)					//正常情况下请求数据的时候isGetData应该是为true的，否则说明正在请求数据
	{
		std::cout << "Another Theard is Recviving Data Now!" << endl;
		return 0;
	}
	isGetData = false;
	while (isSend);
	SendCANMessage(&msg);
	int i = 0;
	while (!isGetData&&i<1000)				//正在等待数据返回,接收数据的线程会置位isGetData
	{
		i++;
		Sleep(1);
	}
	if (isGetData)							//是否在100ms内受到了数据，没有的话，退出
	{
		std::cout << "RECEVING DATA:" << m_motorPos << endl;
		return true;
	}
	else
	{
		std::cout << "NOT RECEVING DATA:" << endl;
		isGetData = true;
		return false;
	}
}

/*******************************************************************************
* Function Name  : Send_MotorCtlEnable_Frame
* Description    : 电机使能帧发送函数
* Input          : TargetID: 目标设备ID号
				   MotorPOS: 目标位置
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_MotorCtlEnable_Frame(unsigned char TargetID, unsigned char MotorEn)							//电机使能帧发送函数
{
	CAN_msg msg;
	msg.id = BuildId(0, MOTOR_CTLENABLE_PRIO, TargetID, MOTOR_CTLEN);
	msg.len = 1;
	*((unsigned char*)msg.data) = MotorEn;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}

void HUST_CAN::Send_MotorInit_Frame(unsigned char TargetID, int posAim)												//电机初始化发送函数
{
	CAN_msg msg;
	msg.id = BuildId(0, 3, TargetID, SYSINIT);
	msg.len = 0;
	//*((DWORD*)msg.data) = posAim;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}

void HUST_CAN::Send_MotorSpeedTest_Frame(unsigned char TargetID, int SetSpeed)						//设定测试的电机速度环目标速度
{

}
/*******************************************************************************
* Function Name  : Send_Jiqiu_Frame
* Description    : 发送电机击球帧
* Input          : TargetID: 目标设备ID号
  ExceptPos		 : 电机要击打运动到的位置
  Reserve		 : 保留参数
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_Jiqiu_Frame(unsigned char TargetID, int ExceptPos, int Reserve)
{
	CAN_msg msg;
	msg.id = BuildId(0, DEFAULT_PRIO, TargetID, MOTOR_WAVEBAT_JIQIU);
	msg.len = 8;
	*((DWORD*)msg.data) = ExceptPos;
	*((DWORD*)(msg.data + 4)) = Reserve;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}

//Speed 用 有符号16位表示，有三档 （ 700 1000 1600），time 也是用有符号16位表示。
void HUST_CAN::Send_Jiqiu_Frame(unsigned char TargetID, int ExceptPos, short Speed, short timeRemain)
{
	CAN_msg msg;
	msg.id = BuildId(0, DEFAULT_PRIO, TargetID, MOTOR_WAVEBAT_JIQIU);
	msg.len = 8;
	*((DWORD*)msg.data) = ExceptPos;
	if (ExceptPos > 0)//速度可以是绝对值，这时候就要通过目标位置确定速度符号
	{
		Speed = abs(Speed);
	}
	else
	{
		Speed = -abs(Speed);
	}
	*((short*)(msg.data + 4)) = Speed;
	*((short*)(msg.data + 6)) = timeRemain<0 ? 0 : timeRemain;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}

void HUST_CAN::Send_Jiqiu_Frame(unsigned char TargetID, short HitBallPos, short StepPos, short Speed, short timeRemain)		//发送击球
{
	CAN_msg msg;
	msg.id = BuildId(0, DEFAULT_PRIO, TargetID, MOTOR_WAVEBAT_JIQIU);
	msg.len = 8;
	*((short*)msg.data) = HitBallPos;
	if (HitBallPos > 0)//速度可以是绝对值，这时候就要通过目标位置确定速度符号
	{
		*((short*)(msg.data + 2)) = (HitBallPos + StepPos);
		Speed = abs(Speed);
	}
	else
	{
		*((short*)(msg.data + 2)) = (HitBallPos - StepPos);
		Speed = -abs(Speed);
	}
	*((short*)(msg.data + 4)) = Speed;
	*((short*)(msg.data + 6)) = timeRemain<0 ? 0 : timeRemain;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}
/*******************************************************************************
* Function Name  : Send_Faqiu_Frame
* Description    : 发送电机击球帧
* Input          : TargetID: 目标设备ID号
ExceptPos		 : 电机要击打运动到的位置
timeRemain		 : 时间控制
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_Faqiu_Frame(unsigned char TargetID, int ExceptPos, int timeDelay)
{
	CAN_msg msg;
	msg.id = BuildId(0, DEFAULT_PRIO, TargetID, MOTOR_WAVEBAT_FAQIU);
	msg.len = 8;
	*((DWORD*)msg.data) = ExceptPos;
	*((DWORD*)(msg.data + 4)) = timeDelay;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}
/*******************************************************************************
* Function Name  : Send_RobotPosition_Frame
* Description    : 发送机器人目标位置
* Input          : TargetID: 目标设备ID号
Robot_x		 : 目标X
Robot_y		 : 目标Y
Robot_time		:时间
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_RobotPosition_Frame(unsigned char TargetID, short Robot_x, short Robot_y, short Robot_time){
	CAN_msg msg;
	msg.id = BuildId(0, DEFAULT_PRIO, TargetID, ROBOT_CTLPOS);
	msg.len = 6;
	*((short*)msg.data) = Robot_x;
	*((short*)(msg.data + 2)) = Robot_y;
	*((short*)(msg.data + 4)) = Robot_time;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}

/*******************************************************************************
* Function Name  : Send_RobotAngle_Frame
* Description    : 发送机器人目标姿态
* Input          : TargetID: 目标设备ID号
Angle		 : 目标角度
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_RobotAngle_Frame(unsigned char TargetID, float Angle)//发送目标姿态
{
	CAN_msg msg;
	msg.id = BuildId(0, DEFAULT_PRIO, TargetID, ROBOT_CTLAGL);
	msg.len = 4;
	*((float*)msg.data) = Angle;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}

/*******************************************************************************
* Function Name  : Send_RobotRelocalPosition_Frame
* Description    : 重定位发送机器人位置偏移
* Input          : TargetID: 目标设备ID号
x_shifting		 ：X偏移量
y_shifting		 ：Y偏移量
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_RobotRelocalPosition_Frame(unsigned char TargetID, float x_shifting, float y_shifting)
{
	CAN_msg msg;
	msg.id = BuildId(0, DEFAULT_PRIO, TargetID, ROBOT_RELOCALPOS);
	msg.len = 8;
	*((float*)msg.data) = x_shifting;
	*((float*)(msg.data + 4)) = y_shifting;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}

/*******************************************************************************
* Function Name  : Send_RobotRelocalAngle_Frame
* Description    : 重定位发送机器人角度偏移
* Input          : TargetID: 目标设备ID号
angle_shifting	 ：角度偏移
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_RobotRelocalAngle_Frame(unsigned char TargetID, float angle_shifting)
{
	CAN_msg msg;
	msg.id = BuildId(0, DEFAULT_PRIO, TargetID, ROBOT_RELOCALAGL);
	msg.len = 4;
	*((float*)msg.data) = angle_shifting;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}

/*******************************************************************************
* Function Name  : Send_CylindControl_Frame
* Description    : 二代A车的气缸控制
* Input          : TargetID: 目标设备ID号
CtlMode	 ：控制模式 0表示发球 1表示机构展开 2表示机构折叠
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_CylindControl_Frame(unsigned char TargetID, char CtlMode)
{
	CAN_msg msg;
	msg.id = BuildId(0, DEFAULT_PRIO, TargetID, CYLIND_CONTROL);
	msg.len = 1;
	*((char*)msg.data) = CtlMode;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}
/*******************************************************************************
* Function Name  : Send_RobotAttackState
* Description    : 举小旗子状态控制
* Input          : TargetID: 目标设备ID号
CtlMode	 ：控制模式 
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_RobotAttackState(unsigned char TargetID, unsigned char attackState)//发送举小旗子状态
{
	if (this->RobotAttackState == attackState) 
	{
		return;//状态相同就不发送
	}
	else
	{
		this->RobotAttackState = attackState;
		CAN_msg msg;
		msg.id = BuildId(0, DEFAULT_PRIO, TargetID, ROBOT_RST_ATTACK);
		msg.len = 1;
		*((unsigned char*)msg.data) = attackState;
		msg.type = DATA_FRAME;
		msg.format = EXTENDED_FORMAT;
		while (isSend);
		SendCANMessage(&msg);
	}


}


void  HUST_CAN::Send_RobotPositionReset_Frame(unsigned char TargetID, short Robot_x, short Robot_y, short Robot_time)
{
	CAN_msg msg;
	msg.id = BuildId(0, DEFAULT_PRIO, TargetID, ROBOT_POSRST);
	msg.len = 6;
	*((short*)msg.data) = Robot_x;
	*((short*)(msg.data + 2)) = Robot_y;
	*((short*)(msg.data + 4)) = Robot_time;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);

}
/*******************************************************************************
* Function Name  : Send_MC_Parame_Frame
* Description    : 向底盘从站发调试信息
* Input          : TargetID: 目标设备ID号
				   p1:从站1参数
				   p2:从站2参数
				   p3:从站3参数
				   MC_Parame:参数类型
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_MC_Parame_Frame(char TargetID, short p1, short p2, short p3, MC_Parame_Typedef  MC_Parame)
{
	/* Transmit */
	CAN_msg msg;
	msg.id = BuildId(0, SEND_PARAME_PRIO, TargetID, SEND_PARAME);

	msg.len = 8;
	*((short*)msg.data) = p1;
	*((short*)(msg.data + 2)) = p2;
	*((short*)(msg.data + 4)) = p3;
	*((char*)(msg.data + 6)) = MC_Parame;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}

/*******************************************************************************
* Function Name  : Send_MC_Parame_Frame
* Description    : 向底盘从站发调试信息
* Input          : TargetID: 目标设备ID号
					p1:从站1参数
					p2:从站2参数
					p3:从站3参数
					MC_Parame:参数类型
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_MC_Parame_Frame(char TargetID, float p1, float p2, float p3, MC_Parame_Typedef  MC_Parame)//向从站发调试信息
{
	/* Transmit */
	CAN_msg msg;
	msg.id = BuildId(0, SEND_PARAME_PRIO, TargetID, SEND_PARAME);

	msg.len = 8;
	*((short*)msg.data) = (short)(p1*100);
	*((short*)(msg.data + 2)) = (short)(p2*100);
	*((short*)(msg.data + 4)) = (short)(p3*100);
	*((char*)(msg.data + 6)) = MC_Parame;


	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}

/*******************************************************************************
* Function Name  : Send_RobotHander
* Description    : 发送云台参数信息
* Input          : TargetID: 目标设备ID号
pitch:俯仰角

yaw:偏航角
MC_Parame:参数类型
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_RobotHolder(unsigned char TargetID, float pitch, float yaw)
{
	CAN_msg msg;
	msg.id = BuildId(0, DEFAULT_PRIO, TargetID, ROBOT_HANDER);
	msg.len = 8;
	*((float*)msg.data) = pitch;
	*((float*)(msg.data + 4)) = yaw;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}

/*******************************************************************************
* Function Name  : Send_HeartPack
* Description    : 发送CAN心跳
* Input          : TargetID: 目标设备ID号
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Send_HeartPack(unsigned char TargetID)
{
	CAN_msg msg;
	msg.id = BuildId(0, 7, TargetID, ROBOT_CAN_HEARTPACK);
	msg.len = 1;
	*((char*)msg.data) = 0xff;
	msg.type = DATA_FRAME;
	msg.format = EXTENDED_FORMAT;
	while (isSend);
	SendCANMessage(&msg);
}

/*******************************************************************************
* Function Name  : BuildId
* Description    : 生成和Robocon团队CAN通信相对应的ID
* Input          : byBuf: CAN协议有效位，为0
				   Priority: 优先级
				   DeviceID: 设备ID号
				   Property: 帧属性
* Output         : 29的位ID
* Return         : DWORD
*******************************************************************************/
DWORD HUST_CAN::BuildId(unsigned char Permit, unsigned char Priority, unsigned char DeviceId, unsigned int Property)
{
	return  (DWORD)Permit | (DWORD)Priority << 1 | (DWORD)DeviceId << 5 | (DWORD)Property << 13;
}

/*******************************************************************************
* Function Name  : GetAimDeviceId
* Description    : 获得CAN数据的目的设备
* Input          : m_can_msg: CAN数据
* Output         : 设备的ID
* Return         : int
*******************************************************************************/
int HUST_CAN::GetAimDeviceId(CAN_msg m_can_msg)
{
	return (int)((m_can_msg.id  >> 5)&0xFF);
}


/*******************************************************************************
* Function Name  : OnRecvPackage
* Description    : 在串口的接收线程内被调用，表示接收到一个完整的CAN数据包
* Input          : byBuf: 接收到的数据
dwLen: 接收到的数据长度
* Output         : None
* Return         : None
*******************************************************************************/
//#define _GK_TEST
void HUST_CAN::OnRecvPackage(const BYTE * byBuf, DWORD dwLen)
{
	Header hdr;
//	cout << "HUST_CAN::cout<<OnRecvPackage" << endl;
	CAN_msg* msg;
	msg = (CAN_msg *)byBuf;
	if (dwLen == sizeof(CAN_msg))
	{
		if (msg->ch == CAN_CONFIG_CHANNEL)
		{
			m_CANConfigMsg = *msg;
			SetEvent(m_hConfigEvnet);
			Sleep(1);
		}
		else
		{
			m_CANRecvMsg = *msg;
			SetEvent(m_hRecvMsgEvnet);
			RT_COM::OnRecvPackage(byBuf, dwLen);
		}
	}
	//以下赋值可以提取出我们STM32使用的值
	hdr = (Header)BuildHeader(msg->id);
#ifdef _GK_TEST
	switch (hdr->Property)
	{
	case MOTOR_STSPOS:
		cout << "POS\n";
		time_count++;
		//cout << time_count << endl;
		time_count %= 1000;
		if (!time_count)
		{
			tim2 = clock();
			Recv_RobotPosition();
			Send_MotorPositionSet_Frame(0x02, 100);
			cout << tim2 - tim1 << endl;
			tim1 = tim2;
		}
		else
		{
		Recv_RobotPosition();
		Send_MotorPositionSet_Frame(0x02, 100);
//		time_count = 0;
		}
		Recv_RobotPosition();
		break;
	case MOTOR_STSAGL:
		cout << "ANG\n";
		time_count++;
		cout << time_count << endl;
		Recv_RobotAngle();
		break;
	default:
		break;
	}
#else
	switch (hdr->Property)
	{
	case ROBOT_CTLPOS:
		Recv_RobotPosition();
		break;
	case ROBOT_CTLAGL:
		Recv_RobotAngle();
		break;
	case ROBOT_RELOCALPOS:
		cout << "ROBOT_RELOCALPOS" << endl;
		//		*pm_camState = RE_POS;
		break;
	case ROBOT_RELOCALAGL:
		cout << "ROBOT_RELOCALAGL" << endl;
//		*pm_camState = RE_AGL;
		break;
	case ROBOT_INFORMATION:
		Recv_Information();
		break;
	case MOTOR_CTLPOS:
		if (hdr->DeviceId != 1)
		{
			if (isGetData == false)	//表示正在请求数据
			{
				m_motorPos = *((int*)m_CANRecvMsg.data);
				isGetData = true;
			}
		}
		break;
	case ROBOT_CONTROL_JIQIU_MODE:
		Recv_ControlJiqiuMode();
		break;
	case ROBOT_RST_ADJUST:
		Recv_ResetPara();
		break;

	case ROBOT_STATE:
		Recv_RobotState(); 
		break;
	case PC_POWEROFF:
		cout << "关机" << endl;
		isPowerOff = true;
		break;
	case ROBOT_ATTACK_SCOPE:
	//	attackScope = *((char*)m_CANRecvMsg.data);
		cout << "scope:"<<(int)attackScope<<endl;
		break;
	case KINECT_PITCH:
		Recv_KinectPitch();
		break;
	case TEST:
		Send_Test_Frame(0x01,1,10);
		break;
	case ROBOT_TOWERSWITCH:
		Recv_TowerSwitch();
		break;
	case ROBOT_PALYFIRSTBALL:
		Recv_IsFirstPlayBall();
		break;
	case ROBOT_NEEDTH:
		isNeedTH = *((char*)m_CANRecvMsg.data);
		cout << "need TH" << "\t" << (int)isNeedTH << endl;
		break;
	default:
		break;
	}
	isConnect = true;
	//std::cout << hdr->DeviceId <<"\t"<<hdr->Property<< endl;
#endif
}

/*******************************************************************************
* Function Name  : Recv_RobotPosition
* Description    : 接收当前的机器人位置
* Input          : m_can_msg: CAN数据
* Output         : None
* Return         : None4
*******************************************************************************/
void HUST_CAN::Recv_RobotPosition()
{
	//static int lasttime;
	//m_robotState.robot_x = *((float*)m_CANRecvMsg.data);
	//m_robotState.robot_y = *((float*)m_CANRecvMsg.data+1);
	//cout << "GKGK::\t Recv Robot Position:" << m_robotState.robot_x << "\t" << m_robotState.robot_y << endl;
	
	m_robotState.robot_xNow = m_robotState.robot[m_robotState.receivenum[0]][0] = *((float*)m_CANRecvMsg.data);
	m_robotState.robot_yNow = m_robotState.robot[m_robotState.receivenum[0]][1] = *((float*)m_CANRecvMsg.data + 1);

	//cout << m_robotState.robot_xNow << "	" << m_robotState.robot_yNow << endl;

	m_robotState.receivenum[0]++;
	if (m_robotState.receivenum[0] > (KINECTDELAYNUM-1))
	{
		m_robotState.receivenum[0] = 0;
	}

	m_robotState.robot_x = m_robotState.robot[m_robotState.receivenum[0]][0];
	m_robotState.robot_y = m_robotState.robot[m_robotState.receivenum[0]][1];

	//cout << m_robotState.robot_x << endl; 

	//if (abs(Gettime() - lasttime - 5) > 2)
	//{
	//	cout << "CAN FIELED!" << endl;
	//}
	//lasttime=Gettime();
}

/*******************************************************************************
* Function Name  : Recv_RobotPosition
* Description    : 接收当前的机器人位姿
* Input          : m_can_msg: CAN数据
* Output         : None
* Return         : None
*******************************************************************************/
void HUST_CAN::Recv_RobotAngle()
{
	m_robotState.angleNow = m_robotState.robot[m_robotState.receivenum[1]][2] = *((float*)m_CANRecvMsg.data);
	float speed = *((float*)m_CANRecvMsg.data+1);
	if (abs(m_robotState.angleNow) > 20 || abs(speed) > 10)
	{
		//cout << "error angle:" << m_robotState.angleNow << "\t" << speed << endl;
	}
	m_robotState.receivenum[1]++;
	if (m_robotState.receivenum[1] > (KINECTDELAYNUM-1))
	{
		m_robotState.receivenum[1] = 0;
	}

	m_robotState.angle = m_robotState.robot[m_robotState.receivenum[1]][2];
}


void HUST_CAN::Recv_RobotState()
{
	m_robotState.robot_xNow = m_robotState.robot[m_robotState.receivenum[0]][0] = (float)(*((short*)m_CANRecvMsg.data));
	m_robotState.robot_yNow = m_robotState.robot[m_robotState.receivenum[0]][1] = (float)(*((short*)m_CANRecvMsg.data + 1)-1000);
	m_robotState.angleNow = m_robotState.robot[m_robotState.receivenum[1]][2] = (float)(*((short*)m_CANRecvMsg.data+2))/100;

	

	m_robotState.receivenum[0]++;
	if (m_robotState.receivenum[0] > (KINECTDELAYNUM - 1))
	{
		m_robotState.receivenum[0] = 0;
	}
	m_robotState.robot_x = m_robotState.robot[m_robotState.receivenum[0]][0];
	m_robotState.robot_y = m_robotState.robot[m_robotState.receivenum[0]][1];


	//cout << m_robotState.robot_xNow << "\t" << m_robotState.robot_yNow << "\t" << m_robotState.angleNow << "	"<<m_robotState.robot_x << endl;


	m_robotState.receivenum[1]++;
	if (m_robotState.receivenum[1] > (KINECTDELAYNUM - 1))
	{
		m_robotState.receivenum[1] = 0;
	}
	m_robotState.angle = m_robotState.robot[m_robotState.receivenum[1]][2];
}




/*******************************************************************************
* Function Name  : Recv_RobotPosition
* Description    : 接收当前的要击球的方向
* Input          : m_can_msg: CAN数据
* Output         : None
* Return         : None
*******************************************************************************/
void HUST_CAN::Recv_ControlJiqiuMode()
{
	
	char dirTemp = *((unsigned char*)m_CANRecvMsg.data);//0:左 1:中 2:右 3:不更改
	if (dirTemp != 3)
	{
		playDir = dirTemp;
		detectEnable = true;
	}
	else
	{
		char detectEnableTemp = *((unsigned char*)m_CANRecvMsg.data + 1);//0：允许检测 1：不允许检测 2：不改变
		if (0 == detectEnableTemp)
		{
			cout << "失能" << endl;
			detectEnable = detectEnableTemp;
			Send_Jiqiu_Frame(0x06, 20, 0, 300, 0);
			Send_Jiqiu_Frame(0x09, -20, 0, 300, 0);
		}
		else if (1 == detectEnableTemp)
		{
			cout << "使能" << endl;
			detectEnable = detectEnableTemp;
			Send_Jiqiu_Frame(0x06, -20, 0, 300, 0);
			Send_Jiqiu_Frame(0x09, 20, 0, 300, 0);
		}
	}

	cout << "************击球方向************" << (int)playDir << "\t" << (int)detectEnable << endl;
}
/*******************************************************************************
* Function Name  : Recv_Information
* Description    : 输出一些调试用的信息（可自定义，按需求填写东西）
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Recv_Information()
{
	//std::cout << "information:" << (*((int*)m_CANRecvMsg.data)) << "\t" << (*((int*)m_CANRecvMsg.data + 1)) << endl;
	  
	std::cout << "info:" << (*((short*)m_CANRecvMsg.data + 0)) << "\t" << (*((short*)m_CANRecvMsg.data + 1)) << "\t" << (*((short*)m_CANRecvMsg.data + 2)) << "\t" << (*((short*)m_CANRecvMsg.data + 3)) << endl;
}

/*******************************************************************************
* Function Name  : Recv_ResetPara
* Description    : 接收RESET的值
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
void HUST_CAN::Recv_ResetPara()//接收RESET的值
{
	resetX = *((float*)m_CANRecvMsg.data);
	resetY = *((float*)m_CANRecvMsg.data + 1);
}


void HUST_CAN::Recv_KinectPitch()//接收KinectPitch
{
	float pitch= *((float*)m_CANRecvMsg.data);
	cout << "pitch" << pitch << "\t" << *kinectPitch << endl;
	if (abs(pitch - *kinectPitch) > 1)
	{
		*kinectPitch = pitch;
		Send_RobotHolder(0x03, pitch, 0);
	}
}

void HUST_CAN::Recv_TowerSwitch()
{
	towerSwitch = *((char*)m_CANRecvMsg.data);
	cout << "tower switch" << (int)towerSwitch << endl;
}

void HUST_CAN::Recv_IsFirstPlayBall()
{
	isFirstPlayBall =(bool)(*((char*)m_CANRecvMsg.data));
	cout << "isFirstPlayBall" << (int)isFirstPlayBall << endl;
}