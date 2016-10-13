#include "Parameter.h"




//XBOX V2
//
//形式：
//	fx	0	Cx	0
//	0	fy	Cy	0
//	0	0	1	0
//double gIntr[3][4] =
//{
//	362.640120946265, 0, 256.394388868548, 0,
//	0, 363.022773049737, 214.523324945038, 0,
//	0, 0, 1, 0
//};
//
//
////左摄像机畸变
//double Distortion[5] =		//畸变系数	k1,k2,p1,p2,k3
//{ 0.102441601723227, -0.293009316323171, 0.00298144065426142, 0.00101476034794127, 0.105839084598795 };



// NEW KINECT
double gIntr[3][4] =
{
	364.116622852796, 0, 261.002858634368, 0,
	0, 363.641516295353, 207.986347601942, 0,
	0, 0, 1, 0
};

//摄像机畸变
double Distortion[5] =		//畸变系数	k1,k2,p1,p2,k3
{ 0.106247915095759, -0.286241101198733, 0.000923472723957, 0.000830696130348013, 0.0988241724820586 };



//////kinect V2
//double gIntr[3][4] =
//{
//	374.512527046430, 0, 262.432739207866, 0,
//	0, 373.909051744314, 201.890967146750, 0,
//	0, 0, 1, 0
//};
//
////摄像机畸变
//double Distortion[5] =		//畸变系数	k1,k2,p1,p2,k3
//{ 0.104203526347442, -0.295322151461102, 7.99442949973438e-05, 0.00156812499558985, 0.103546791635913 };

//int HitPos[2] = { 0, 0 };

badminton Poslist[MAXTRACK][50000];
int Posnum[MAXTRACK], Picnum = 0, savenum = 0;
ushort depmap[480][640];
CSerialPort mySerialPort;
bool mapb[480][640], mapp[480][640];
ushort listLT[300000][2];
int starttime;
Mat Piclist[30];
Mat image(424, 512, CV_8UC1);
Mat show(424, 512, CV_8UC1);

Mat DepthMatList[ListLong];
Mat imageMatList[ListLong];
int timeList[ListLong];
float RobotStateList[ListLong][3];
Mat saveList[ListLong * 10];
bool canshow = 0;
int canrun = 1;
double NeedToGo[MAXHITLEVEL + 1][5];
int DepthMatHead, DepthMatTail;

double Drowlist[100000][3];

float KinectAngle = 70.0;

unsigned char flynow;
unsigned char passnum;

HUST_CAN CAN;
HUST_ReLocal RELOCAL;
//Coordinate *Coordinate_robot;
vector<enumTable> mResultTable;
vector<enumTable> mResultTableL;
vector<enumTable> mResultTableR;
vector<enumTable> mResultTableLN;
vector<enumTable> mResultTableLW;
vector<enumTable> mResultTableRN;
vector<enumTable> mResultTableRW;
RobotStateAdjustTHAndLA robotStateAdjustTHAndLA = TH_STATE;
bool isPlayBall = false;
char KinectEnable = 0;

bool TrackNow = 0;
bool CanErode = 0;
bool CanErode2 = 0;

/****************************************************************
****美丽的分割线，下面都是康哥使用的，不要删了哦~不然要切JJ的****
*****************************************************************/
ofstream playBallLog;
ofstream LA_ofile;
time_t nowTime;
struct tm* timeInfo;

int yawThresholdLA[2][2] = { { 75, 90 },
                              {90,110} };
int playBallSpeed = 1500;

int yawPos[180];//yaw角度对应的位置

bool isLAEnemyJianCeDao = false;

#ifdef USENET
stringstream gkout;
string str;
#endif
/****************************************************************
****美丽的分割线，上面都是康哥使用的，不要删了哦~不然要切JJ的****
*****************************************************************/