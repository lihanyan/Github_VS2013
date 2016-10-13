#include "KinectSleep.h"

DWORD WINAPI KinectSleep(LPVOID lpParamter)    //Ïß³Ì
{
	Sleep(65);
	KinectEnable = 0;
	Sleep(100);
	KinectEnable = 1;
	return 0;
}
