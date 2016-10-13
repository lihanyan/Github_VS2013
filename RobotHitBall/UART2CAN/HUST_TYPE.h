#pragma once
#include <opencv2\opencv.hpp>

//颜色定义
#define C_BLACKGROUND	0
#define C_CORRECT		10
#define C_NOMAL			11
#define C_WARNING		12
#define C_INFORMATION	13

#define KINECTDELAYNUM 16

/****电机参数类型******/  //用于主站发送重展示速度环参数的标志
typedef enum
{
	SPEED_P,
	SPEED_I,
	SPEED_D,
	CURRENT_MAX, //速度环的最大输出
	SPEED_INTER_MAX, //积分最大
	CURRENT_P,
	CURRENT_I,
	CURRENT_D,
	PWM_MAX, //电流环的最大输出
	CURRENT_INTER_MAX, //积分最大
//	CURRENT_INTER_INIT  //电流环积分初值
}MC_Parame_Typedef;

typedef enum __MyCamState{
	IDLE,					//空闲 
	RE_AGL,					//角度矫正
	RE_POS,					//位置矫正
	TEST_AGL,				//角度测试				
	TEST_POS,				//位置测试
	OTHER,
}MyCamState;

typedef struct __MyPoint{
	int x = -1;
	int y = -1;
}MyPoint;

typedef struct __MyConner{
	bool isConner = false;
	CvPoint leftup = cvPoint(-1, -1);
	CvPoint rightup = cvPoint(-1, -1);
	CvPoint leftdown = cvPoint(-1, -1);
	CvPoint rightdown = cvPoint(-1, -1);
	CvPoint mx_count_VL = cvPoint(0, 0);
	CvPoint mx_count_VR = cvPoint(0, 0);
	CvPoint my_count_HU = cvPoint(0, 0);
	CvPoint my_count_HD = cvPoint(0, 0);
}MyConner;

typedef struct __MyDisErr{
	bool isErrValue = false;
	float x;
	float y;
}MyDisErr;

typedef enum __MyPosition{
	NON = 0,
	UP = 1,
	DOWN = 2,
	LEFT = 3,
	RIGHT = 4
}MyPoition;

typedef struct __MyLine{
	char lineFlag;
	float theta;
	float x;
	float y;
}MyLine;


/**********************
*ROBOCON CAN通信头定义*
**********************/
typedef struct
{
	unsigned int Permit : 1;			//RobotCan协议许可位，0有效
	unsigned int Priority : 4;			//优先级
	unsigned int DeviceId : 8;			//设备ID
	unsigned int Property : 16;			//指令属性值定义	
}*Header;

typedef struct {
	int x;
	int y;
	int z;
}M_pos;//空间点坐标


typedef struct {
	//	bool isCorrectData = false;
	int m_m3;
	int m_m4;
	int m_m5;
	int m_m6;
	int FaceNum; //解的编号
	M_pos m_pos;
	int timeRemain;
}M_motorState;//电机状态,统一用角度制表示



typedef struct {
	float robot_xNow;
	float robot_yNow;
	float angleNow;
	float robot_x;
	float robot_y;
	float angle; //
	float robot[KINECTDELAYNUM][3];
	char receivenum[2];
}M_robotState;

//查表数据
typedef struct {
	int m_m5;		//旋转电机
	int m_m6;		//挥拍电机
	int x;			//空间X位置
	int y;			//空间Y位置
	int z;			//空间Z位置
}M_motorStatePosEnum;//电机状态,统一用角度制表示



//typedef enum __playDirection
//{
//	PLAY_LEFT, PLAY_MID, PLAY_RIGHT
//}M_playDirection;

/****击球电机力度等级******/
typedef enum
{
	SPEED_400 = 0,
	SPEED_600 = 1,
	SPEED_800 = 2,
	SPEED_1000 = 3,
	SPEED_1200 = 4,

}Motor_SpeedEnum;

//击球所用塔的选择
typedef enum __playSelect
{
	SELECT_LEFT, SELECT_RIGHT
}M_playSelect;

//击球所用表的选择
typedef enum __playDirect
{
	PLW, PLN, PRW, PRN, PMID
}M_playDirect;

//击球所用表的选择
typedef enum __playLidu
{
	XiaoQiu, NomalQiu, DaQiu
}M_playLidu;

typedef struct __enumTable
{
	M_playDirect playDirect = PMID;
	int panel = 0;					//平面
	int theta1 = 0;					//旋转电机
	int theta2 = 0;					//挥拍电机
	float x = 0;					//电机状态所对应的X
	float y = 0;					//电机状态所对应的Y
	float z = 0;					//电机状态所对应的Z
	float yaw = 90;
	float pitch = 0;
	M_pos mPos;						//期望位置
	M_pos needMove;					//移动位置
	int ResultZMin = 50;				//B车枚举的解中Z的最小值
	int timeLeft = 0;				//剩余时间
	int speed;
}enumTable;


typedef enum __RobotStateAdjustTHAndLA
{
	TH_STATE, LA_STATE
}RobotStateAdjustTHAndLA;
/***
属性帧优先级
***/
#define DEFAULT_PRIO			1
#define MOTOR_INIT_PRIO			0
#define MOTOR_CTLENABLE_PRIO	1
#define MOTOR_CTLMODE_PRIO		2
#define MOTOR_CTLPARAMS_PRIO	3
#define MOTOR_POSTION_PRIO		4
#define MOTOR_SPEED_PRIO		5
#define MOTOR_SPEED_TEST_PRIO	5
#define SEND_PARAME_PRIO		2