#include "HitBall.h"
#include "TCPSOCKET.H"
#include <ctime>
#define A_CAR
using  namespace std;


struct SpacePoint
{
	double L;
	double W;
	double H;
};

int color = 1;
int Run[10], zerotime, flytime = 0;
int tishengTime = 0, tishengMinTime = 350;
vector<enumTable> resultTable;
bool isTishengEnd = false;
int speed = 0;
int start = 0;



bool isEnablePlayBall = false;
int oneResultMoveCount = 0;
bool oneResultIsFirst = true;
M_playDirect playState = PMID;
enumTable lastResult;
bool isFindResult = false;	//已经找到解
int pitchMin = 0;
int pitchMax = 100;
int yawMin[2];//0;
int yawMax[2];

double ZY_ca;
double ZY_sa;

#define ARRAY_SPD_DEPTH 8
#define ARRAY_AGL_DEPTH 12
#define SPD_MIN 500
#define SPD_STEP 200
#define X_MOVE_LIMIT 2000//1500//X移动限制
#define Y_MOVE_LIMIT 2000//1000//Y移动限制
#define Z_MAX 420 //Z方向所能运动的最大距离
int timeDelay = 75;
int errorDelayTime = 1;// 55;// 1;// 55;
int resetCount = 300;//复位计数
int gaoQiuDelayTimeGR = -10;
int gaoQiuDelayTimeDR = -5;

int gaoQiuDelayTimeGL = -10;
int gaoQiuDelayTimeDL = 0;

//2015.6.10以前的数据
//int jiqiuDelayL_HIGH[ARRAY_SPD_DEPTH][ARRAY_AGL_DEPTH] = {
//	{ 126, 147, 167, 185, 203, 222, 241, 263, 283, 304, 325, 346 },
//	{ 91, 104, 117, 133, 148, 162, 176, 191, 204, 219, 232, 246 },
//	{ 72, 82, 92, 103, 114, 124, 137, 149, 161, 172, 183, 194 },
//	{ 62, 70, 79, 86, 94, 103, 111, 120, 131, 140, 150, 160 },
//	{ 56, 63, 69, 75, 82, 89, 96, 103, 111, 118, 125, 133 },
//	{ 53, 58, 65, 70, 76, 81, 87, 94, 100, 105, 113, 118 },
//	{ 51, 57, 62, 68, 73, 78, 84, 89, 95, 99, 105, 110 },
//};//1500 



int jiqiuDelayL_HIGH[ARRAY_SPD_DEPTH][ARRAY_AGL_DEPTH] = {
	138, 158, 178, 200, 221, 243, 264, 285, 306, 326, 346, 366,
	102, 117, 131, 146, 161, 175, 191, 206, 221, 235, 251, 265,
	84, 95, 106, 117, 129, 140, 151, 163, 174, 186, 198, 210,
	74, 82, 90, 99, 108, 118, 127, 136, 146, 155, 165, 174,
	66, 74, 81, 87, 95, 102, 110, 118, 126, 134, 142, 151,
	60, 67, 74, 80, 86, 92, 98, 105, 112, 119, 126, 133,
	55, 62, 68, 74, 80, 85, 90, 96, 102, 108, 114, 121
	/*{ 135, 155, 176, 196, 217, 238, 264, 284, 305, 326, 347, 367 },
	{ 104, 119, 133, 148, 163, 177, 192, 208, 222, 237, 252, 267 },
	{ 86, 96, 108, 119, 130, 141, 153, 165, 176, 188, 200, 212 },
	{ 76, 84, 93, 101, 111, 120, 129, 137, 148, 157, 167, 176 },
	{ 68, 75, 82, 89, 96, 104, 112, 120, 128, 136, 144, 152 },
	{ 62, 68, 75, 81, 87, 93, 100, 107, 114, 121, 128, 135 },
	{ 57, 64, 70, 75, 81, 86, 92, 97, 103, 110, 116, 122 },
	{ 53, 59, 64, 70, 75, 81, 86, 91, 96, 101, 106, 112 }*/
};


/*

{

	{139,159,179,200,222,243,264,284,305,326,346,366},
	//{ 121, 141, 164, 185, 205, 225, 245, 265, 284, 303, 322, 342 },
	{103,117,131,146,161,176,191,206,220,235,250,265 },
	//{ 89, 102, 115, 130, 147, 160, 176, 190, 205, 219, 233, 246 },
	{85,95,105,117,129,139,151,162,174,186,197,209},
	//{ 73, 82, 92, 103, 113, 124, 134, 147, 159, 171, 183, 194 },
	{75,82,91,100,108,118,127,136,146,156,165,174},
	//{ 63, 71, 78, 86, 94, 102, 111, 119, 128, 137, 147, 158 },
	{66,74,81,88,95,103,111,119,126,135,143,151},
	//{ 56, 63, 70, 76, 82, 89, 95, 101, 110, 116, 125, 132 },
	{60,67,74,81,87,93,99,105,113,120,127,134},
	//{ 53, 58, 64, 69, 75, 80, 86, 91, 98, 104, 109, 117 },
	{56,62,69,75,80,86,91,97,102,109,115,121},
	//{ 51, 56, 62, 67, 73, 77, 82, 88, 93, 97, 102, 107 },
	{53,59,64,70,75,81,86,91,96,101,106,112}
};//1500 
*/



int jiqiuDelayR_HIGH[ARRAY_SPD_DEPTH][ARRAY_AGL_DEPTH] = 
{
	130, 151, 171, 192, 212, 232, 252, 273, 292, 313, 333, 353,
	97, 111, 124, 140, 155, 170, 185, 199, 213, 227, 242, 256,
	80, 90, 101, 110, 122, 133, 146, 157, 168, 179, 191, 202,
	70, 78, 86, 94, 102, 111, 119, 129, 138, 148, 157, 167,
	62, 70, 76, 83, 90, 96, 104, 110, 118, 126, 134, 143,
	56, 63, 69, 75, 81, 87, 92, 98, 105, 111, 118, 125,
	52, 58, 64, 70, 75, 80, 85, 90, 96, 101, 106, 111
	//{129,150,170,189,211,231,251,271,291,311,332,352},
	////{ 127, 146, 165, 184, 205, 224, 246, 266, 287, 308, 328, 350 },
	//{110,123,139,153,167,181,194,210,226,241,256},
	////{ 97, 110, 123, 136, 149, 163, 178, 192, 206, 221, 235, 250 },
	//{78,89,100,110,122,131,143,154,165,178,189,199},
	////{ 81, 91, 101, 111, 120, 131, 142, 152, 164, 175, 186, 197 },
	//{69,77,85,93,101,110,119,128,137,147,156,166},
	////{ 71, 79, 87, 94, 102, 110, 118, 128, 137, 146, 154, 163 },
	//{62,69,76,82,89,96,102,110,118,126,134,142},
	////{ 64, 71, 78, 85, 91, 97, 104, 111, 119, 126, 134, 141 },
	//{57,63,69,75,80,86,93,99,105,111,118,126},
	////{ 58, 65, 71, 77, 83, 89, 94, 100, 106, 113, 119, 126 },
	//{52,59,64,70,75,80,86,90,96,101,106,113},
	////{ 54, 60, 66, 72, 77, 82, 87, 93, 98, 103, 109, 114 },
	//{50,56,61,67,72,77,82,86,91,96,101,106}
};

//int jiqiuDelayL[ARRAY_SPD_DEPTH][ARRAY_AGL_DEPTH] = {
//	120, 139, 159, 181, 203, 224, 244,//500
//	90, 103, 115, 129, 143, 157, 172, //700
//	75, 84, 93, 102, 113, 123, 134,//900
//	67, 75, 82, 89, 97, 105, 112,//1100
//	63, 70, 77, 84, 90, 97, 103,//1300
//	60, 67, 73, 79, 85, 91, 96,//1500
//	60, 67, 73, 79, 85, 91, 96 };//1600 
//
//int jiqiuDelayR[ARRAY_SPD_DEPTH][ARRAY_AGL_DEPTH] = {
//	144, 163, 183, 204, 224, 244, 265,//500
//	110, 124, 138, 153, 167, 181, 196, //700
//	91, 102, 113, 124, 135, 146, 157,//900
//	80, 89, 98, 107, 116, 125, 134,//1100
//	72, 80, 87, 95, 102, 110, 118,//1300
//	68, 75, 82, 88, 95, 102, 107,//1500
//	68, 75, 82, 88, 95, 102, 107 };//1600 

int* playBallDelayTime;


const int yawThreshold[12][2] =
{
	{ 60, 75 }, { 70, 90 }, { 90, 110 }, {100,120 },
	{ 65, 90 }, { 70, 90 }, { 90, 110 }, {95 ,115 },
	{ 70, 90 }, { 70, 90 }, { 90, 110 }, { 95, 110 }
};

const int pitchThreshold[12][2] =
{
	{ 10, 50 }, { 10, 50 }, { 10, 50 }, { 10, 50 },
	{ 10, 50 }, { 10, 50 }, { 10, 50 }, { 10, 50 },
	{ 10, 50 }, { 10, 50 }, { 10, 50 }, { 10, 50 }
};
void FindResult(vector<enumTable>& tableIn, vector<enumTable>& tableOut, float x, float y, float z, int timeRemain, int panel)
{
	enumTable temp;
	float Distance = 0;
#define      K_D            0.8
#define		 K_Z		    0.2
#define		 K_THETA	    0


	//cout << x << "\t" << y << "\t" << z << endl;
	for (int n = 0; n < 2; n++)
	{
		for (int i = yawPos[yawMin[n]]; i < yawPos[yawMax[n]]; i++)
		{
			if (tableIn[i].pitch<pitchMin || tableIn[i].pitch>pitchMax)
			{
				continue;
			}

			if (KinectAngle>69.0)//高球求解限制
			{
				//if (tableIn[i].theta1*tableIn[i].theta2>0) continue;
				if (abs(tableIn[i].theta1)<60) continue;//用挑球或者扣球
			}

			if (timeRemain < 600 && playState!=PMID)//最后的时候不换击球方向
			{
				if (lastResult.playDirect != tableIn[i].playDirect)
				{
					continue;
				}
			}
			
			switch (CAN.towerSwitch)
			{
			case 0:
				continue;
				break;
			case 1://左塔击球
				if (tableIn[i].playDirect == PRN || tableIn[i].playDirect == PRW)
				{
					continue;
				}
				break;
			case 2://右塔击球
				if (tableIn[i].playDirect == PLN || tableIn[i].playDirect == PLW)
				{
					continue;
				}
				break;
			case 3://全塔击球
				break;
			default:
				break;
			}

			int pintchMinTemp = atand(((1600 - z)*sind(tableIn[i].yaw)) / (2000 - y));
			tableIn[i].needMove.z = z - tableIn[i].z;
			
			if (tableIn[i].needMove.z>tableIn[i].ResultZMin && tableIn[i].needMove.z < Z_MAX
				&& abs(tableIn[i].pitch) >= (pintchMinTemp+10)
				)
			{
				if (speed <= 900 && z>750 && abs(tableIn[i].theta1) < 50)
				{
					continue;
				}
				tableIn[i].needMove.x = (x - tableIn[i].x)*ZY_ca - (y - tableIn[i].y)*ZY_sa;
				tableIn[i].needMove.y = (x - tableIn[i].x)*ZY_sa + (y - tableIn[i].y)*ZY_ca;
			
				//playState标记了是否是第一次得到解，如果一次击球没有得到有效解，则，playState位MID，用的 总表，并且求解基于地盘移动最小。
				//否则，求解筛选基于和上次比较的偏差最小
				if (playState == PMID)
				{
					Distance = K_D*sqrt((CAN.m_robotState.robot_xNow - tableIn[i].needMove.x)*(CAN.m_robotState.robot_xNow - tableIn[i].needMove.x) + (CAN.m_robotState.robot_yNow - tableIn[i].needMove.y)*(CAN.m_robotState.robot_yNow - tableIn[i].needMove.y));
				}
				else
				{
					/*Distance = K_X*abs(lastResult.needMove.x - tableIn[i].needMove.x) + K_Y*abs(lastResult.needMove.y - tableIn[i].needMove.y)
						+ K_Z*abs(lastResult.needMove.z - tableIn[i].needMove.z) + K_THETA*abs(lastResult.theta1 - tableIn[i].theta1);*/
					Distance =K_D*sqrt((CAN.m_robotState.robot_xNow - tableIn[i].needMove.x)*(CAN.m_robotState.robot_xNow - tableIn[i].needMove.x) + (CAN.m_robotState.robot_yNow - tableIn[i].needMove.y)*(CAN.m_robotState.robot_yNow - tableIn[i].needMove.y))
					+ K_Z*abs(lastResult.needMove.z - tableIn[i].needMove.z) + K_THETA*abs(lastResult.theta1 - tableIn[i].theta1); 
				}

				//超过运动范围，舍去这个解
				if (abs(tableIn[i].needMove.x - CAN.m_robotState.robot_xNow) > (X_MOVE_LIMIT - CAN.attackScope * 100) || abs(tableIn[i].needMove.y - CAN.m_robotState.robot_yNow) > (Y_MOVE_LIMIT-CAN.attackScope * 100))
				{
					continue;
				}
				

				//标记在这之前是否已经有了可以作对比的解，没有的话默认认为这是一个最优解
				if (oneResultIsFirst)
				{
					tableIn[i].mPos.x = x;
					tableIn[i].mPos.y = y;
					tableIn[i].mPos.z = z;
					if (KinectAngle > 69.0)
					{
						if (tableIn[i].theta1*tableIn[i].theta2>0)//向上打
						{
							if (tableIn[i].playDirect == PLN || tableIn[i].playDirect == PLW)
							{
								tableIn[i].timeLeft = timeRemain - gaoQiuDelayTimeGL;
							}
							else
							{
								tableIn[i].timeLeft = timeRemain - gaoQiuDelayTimeGR;
							}
							
						}
						else//向下打
						{
							if (tableIn[i].playDirect == PLN || tableIn[i].playDirect == PLW)
							{
								tableIn[i].timeLeft = timeRemain - gaoQiuDelayTimeDL;
							}
							else
							{
								tableIn[i].timeLeft = timeRemain - gaoQiuDelayTimeDR;
							}
						}
					
					}
					else
					{
						tableIn[i].timeLeft = timeRemain;
					}
					
					tableIn[i].panel = panel;
					tableIn[i].speed = speed;
					tableOut.push_back(tableIn[i]);
					oneResultMoveCount = Distance;
					oneResultIsFirst = false;
				}
				// 之前已经有一个解了
				else if (Distance < oneResultMoveCount)
				{
					/*if (abs(tableIn[i].theta1 - lastResult.theta1) > 30 && timeRemain<300)
					{
						continue;
					}*/
					//如果两次解的时间变化太大，那么认为是不是会产生很大的跳动，舍弃，第一个条件标记的我已经发出去一个数据了，同时也说明lastResult中是有数据的。
					/*if (playState != PMID  && abs(timeRemain - lastResult.timeLeft) > 200)
					{
						continue;
					}*/
					tableIn[i].mPos.x = x;
					tableIn[i].mPos.y = y;
					tableIn[i].mPos.z = z;
					if (KinectAngle > 69.0)
					{
						if (tableIn[i].theta1*tableIn[i].theta2>0)//向上打
						{
							if (tableIn[i].playDirect == PLN || tableIn[i].playDirect == PLW)
							{
								tableIn[i].timeLeft = timeRemain - gaoQiuDelayTimeGL;
							}
							else
							{
								tableIn[i].timeLeft = timeRemain - gaoQiuDelayTimeGR;
							}

						}
						else//向下打
						{
							if (tableIn[i].playDirect == PLN || tableIn[i].playDirect == PLW)
							{
								tableIn[i].timeLeft = timeRemain - gaoQiuDelayTimeDL;
							}
							else
							{
								tableIn[i].timeLeft = timeRemain - gaoQiuDelayTimeDR;
							}
						}

					}
					else
					{
						tableIn[i].timeLeft = timeRemain;
					}
					tableIn[i].panel = panel;
					tableIn[i].speed = speed;
					tableOut.pop_back();
					tableOut.push_back(tableIn[i]);
					oneResultMoveCount = Distance;
				}
			}
		}
	}
}

void SetPara(int n, float y, float z)
{

	if (playState == PMID && isLAEnemyJianCeDao)
	{
		yawMin[0] = yawThresholdLA[0][0];
		yawMax[0] = yawThresholdLA[0][1];

		yawMin[1] = yawThresholdLA[1][0];
		yawMax[1] = yawThresholdLA[1][1];
		speed = playBallSpeed>1550 ? 1500 : playBallSpeed;
	}

	else if (playState == PMID)
	{
		switch (CAN.playDir)
		{
		case 0://左
			if (CAN.m_robotState.robot_xNow < 0)
			{
				double yawMinTemp = -atand((8000 - CAN.m_robotState.robot_yNow) / CAN.m_robotState.robot_xNow);
				double yawMaxTemp = 180 - atand((8000 - CAN.m_robotState.robot_yNow) / (3000 + CAN.m_robotState.robot_xNow));
				yawMin[0] = yawMinTemp - CAN.m_robotState.angleNow;
				yawMax[0] = yawMaxTemp - 5 - CAN.m_robotState.angleNow;
			}
			else
			{
				double yawMinTemp = 180 - atand((8000 - CAN.m_robotState.robot_yNow) / CAN.m_robotState.robot_xNow);
				double yawMaxTemp = 180 - atand((8000 - CAN.m_robotState.robot_yNow) / (3000 + CAN.m_robotState.robot_xNow));
				yawMin[0] = yawMinTemp - CAN.m_robotState.angleNow;
				yawMax[0] = yawMaxTemp - 5 - CAN.m_robotState.angleNow;
			}
			break;
		case 1://中
			/*if (CAN.m_robotState.robot_xNow < -1500)
			{
				double yawMinTemp = atand((8000 - CAN.m_robotState.robot_yNow) / (1500 - CAN.m_robotState.robot_xNow));
				double yawMaxTemp = atand((8000 - CAN.m_robotState.robot_yNow) / (-1500-CAN.m_robotState.robot_xNow));
				yawMin[0] = yawMinTemp - CAN.m_robotState.angleNow;
				yawMax[0] = yawMaxTemp - CAN.m_robotState.angleNow;
			}
			else if (CAN.m_robotState.robot_xNow > 1500)
			{
				double yawMinTemp = 180 - atand((8000 - CAN.m_robotState.robot_yNow) / (CAN.m_robotState.robot_xNow-1500));
				double yawMaxTemp = 180 - atand((8000 - CAN.m_robotState.robot_yNow) / (1500 + CAN.m_robotState.robot_xNow));
				yawMin[0] = yawMinTemp - CAN.m_robotState.angleNow;
				yawMax[0] = yawMaxTemp - CAN.m_robotState.angleNow;
			}
			else
			{
				double yawMinTemp = atand((8000 - CAN.m_robotState.robot_yNow) / (1500-CAN.m_robotState.robot_xNow));
				double yawMaxTemp = 180 - atand((8000 - CAN.m_robotState.robot_yNow) / (1500 + CAN.m_robotState.robot_xNow));
				yawMin[0] = yawMinTemp - CAN.m_robotState.angleNow;
				yawMax[0] = yawMaxTemp - CAN.m_robotState.angleNow;
			}*/
			yawMin[0] = 89;
			yawMax[0] = 91;
			break;
		case 2://右
			if (CAN.m_robotState.robot_xNow < 0)
			{
				double yawMinTemp = atand((8000 - CAN.m_robotState.robot_yNow) / (3000 - CAN.m_robotState.robot_xNow));
				double yawMaxTemp = -atand((8000 - CAN.m_robotState.robot_yNow) / CAN.m_robotState.robot_xNow);
				yawMin[0] = yawMinTemp + 5 - CAN.m_robotState.angleNow;
				yawMax[0] = yawMaxTemp - CAN.m_robotState.angleNow;
			}
			else
			{
				double yawMinTemp = atand((8000 - CAN.m_robotState.robot_yNow) / (3000 - CAN.m_robotState.robot_xNow));
				double yawMaxTemp = 180 - atand((8000 - CAN.m_robotState.robot_yNow) / CAN.m_robotState.robot_xNow);
				yawMin[0] = yawMinTemp + 5 - CAN.m_robotState.angleNow;
				yawMax[0] = yawMaxTemp - CAN.m_robotState.angleNow;
			}
			break;
		default:
			break;
		}
		yawMin[1] = 0;
		yawMax[1] = 0;
		switch (n)
		{
			//策略球
		case 1:
		case 2:
			speed = 1100;
			break;
		case 5:
		case 6:
			speed = 1700;
		default:
			speed = 1700;
			break;
		}
	}

	speed = 1500;

	//yawMin[0] = 89;
	//yawMax[0] = 91;

}

vector<enumTable>& GetTable(M_playDirect mPlayDirection, float x, float y, float z)
{
	if (x < 0){
		switch ((int)(x / 500))
		{
		case 0:
		case -1:
			if (y > 0)
			{
				//策略球 区域1
				SetPara(1, y, z);
			}
			else
			{
				switch ((int)(y / 2500))
				{
				case 0:
					//区域5
					SetPara(5, y, z);
					break;
				case -1:
					//区域9
					SetPara(9, y, z);
					break;
				default:
					yawMin[0] = 0;
					yawMax[0] = 0;
					yawMin[1] = 0;
					yawMax[1] = 0;
					cout << "gkgk error0" << endl;
					break;
				}
			}
			break;
		case -2:
		case -3:
		case -4:
		case -5:
			if (y > 0)
			{
				// 区域0
				SetPara(0, y, z);
			}
			else
			{
				switch ((int)(y / 2500))
				{
				case 0:
					//区域4
					SetPara(4, y, z);
					break;
				case -1:
					//区域8
					SetPara(8, y, z);
					break;
				default:
					yawMin[0] = 0;
					yawMax[0] = 0;
					yawMin[1] = 0;
					yawMax[1] = 0;
					cout << "gkgk error1" << endl;
					break;
				}
			}
			break;
		default://这样算是不求解
			yawMin[0] = 0;
			yawMax[0] = 0;
			yawMin[1] = 0;
			yawMax[1] = 0;
			cout << "gkgk error2" << endl;
			break;
		}
	}
	else
	{
		switch ((int)(x / 500))
		{
		case 0:
		case 1:
			if (y > 0)
			{
				//策略球 区域2
				SetPara(2, y, z);
			}
			else
			{
				switch ((int)(y / 2500))
				{
				case 0:
					//区域6
					SetPara(6, y, z);
					break;
				case -1:
					//区域10
					SetPara(10, y, z);
					break;
				default:
					yawMin[0] = 0;
					yawMax[0] = 0;
					yawMin[1] = 0;
					yawMax[1] = 0;
					cout << "gkgk error3" << endl;
					break;
				}
			}
			break;
		case 2:
		case 3:
		case 4:
		case 5:
			if (y > 0)
			{
				//区域3
				SetPara(3, y, z);
			}
			else
			{
				switch ((int)(y / 2500))
				{
				case 0:
					//区域7
					SetPara(7, y, z);
					break;
				case -1:
					//区域11
					SetPara(11, y, z);
					break;
				default:
					yawMin[0] = 0;
					yawMax[0] = 0;
					yawMin[1] = 0;
					yawMax[1] = 0;
					cout << "gkgk error4" << endl;
					break;
				}
			}
			break;
		default:
			yawMin[0] = 0;
			yawMax[0] = 0;
			yawMin[1] = 0;
			yawMax[1] = 0;
			cout << "gkgk error5" << endl;
			break;
		}
	}
	return mResultTable;
}


DWORD WINAPI MotorRun(LPVOID lpParamter)    //线程
{
	//int angle;
	//while (cin >> angle)
	//{
	//	CAN.Send_RobotHolder(0x03, angle, 0);
	//}


	KinectAngle = 55 ;
	//KinectAngle = 50;


//	CAN.Send_MotorPositionSet_Frame(6, 30);
	//CAN.Send_MotorPositionSet_Frame(9, -30);
	//CAN.Send_RobotHolder(0x03, KinectAngle, 0);
	////CAN.Send_MotorPositionSet_Frame(4, 200, -0, 300, 30);
	////CAN.Send_MotorPositionSet_Frame(6, 0);
	//

	while (!CAN.isConnect)//Sleep一下这么难？？
	{
		if (CAN.isPowerOff)
		{
			time(&nowTime);//获取当前时间
			timeInfo = localtime(&nowTime);
			playBallLog << endl << 1900 + timeInfo->tm_year << "/" << 1 + timeInfo->tm_mon << "/" << timeInfo->tm_mday
				<< "\t" << timeInfo->tm_hour
				<< ":" << timeInfo->tm_min
				<< ":" << timeInfo->tm_sec << "\t" << "宝越怒按关机!" << endl;
			playBallLog.close();
			system("shutdown -s -t 1");
		}
		Sleep(1);
	}


	/*while (!CAN.isFirstPlayBall)
	{
		Sleep(1);
	}*/


	//CAN.Send_Jiqiu_Frame(6, 10, 0, 500, 0 );
	//CAN.Send_Jiqiu_Frame(9, 10, 0, 500, 0 );

	Sleep(20);
	cout << "motorRun!!!" << endl;
	canrun = 1;
	CAN.Send_MotorPositionSet_Frame(4,200, 0, 200, 0);
	CAN.Send_MotorPositionSet_Frame(6, 0);
	CAN.Send_MotorPositionSet_Frame(9, 0);




	//TH20150806
	CAN.Send_RobotHolder(0x03, KinectAngle, 0);


	//CAN.Send_RobotPositionReset_Frame(1, CAN.resetX, CAN.resetY, 600);

	while (1)
	{
		if (CAN.isPowerOff)
		{
			time(&nowTime);//获取当前时间
			timeInfo = localtime(&nowTime);
			playBallLog << endl << 1900 + timeInfo->tm_year << "/" << 1 + timeInfo->tm_mon << "/" << timeInfo->tm_mday
				<< "\t" << timeInfo->tm_hour
				<< ":" << timeInfo->tm_min
				<< ":" << timeInfo->tm_sec << "\t" << "宝越怒按关机!" << endl;
			playBallLog.close();
			system("shutdown -s -t 1");
		}
		if ((flytime) && (!isPlayBall))
		{
			if (Gettime() - zerotime > flytime - 0)
			{

				Run[6] = 0;
				Run[9] = 0;

				

				

#ifdef USETH
				//置位小状态
				//m_ActionType = Wait;
				//A车可以置位大状态
				//m_StateOfAB = AllWait;
#endif
				
				flytime = 0;
				isPlayBall = true;
				CAN.isFirstPlayBall = false;
				playBallLog << "hit ball !\t" << (int)CAN.isFirstPlayBall << endl;
				cout << "hit ball !\t" << (int)CAN.isFirstPlayBall << endl;
				//canrun = 1;
			}
		}

		if ((tishengTime) && (!isTishengEnd) && playState != PMID)
		{
			if (Gettime() - zerotime > tishengTime - 0)
			{
				//std:gkout << "tishengtisheng" << endl;
				CAN.Send_MotorPositionSet_Frame(4, Run[4], Run[5], Run[7], Run[8]);
				tishengTime = 0;
			//	canrun = 1;
			}
		}
		if (canrun)
		{
			canrun++;
			if (canrun > resetCount)
			{
				if (isPlayBall){
					
					cout << "Reset!!" << CAN.resetX << "\t" << CAN.resetY << endl;
					Run[4] = 200;
					Run[5] = 0;
					Run[6] = 0;

					Run[7] = 200;
					Run[8] = 0;
					Run[9] = 0;

					KinectAngle = 55;
					//KinectAngle = 50;


					Sleep(200);
				//	KinectEnable = 5;

					//TH20150806
					//CAN.Send_RobotHolder(0x03, KinectAngle, 0);


					//CAN.Send_RobotPositionReset_Frame(1, CAN.resetX, CAN.resetY, 600);
					
					CAN.Send_MotorPositionSet_Frame(4, Run[4], Run[5], Run[7], Run[8]);
	
	
#ifdef MOQIU
					CAN.Send_MotorPositionSet_Frame(6, Run[6]);
					CAN.Send_MotorPositionSet_Frame(9, Run[9]);
#endif
					canrun = 1;
					playState = PMID;
					color = (color++) % 15;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color | (0));
					flytime = 0;
					isPlayBall = false;
					isTishengEnd = false;
					isLAEnemyJianCeDao = false; 
					if (CAN.isConnect == false)
					{
						gkout << "CAN ERROR!" << endl;
					}
					else
					{
						CAN.isConnect = false;
					}
				}
				else
				{
#ifdef USE_CAN
					if (CAN.CAN_Error)
					{
						gkout << "can error!";
						CAN.Close();
						if (!CAN.Open(CAN_PORT, 500000))
						{
							gkout << "initCanPort fail !";
						}
						else
						{
							CAN.CAN_Error = false;
						}
					}
					if (CAN.isConnect == false)
					{
						gkout << "CAN ERROR!";
					}
					else
					{
						CAN.isConnect = false;
					}
#endif
				}
				canrun = 1;
			}
			Sleep(1);
			continue;
		}
		start = Gettime();				//运算开始的时间
		if (playState == PMID)
		{
			time(&nowTime);//获取当前时间
			timeInfo = localtime(&nowTime);
			playBallLog << endl << 1900 + timeInfo->tm_year << "/" << 1 + timeInfo->tm_mon << "/" << timeInfo->tm_mday
				<< "\t" << timeInfo->tm_hour
				<< ":" << timeInfo->tm_min
				<< ":" << timeInfo->tm_sec << "\t" << "ready getResult and detectEnable is " << (int)CAN.detectEnable << "isPlayBall is " << (int)isPlayBall << endl;
		}

		if (CAN.isFirstPlayBall)
		{
			if (CAN.isFirstPlayBallChujie)
			{
				cout << "出界了出界了！！吓死啦吓死啦！！" << endl;
				CAN.Send_Jiqiu_Frame(0x06, 0, 0, 200, 0);
				CAN.Send_Jiqiu_Frame(0x09, 0, 0, 200, 0);
				CAN.Send_MotorPositionSet_Frame(4, 200, 0, 200, 0);

				flytime = 0;
				tishengTime = 0;
				isPlayBall = false;
				isTishengEnd = false;
				isLAEnemyJianCeDao = false;
				playState = PMID;
				Run[4] = 200;
				Run[5] = 0;
				Run[6] = 0;

				Run[7] = 200;
				Run[8] = 0;
				Run[9] = 0;
				/*for (int i = MINHITLEVEL; i <= MAXHITLEVEL; i++)
				{
					NeedToGo[i][4] = 0.009;
				}*/
				canrun = 1;
				continue;
			}
		}
		if (CAN.detectEnable)
		{
			GetResultPlayBall();
			isEnablePlayBall = true;
		}
		else if (!CAN.detectEnable && isEnablePlayBall)
		{
			playBallLog << "buyunxujiance" << endl;
			cout << "buyunxujiance" << endl;
			isEnablePlayBall = false;
			flytime = 0;
			tishengTime = 0;
			isPlayBall = false;
			isTishengEnd = false;
			isLAEnemyJianCeDao = false;
			playState = PMID;
			Run[4] = 200;
			Run[5] = 0;
			Run[6] = 0;

			Run[7] = 200;
			Run[8] = 0;
			Run[9] = 0;
			//for (int i = MINHITLEVEL; i <= MAXHITLEVEL; i++)
			//{
			//	NeedToGo[i][4] = 0.009;
			//}
		}
		else
		{
			cout << "play ball already or detectNOTEnable" << endl;
			playBallLog << "play ball already or detectNOTEnable!" << endl;
			/*for (int i = MINHITLEVEL; i <= MAXHITLEVEL; i++)
			{
				NeedToGo[i][4] = 0.009;
			}*/
		}
		canrun = 1;
	}
	return 0;
}

void GetResultPlayBall()
{
	static int lastTime = Gettime();
	bool isDelayTimeError = false;
	resultTable.clear();
	
	for (int i = MINHITLEVEL; i < MAXHITLEVEL; i += 5)
	{
		double zy_globalX = -NeedToGo[i][1];
		double zy_globalY = NeedToGo[i][0];
		double zy_globalZ = NeedToGo[i][2];
		double zy_alpha = 0.0;
		//if (abs(CAN.m_robotState.angleNow) <= 3)
		//{
		//	zy_alpha = 0.0;
		//}
		//else
		//{
			zy_alpha = CAN.m_robotState.angleNow*pi / 180.0;
		//}
		double ca = cos(zy_alpha);
		double sa = sin(zy_alpha);

		ZY_ca = ca;
		ZY_sa = sa;

		double zy_localX = zy_globalX*ca + zy_globalY*sa;
		double zy_localY = -zy_globalX*sa + zy_globalY*ca;
		double zy_localZ = NeedToGo[i][2];

		FindResult(GetTable(playState, -NeedToGo[i][1], NeedToGo[i][0], NeedToGo[i][2]), resultTable, zy_localX, zy_localY, zy_localZ, NeedToGo[i][3], i);
	//	FindResult(GetTable(playState, -NeedToGo[i][1], NeedToGo[i][0], NeedToGo[i][2]), resultTable, -NeedToGo[i][1], NeedToGo[i][0], NeedToGo[i][2], NeedToGo[i][3], i);
	}
	if (resultTable.size()>0)
	{
		int num = ((abs(resultTable[0].theta2) - 60) / 10);
#ifdef A_CAR
		num<0 ? 0 : (num>11 ? 11 : num);
#else
		num<0 ? 0 : (num>6 ? 6 : num);
#endif
		cout << Gettime() << endl;
		if (((Gettime() - lastTime) - (lastResult.timeLeft - resultTable[0].timeLeft) > 66) && (playState != PMID))
		{
			//cout << "挂了挂了挂了,吓尿了~~NOW:" << resultTable[0].mPos.z << "\t LAST:" << lastResult.mPos.z << "\t" << resultTable[0].timeLeft << endl;
			if (abs(lastResult.z - resultTable[0].z)<100)
			{
				isDelayTimeError = true;
				//cout << "*************" << endl;
			}
			//cout << "timea	" << Gettime() - timeA << endl;
		}

		playState = resultTable[0].playDirect;		//记录击球的方向
		zerotime = Gettime();//获取当前时间，计算结束的时间
		speed = resultTable[0].speed;

		switch (playState)
		{
		case PLN:
		case PLW:
			//击球剩余时间为：解里头的剩余时间-图像延时-击球所需时间-求解时间-各种误差时间
			if (isDelayTimeError){
				playBallDelayTime = &jiqiuDelayL_HIGH[0][0];
				tishengTime = lastResult.timeLeft - timeDelay - (zerotime - start) - errorDelayTime - tishengMinTime - (Gettime() - lastTime);
				flytime = lastResult.timeLeft - timeDelay - *(playBallDelayTime + (ARRAY_AGL_DEPTH*(int)((speed - SPD_MIN) / SPD_STEP) + num)) - (zerotime - start) - errorDelayTime - (Gettime() - lastTime);
			}
			else
			{
				playBallDelayTime = &jiqiuDelayL_HIGH[0][0];
				tishengTime = resultTable[0].timeLeft - timeDelay - (zerotime - start) - errorDelayTime - tishengMinTime;
				flytime = resultTable[0].timeLeft - timeDelay - *(playBallDelayTime + (ARRAY_AGL_DEPTH*(int)((speed - SPD_MIN) / SPD_STEP) + num)) - (zerotime - start) - errorDelayTime;
			}
			break;
		case PRN:
		case PRW:
			//击球剩余时间为：解里头的剩余时间-图像延时-击球所需时间-求解时间-各种误差时间
			if (isDelayTimeError){
				playBallDelayTime = &jiqiuDelayR_HIGH[0][0];
				tishengTime = lastResult.timeLeft - timeDelay - (zerotime - start) - errorDelayTime - tishengMinTime - (Gettime() - lastTime);
				flytime = lastResult.timeLeft - timeDelay - *(playBallDelayTime + (ARRAY_AGL_DEPTH*(int)((speed - SPD_MIN) / SPD_STEP) + num)) - (zerotime - start) - errorDelayTime - (Gettime() - lastTime);
			}
			else
			{
				playBallDelayTime = &jiqiuDelayR_HIGH[0][0];
				tishengTime = resultTable[0].timeLeft - timeDelay - (zerotime - start) - errorDelayTime - tishengMinTime;
				flytime = resultTable[0].timeLeft - timeDelay - *(playBallDelayTime + (ARRAY_AGL_DEPTH*(int)((speed - SPD_MIN) / SPD_STEP) + num)) - (zerotime - start) - errorDelayTime;
			}
			break;
		default:
			break;
		}
		//cout << Gettime() << endl;
		if (isPlayBall && (flytime > 100))
		{
			//cout << "吓死了吓死了！！！！！\t" << flytime << endl;
		}
		if ((resultTable[0].timeLeft > (timeDelay + errorDelayTime + (Gettime() - start) + 100))//底盘运动时间小于100ms不发了
			&& (!isPlayBall || (flytime > 100 && flytime <1500))													//去除万一检测失误出现的不发的情况
			)
		{	
			isPlayBall = false;
			isTishengEnd = false;

			if (CAN.isFirstPlayBall)
			{
				if (resultTable[0].timeLeft < 800)
				{
					if (isDelayTimeError)
					{
						CAN.Send_RobotPosition_Frame(0x01, (short)resultTable[0].needMove.x, (short)resultTable[0].needMove.y, (short)lastResult.timeLeft - timeDelay - (zerotime - start) - errorDelayTime - (Gettime() - lastTime));
					}
					else
					{
						CAN.Send_RobotPosition_Frame(0x01, (short)resultTable[0].needMove.x, (short)resultTable[0].needMove.y, (short)resultTable[0].timeLeft - timeDelay - (zerotime - start) - errorDelayTime);
					}
				}
			}
			else
			{
				if (isDelayTimeError)
				{
					CAN.Send_RobotPosition_Frame(0x01, (short)resultTable[0].needMove.x, (short)resultTable[0].needMove.y, (short)lastResult.timeLeft - timeDelay - (zerotime - start) - errorDelayTime -(Gettime() - lastTime));
					//cout <<"error	"<< (short)lastResult.timeLeft - timeDelay - (zerotime - start) - errorDelayTime - (Gettime() - lastTime) << endl;
				}
				else
				{
					CAN.Send_RobotPosition_Frame(0x01, (short)resultTable[0].needMove.x, (short)resultTable[0].needMove.y, (short)resultTable[0].timeLeft - timeDelay - (zerotime - start) - errorDelayTime);
					//cout <<"OK	" <<(short)resultTable[0].timeLeft - timeDelay - (zerotime - start) - errorDelayTime << endl;
				}

			}
			lastTime = Gettime();



			//TH20151114
			//决策机失效
			//给另一台车发送目标位置，用于躲避
			//KinectSerialPort.AddDataToSend(AimPosFrame, AGet_HighBall, (short)resultTable[0].needMove.x, (short)resultTable[0].needMove.y, (short)resultTable[0].timeLeft - timeDelay - (zerotime - start) - errorDelayTime);




			switch (playState)
			{
			case PLN:
			case PLW:
				Run[6] = -resultTable[0].theta2;
				Run[9] = 0;
				Run[4] = resultTable[0].needMove.z;
				Run[5] = -(resultTable[0].theta1);
				Run[7] = resultTable[0].needMove.z;
				Run[8] = 0; //-resultTable[0].theta1;
#ifdef MOQIU
				CAN.Send_MotorPositionSet_Frame(6, Run[6]);
				CAN.Send_MotorPositionSet_Frame(9, Run[9]);
#endif

				gkout << "L:" << (int)resultTable[0].theta1 << "\t" << (int)resultTable[0].theta2 << "\t"
					<< (int)resultTable[0].needMove.x << "\t" << (int)resultTable[0].needMove.y << "\t" << (int)resultTable[0].needMove.z << "\t"
					<< "TIME LEFT:" << (int)resultTable[0].timeLeft - timeDelay << "\t" << (int)resultTable[0].yaw << "\t" << (int)resultTable[0].pitch << "\t" << flytime << endl;
				playBallLog << resultTable[0].mPos.x << "\t" << resultTable[0].mPos.y << "\t" << resultTable[0].mPos.z << "\t" << resultTable[0].speed << "\t" << resultTable[0].needMove.x - CAN.m_robotState.robot_xNow << "\t" << resultTable[0].needMove.y - CAN.m_robotState.robot_yNow << endl;
				playBallLog << "L:" << (int)resultTable[0].theta1 << "\t" << (int)resultTable[0].theta2 << "\t"
					<< (int)resultTable[0].needMove.x << "\t" << (int)resultTable[0].needMove.y << "\t" << (int)resultTable[0].needMove.z << "\t"
					<< "TIME LEFT:" << (int)resultTable[0].timeLeft - timeDelay - errorDelayTime << "\t" << (int)resultTable[0].yaw << "\t" << (int)resultTable[0].pitch << "\t" << flytime << endl;//TCPSendString(sockClientKinect, str, STRING_PACK);
				break;
			case PRN:
			case PRW:
				Run[6] = 0;
				Run[9] = -resultTable[0].theta2;
				Run[4] = resultTable[0].needMove.z;
				Run[5] = 0;
				Run[7] = resultTable[0].needMove.z;
				Run[8] = -(resultTable[0].theta1);

#ifdef MOQIU
				CAN.Send_MotorPositionSet_Frame(6, Run[6]);
				CAN.Send_MotorPositionSet_Frame(9, Run[9]);
#endif
				gkout << "R:" << (int)resultTable[0].theta1 << "\t" << (int)resultTable[0].theta2 << "\t"
					<< (int)resultTable[0].needMove.x << "\t" << (int)resultTable[0].needMove.y << "\t" << (int)resultTable[0].needMove.z << "\t"
					<< "TIME LEFT:" << (int)resultTable[0].timeLeft - timeDelay << "\t" << (int)resultTable[0].yaw << "\t" << (int)resultTable[0].pitch << "\t" << flytime << endl;
				playBallLog << resultTable[0].mPos.x << "\t" << resultTable[0].mPos.y << "\t" << resultTable[0].mPos.z << "\t" << resultTable[0].speed << "\t" << resultTable[0].needMove.x - CAN.m_robotState.robot_xNow << "\t" << resultTable[0].needMove.y - CAN.m_robotState.robot_yNow << endl;
				playBallLog << "R:" << (int)resultTable[0].theta1 << "\t" << (int)resultTable[0].theta2 << "\t"
					<< (int)resultTable[0].needMove.x << "\t" << (int)resultTable[0].needMove.y << "\t" << (int)resultTable[0].needMove.z << "\t"
					<< "TIME LEFT:" << (int)resultTable[0].timeLeft - timeDelay - errorDelayTime << "\t" << (int)resultTable[0].yaw << "\t" << (int)resultTable[0].pitch << "\t" << flytime << endl;//TCPSendString(sockClientKinect, str, STRING_PACK);
				break;
			default:
				break;
			}
		/*	CAN.Send_MotorPositionSet_Frame(5, Run[5]);
			CAN.Send_MotorPositionSet_Frame(8, Run[8]);*/
#ifndef MOQIU
#ifdef A_CAR

			
			//TH20150923
			cout<<"KinectAngle"<<KinectAngle<<endl;

			CAN.Send_Jiqiu_Frame(6, Run[6], abs(Run[6]) * 0.5, speed, flytime-2);
			CAN.Send_Jiqiu_Frame(9, Run[9], abs(Run[9]) * 0.5, speed, flytime-2);
#else
			CAN.Send_Jiqiu_Frame(6, Run[6], abs(Run[6]) * 0.5 > 20 ? 20 : abs(Run[6]) * 0.5, speed, flytime);
			CAN.Send_Jiqiu_Frame(9, Run[9], abs(Run[9]) * 0.5 > 20 ? 20 : abs(Run[9]) * 0.5, speed, flytime);
#endif
#endif
		}
		else
		{
			//CAN.Send_RobotPosition_Frame(0x01, (short)resultTable[0].needMove.x, (short)resultTable[0].needMove.y, (short)resultTable[0].timeLeft - timeDelay - (zerotime - start) - errorDelayTime);
			//CAN.Send_MotorPositionSet_Frame(4, Run[4], Run[5], Run[7], Run[8]);
			playBallLog << "TIME TOO SHORT:" << (int)resultTable[0].timeLeft - timeDelay - errorDelayTime << endl;
			gkout << "TIME TOO SHORT:" << (int)resultTable[0].timeLeft - timeDelay - errorDelayTime << endl;
			isTishengEnd = true;
		}
		lastResult = resultTable[0];
	}
	else
	{
		gkout << "NOT find result" << endl;
		playBallLog << "NOT find result and postion,now position:" << CAN.m_robotState.robot_xNow << "\t" << CAN.m_robotState.robot_yNow << endl;
		for (int i = MINHITLEVEL; i <= MAXHITLEVEL; i += 5)
		{
			playBallLog << (int)-NeedToGo[i][1] << "\t" << (int)NeedToGo[i][0] << "\t" << (int)NeedToGo[i][2] << endl;
		}
	}
	oneResultIsFirst = true;//一组平面解完之后的移动最小的解置零

	//for (int i = MINHITLEVEL; i <= MAXHITLEVEL; i++)
	//{
	//	NeedToGo[i][4] = 0.009;
	//}

	canrun = 1;

	gkout << "getResult:" << Gettime() - start << endl;
	playBallLog << "getResult:" << Gettime() - start << endl;
}