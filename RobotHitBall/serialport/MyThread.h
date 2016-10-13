#pragma once

//#include <cv.h>
//#include <highgui.h>
#include <stdio.h>
#include<iostream>

//#include"GlobalParameter.h"
#include"SerialPort.h"
#include"..\Gettime.h"
#include"..\UART2CAN\HUST_CAN.h"
#include "TCPSOCKET.H"

using namespace std;

#define Mysquare(x) ((x)*(x))



extern CSerialPort KinectSerialPort;
extern CSerialPort THSerialPort;

extern StateOfAB m_StateOfAB;//��״̬
extern double GeStateOfABTime;

extern ActionType m_ActionType;//С״̬
extern double GetActionTypeTime;


extern ofstream playBallLog;



DWORD WINAPI ContrlOfABCarThread(LPVOID p_lpVoid);

