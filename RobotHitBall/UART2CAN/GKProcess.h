#pragma once
#include "HUST_CAN.h"

#define enumArrayNum 91

extern M_motorStatePosEnum enumelateArray[enumArrayNum];

M_pos State2Pos_A(M_motorState m_motorState);	//���״̬���ռ�λ�õ�ת����ö���ã�A��
M_pos State2Pos_B(M_motorState m_motorState);//���״̬���ռ�λ�õ�ת����ö���ã�B��

inline bool judgeMotorState(M_motorState& m_motorState);
bool Pos2StateB_Array(M_pos m_pos, M_motorState& m_motorState, M_motorStatePosEnum* enumelate);

bool Pos2State2D_BGK(M_pos* m_pos, M_motorState* m_motorState, int num);