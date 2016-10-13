#include "Gettime.h"

using namespace std;

int Gettime()
{

	LARGE_INTEGER limtp, freq;

	//while (1)
	{
		QueryPerformanceFrequency(&freq);//��õ�ǰ�ļ���Ƶ��,��ÿ����ж��ٴμ���

		//cout << freq.QuadPart << endl;

		QueryPerformanceCounter(&limtp);//��ȡ��ǰ��������

		//cout << limtp.QuadPart<<endl;

		//cout << (limtp.QuadPart/1000) / (freq.QuadPart/1000) << "   " << limtp.QuadPart * 1000 / freq.QuadPart % 1000 << "   " << limtp.QuadPart * 10000000 / freq.QuadPart % 1000 << endl;

		//Sleep(1);

		//cout << limtp.QuadPart / freq.QuadPart + limtp.QuadPart * 1000 / freq.QuadPart % 1000 << endl;

		return limtp.QuadPart / freq.QuadPart * 1000 + limtp.QuadPart * 1000 / freq.QuadPart % 1000;
	}

}

double THGettime()
{

	static LARGE_INTEGER limtp, freq;

	static bool Once_TH = true;

	if (Once_TH)
	{
		QueryPerformanceFrequency(&freq);//��õ�ǰ�ļ���Ƶ��,��ÿ����ж��ٴμ���
		Once_TH = false;
	}

	QueryPerformanceCounter(&limtp);//��ȡ��ǰ��������

	return ((double)limtp.QuadPart / freq.QuadPart * 1000);


}


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