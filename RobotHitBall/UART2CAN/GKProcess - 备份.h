#pragma once
#include "HUST_CAN.h"

extern M_motorState motorState;

M_motorState Pos2State(M_pos m_pos);		//״̬�����״̬��ת��  һ������ʱ����
M_motorState Pos2State2D(M_pos m_pos, HUST_CAN* CAN);	//״̬�����״̬��ת��  һ������ʱ����
M_motorState Pos2State2D(int x, int y, int z, HUST_CAN* CAN);	//״̬�����״̬��ת��  һ������ʱ����
M_motorState Pos2State2D_B(M_pos m_pos);		//״̬�����״̬��ת��  ����B��ǰ������ʱ����
M_motorState Pos2State2D_B2(M_pos m_pos);		//״̬�����״̬��ת��  ����B��ǰ������ʱ����
BOOLEAN Pos2State2D_BT(M_pos m_pos);
//M_motorState Pos2State2D_BT2(M_pos m_pos);
BOOLEAN Pos2State2D_BT2(M_pos m_pos);
void Pos2State2D_BT3(M_pos* m_pos);
M_pos State2Pos_A(M_motorState m_motorState);	//���״̬���ռ�λ�õ�ת����ö���ã�A��
M_pos State2Pos_B(M_motorState m_motorState);//���״̬���ռ�λ�õ�ת����ö���ã�B��