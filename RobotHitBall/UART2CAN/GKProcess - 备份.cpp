#include <iostream>
#include "stdafx.h"
#include "GKProcess.h"
#include <math.h>
#include <time.h>
#include<fstream>
using namespace std;

#define __GKDEBUG

#define PI 3.1415926
#define HUIPAI_R 450
#define l1 100
#define l2 100
#define l3 100
#define l4 100

#define X_MIN -300
#define X_MAX 300
#define Z_MIN 0
#define Z_MAX 800
#define Z_MINMID 330
#define Z_MAXMID 650

M_pos State2Pos_A(M_motorState m_motorState)
{

	M_pos m_pos;
	m_pos.x = (int)(m_motorState.m_m3 - 135 * cos((double)m_motorState.m_m4) + 450 * sin((double)m_motorState.m_m4)*sin((double)m_motorState.m_m5));
	m_pos.y = (int)((407.8385 * cos((double)m_motorState.m_m5)) - 57.0535 * sin((double)m_motorState.m_m4) - (190.1782 * cos((double)m_motorState.m_m4)*sin((double)m_motorState.m_m5)) + 210);
	m_pos.z = (int)(190.1782	* cos((double)m_motorState.m_m5) + 122.3516 * sin((double)m_motorState.m_m4) + (407.8385* cos((double)m_motorState.m_m4)*sin((double)m_motorState.m_m5)) + 840);
	return m_pos;
}

//基于视觉的机器人定位及通信系统设计

M_pos State2Pos_B(M_motorState m_motorState) //电机状态到空间位置的转换，枚举用，B车
{
	M_pos m_pos;
	m_pos.x = (int)(((double)m_motorState.m_m3 + 100 * sin(PI*m_motorState.m_m5 / 180) - 450 * cos(PI*m_motorState.m_m5 / 180)*sin(PI*m_motorState.m_m6 / 180)));
	m_pos.y = (int)((-((double)154032179003468525 * cos(PI*m_motorState.m_m5 / 180)) / 4503599627370496 - (476099925391599975 * cos(PI*m_motorState.m_m6 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*m_motorState.m_m5 / 180)*sin(PI*m_motorState.m_m6 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
	m_pos.z = (int)(((double)m_motorState.m_m4 + (52899991710177775 * cos(PI*m_motorState.m_m5 / 180)) / 562949953421312 - (1386289611031216725 * cos(PI*m_motorState.m_m6 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI*m_motorState.m_m5 / 180)*sin(PI*m_motorState.m_m6 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104));
	std::cout << "output:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" <<  endl;
	return m_pos;
}

M_motorState Pos2State2D_B2(M_pos m_pos)
{
	M_motorState m_motorState;
	m_motorState.m_m3 = 0;
	m_motorState.m_m4 = 0;
	m_motorState.m_m5 = 0;
	m_motorState.m_m6 = 0;
	double y = 0.0, z = 0.0;
	int start = clock();
	bool isData = false;
	if (m_pos.x < 0)
	{
		
		for (int i = Z_MIN; i < Z_MAX; i += 2)	//m4距离
		{
			if (i < Z_MINMID)
			{
				m_motorState.m_m6 = 90;
				for (int j = -3; j < 3; j++)		//m5角度
				{
					z = (int)(((double)i + (52899991710177775 * cos(PI*j / 180)) / 562949953421312 - (1386289611031216725 * cos(PI * m_motorState.m_m6 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI * j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104));
					if (abs(z - m_pos.z) < 10){
						y = (int)((-((double)154032179003468525 * cos(PI * j / 180)) / 4503599627370496 - (476099925391599975 * cos(PI * m_motorState.m_m6 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
						if (abs(y - m_pos.y) < 50)
						{
							m_motorState.m_m4 = i;
							m_motorState.m_m5 = j;
							m_motorState.m_m3 = m_pos.x - (100 * sin(PI*m_motorState.m_m5 / 180) - 450 * cos(PI*m_motorState.m_m5 / 180)*sin(PI*m_motorState.m_m6 / 180));					
#ifdef __GKDEBUG
							std::cout <<"input:\t"<< m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << endl;
							cout << "m3:" << m_motorState.m_m3 << "\t" << "m4:" << m_motorState.m_m4 << "\t" << "m5:" << m_motorState.m_m5 << "\t" << "m6:" << m_motorState.m_m6 << endl;
							State2Pos_B(m_motorState);
							cout << endl << endl;
#else
							isData = true;
							break;
#endif
						}
					}
				}
			}
			else if (i > Z_MAXMID)
			{
				m_motorState.m_m6 = -90;
				for (int j = -180; j < -120; j++)		//m5角度
				{
					z = (int)(((double)i + (52899991710177775 * cos(PI*j / 180)) / 562949953421312 - (1386289611031216725 * cos(PI * m_motorState.m_m6 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI * j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104));
					if (abs(z - m_pos.z) < 10){
						y = (int)((-((double)154032179003468525 * cos(PI * j / 180)) / 4503599627370496 - (476099925391599975 * cos(PI * m_motorState.m_m6 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
						if (abs(y - m_pos.y) < 50)
						{


							m_motorState.m_m4 = i;
							m_motorState.m_m5 = j;
							m_motorState.m_m3 = m_pos.x - (100 * sin(PI*m_motorState.m_m5 / 180) - 450 * cos(PI*m_motorState.m_m5 / 180)*sin(PI*m_motorState.m_m6 / 180));
#ifdef __GKDEBUG
							std::cout << "input:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << endl;
							cout << "m3:" << m_motorState.m_m3 << "\t" << "m4:" << m_motorState.m_m4 << "\t" << "m5:" << m_motorState.m_m5 << "\t" << "m6:" << m_motorState.m_m6 << endl;
							State2Pos_B(m_motorState);
							cout << endl << endl;
#else
							isData = true;
							break;
#endif
						}
					}
				}
			}
			else
			{
				m_motorState.m_m6 = 90;
				for (int j = -70; j < 20; j++)		//m5角度
				{
					z = (int)(((double)i + (52899991710177775 * cos(PI*j / 180)) / 562949953421312 - (1386289611031216725 * cos(PI * m_motorState.m_m6 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI * j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104));
					if (abs(z - m_pos.z) < 10){
						y = (int)((-((double)154032179003468525 * cos(PI * j / 180)) / 4503599627370496 - (476099925391599975 * cos(PI * m_motorState.m_m6 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
						if (abs(y - m_pos.y) < 50)
						{


							m_motorState.m_m4 = i;
							m_motorState.m_m5 = j;
							m_motorState.m_m3 = m_pos.x - (100 * sin(PI*m_motorState.m_m5 / 180) - 450 * cos(PI*m_motorState.m_m5 / 180)*sin(PI*m_motorState.m_m6 / 180));
#ifdef __GKDEBUG
							std::cout << "input:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << endl;
							cout << "m3:" << m_motorState.m_m3 << "\t" << "m4:" << m_motorState.m_m4 << "\t" << "m5:" << m_motorState.m_m5 << "\t" << "m6:" << m_motorState.m_m6 << endl;
							State2Pos_B(m_motorState);
							cout << endl << endl;
#else
							isData = true;
							break;
#endif
						}
					}
				}
			}
			if (isData)
			{
				break;
			}
		}
	}
	else
	{	
		for (int i = Z_MIN; i < Z_MAX; i += 2)	//m3距离
		{
			if (i < Z_MINMID)
			{
				m_motorState.m_m6 = -90;
				for (int j = -3; j < 3; j++)		//m5角度
				{
					z = (int)(((double)i + (52899991710177775 * cos(PI*j / 180)) / 562949953421312 - (1386289611031216725 * cos(PI * m_motorState.m_m6 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI * j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104));
					if (abs(z - m_pos.z) < 10){
						y = (int)((-((double)154032179003468525 * cos(PI * j / 180)) / 4503599627370496 - (476099925391599975 * cos(PI * m_motorState.m_m6 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
						if (abs(y - m_pos.y) < 50)
						{


							m_motorState.m_m4 = i;
							m_motorState.m_m5 = j;
							m_motorState.m_m3 = m_pos.x - (100 * sin(PI*m_motorState.m_m5 / 180) - 450 * cos(PI*m_motorState.m_m5 / 180)*sin(PI*m_motorState.m_m6 / 180));
#ifdef __GKDEBUG
							std::cout << "input:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << endl;
							cout << "m3:" << m_motorState.m_m3 << "\t" << "m4:" << m_motorState.m_m4 << "\t" << "m5:" << m_motorState.m_m5 << "\t" << "m6:" << m_motorState.m_m6 << endl;
							State2Pos_B(m_motorState);
							cout << endl << endl;
#else
							isData = true;
							break;
#endif
						}
					}
				}
			}
			else if (i > Z_MAXMID)
			{
				m_motorState.m_m6 = 90;
				for (int j = 120; j < 180; j++)		//m5角度
				{
					z = (int)(((double)i + (52899991710177775 * cos(PI*j / 180)) / 562949953421312 - (1386289611031216725 * cos(PI * m_motorState.m_m6 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI * j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104));
					if (abs(z - m_pos.z) < 10){
						y = (int)((-((double)154032179003468525 * cos(PI * j / 180)) / 4503599627370496 - (476099925391599975 * cos(PI * m_motorState.m_m6 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
						if (abs(y - m_pos.y) < 50)
						{


							m_motorState.m_m4 = i;
							m_motorState.m_m5 = j;
							m_motorState.m_m3 = m_pos.x - (100 * sin(PI*m_motorState.m_m5 / 180) - 450 * cos(PI*m_motorState.m_m5 / 180)*sin(PI*m_motorState.m_m6 / 180));
#ifdef __GKDEBUG
							std::cout << "input:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << endl;
							cout << "m3:" << m_motorState.m_m3 << "\t" << "m4:" << m_motorState.m_m4 << "\t" << "m5:" << m_motorState.m_m5 << "\t" << "m6:" << m_motorState.m_m6 << endl;
							State2Pos_B(m_motorState);
							cout << endl << endl;
#else
							isData = true;
							break;
#endif
						}
					}
				}
			}
			for (int j = -20; j < 70; j++)		//m5角度
			{
				m_motorState.m_m6 = -90;
				z = (int)(((double)i + (52899991710177775 * cos(PI*j / 180)) / 562949953421312 - (1386289611031216725 * cos(PI*m_motorState.m_m6 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI * j / 180)*sin(PI*m_motorState.m_m6 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104));
				if (abs(z - m_pos.z) < 10){
					y = (int)((-((double)154032179003468525 * cos(PI * j / 180)) / 4503599627370496 - (476099925391599975 * cos(PI *m_motorState.m_m6 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*j / 180)*sin(PI *m_motorState.m_m6 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
					if (abs(y - m_pos.y) < 50)
					{
						
						m_motorState.m_m4 = i;
						m_motorState.m_m5 = j;
						m_motorState.m_m3 = m_pos.x - (100 * sin(PI*m_motorState.m_m5 / 180) - 450 * cos(PI*m_motorState.m_m5 / 180)*sin(PI*m_motorState.m_m6 / 180));
#ifdef __GKDEBUG
						std::cout << "input:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << endl;
						cout << "m3:" << m_motorState.m_m3 << "\t" << "m4:" << m_motorState.m_m4 << "\t" << "m5:" << m_motorState.m_m5 << "\t" << "m6:" << m_motorState.m_m6 << endl;
						State2Pos_B(m_motorState);
						cout << endl << endl;
#else
						isData = true;
						break;
#endif
					}
				}
			}
			if (isData)
			{
				break;
			}
		}
	}
	cout << "using time:" << clock() - start << endl << endl << endl;
	//m_motorState.m_m6 = 0;
	return m_motorState;
}
M_motorState Pos2State2D_B(M_pos m_pos)		//状态到电机状态的转换  二代B车前方比利时机构
{
	M_motorState m_motorState;
	ofstream fout("meiju.txt");
	m_motorState.m_m3 = 0; 
	m_motorState.m_m4 = 0;
	m_motorState.m_m5 = 0;
	m_motorState.m_m6 = 0;
	double x = 0.0, y = 0.0;
	int start = clock();
	bool isData = false;
	for (int count = 0; count < 100; count++)
	{
		if (m_pos.x < 0)
		{
			//		m_motorState.m_m4 = m_pos.z > 480 ? m_pos.z - 480 : 0;
			m_motorState.m_m6 = 90;
			for (int i = X_MIN; i < X_MAX; i += 10)	//m3距离
			{
				for (int j = -70; j < 20; j += 2)		//m5角度
				{
					//x = (int)(((double)i + 100 * sin(PI*j / 180) - 450 * cos(PI*j / 180)*sin(PI*90 / 180)));
					x = (int)(((double)i + 100 * sin(PI*j / 180) - 450 * cos(PI*j / 180)*sin(PI * 90 / 180)));
					if (abs(x - m_pos.x) < 10){
						y = (int)((-((double)154032179003468525 * cos(PI * j / 180)) / 4503599627370496 - (476099925391599975 * cos(PI * 90 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*j / 180)*sin(PI * 90 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
						if (abs(y - m_pos.y) < 10)
						{
							m_motorState.m_m3 = i;
							m_motorState.m_m4 = m_pos.z - (int)(((double)52899991710177775 * cos(PI*j / 180)) / 562949953421312 - (1386289611031216725 * cos(PI*90 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI*j / 180)*sin(PI*90 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104);
							m_motorState.m_m5 = j;
#ifdef __GKDEBUG
							fout << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t"  << m_motorState.m_m3 << "\t" << m_motorState.m_m4 << "\t" << m_motorState.m_m5 << "\t" << m_motorState.m_m6 << endl;
							std::cout << "input:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << endl;
							cout << "m3:" << m_motorState.m_m3 << "\t" << "m4:" << m_motorState.m_m4 << "\t" << "m5:" << m_motorState.m_m5 << "\t" << "m6:" << m_motorState.m_m6 << endl;
							State2Pos_B(m_motorState);
							cout << endl << endl;
#else
							isData = true;
#endif
						}
					}
				}
				if (isData)
				{
					break;
				}
			}
		}
		else
		{
			m_motorState.m_m6 = -90;
			for (int i = X_MIN; i < X_MAX; i += 10)	//m3距离
			{
				for (int j = -20; j < 70; j += 2)		//m5角度
				{
					//x = (int)(((double)i + 100 * sin(PI*j / 180) - 450 * cos(PI*j / 180)*sin(PI*90 / 180)));
					x = (int)(((double)i + 100 * sin(PI*j / 180) - 450 * cos(PI*j / 180)*sin(PI *m_motorState.m_m6 / 180)));
					if (abs(x - m_pos.x) < 10){
						y = (int)((-((double)154032179003468525 * cos(PI * j / 180)) / 4503599627370496 - (476099925391599975 * cos(PI * m_motorState.m_m6 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
						if (abs(y - m_pos.y) < 10)
						{
							m_motorState.m_m3 = i;
							m_motorState.m_m4 = m_pos.z - (int)(((double)52899991710177775 * cos(PI*j / 180)) / 562949953421312 - (1386289611031216725 * cos(PI * m_motorState.m_m6 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI*j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104);
							m_motorState.m_m5 = j;
#ifdef __GKDEBUG
							fout << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << m_motorState.m_m3 << "\t" << m_motorState.m_m4 << "\t" << m_motorState.m_m5 << "\t" << m_motorState.m_m6 << endl;
							std::cout << "input:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << endl;
							cout << "m3:" << m_motorState.m_m3 << "\t" << "m4:" << m_motorState.m_m4 << "\t" << "m5:" << m_motorState.m_m5 << "\t" << "m6:" << m_motorState.m_m6 << endl;
							State2Pos_B(m_motorState);
							cout << endl << endl;
#else
							isData = true;
#endif
						}
					}
				}
				if (isData)
				{
					break;
				}
			}
		}
	}
	cout << "using time:" << clock() - start << endl;
	//m_motorState.m_m6 = 0;
	fout.close();
	return m_motorState;
}

M_motorState Pos2State2D_BT2(M_pos m_pos)		//状态到电机状态的转换  二代B车前方比利时机构
{
	M_motorState m_motorState;
	m_motorState.m_m3 = 0;
	m_motorState.m_m4 = 0;
	m_motorState.m_m5 = 0;
	m_motorState.m_m6 = 0;
	double x = 0.0, y = 0.0;
	int start = clock();
	bool isData = false;
	if (m_pos.x < 0)
	{
		//		m_motorState.m_m4 = m_pos.z > 480 ? m_pos.z - 480 : 0;
		m_motorState.m_m6 = 90;
		for (int i = X_MIN; i < X_MAX; i += 2)	//m3距离
		{
			for (int j = -20; j < 70; j++)		//m5角度
			{
				//x = (int)(((double)i + 100 * sin(PI*j / 180) - 450 * cos(PI*j / 180)*sin(PI*90 / 180)));
				x = (int)(((double)i + 100 * sin(PI*j / 180) - 450 * cos(PI*j / 180)*sin(PI * 90 / 180)));
				if (abs(x - m_pos.x) < 10){
					y = (int)((-((double)154032179003468525 * cos(PI * j / 180)) / 4503599627370496 - (476099925391599975 * cos(PI * 90 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*j / 180)*sin(PI * 90 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
					if (abs(y - m_pos.y) < 10)
					{
						m_motorState.m_m3 = i;
						m_motorState.m_m4 = m_pos.z - (int)(((double)52899991710177775 * cos(PI*j / 180)) / 562949953421312 - (1386289611031216725 * cos(PI * 90 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI*j / 180)*sin(PI * 90 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104);
						m_motorState.m_m5 = j;
#ifdef __GKDEBUG
						std::cout << "input:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << endl;
						cout << "m3:" << m_motorState.m_m3 << "\t" << "m4:" << m_motorState.m_m4 << "\t" << "m5:" << m_motorState.m_m5 << "\t" << "m6:" << m_motorState.m_m6 << endl;
						State2Pos_B(m_motorState);
						cout << endl << endl;
#else
						isData = true;
#endif
					}
				}
			}
			if (isData)
			{
				break;
			}
		}
	}
	else
	{
		m_motorState.m_m6 = -90;
		for (int i = X_MIN; i < X_MAX; i += 2)	//m3距离
		{
			for (int j = -70; j < 20; j++)		//m5角度
			{
				//x = (int)(((double)i + 100 * sin(PI*j / 180) - 450 * cos(PI*j / 180)*sin(PI*90 / 180)));
				x = (int)(((double)i + 100 * sin(PI*j / 180) - 450 * cos(PI*j / 180)*sin(PI *m_motorState.m_m6 / 180)));
				if (abs(x - m_pos.x) < 5){
					y = (int)((-((double)154032179003468525 * cos(PI * j / 180)) / 4503599627370496 - (476099925391599975 * cos(PI * m_motorState.m_m6 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
					if (abs(y - m_pos.y) < 5)
					{
						m_motorState.m_m3 = i;
						m_motorState.m_m4 = m_pos.z - (int)(((double)52899991710177775 * cos(PI*j / 180)) / 562949953421312 - (1386289611031216725 * cos(PI * m_motorState.m_m6 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI*j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104);
						m_motorState.m_m5 = j;
#ifdef __GKDEBUG
						std::cout << "input:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << endl;
						cout << "m3:" << m_motorState.m_m3 << "\t" << "m4:" << m_motorState.m_m4 << "\t" << "m5:" << m_motorState.m_m5 << "\t" << "m6:" << m_motorState.m_m6 << endl;
						State2Pos_B(m_motorState);
						cout << endl << endl;
#else
						isData = true;
#endif
					}
				}
			}
			if (isData)
			{
				break;
			}
		}
	}
	//cout << "using time:" << clock() - start << endl;
	//m_motorState.m_m6 = 0;
	return m_motorState;
}


M_motorState Pos2State2D_BT(M_pos m_pos)		//状态到电机状态的转换  二代B车前方比利时机构
{
	M_motorState m_motorState;
	m_motorState.m_m3 = 0;
	m_motorState.m_m4 = 0;
	m_motorState.m_m5 = 0;
	m_motorState.m_m6 = 0;
	double y = 0.0, z = 0.0;
	int start = clock();
	bool isData = false;
	if (m_pos.x < 0)
	{

		for (int i = Z_MIN; i < Z_MAX; i += 2)	//m4距离
		{
			if (i < Z_MINMID)
			{
				m_motorState.m_m6 = 90;
				for (int j = -3; j < 3; j++)		//m5角度
				{
					z = (int)(((double)i + (52899991710177775 * cos(PI*j / 180)) / 562949953421312 - (1386289611031216725 * cos(PI * m_motorState.m_m6 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI * j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104));
					if (abs(z - m_pos.z) < 10){
						y = (int)((-((double)154032179003468525 * cos(PI * j / 180)) / 4503599627370496 - (476099925391599975 * cos(PI * m_motorState.m_m6 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
						if (abs(y - m_pos.y) < 10)
						{
							m_motorState.m_m4 = i;
							m_motorState.m_m5 = j;
							m_motorState.m_m3 = m_pos.x - (100 * sin(PI*m_motorState.m_m5 / 180) - 450 * cos(PI*m_motorState.m_m5 / 180)*sin(PI*m_motorState.m_m6 / 180));
#ifdef __GKDEBUG
							std::cout << "input:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << endl;
							cout << "m3:" << m_motorState.m_m3 << "\t" << "m4:" << m_motorState.m_m4 << "\t" << "m5:" << m_motorState.m_m5 << "\t" << "m6:" << m_motorState.m_m6 << endl;
							State2Pos_B(m_motorState);
							cout << endl << endl;
#else
							isData = true;
#endif
						}
					}
				}
			}
			else if (i > Z_MAXMID)
			{
				m_motorState.m_m6 = -90;
				for (int j = 70; j < 180; j++)		//m5角度
				{
					z = (int)(((double)i + (52899991710177775 * cos(PI*j / 180)) / 562949953421312 - (1386289611031216725 * cos(PI * m_motorState.m_m6 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI * j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104));
					if (abs(z - m_pos.z) < 10){
						y = (int)((-((double)154032179003468525 * cos(PI * j / 180)) / 4503599627370496 - (476099925391599975 * cos(PI * m_motorState.m_m6 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
						if (abs(y - m_pos.y) < 10)
						{


							m_motorState.m_m4 = i;
							m_motorState.m_m5 = j;
							m_motorState.m_m3 = m_pos.x - (100 * sin(PI*m_motorState.m_m5 / 180) - 450 * cos(PI*m_motorState.m_m5 / 180)*sin(PI*m_motorState.m_m6 / 180));
#ifdef __GKDEBUG
							std::cout << "input:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << endl;
							cout << "m3:" << m_motorState.m_m3 << "\t" << "m4:" << m_motorState.m_m4 << "\t" << "m5:" << m_motorState.m_m5 << "\t" << "m6:" << m_motorState.m_m6 << endl;
							State2Pos_B(m_motorState);
							cout << endl << endl;
#else
							isData = true;
#endif
						}
					}
				}
			}
			else
			{
				m_motorState.m_m6 = 90;
				for (int j = -20; j < 70; j++)		//m5角度
				{
					z = (int)(((double)i + (52899991710177775 * cos(PI*j / 180)) / 562949953421312 - (1386289611031216725 * cos(PI * m_motorState.m_m6 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI * j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104));
					if (abs(z - m_pos.z) < 10){
						y = (int)((-((double)154032179003468525 * cos(PI * j / 180)) / 4503599627370496 - (476099925391599975 * cos(PI * m_motorState.m_m6 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
						if (abs(y - m_pos.y) < 10)
						{


							m_motorState.m_m4 = i;
							m_motorState.m_m5 = j;
							m_motorState.m_m3 = m_pos.x - (100 * sin(PI*m_motorState.m_m5 / 180) - 450 * cos(PI*m_motorState.m_m5 / 180)*sin(PI*m_motorState.m_m6 / 180));
#ifdef __GKDEBUG
							std::cout << "input:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << endl;
							cout << "m3:" << m_motorState.m_m3 << "\t" << "m4:" << m_motorState.m_m4 << "\t" << "m5:" << m_motorState.m_m5 << "\t" << "m6:" << m_motorState.m_m6 << endl;
							State2Pos_B(m_motorState);
							cout << endl << endl;
#else
							isData = true;
#endif
						}
					}
				}
			}
			if (isData)
			{
				break;
			}
		}
	}
	else
	{

		for (int i = Z_MIN; i < Z_MAX; i += 2)	//m3距离
		{
			if (i < Z_MINMID)
			{
				m_motorState.m_m6 = -90;
				for (int j = -3; j < 3; j++)		//m5角度
				{
					z = (int)(((double)i + (52899991710177775 * cos(PI*j / 180)) / 562949953421312 - (1386289611031216725 * cos(PI * m_motorState.m_m6 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI * j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104));
					if (abs(z - m_pos.z) < 10){
						y = (int)((-((double)154032179003468525 * cos(PI * j / 180)) / 4503599627370496 - (476099925391599975 * cos(PI * m_motorState.m_m6 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
						if (abs(y - m_pos.y) < 10)
						{


							m_motorState.m_m4 = i;
							m_motorState.m_m5 = j;
							m_motorState.m_m3 = m_pos.x - (100 * sin(PI*m_motorState.m_m5 / 180) - 450 * cos(PI*m_motorState.m_m5 / 180)*sin(PI*m_motorState.m_m6 / 180));
#ifdef __GKDEBUG
							std::cout << "input:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << endl;
							cout << "m3:" << m_motorState.m_m3 << "\t" << "m4:" << m_motorState.m_m4 << "\t" << "m5:" << m_motorState.m_m5 << "\t" << "m6:" << m_motorState.m_m6 << endl;
							State2Pos_B(m_motorState);
							cout << endl << endl;
#else
							isData = true;
#endif
						}
					}
				}
			}
			else if (i > Z_MAXMID)
			{
				m_motorState.m_m6 = 90;
				for (int j = -180; j < -70; j++)		//m5角度
				{
					z = (int)(((double)i + (52899991710177775 * cos(PI*j / 180)) / 562949953421312 - (1386289611031216725 * cos(PI * m_motorState.m_m6 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI * j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104));
					if (abs(z - m_pos.z) < 10){
						y = (int)((-((double)154032179003468525 * cos(PI * j / 180)) / 4503599627370496 - (476099925391599975 * cos(PI * m_motorState.m_m6 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*j / 180)*sin(PI * m_motorState.m_m6 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
						if (abs(y - m_pos.y) < 10)
						{


							m_motorState.m_m4 = i;
							m_motorState.m_m5 = j;
							m_motorState.m_m3 = m_pos.x - (100 * sin(PI*m_motorState.m_m5 / 180) - 450 * cos(PI*m_motorState.m_m5 / 180)*sin(PI*m_motorState.m_m6 / 180));
#ifdef __GKDEBUG
							std::cout << "input:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << endl;
							cout << "m3:" << m_motorState.m_m3 << "\t" << "m4:" << m_motorState.m_m4 << "\t" << "m5:" << m_motorState.m_m5 << "\t" << "m6:" << m_motorState.m_m6 << endl;
							State2Pos_B(m_motorState);
							cout << endl << endl;
#else
							isData = true;
#endif
						}
					}
				}
			}
			for (int j = -70; j < 20; j++)		//m5角度
			{
				m_motorState.m_m6 = -90;
				z = (int)(((double)i + (52899991710177775 * cos(PI*j / 180)) / 562949953421312 - (1386289611031216725 * cos(PI*m_motorState.m_m6 / 180)) / 9007199254740992 + (476099925391599975 * sin(PI * j / 180)*sin(PI*m_motorState.m_m6 / 180)) / 1125899906842624 + 1657432747656513 / 4398046511104));
				if (abs(z - m_pos.z) < 10){
					y = (int)((-((double)154032179003468525 * cos(PI * j / 180)) / 4503599627370496 - (476099925391599975 * cos(PI *m_motorState.m_m6 / 180)) / 1125899906842624 - (1386289611031216725 * sin(PI*j / 180)*sin(PI *m_motorState.m_m6 / 180)) / 9007199254740992 - 5620624119206389 / 35184372088832));
					if (abs(y - m_pos.y) < 10)
					{

						m_motorState.m_m4 = i;
						m_motorState.m_m5 = j;
						m_motorState.m_m3 = m_pos.x - (100 * sin(PI*m_motorState.m_m5 / 180) - 450 * cos(PI*m_motorState.m_m5 / 180)*sin(PI*m_motorState.m_m6 / 180));
#ifdef __GKDEBUG
						std::cout << "input:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" << endl;
						cout << "m3:" << m_motorState.m_m3 << "\t" << "m4:" << m_motorState.m_m4 << "\t" << "m5:" << m_motorState.m_m5 << "\t" << "m6:" << m_motorState.m_m6 << endl;
						State2Pos_B(m_motorState);
						cout << endl << endl;
#else
						isData = true;
#endif
					}
				}
			}
			if (isData)
			{
				break;
			}
		}
	}
	cout << "using time:" << clock() - start << endl << endl << endl;
	//m_motorState.m_m6 = 0;
	return m_motorState;
}
M_motorState Pos2State2D(int x, int y, int z, HUST_CAN* CAN)
{
	M_motorState  m_motorState;

	int O1B = 0;
	//
	m_motorState.m_m4 = 0;
	m_motorState.m_m5 = 0;
	m_motorState.m_m3 = 0;
	if ((x > (325 + HUIPAI_R)) || (x < (-325 - HUIPAI_R)))	// || (y > (380)) ||(y < (19)) || (z > (1227)) ||(z < (453)))
	{
		//		MotorA.PositionExpected = 0;
		//		STATE = PIDPOSITION;
		return m_motorState;
	}

	O1B = (int)(sqrt((double)((x - 325)*(x - 325) + (y - 200)*(y - 200) + (z - 840)*(z - 840))));
	//
	if (O1B >= HUIPAI_R)
	{
		m_motorState.m_m3 = (int)(x + sqrt((double)(450 * 450) - (y - 200)*(y - 200) - (z - 840)*(z - 840)));
		m_motorState.m_m4 = (int)(asin((double)(sqrt((double)(y - 200)*(y - 200) + (z - 840)*(z - 840)) / HUIPAI_R)) / 3.1415926 * 180);
		//		param_BWM[3] = alpha;
		if (y < 200)
		{
			m_motorState.m_m4 = -m_motorState.m_m4;
		}
	}

	else
	{
		m_motorState.m_m3 = (int)(x - sqrt((double)(450 * 450) - (y - 200)*(y - 200) - (z - 840)*(z - 840)));
		m_motorState.m_m4 = (int)(asin(sqrt((double)(y - 200)*(y - 200) + (z - 840)*(z - 840)) / HUIPAI_R) / 3.1415926 * 180);
		if (y > 200)
		{
			m_motorState.m_m4 = 180 - m_motorState.m_m4;
		}
		else
		{
			m_motorState.m_m4 = m_motorState.m_m4 - 180;
		}
	}
	return m_motorState;
}
M_motorState Pos2State2D(M_pos m_pos, HUST_CAN *CAN){
	M_motorState  m_motorState;
	
	int O1B = 0;
	//
	m_motorState.m_m4 = 0;
	m_motorState.m_m5 = 0;
	m_motorState.m_m3 = 0;
	m_motorState.m_m6 = 0;
	if ((m_pos.x > (325 + HUIPAI_R)) || (m_pos.x < (-325 - HUIPAI_R)))	// || (y > (380)) ||(y < (19)) || (z > (1227)) ||(z < (453)))
	{
//		MotorA.PositionExpected = 0;
//		STATE = PIDPOSITION;
		return m_motorState;
	}

	O1B = (int)(sqrt((double)((m_pos.x - 325)*(m_pos.x - 325) + (m_pos.y - 200)*(m_pos.y - 200) + (m_pos.z - 840)*(m_pos.z - 840))));
	//
	if (O1B >= HUIPAI_R)
	{
		m_motorState.m_m3 = (int)(m_pos.x + sqrt((double)(450 * 450) - (m_pos.y - 200)*(m_pos.y - 200) - (m_pos.z - 840)*(m_pos.z - 840)));
		m_motorState.m_m4 = (int)(asin((double)(sqrt((double)(m_pos.y - 200)*(m_pos.y - 200) + (m_pos.z - 840)*(m_pos.z - 840)) / HUIPAI_R)) / 3.1415926 * 180);
//		param_BWM[3] = alpha;
		if (m_pos.y < 200)
		{
			m_motorState.m_m4 = -m_motorState.m_m4;
		}
	}

	else
	{
		m_motorState.m_m3 = (int)(m_pos.x - sqrt((double)(450 * 450) - (m_pos.y - 200)*(m_pos.y - 200) - (m_pos.z - 840)*(m_pos.z - 840)));
		m_motorState.m_m4 = (int)(asin(sqrt((double)(m_pos.y - 200)*(m_pos.y - 200) + (m_pos.z - 840)*(m_pos.z - 840)) / HUIPAI_R) / 3.1415926 * 180);
		if (m_pos.y > 200)
		{
			m_motorState.m_m4 = 180 - m_motorState.m_m4;
		}
		else
		{
			m_motorState.m_m4 = m_motorState.m_m4 - 180;
		}
	}
	return m_motorState;
}