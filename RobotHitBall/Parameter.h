#pragma once

#include <opencv2/opencv.hpp>
#include "serialPort/SerialPort.h"
#include"serialport\MyThread.h"
#include "HUST_CAN.h"
#include "HUST_ReLocal.h"
//#include "GetResult.h"

#define TanX 0.577  //0.415  0.425
#define TanY 0.7  //0.553  0.57
#define SerPort 4
#define KKK 500


#define AddZ  (0.22+sin(KinectAngle/180*pi)*0.03)//0.25 //0.34//0.23 //0.51//kinect 到地面高度
#define AddY  (cos(KinectAngle/180*pi)*0.03)
//60 70 90
//#define SinZ   (KinectAngle<=61?sin(41.18355/180*pi):(KinectAngle<=71?sin(50.888/180*pi):sin(69.335321/180*pi)))                                              //    (sin((KinectAngle+1.5)/180*pi))                                 //(5350.0-AddZ*1000)/7480.0    //0.5 //0.7071   0.613  0.260
#define SinZ   (sin((KinectAngle)/180*pi))
#define CosZ   (sqrt(1-SinZ*SinZ))                               //(sqrt(1-SinZ*SinZ)) //0.866 //0.7071

//#define SinZ   (5350.0-AddZ*1000)/7030.0    //0.5 //0.7071   0.613  0.260
//#define CosZ   (sqrt(1-SinZ*SinZ)) //0.866 //0.7071

#define ListLong 200
#define MAXTRACK 5
#define HITPOINT 0
#define CAN_PORT 2
#define CAM_DEVNUM 1	//摄像头设备号
//#define X_CAMERA_TO_ROBOT 44
//#define Y_CAMERA_TO_ROBOT 590
//#define Z_CAMERA_TO_ROBOT 0
#define ANGLE_CAMERA_TO_ROBOT 0
#define KinectToRobotZ (0)    //(0.055)//kinect到车码盘中心
#define KinectToRobotX (0)//(-0.11)//kinect到车码盘中心

#define MAXHITLEVEL 180
#define MINHITLEVEL 45


//#define L_SHIFTING          425.0//425.0//gk415.0
//#define L_HEIGHT            925.0//910.0
//#define L_BATSHIFTING       115.0
//#define L_BATLENGTH         480///480.0//gk475.0
//#define L_RADIUS            130.0
//#define L_ANGLE             30.0
#define L_SHIFTING          410.0 //425.0
#define L_HEIGHT            935.0   // 915.0
#define L_BATSHIFTING       115.0
#define L_BATLENGTH         475.0
//#define L_RADIUS            130.0
#define L_ANGLE             30.0

#define R_SHIFTING          410.0 //425.0//425.0//gk415.0
#define R_HEIGHT            940.0   // 915.0//910.0
#define R_BATSHIFTING       115.0 
#define R_BATLENGTH         475.0  //480.0//gk475.0
//#define R_RADIUS            130.0
#define R_ANGLE             30.0

#define UPSIDESHIFTING      105.0 //(105.0- cos(KinectAngle/180*pi)*30)//340.0 //300

#define WORKTIMEDELAY        (Gettime()-timeList[DepthMatHead])


const double pi = 3.14159265358;

struct kal
{
	double f[81], q[81], r[81], h[81], y[8100], x[8100], p[81], g[81];
	double tmp[9], e[81], a[81], b[81];
};

struct badminton
{
	int S = 0;
	double X = 0, Y = 0, Z = 0, T = 0;
	double PX = 0, PY = 0, PZ = 0;
	double  V = 0, VX = 0, VY = 0, VZ = 0;
	double AC = 0, AX = 0, AY = 0, AZ = 0;
};

using namespace std;
using namespace cv;


//kinect
extern double gIntr[3][4];

//摄像机畸变
extern double Distortion[5]; 	//畸变系数	k1,k2,p1,p2,k3;

extern int HitPos[2];


extern int AnotherCarX;
extern int AnotherCarY;

extern badminton Poslist[MAXTRACK][50000];
extern int Posnum[MAXTRACK], Picnum, savenum;
extern ushort depmap[480][640];
extern CSerialPort mySerialPort;
extern bool mapb[480][640], mapp[480][640];
extern ushort listLT[300000][2];
extern int starttime;
extern Mat Piclist[30];
extern Mat image;
extern Mat show;

extern Mat DepthMatList[ListLong];
extern Mat imageMatList[ListLong];
extern int timeList[ListLong];
extern float RobotStateList[ListLong][3];
extern Mat saveList[ListLong * 10];
extern bool canshow;
extern int canrun;
extern double NeedToGo[MAXHITLEVEL + 1][5];
extern int DepthMatHead, DepthMatTail;

extern double Drowlist[100000][3];

extern float KinectAngle;

extern unsigned char flynow;
extern unsigned char passnum;

extern HUST_CAN CAN;
extern HUST_ReLocal RELOCAL;
//Coordinate *Coordinate_robot;

extern vector<enumTable> mResultTable;
extern vector<enumTable> mResultTableL;
extern vector<enumTable> mResultTableR;
extern vector<enumTable> mResultTableLN;
extern vector<enumTable> mResultTableLW;
extern vector<enumTable> mResultTableRN;
extern vector<enumTable> mResultTableRW;
extern RobotStateAdjustTHAndLA robotStateAdjustTHAndLA;
extern bool isPlayBall;
extern char KinectEnable;

extern bool TrackNow;
extern bool CanErode;
extern bool CanErode2;

/****************************************************************
****美丽的分割线，下面都是康哥使用的，不要删了哦~不然要切JJ的****
*****************************************************************/
extern ofstream playBallLog;
extern ofstream LA_ofile;
extern time_t nowTime;
extern struct tm* timeInfo;

extern int yawThresholdLA[2][2];
extern int playBallSpeed;

extern int yawPos[180];//yaw角度对应的位置
extern bool isLAEnemyJianCeDao;

//#define USENET
#define USETH
#define USE_CAN
//#define MOQIU


#ifdef USENET
extern stringstream gkout;
extern string str;
#else
#define gkout std::cout
#endif

/****************************************************************
****美丽的分割线，上面都是康哥使用的，不要删了哦~不然要切JJ的****
*****************************************************************/
