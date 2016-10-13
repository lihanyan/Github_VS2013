#pragma once
#include <opencv2\opencv.hpp>

//��ɫ����
#define C_BLACKGROUND	0
#define C_CORRECT		10
#define C_NOMAL			11
#define C_WARNING		12
#define C_INFORMATION	13

#define KINECTDELAYNUM 16

/****�����������******/  //������վ������չʾ�ٶȻ������ı�־
typedef enum
{
	SPEED_P,
	SPEED_I,
	SPEED_D,
	CURRENT_MAX, //�ٶȻ���������
	SPEED_INTER_MAX, //�������
	CURRENT_P,
	CURRENT_I,
	CURRENT_D,
	PWM_MAX, //��������������
	CURRENT_INTER_MAX, //�������
//	CURRENT_INTER_INIT  //���������ֳ�ֵ
}MC_Parame_Typedef;

typedef enum __MyCamState{
	IDLE,					//���� 
	RE_AGL,					//�ǶȽ���
	RE_POS,					//λ�ý���
	TEST_AGL,				//�ǶȲ���				
	TEST_POS,				//λ�ò���
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
*ROBOCON CANͨ��ͷ����*
**********************/
typedef struct
{
	unsigned int Permit : 1;			//RobotCanЭ�����λ��0��Ч
	unsigned int Priority : 4;			//���ȼ�
	unsigned int DeviceId : 8;			//�豸ID
	unsigned int Property : 16;			//ָ������ֵ����	
}*Header;

typedef struct {
	int x;
	int y;
	int z;
}M_pos;//�ռ������


typedef struct {
	//	bool isCorrectData = false;
	int m_m3;
	int m_m4;
	int m_m5;
	int m_m6;
	int FaceNum; //��ı��
	M_pos m_pos;
	int timeRemain;
}M_motorState;//���״̬,ͳһ�ýǶ��Ʊ�ʾ



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

//�������
typedef struct {
	int m_m5;		//��ת���
	int m_m6;		//���ĵ��
	int x;			//�ռ�Xλ��
	int y;			//�ռ�Yλ��
	int z;			//�ռ�Zλ��
}M_motorStatePosEnum;//���״̬,ͳһ�ýǶ��Ʊ�ʾ



//typedef enum __playDirection
//{
//	PLAY_LEFT, PLAY_MID, PLAY_RIGHT
//}M_playDirection;

/****���������ȵȼ�******/
typedef enum
{
	SPEED_400 = 0,
	SPEED_600 = 1,
	SPEED_800 = 2,
	SPEED_1000 = 3,
	SPEED_1200 = 4,

}Motor_SpeedEnum;

//������������ѡ��
typedef enum __playSelect
{
	SELECT_LEFT, SELECT_RIGHT
}M_playSelect;

//�������ñ��ѡ��
typedef enum __playDirect
{
	PLW, PLN, PRW, PRN, PMID
}M_playDirect;

//�������ñ��ѡ��
typedef enum __playLidu
{
	XiaoQiu, NomalQiu, DaQiu
}M_playLidu;

typedef struct __enumTable
{
	M_playDirect playDirect = PMID;
	int panel = 0;					//ƽ��
	int theta1 = 0;					//��ת���
	int theta2 = 0;					//���ĵ��
	float x = 0;					//���״̬����Ӧ��X
	float y = 0;					//���״̬����Ӧ��Y
	float z = 0;					//���״̬����Ӧ��Z
	float yaw = 90;
	float pitch = 0;
	M_pos mPos;						//����λ��
	M_pos needMove;					//�ƶ�λ��
	int ResultZMin = 50;				//B��ö�ٵĽ���Z����Сֵ
	int timeLeft = 0;				//ʣ��ʱ��
	int speed;
}enumTable;


typedef enum __RobotStateAdjustTHAndLA
{
	TH_STATE, LA_STATE
}RobotStateAdjustTHAndLA;
/***
����֡���ȼ�
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