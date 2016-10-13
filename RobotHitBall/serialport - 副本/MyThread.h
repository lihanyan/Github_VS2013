#pragma once

//#include <cv.h>
//#include <highgui.h>
#include <stdio.h>
#include<iostream>

//#include"GlobalParameter.h"
#include"SerialPort.h"
#include"..\Gettime.h"
#include"..\UART2CAN\HUST_CAN.h"

using namespace std;

#define Mysquare(x) (x)*(x)


//外部可见标志位
extern CSerialPort Me;


extern int AnotherCarX;
extern int AnotherCarY;

//extern double DipanAimx;
//extern double DipanAimy;



DWORD WINAPI ContrlOfABCarThread(LPVOID p_lpVoid);

void ContrlOfABCar();