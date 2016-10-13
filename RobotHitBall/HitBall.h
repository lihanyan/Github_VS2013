#include "gettime.h"
#include "Parameter.h"
#include "HUST_CAN.h"
#include "ReverseTheta.h"
#include"serialport\MyThread.h"

DWORD WINAPI MotorRun(LPVOID lpParamter);    //Ïß³Ì
void GetResultPlayBall();