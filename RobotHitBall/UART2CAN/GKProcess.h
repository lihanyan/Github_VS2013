#pragma once
#include "HUST_CAN.h"

#define enumArrayNum 91

extern M_motorStatePosEnum enumelateArray[enumArrayNum];

M_pos State2Pos_A(M_motorState m_motorState);	//电机状态到空间位置的转换，枚举用，A车
M_pos State2Pos_B(M_motorState m_motorState);//电机状态到空间位置的转换，枚举用，B车

inline bool judgeMotorState(M_motorState& m_motorState);
bool Pos2StateB_Array(M_pos m_pos, M_motorState& m_motorState, M_motorStatePosEnum* enumelate);

bool Pos2State2D_BGK(M_pos* m_pos, M_motorState* m_motorState, int num);