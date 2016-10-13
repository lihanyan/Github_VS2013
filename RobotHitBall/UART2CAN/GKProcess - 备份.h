#pragma once
#include "HUST_CAN.h"

extern M_motorState motorState;

M_motorState Pos2State(M_pos m_pos);		//状态到电机状态的转换  一代比利时机构
M_motorState Pos2State2D(M_pos m_pos, HUST_CAN* CAN);	//状态到电机状态的转换  一代比利时机构
M_motorState Pos2State2D(int x, int y, int z, HUST_CAN* CAN);	//状态到电机状态的转换  一代比利时机构
M_motorState Pos2State2D_B(M_pos m_pos);		//状态到电机状态的转换  二代B车前方比利时机构
M_motorState Pos2State2D_B2(M_pos m_pos);		//状态到电机状态的转换  二代B车前方比利时机构
BOOLEAN Pos2State2D_BT(M_pos m_pos);
//M_motorState Pos2State2D_BT2(M_pos m_pos);
BOOLEAN Pos2State2D_BT2(M_pos m_pos);
void Pos2State2D_BT3(M_pos* m_pos);
M_pos State2Pos_A(M_motorState m_motorState);	//电机状态到空间位置的转换，枚举用，A车
M_pos State2Pos_B(M_motorState m_motorState);//电机状态到空间位置的转换，枚举用，B车