//#include "stdafx.h"

#include"GETTIME.h"

double GETTIME()
{

	static LARGE_INTEGER limtp, freq;

	static bool Once_TH = true;

	//第一次进入，获取外部频率
	if (Once_TH)
	{
		QueryPerformanceFrequency(&freq);
		Once_TH = false;
	}

	QueryPerformanceCounter(&limtp);

	//输出单位 ms
	return ((double)(limtp.QuadPart * 1000) / freq.QuadPart);
}
