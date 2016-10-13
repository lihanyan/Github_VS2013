#include "GetResult.h"


//以Z为关键字进行排序（快排）

void QuickSort_Z(vector<enumTable>& a, int p, int r)
{
	int Partition_Z(vector<enumTable>& a, int p, int r);			//以X为关键字进行排序内部调用
	if (p<r)
	{
		int q = Partition_Z(a, p, r);
		QuickSort_Z(a, p, q - 1);
		QuickSort_Z(a, q + 1, r);
	}
}
int Partition_Z(vector<enumTable>& a, int p, int r)
{
	int i = p - 1;
	float z = a[r - 1].z;
	for (int j = p; j<r; j++)
	{
		if (a[j - 1].z <= z)
		{
			i = i + 1;
			enumTable temp;
			temp = a[j - 1];
			a[j - 1] = a[i - 1];
			a[i - 1] = temp;
		}
	}
	enumTable temp;
	temp = a[i];
	a[i] = a[r - 1];
	a[r - 1] = temp;
	return i + 1;
}



int GetInt(double num)
{
	if (num < 0)
	{
		return (int)(num - 0.5);
	}
	return (int)(num + 0.5);
}

#define THETA2MIN 60
#define THETA2MAX 170
#define A_CAR
//#define B_CAR
#ifdef A_CAR
void CreateTable(vector<enumTable>& mTable, vector<enumTable>& mTableL, vector<enumTable>& mTableLN, vector<enumTable>& mTableLW, vector<enumTable>& mTableR, vector<enumTable>& mTableRN, vector<enumTable>& mTableRW)
{

	ZY_Vector testVector;
	Theta  testTheta;
	Position testPosition;
	enumTable correctResult;
	M_playDirect playDir;

	//左塔内侧
	for (int i = -70; i <= 70; i++)	//旋转电机
	{
		for (int j = THETA2MIN; j <= THETA2MAX; j += 10)
		{
			double row;
			double pitch;
			testTheta.theta1 = i / 180.0*PI;
			testTheta.theta2 = j / 180.0*PI;
			testVector = ForwardSolutionR(testTheta);
			testPosition = ForwardSolutionPositionL(testTheta);
			row = atan2(testVector.y, testVector.x);
			if (row<0)
			{
				row = PI + row;
			}
			pitch = asin(testVector.z);
			if ((testVector.y>0)
				//&& (pitch / PI * 180 >= 10) && (pitch / PI * 180<=60)
				&& (abs(row / PI * 180) >= 60) && (abs(row / PI * 180) <= 120))
			{
				correctResult.theta1 = GetInt(testTheta.theta1 / PI * 180);
				correctResult.theta2 = GetInt(testTheta.theta2 / PI * 180);
				correctResult.x = testPosition.x;
				correctResult.y = testPosition.y;
				correctResult.z = testPosition.z;
				correctResult.yaw = GetInt(row / PI * 180);
				correctResult.pitch = GetInt(pitch / PI * 180);
				correctResult.playDirect = PLN;

				mTable.push_back(correctResult);
				mTableL.push_back(correctResult);
				mTableLN.push_back(correctResult);
			}

			testTheta.theta1 = i / 180.0*PI;
			testTheta.theta2 = j / 180.0*PI;
			testVector = ForwardSolutionR(testTheta);
			testPosition = ForwardSolutionPositionR(testTheta);
			row = atan2(testVector.y, testVector.x);
			if (row<0)
			{
				row = PI + row;
			}
			pitch = asin(testVector.z);
			if ((testVector.y>0)
				//&& (pitch / PI * 180 >= 10) && (pitch / PI * 180 <= 60)
				&& (abs(row / PI * 180) >= 60) && (abs(row / PI * 180) <= 120))
			{
				correctResult.theta1 = GetInt(testTheta.theta1 / PI * 180);
				correctResult.theta2 = GetInt(testTheta.theta2 / PI * 180);
				correctResult.x = testPosition.x;
				correctResult.y = testPosition.y;
				correctResult.z = testPosition.z;
				correctResult.yaw = GetInt(row / PI * 180);
				correctResult.pitch = GetInt(pitch / PI * 180);
				correctResult.playDirect = PRW;

				mTable.push_back(correctResult);
				mTableR.push_back(correctResult);
				mTableRW.push_back(correctResult);
			}
		}
	}

	//左塔外侧
	for (int i = -70; i <= 70; i++)	//旋转电机
	{
		for (int j = -THETA2MIN; j >= -THETA2MAX; j -= 10)
		{
			double row;
			double pitch;
			testTheta.theta1 = i / 180.0*PI;
			testTheta.theta2 = j / 180.0*PI;
			testVector = ForwardSolutionL(testTheta);
			testPosition = ForwardSolutionPositionL(testTheta);
			row = atan2(testVector.y, testVector.x);
			if (row<0)
			{
				row = PI + row;
			}
			pitch = asin(testVector.z);
			if ((testVector.y>0)
				//&& (pitch / PI * 180 >= 10) && (pitch / PI * 180 <= 60)
				//&& (abs(row / PI * 180) >= 60) && (abs(row / PI * 180) <= 120)
				)
			{
				correctResult.theta1 = GetInt(testTheta.theta1 / PI * 180);
				correctResult.theta2 = GetInt(testTheta.theta2 / PI * 180);
				correctResult.x = testPosition.x;
				correctResult.y = testPosition.y;
				correctResult.z = testPosition.z;
				correctResult.yaw = GetInt(row / PI * 180);
				correctResult.pitch = GetInt(pitch / PI * 180);
				correctResult.playDirect = PLW;

				mTable.push_back(correctResult);
				mTableL.push_back(correctResult);
				mTableLW.push_back(correctResult);
			}


			testTheta.theta1 = i / 180.0*PI;
			testTheta.theta2 = j / 180.0*PI;
			testVector = ForwardSolutionL(testTheta);
			testPosition = ForwardSolutionPositionR(testTheta);
			row = atan2(testVector.y, testVector.x);
			if (row<0)
			{
				row = PI + row;
			}
			pitch = asin(testVector.z);
			if ((testVector.y>0)
				//&& (pitch / PI * 180 >= 10) && (pitch / PI * 180 <= 60)
				&& (abs(row / PI * 180) >= 60) && (abs(row / PI * 180) <= 120))
			{
				correctResult.theta1 = GetInt(testTheta.theta1 / PI * 180);
				correctResult.theta2 = GetInt(testTheta.theta2 / PI * 180);
				correctResult.x = testPosition.x;
				correctResult.y = testPosition.y;
				correctResult.z = testPosition.z;
				correctResult.yaw = GetInt(row / PI * 180);
				correctResult.pitch = GetInt(pitch / PI * 180);
				correctResult.playDirect = PRN;

				mTable.push_back(correctResult);
				mTableR.push_back(correctResult);
				mTableRN.push_back(correctResult);
			}
		}
	}

}
#else
#ifdef B_CAR
void CreatTable(vector<enumTable>& mTable, vector<enumTable>& mTableL, vector<enumTable>& mTableLN, vector<enumTable>& mTableLW, vector<enumTable>& mTableR, vector<enumTable>& mTableRN, vector<enumTable>& mTableRW)
{

	ZY_Vector testVector;
	Theta  testTheta;
	Position testPosition;
	enumTable correctResult;
	M_playDirect playDir;

	//左塔内侧
	for (int i = -80; i <= 80; i++)	//旋转电机
	{
		for (int j = THETA2MIN; j <= THETA2MAX; j += 10)
		{
			double row;
			double pitch;
			testTheta.theta1 = i / 180.0*PI;
			testTheta.theta2 = j / 180.0*PI;
			testVector = ForwardSolutionR(testTheta);
			testPosition = ForwardSolutionPositionL(testTheta);
			row = atan2(testVector.y, testVector.x);
			if (row<0)
			{
				row = PI + row;
			}
			pitch = asin(testVector.z);
			if ((testVector.y>0)
				&& (pitch / PI * 180 >= 10) && (pitch / PI * 180 <= 60)
				&& (abs(row / PI * 180) >= 60) && (abs(row / PI * 180) <= 120))
			{
				correctResult.theta1 = GetInt(testTheta.theta1 / PI * 180);
				correctResult.theta2 = GetInt(testTheta.theta2 / PI * 180);
				correctResult.x = testPosition.x;
				correctResult.y = testPosition.y;
				correctResult.z = testPosition.z;
				correctResult.yaw = GetInt(row / PI * 180);
				correctResult.pitch = GetInt(pitch / PI * 180);
				correctResult.playDirect = PLN;

				mTable.push_back(correctResult);
				mTableL.push_back(correctResult);
				mTableLN.push_back(correctResult);
			}

			testTheta.theta1 = i / 180.0*PI;
			testTheta.theta2 = j / 180.0*PI;
			testVector = ForwardSolutionR(testTheta);
			testPosition = ForwardSolutionPositionR(testTheta);
			row = atan2(testVector.y, testVector.x);
			if (row<0)
			{
				row = PI + row;
			}
			pitch = asin(testVector.z);
			if ((testVector.y>0)
				&& (pitch / PI * 180 >= 10) && (pitch / PI * 180 <= 60)
				&& (abs(row / PI * 180) >= 60) && (abs(row / PI * 180) <= 120))
			{
				correctResult.theta1 = GetInt(testTheta.theta1 / PI * 180);
				correctResult.theta2 = GetInt(testTheta.theta2 / PI * 180);
				correctResult.x = testPosition.x;
				correctResult.y = testPosition.y;
				correctResult.z = testPosition.z;
				correctResult.yaw = GetInt(row / PI * 180);
				correctResult.pitch = GetInt(pitch / PI * 180);
				correctResult.playDirect = PRW;

				mTable.push_back(correctResult);
				mTableR.push_back(correctResult);
				mTableRW.push_back(correctResult);
			}
		}
	}

	//左塔外侧
	for (int i = -80; i <= 80; i++)	//旋转电机
	{
		for (int j = -THETA2MIN; j >= -THETA2MAX; j -= 10)
		{
			double row;
			double pitch;
			testTheta.theta1 = i / 180.0*PI;
			testTheta.theta2 = j / 180.0*PI;
			testVector = ForwardSolutionL(testTheta);
			testPosition = ForwardSolutionPositionL(testTheta);
			row = atan2(testVector.y, testVector.x);
			if (row<0)
			{
				row = PI + row;
			}
			pitch = asin(testVector.z);
			if ((testVector.y>0)
				&& (pitch / PI * 180 >= 10) && (pitch / PI * 180 <= 60)
				&& (abs(row / PI * 180) >= 60) && (abs(row / PI * 180) <= 120))
			{
				correctResult.theta1 = GetInt(testTheta.theta1 / PI * 180);
				correctResult.theta2 = GetInt(testTheta.theta2 / PI * 180);
				correctResult.x = testPosition.x;
				correctResult.y = testPosition.y;
				correctResult.z = testPosition.z;
				correctResult.yaw = GetInt(row / PI * 180);
				correctResult.pitch = GetInt(pitch / PI * 180);
				correctResult.playDirect = PLW;

				mTable.push_back(correctResult);
				mTableL.push_back(correctResult);
				mTableLW.push_back(correctResult);
			}


			testTheta.theta1 = i / 180.0*PI;
			testTheta.theta2 = j / 180.0*PI;
			testVector = ForwardSolutionL(testTheta);
			testPosition = ForwardSolutionPositionR(testTheta);
			row = atan2(testVector.y, testVector.x);
			if (row<0)
			{
				row = PI + row;
			}
			pitch = asin(testVector.z);
			if ((testVector.y>0)
				&& (pitch / PI * 180 >= 10) && (pitch / PI * 180 <= 60)
				&& (abs(row / PI * 180) >= 60) && (abs(row / PI * 180) <= 120))
			{
				correctResult.theta1 = GetInt(testTheta.theta1 / PI * 180);
				correctResult.theta2 = GetInt(testTheta.theta2 / PI * 180);
				correctResult.x = testPosition.x;
				correctResult.y = testPosition.y;
				correctResult.z = testPosition.z;
				correctResult.yaw = GetInt(row / PI * 180);
				correctResult.pitch = GetInt(pitch / PI * 180);
				correctResult.playDirect = PRN;

				mTable.push_back(correctResult);
				mTableR.push_back(correctResult);
				mTableRN.push_back(correctResult);
			}
		}
	}

}
#endif
#endif

//以Y为关键字进行排序（快排）

void QuickSort_BYaw(vector<enumTable>& a, int p, int r)
{
	int Partition_BYaw(vector<enumTable>& a, int p, int r);			//以X为关键字进行排序内部调用
	if (p<r)
	{
		int q = Partition_BYaw(a, p, r);
		QuickSort_BYaw(a, p, q - 1);
		QuickSort_BYaw(a, q + 1, r);
	}
}
int Partition_BYaw(vector<enumTable>& a, int p, int r)
{
	int i = p - 1;
	float yaw = a[r - 1].yaw;
	for (int j = p; j<r; j++)
	{
		if (a[j - 1].yaw <= yaw)
		{
			i = i + 1;
			enumTable temp;
			temp = a[j - 1];
			a[j - 1] = a[i - 1];
			a[i - 1] = temp;
		}
	}
	enumTable temp;
	temp = a[i];
	a[i] = a[r - 1];
	a[r - 1] = temp;
	return i + 1;
}
void ResetYawPos(vector<enumTable>& tableIn, int* yawPos)
{
	int posNow = 0;
	if (tableIn.size() < 1)
	{
		return;
	}
	for (int i = 0; i < tableIn.size(); i++)
	{
		for (int j = posNow; j <= tableIn[i].yaw; j++)
		{
			yawPos[posNow] = (i - 1)>0 ? i - 1 : 0;
			posNow++;
		}
	}
	for (int i = posNow; i < 180; i++)
	{
		yawPos[i] = yawPos[i - 1];
	}
}
