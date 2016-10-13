//#include "stdafx.h"

#include"GETTIME.h"

double GETTIME()
{

	static LARGE_INTEGER limtp, freq;

	static bool Once_TH = true;

	//��һ�ν��룬��ȡ�ⲿƵ��
	if (Once_TH)
	{
		QueryPerformanceFrequency(&freq);
		Once_TH = false;
	}

	QueryPerformanceCounter(&limtp);

	//�����λ ms
	return ((double)(limtp.QuadPart * 1000) / freq.QuadPart);
}
