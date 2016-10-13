#include "PrintScreen.h"
#include <iostream>

using namespace std;

DWORD WINAPI PrintScreen(LPVOID lpParamter)
{
	while (1)
	{
		if (!canprint)
		{
			Sleep(1);
			continue;
		}

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13 | 0);

		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				cout << PrintP[i][j] << "	  ";
			}
			cout << endl;
		}

		canprint = 0;

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), C_NOMAL | 0);

		cout << endl;
		cout << endl;

	}

	return 0;

}
