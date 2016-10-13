#include <iostream>
#include "stdafx.h"
#include "GKProcess.h"
#include <math.h>
#include <time.h>
#include <fstream>
using namespace std;

//#define __GKDEBUG

#define PI 3.1415926
#define HUIPAI_R  480  //450
#define MY_L1     107    //���Ӽгֵ㵽��ת������ĵľ���
#define MY_ANGLE  18.8   

#define ZY_H  (235.0-90)//210//235 //m2���ߵ������Ǧ������
#define ZY_L1 185.0 //m4���ߵ�m5���ߵ�Ǧ������
#define ZY_L2 225.0 //б�߳���
#define ZY_L3 105.0 //���ļгֵ㵽m5���߾���  
#define ZY_L4 470.0//480 //�������ĵ㵽���ļгֵ����
#define ZY_Theta 29.0 //�ߵ����

#define l1 100
#define l2 100
#define l3 100
#define l4 100
#define _180TOPI  0.01745329 

#define X_MIN     -500
#define X_MAX     600
#define Z_MIN     230
#define Z_MAX     800       //800
#define Z_MINMID  527
#define Z_MAXMID  680     //����������

#define DATADEPTH 10000

#define yMoveMax 500
M_motorStatePosEnum enumelateArray[enumArrayNum];

M_pos State2Pos_A(M_motorState m_motorState)
{

	M_pos m_pos;
	m_pos.x = (int)(m_motorState.m_m3 - 135 * cos((double)m_motorState.m_m4) + 450 * sin((double)m_motorState.m_m4)*sin((double)m_motorState.m_m5));
	m_pos.y = (int)((407.8385 * cos((double)m_motorState.m_m5)) - 57.0535 * sin((double)m_motorState.m_m4) - (190.1782 * cos((double)m_motorState.m_m4)*sin((double)m_motorState.m_m5)) + 210);
	m_pos.z = (int)(190.1782	* cos((double)m_motorState.m_m5) + 122.3516 * sin((double)m_motorState.m_m4) + (407.8385* cos((double)m_motorState.m_m4)*sin((double)m_motorState.m_m5)) + 840);
	return m_pos;
}

//�����Ӿ��Ļ����˶�λ��ͨ��ϵͳ���

M_pos State2Pos_B(M_motorState m_motorState) //���״̬���ռ�λ�õ�ת����ö���ã�B��
{
	M_pos m_pos;
	m_pos.x = (int)(m_motorState.m_m3 + ZY_L3 * sin(_180TOPI*m_motorState.m_m5) - ZY_L4 * cos(_180TOPI*m_motorState.m_m5) * sin(_180TOPI*(m_motorState.m_m6))); //x����ZY_L3,ZY_L4��Ӱ�졣
	m_pos.y = (int)(-ZY_L2*cos(_180TOPI*ZY_Theta) - ZY_L4*cos(_180TOPI*(m_motorState.m_m6))*cos(_180TOPI*ZY_Theta) - ZY_L3*cos(_180TOPI*m_motorState.m_m5)*sin(_180TOPI*ZY_Theta) - ZY_L4*sin(_180TOPI*m_motorState.m_m5)*sin(_180TOPI*(m_motorState.m_m6))*sin(_180TOPI*ZY_Theta));
	m_pos.z = (int)((double)m_motorState.m_m4 - (ZY_L2*sin(_180TOPI*ZY_Theta) - ZY_L3*cos(_180TOPI*m_motorState.m_m5)*cos(_180TOPI*ZY_Theta) + ZY_L4*cos(_180TOPI*(m_motorState.m_m6))*sin(_180TOPI*ZY_Theta) - ZY_L4*cos(_180TOPI*ZY_Theta)*sin(_180TOPI*m_motorState.m_m5)*sin(_180TOPI*(m_motorState.m_m6)) - (ZY_L1 + ZY_H)));
	/*std::cout << "output:\t" << m_pos.x << "\t" << m_pos.y << "\t" << m_pos.z << "\t" <<  endl;*/
	return m_pos;
}


/*******************************************************************************
* Function Name  : judgeMotorState
* Description    : ��ö�ٽ�������ж�����ȥ�����ö�ٽ��
* Input          : m_motorState�����ж���ö�ٽ�
* Output         : void
* Return         : bool ������ⲻ���棬�򷵻�true
*******************************************************************************/
inline bool judgeMotorState(M_motorState& m_motorState)
{
	int AngleLimit = 79.66*sin(0.003669*m_motorState.m_m4 - 0.8346); // ��ϵõ�
	// ���ɶ�����г�����
	if ((m_motorState.m_m3 >X_MAX) || (m_motorState.m_m3< X_MIN) || (m_motorState.m_m4>Z_MAX) || (m_motorState.m_m4<Z_MIN))
	{
		return false;
	}
	if (m_motorState.m_m4 < Z_MINMID) //Ҫ����ת������������
	{
		if (m_motorState.m_m6 >0)
		{
			if (m_motorState.m_m5 < -AngleLimit)
			{
				return false;
			}
		}
		else
		{
			if (m_motorState.m_m5 > AngleLimit)
			{
				return false;
			}
		}
	}

	if (m_motorState.m_m4 > Z_MAXMID) // ��ֹ������
	{
		if (m_motorState.m_m6 >0)
		{
			if (m_motorState.m_m5 > 10)
			{
				return false;
			}
		}
		else
		{
			if (m_motorState.m_m5 < -10)
			{
				return false;
			}
		}
	}

	return true;
}


/*******************************************************************************
* Function Name  : Pos2StateB_Array
* Description    : ����Ŀ��λ�ò��õ���
* Input          : m_pos��������Ŀ��λ�� ��m_motorState�� ���״̬�� ��enumelate ������������ָ��
* Output         : void
* Return         : bool ���������ڣ��򷵻�true
*******************************************************************************/
bool Pos2StateB_Array(M_pos m_pos, M_motorState& m_motorState, M_motorStatePosEnum* enumelate)
{
	int yErrorMin = 0;

	if (m_pos.y < (enumelate[0].y - yMoveMax))	//��ʾ��ʱ�ƶ�Ҳ�򲻵�����Ϊ���̵�ǰ������ƶ�������һ���ģ�
	{
		return false;
	}
	else if (m_pos.y < (enumelate[0].y))		//��ʾ����ͨ���ƶ���������
	{

		if (m_pos.x<0)					//��ʾҪ90�����
		{
			m_motorState.m_m5 = enumelate[0].m_m5;
			m_motorState.m_m6 = 90;
			m_motorState.m_m3 = m_pos.x - enumelate[0].x;
			m_motorState.m_m4 = m_pos.z - enumelate[0].z;
			return judgeMotorState(m_motorState);
		}
		else
		{
			m_motorState.m_m5 = -enumelate[0].m_m5;
			m_motorState.m_m6 = -90;
			m_motorState.m_m3 = m_pos.x + enumelate[0].x;
			m_motorState.m_m4 = m_pos.z - enumelate[0].z;
			return judgeMotorState(m_motorState);
		}
	}
	else if (m_pos.y < (enumelate[enumArrayNum - 1].y))		//���������ܴ���
	{
		for (int i = 1; i < enumArrayNum; i++)
		{
			if (m_pos.y < (enumelate[i].y))					//��ʾ��I�������Ѿ���Ԥ����Y������
			{
				int j = (abs(m_pos.y - (enumelate[i - 1].y)) > abs(m_pos.y - (enumelate[i].y))) ? i : (i - 1);//�洢��ӽ�������
				//�о�J��û��Ҫ��
				if (m_pos.x<0)					//��ʾҪ90�����
				{
					m_motorState.m_m5 = enumelate[j].m_m5;
					m_motorState.m_m6 = 90;
					m_motorState.m_m3 = m_pos.x - enumelate[j].x;
					m_motorState.m_m4 = m_pos.z - enumelate[j].z;

					return judgeMotorState(m_motorState);
				}
				else
				{
					m_motorState.m_m5 = -enumelate[j].m_m5;
					m_motorState.m_m6 = -90;
					m_motorState.m_m3 = m_pos.x + enumelate[j].x;
					m_motorState.m_m4 = m_pos.z - enumelate[j].z;

					return judgeMotorState(m_motorState);
				}
			}
		}
	}
	else if (m_pos.y < (enumelate[enumArrayNum - 1].y + yMoveMax))
	{

		if (m_pos.x<0)					//��ʾҪ90�����
		{
			m_motorState.m_m5 = enumelate[enumArrayNum - 1].m_m5;
			m_motorState.m_m6 = 90;
			m_motorState.m_m3 = m_pos.x - enumelate[enumArrayNum - 1].x;
			m_motorState.m_m4 = m_pos.z - enumelate[enumArrayNum - 1].z;

			return judgeMotorState(m_motorState);
		}
		else
		{
			m_motorState.m_m5 = -enumelate[enumArrayNum - 1].m_m5;
			m_motorState.m_m6 = -90;
			m_motorState.m_m3 = m_pos.x + enumelate[enumArrayNum - 1].x;
			m_motorState.m_m4 = m_pos.z - enumelate[enumArrayNum - 1].z;

			return judgeMotorState(m_motorState);;
		}
	}
	else
	{
		return false;
	}
	return false;
}

bool Pos2State2D_BGK(M_pos* m_pos, M_motorState* m_motorState,int num)
{
	for (int i = 0; i < num; i++)
	{
	//	if (Pos2StateB_Array(m_pos[i], *m_motorState))
		{
			std::cout <<"ƽ��Ϊ��"<< m_pos[i].y << endl;
			return true;
		}
	}
	return false;
}