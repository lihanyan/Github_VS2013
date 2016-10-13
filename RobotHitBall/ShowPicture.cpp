#include "Showpicture.h"


#define ImageBuffList 30

Mat ImageBuff[ImageBuffList];




DWORD WINAPI ShowPic(LPVOID lpParamter)    //Ïß³Ì
{

	/*Mat tempmat(424, 512, CV_8UC1);
	for (int i = 0; i < ImageBuffList; i++)
	{

		ImageBuff[i] = tempmat;
	}


	int NumBuff = 0;*/

	char Key;

	while (1)
	{
		//Gettime();
		// cout << Gettime() << endl;
		if (!canshow)
		{
			Sleep(1);
			continue;
		}

		imshow("depth", show);
		canshow = 0;


		//show.copyTo(ImageBuff[NumBuff]);

		//NumBuff++;

		//if (NumBuff == ImageBuffList) NumBuff = 0;


		Key = cv::waitKey(5);

		if (Key == VK_ESCAPE)
		{
			break;
		}
		
		/*if (Key == 13)
		{
			for (int i = 1; i < ImageBuffList; i++)
			{
				stringstream ss;
				string s = ".\\PictureSave\\";
				ss << i;
				ss >> s;
				s = s + ".jpg";
				imwrite(s, ImageBuff[i]);

				cout << "Save image " << i << endl;
			}

			return 0;
		}*/

		//cout << "M" << endl;

	}

	return 0;

}

DWORD WINAPI CANHeart(LPVOID lpParamter)
{
	while (1)
	{
	//	if (CAN.IsOpened())
	//	{
			CAN.Send_HeartPack(0x01);
	//	}
	//	else
	//	{
	////		cout << "asdjkfhkajlghfjiahjkfglashfdjkasgi!!!!!!" << endl;
	//	}
			//cout << "*" ;
		Sleep(200);
	}


	cout << "**************" << endl;
	return 0;
}