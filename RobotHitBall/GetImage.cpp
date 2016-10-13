#include"GetImage.h"


DWORD WINAPI ImageCollect(LPVOID lpParamter)    //线程
{
	//OpenCV中开启CPU的硬件指令优化功能函数
	setUseOptimized(true);

	// 打开kinect
	IKinectSensor* pSensor;
	HRESULT hResult = S_OK;
	hResult = GetDefaultKinectSensor(&pSensor);
	if (FAILED(hResult)){
		cerr << "Error : GetDefaultKinectSensor" << std::endl;
		return -1;
	}

	hResult = pSensor->Open();
	if (FAILED(hResult)){
		cerr << "Error : IKinectSensor::Open()" << std::endl;
		return -1;
	}

	// 深度帧源
	IDepthFrameSource* pDepthSource;
	hResult = pSensor->get_DepthFrameSource(&pDepthSource);
	if (FAILED(hResult)){
		cerr << "Error : IKinectSensor::get_DepthFrameSource()" << std::endl;
		return -1;
	}

	// 深度帧读取
	IDepthFrameReader* pDepthReader;
	hResult = pDepthSource->OpenReader(&pDepthReader);
	if (FAILED(hResult)){
		cerr << "Error : IDepthFrameSource::OpenReader()" << std::endl;
		return -1;
	}

	// Description
	IFrameDescription* pDescription;
	hResult = pDepthSource->get_FrameDescription(&pDescription);
	if (FAILED(hResult)){
		cerr << "Error : IDepthFrameSource::get_FrameDescription()" << std::endl;
		return -1;
	}

	int width = 0;
	int height = 0;
	pDescription->get_Width(&width); // 512
	pDescription->get_Height(&height); // 424
	unsigned int bufferSize = width * height * sizeof(unsigned short);

	// Range
	unsigned short min = 0;
	unsigned short max = 0;
	pDepthSource->get_DepthMinReliableDistance(&min); // 500
	pDepthSource->get_DepthMaxReliableDistance(&max); // 4500
	cout << "Range : " << min << " - " << max << std::endl;

	//创建尺寸为height x width 的1通道8位图像
	Mat bufferMat(height, width, CV_16UC1);
	Mat depthMat(height, width, CV_16UC1);

	//这里我们用灰度图来表述深度数据，越远的数据越暗。
	for (int i = 0; i < 30; i++)
	{
		Piclist[i].create(height, width, CV_8UC1);
	}

	for (int i = 0; i < ListLong; i++)
	{
		DepthMatList[i].create(height, width, CV_16UC1);
	}
	for (int i = 0; i < ListLong; i++)
	{
		imageMatList[i].create(height, width, CV_8UC1);
	}
	for (int i = 0; i < ListLong; i++)
	{
		saveList[i].create(height, width, CV_8UC1);
	}
	DepthMatHead = 0;
	DepthMatTail = 0;

	int tmp = DepthMatTail;
	char keynum;
	Mat BD(height, width, CV_8UC1);

	int GetGrayY = int(gIntr[1][2] - 1), GetGrayX = int(gIntr[0][2] - 1);

	while (1)
	{
		// 更新深度帧
		IDepthFrame* pDepthFrame = nullptr;
		hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame);
		if (SUCCEEDED(hResult)){
			hResult = pDepthFrame->AccessUnderlyingBuffer(&bufferSize, reinterpret_cast<UINT16**>(&bufferMat.data));
			if (SUCCEEDED(hResult))
			{
				//timeList[tmp] = Gettime();
				tmp++;
				if (tmp >= ListLong)
				{
					tmp = 0;
				}
				//				Coordinate_robot->Coordinate_get_position_from_can(tmp);
				//Coordinate_robot->Coordinate_printf_information(tmp);
				//timeList[tmp] = Gettime();

				RobotStateList[tmp][0] = CAN.m_robotState.robot_x;
				RobotStateList[tmp][1] = CAN.m_robotState.robot_y;
				RobotStateList[tmp][2] = CAN.m_robotState.angle;

				//cout << "AAA  " << Gettime() << endl;


				//LA_ofile << Gettime() << "	" << CAN.m_robotState.robot_x << "	" << CAN.m_robotState.robot_y << "	" << CAN.m_robotState.angle << endl;

				//cout << Gettime() << "    AAAAA" << endl;

				bufferMat.convertTo(DepthMatList[tmp], CV_16U, 1, 0);
				bufferMat.convertTo(imageMatList[tmp], CV_8U, 255.0f / 8192.0f, 0.0f);

				//cout << Gettime() << "    BBBBB" << endl;


				DepthMatTail = tmp;
				
				//这是有原因的！三帧帧差求的是中间一帧，时间要延后一帧！
				tmp++;
				if (tmp >= ListLong)
				{
					tmp = 0;
				}
				timeList[tmp] = Gettime();

	/*			Sleep(1);
				cout << DepthMatTail - DepthMatHead << endl;
				cout << WORKTIMEDELAY << endl;*/
				//if (timeList[tmp] - timeList[DepthMatTail] > 34)
				//{
				//	cout << timeList[tmp] - timeList[DepthMatTail] << endl;
				//}

				tmp = DepthMatTail;
				//bufferMat.convertTo(image, CV_8U, -255.0f / 8192.0f, 255.0f);
				//cout << DepthMatTail << endl;
				//imshow("caiji", imageMatList[DepthMatTail]);

				BD = imageMatList[DepthMatTail].clone();
				//BD.data[(height / 2 - 1)* width + width / 2 - 1] = 0;
				//BD.data[(height / 2 - 1)* width + width / 2] = 0;
				//BD.data[(height / 2)* width + width / 2 - 1] = 0;
				//BD.data[(height / 2)* width + width / 2] = 0;

				for (int i = -190; i < 191; i++)
				{
					BD.data[int(int(gIntr[1][2] - 1) * width + (gIntr[0][2] - 1) + i)] = 255;
					BD.data[int(int(gIntr[1][2] - 1 + i) * width + (gIntr[0][2] - 1))] = 255;
				}

				for (int i = 0; i <= 3; i++)
				{
					BD.data[(GetGrayY + i) * width + GetGrayX + i] = 255;
					BD.data[(GetGrayY + i) * width + GetGrayX - i] = 255;
					BD.data[(GetGrayY - i) * width + GetGrayX + i] = 255;
					BD.data[(GetGrayY - i) * width + GetGrayX - i] = 255;

				}

				//cout << "BBB  " <<Gettime()<< endl;
				
				memset(&BD.data[GetGrayY * width + GetGrayX-200],255,400);

				imshow("caiji", BD);

				//cout << RobotStateList[DepthMatTail][0] << endl;

				keynum = waitKey(5);
				//cout << int(keynum) << endl;
				if (keynum == 13)
				{
					/*double centerdep = (DepthMatList[DepthMatTail].data[(height / 2 - 1)* width + width / 2 - 1] +
					DepthMatList[DepthMatTail].data[(height / 2)* width + width / 2 - 1] +
					DepthMatList[DepthMatTail].data[(height / 2 - 1)* width + width / 2] +
					DepthMatList[DepthMatTail].data[(height / 2)* width + width / 2]) / 4;*/

					double centerdep = depmap[int(gIntr[1][2] - 1)][int(gIntr[0][2] - 1)];

					cout << centerdep << endl;
				}


				if (keynum == 32)
				{
					double PX, PY, PZ, X, Y, Z;
					Z = depmap[GetGrayY][GetGrayX];

					cout << "X=" << GetGrayX << "   Y=" << GetGrayY << "   Gray=" << Z << endl;

					DeDistortion(Distortion, gIntr[0], GetGrayX, GetGrayY, X, Y);

					PX = Z* TanX / 212 * (gIntr[1][2] - Y) / 1000;
					PY = double(Z) / 1000 * SinZ + PX * CosZ + AddZ;
					PZ = Z* TanY / 256 * (X - gIntr[0][2]) / 1000;
					PX = -PX * SinZ + double(Z) / 1000 * CosZ;

					cout << "PX=" << PX << "  PY=" << PY << "  PZ=" << PZ << "	SinZ=" << SinZ << "	Theta=" << KinectAngle << endl;

				}

				if (keynum == 49)//'1'
				{
					cout << "relocal angle" << endl;
					RELOCAL.m_camState = RE_AGL;
				}
				if (keynum == 50)//'2'
				{
					cout << "relocal position" << endl;
					RELOCAL.m_camState = RE_POS;
				}

				if (keynum == 97)//'a'
				{
					GetGrayX--;
				}
				if (keynum == 100)
				{
					GetGrayX++;
				}
				if (keynum == 119)
				{
					GetGrayY--;
				}
				if (keynum == 115)
				{
					GetGrayY++;
				}
				if (keynum == 'h')
				{
					KinectAngle += 0.1;
					cout << "KinectAngle	" << KinectAngle << endl;


					//TH20150806
					//CAN.Send_RobotHolder(0x03, KinectAngle, 0);

				}
				if (keynum == 'l')
				{
					KinectAngle -= 0.1;
					cout << "KinectAngle	" << KinectAngle << endl;

					//TH20150806
					//CAN.Send_RobotHolder(0x03, KinectAngle, 0);


				}
				//
				if (keynum == 27) break;
			}
		}
		else
		{
			Sleep(1);
			continue;
		}
		SafeRelease(pDepthFrame);

		//Sleep(10);
	}

	SafeRelease(pDepthSource);
	SafeRelease(pDepthReader);
	SafeRelease(pDescription);
	if (pSensor){
		pSensor->Close();
	}
	SafeRelease(pSensor);

	return (0);

}

