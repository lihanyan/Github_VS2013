#include "stdafx.h"
// Include standard headers
//#include "tutorial08_basic_shading\yuqiu.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <iostream>
#include <cmath>
#include <math.h>
#include <fstream>

#include "FindtheBall.h"
#include "GetImage.h"
#include "HitBall.h"
#include "LA_Kinect.h"

#include"serialport\SerialPort.h"
#include"serialport\MyThread.h"

#include "Show3D.h"
#include "ShowPicture.h"
#include "initialise.h"
#include "GettheParameter.h"

#include "guess.h"
#include "kalman.h"
#include "gettime.h"
#include "Parameter.h"
#include "GetResult.h"
#include "TCPKinectClient.h"
#include "KinectSleep.h"
#include <ctime>

int main(int argc, char **argv[])
{

	
	stringstream name;
	time(&nowTime);//获取当前时间
	timeInfo = localtime(&nowTime);
	name << "D://gkLog//playBallLog" << 1 + timeInfo->tm_mon << "m" << timeInfo->tm_mday << "d" << timeInfo->tm_hour << "h" << timeInfo->tm_min << "m" <<
		timeInfo->tm_sec << "s" << ".txt";
	playBallLog.open(name.str().c_str());
	name.str("");


#ifdef USE_CAN

	if (!CAN.Open(CAN_PORT, 500000))
	{
		std::cout << "initCanPort fail !" << std::endl;
	
	}
	else
	{
		std::cout << "initCanPort success !" << std::endl;
	}
	CAN.kinectPitch = &KinectAngle;
	CAN.kinectEnable = &KinectEnable;
#endif
	cout << "******************" << endl;



	//重定位初始化
	//RELOCAL.initClass(&CAN, CAM_DEVNUM);
	
//	CAN.Send_RobotPositionReset_Frame(1, 0, 0, 600);
	CreateTable(mResultTable,mResultTableL, mResultTableLN,mResultTableLW, mResultTableR, mResultTableRN, mResultTableRW);
	QuickSort_BYaw(mResultTable, 1, mResultTable.size());
	ResetYawPos(mResultTable, yawPos);
	cout << "表的大小：" << mResultTable.size() << endl;
	cout << "R表的大小：" << mResultTableR.size() << endl;
	cout << "L表的大小：" << mResultTableL.size() << endl;
	ofstream gkTable("gkTable.txt");

	for (int i = 0; i < mResultTable.size(); i++)
	{
		switch (mResultTable[i].playDirect)
		{
		case PRN:
		case PRW:
			gkTable << "R" << "\t";
			gkTable << (int)mResultTable[i].ResultZMin << "\t" << (int)mResultTable[i].theta1 << "\t" << (int)mResultTable[i].theta2 << "\t" <<
				"\t" << (int)mResultTable[i].x << "\t" << (int)mResultTable[i].y << "\t" << (int)mResultTable[i].z/* + (int)mTable[i].ResultZMin*/
				<< "\t" << (int)mResultTable[i].yaw << "\t" << (int)mResultTable[i].pitch << endl;
			break;
		case PLW:
		case PLN:
			gkTable << "L" << "\t";
			gkTable << (int)mResultTable[i].ResultZMin << "\t" << (int)mResultTable[i].theta1 << "\t" << (int)mResultTable[i].theta2 << "\t" <<
				"\t" << (int)mResultTable[i].x << "\t" << (int)mResultTable[i].y << "\t" << (int)mResultTable[i].z/* + (int)mTable[i].ResultZMin*/
				<< "\t" << (int)mResultTable[i].yaw << "\t" << (int)mResultTable[i].pitch << endl;
			break;
		default:
			break;
		}

	}
	
	Theta theta;
	theta.theta1 = 0.0/ 180 * PI;
	theta.theta2 = -100.0 /180*PI;
	int z_temp = 100;
	cout << "R:" <<ForwardSolutionPositionR(theta).x << "\t" << ForwardSolutionPositionR(theta).y << "\t" <<
		ForwardSolutionPositionR(theta).z + z_temp << "\t" << endl;
	cout << "L:" << ForwardSolutionPositionL(theta).x << "\t" << ForwardSolutionPositionL(theta).y << "\t" <<
		ForwardSolutionPositionL(theta).z + z_temp << "\t" << endl;

	
	
	

#ifdef USENET
	//启动网络线程
	hThreadProcessKinect = CreateThread(NULL, 0, ThreadProcessKinect, NULL, 0, NULL);
#endif
	//ConnectSocket();//网络连接
	//cout << "NET RIGHT!" << endl;
	//hThreadSendSocket = CreateThread(NULL, 0, ThreadSend, NULL, 0, NULL);
	//hThreadRecvSocket = CreateThread(NULL, 0, ThreadRecv, NULL, 0, NULL);


	//int width = 0;
	//int height = 0;
	//pDescription->get_Width( &width ); // 512
	//pDescription->get_Height( &height ); // 424
	//unsigned int bufferSize = width * height * sizeof( unsigned short );

	//// Range
	//unsigned short min = 0;
	//unsigned short max = 0;
	//pDepthSource->get_DepthMinReliableDistance( &min ); // 500
	//pDepthSource->get_DepthMaxReliableDistance( &max ); // 4500
	//cout << "Range : " << min << " - " << max << std::endl;

	int height = 424;
	int width = 512;


	//创建尺寸为height x width 的1通道8位图像
	Mat bufferMat(height, width, CV_16UC1);
	Mat depthMat(height, width, CV_16UC1);
	Mat depthMatT(height, width, CV_16UC1);
	Mat imageM(height, width, CV_8UC1);
	Mat imageT(height, width, CV_8UC1);


	Mat image1, image2, imageC1, imageC2;
	double AX[3][4];
	int n = 0;
	//int tmp;
	memset(AX, 0, sizeof(AX));


	image1.create(height, width, CV_8UC1);
	image2.create(height, width, CV_8UC1);
	imageC1.create(height, width, CV_8UC1);
	imageC2.create(height, width, CV_8UC1);
	//这里我们用灰度图来表述深度数据，越远的数据越暗。
	//for (int i = 0; i < 30; i++)
	//{
	//	Piclist[i].create(height, width, CV_8UC1);
	//}

	for (int o = MINHITLEVEL; o <= MAXHITLEVEL; o++)
	{
		NeedToGo[o][4] = 0.009;
	}

	HANDLE hThread0 = CreateThread(NULL, 0, ImageCollect, NULL, 0, NULL);
	HANDLE hThread1 = CreateThread(NULL, 0, ShowPic, NULL, 0, NULL);
	HANDLE hThread2 = CreateThread(NULL, 0, MotorRun, NULL, 0, NULL);
	HANDLE hThreadCANHeart = CreateThread(NULL, 0, CANHeart, NULL, 0, NULL);

	//HANDLE hThread3 = CreateThread(NULL, 0, Drow3D, NULL, 0, NULL);

	Sleep(100);

	//TH20151114
	//CreateThread(NULL, NULL, ContrlOfABCarThread, NULL, NULL, NULL);

	LA_ofile.open("guiji.txt");

	while (1)
	{

		TrackNow = 0;

		if (DepthMatHead == DepthMatTail)
		{
			Sleep(1);
			continue;
		}
		int stime = Gettime();


		DepthMatHead++;
		//cout << WORKTIMEDELAY << endl;
		//DepthMatHead = DepthMatTail;

		//cout << DepthMatHead << ' ' << DepthMatTail << endl;

		//if (!DepthMatHead == DepthMatTail)
		//{
		//	cout << DepthMatHead << ' ' << DepthMatTail << endl;
		//}

		if (DepthMatHead >= ListLong)
		{
			DepthMatHead = 0;
		}

		if (KinectEnable)
		{
			cout << "KinectEnable*****" << endl;
			KinectEnable--;
			continue;
		}


		static double THgettime = GETTIME();
		static double THLastgettime = GETTIME();
		bool ShowIMAGEerroe = false;
		double THthisTime = GETTIME();

		//if (THthisTime - THgettime > 60)
		//{
		//	ShowIMAGEerroe = true;
		//}
		//THLastgettime = THgettime;
		//THgettime = THthisTime;


		//cout << Gettime() << "  I" << endl;

		//DepthMatList[DepthMatHead].convertTo(depthMat, CV_16U, 1, 0);
		depthMat = DepthMatList[DepthMatHead].clone();
		image = imageMatList[DepthMatHead].clone();
		//DepthMatList[DepthMatHead].convertTo(image, CV_8U, 255.0f / 8192.0f, 0.0f);

		//imshow(" ", image);
		//waitKey(5);



		//static double THAtime = GETTIME();
		//if (ShowIMAGEerroe)
		//{
		//	cout << "THAtime" << THAtime - THLastgettime << endl;
		//}
		//THAtime = GETTIME();


		//cout << Gettime() << "  A" << endl;



		//// 更新深度帧
		//IDepthFrame* pDepthFrame = nullptr;
		//hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame);
		//if (SUCCEEDED(hResult)){
		//	hResult = pDepthFrame->AccessUnderlyingBuffer(&bufferSize, reinterpret_cast<UINT16**>(&bufferMat.data));
		//	if (SUCCEEDED(hResult)){
		//		bufferMat.convertTo(depthMat, CV_16U, 1, 0);
		//		bufferMat.convertTo(image, CV_8U, 255.0f / 8192.0f, 0.0f);
		//	}
		//}


		//// 更新深度帧
		//IDepthFrame* pDepthFrame = nullptr;
		//hResult = pDepthReader->AcquireLatestFrame( &pDepthFrame );
		//if( SUCCEEDED( hResult ) ){
		//	hResult = pDepthFrame->AccessUnderlyingBuffer( &bufferSize, reinterpret_cast<UINT16**>( &bufferMat.data ) );
		//	if( SUCCEEDED( hResult ) ){
		//		bufferMat.convertTo( depthMat, CV_16U, 1, 0);
		//		bufferMat.convertTo(image, CV_8U, 255.0f / 8192.0f, 0.0f);
		//		//bufferMat.convertTo(image, CV_8U, -255.0f / 8192.0f, 255.0f);
		//	}
		//}
		//else
		//{
		//	continue;
		//}

		//cout << Gettime() << "  BBBB" << endl;

		//SafeRelease( pDepthFrame );

		//if (Gettime() % 1 == 0)
		//{
		//	//imshow("depth", image);
		//}

		//cout << Gettime() << "  B" << endl;

		//if (n>0)
		//{
		//	memset(mapb, 0, sizeof(mapb));
		//	for (int i = 0; i < imageM.rows; i++)
		//	{
		//		//深度图像数据含有两种格式，这里像素的低12位表示一个深度值，高4位未使用；
		//		//注意这里需要转换，因为每个数据是2个字节，存储的同上面的颜色信息不一样，

		//		for (int j = 0; j<imageM.cols; j++)
		//		{
		//			if ((!mapb[i][j]) && (depmap[i][j]>0))
		//			{
		//				BFS(i, j, &imageM);
		//			}
		//		}
		//	}
		//}

		if (n < 100) n++;

		//imshow("depth2", imageM);
		//dilate(depthMat, imageM, Mat(), Point(-1, -1), 1);
		//dilate(imageM, depthMat, Mat(), Point(-1, -1), 1);
		//dilate(image, imageM, Mat(), Point(-1, -1), 1);
		//dilate(imageM, image, Mat(), Point(-1, -1), 1);
		//dilate(image, imageM, Mat(), Point(-1, -1), 1);
		//erode(depthMat, imageM, Mat(), Point(-1, -1), 1);
		//erode(imageM, depthMat, Mat(), Point(-1, -1), 1);

		//imshow("depth2", imageM);

		//medianBlur(image, imageM, 5);
		//medianBlur(imageM, image, 5);

		//GaussianBlur(image, imageM, Size(9, 9), 0, 0);
		//dilate(imageM, image, Mat(), Point(-1, -1), 0);
		//GaussianBlur(depthMat, imageM, Size(9, 9), 0, 0);
		//dilate(imageM, depthMat, Mat(), Point(-1, -1), 0);

		//blur(image, imageM, Size(3, 3), Point(-1, -1));

		//bilateralFilter(image, imageM, 5, 10.0, 2.0);

		//imshow("depth2", imageM);

		absdiff(image1, image, image2);

		if (CanErode)
		{
			if (CanErode2)
			{
				erode(image2, imageC1, Mat(), Point(-1, -1), 1);
			}
			else
			{
				erode(image2, imageC1, Mat(), Point(-1, -1), 0);
			}

			bitwise_and(imageC2, imageC1, imageT);

			erode(imageT, imageM, Mat(), Point(-1, -1), 1);

		}
		else
		{
			//absdiff(image1, image, image2);

			//dilate(image2, imageC1, Mat(), Point(-1, -1), 1);
			//dilate(imageC1, image2, Mat(), Point(-1, -1), 1);
			erode(image2, imageC1, Mat(), Point(-1, -1), 0);
			//erode(imageC1, image2, Mat(), Point(-1, -1), 1);
			//erode(image2, imageC1, Mat(), Point(-1, -1), 1);
			//imshow("depth2", imageC1);
			//erode(imageC1, imageM, Mat(), Point(-1, -1), 0);
			//medianBlur(imageM, imageC1, 3);
			bitwise_and(imageC2, imageC1, imageT);
			erode(imageT, imageM, Mat(), Point(-1, -1), 0);
			//erode(imageM, imageT, Mat(), Point(-1, -1), 0);
			//medianBlur(imageT, imageM, 3);
		}
		
		//imshow("depthM2", imageM);


		//imshow("depthC1", imageC1);
		//imshow("depthC2", imageC2);

		//cout << Gettime() << "  C" << endl;
		//cout << WORKTIMEDELAY << endl;


		//static double THBtime = GETTIME();
		//if (ShowIMAGEerroe)
		//{
		//	cout << "THBtime" << THBtime - THLastgettime << endl;
		//}
		//THBtime = GETTIME();


		if (n>0)
		{
			//dilate(depthMat, depthMatT, Mat(), Point(-1, -1), 3);
			memset(mapb, 0, sizeof(mapb));
			for (int i = 0; i < imageM.rows; i++)
			{
				//深度图像数据含有两种格式，这里像素的低12位表示一个深度值，高4位未使用；
				//注意这里需要转换，因为每个数据是2个字节，存储的同上面的颜色信息不一样，

				//ushort *ptr = depthMatT.ptr<ushort>(i);
				//unsigned char *ptr1 = imageM.ptr<unsigned char>(i);
				for (int j = 0; j<imageM.cols; j++)
				{
					//depmap[i][j] = ptr[j];
					//ptr1[j] =unsigned char( depmap[i][j] >> 0);
					if ((!mapb[i][j]) && (imageM.data[i * 512 + j] >0))
					{
						//cout << int(imageM.data[i * 512 + j]) << endl;
						BFSdep(i, j, &imageM, DepthMatHead);
					}
				}
			}
			//imshow("depthT", imageM);
			//waitKey(5);
			//cout << WORKTIMEDELAY << endl;
			//dilate(depthMat, imageT, Mat(), Point(-1, -1), 1);
			//dilate(imageT, depthMat, Mat(), Point(-1, -1), 1);

			//memset(mapp, 0, sizeof(mapp));
			//for (int i = 0; i < imageM.rows; i++)
			//{
			//	//深度图像数据含有两种格式，这里像素的低12位表示一个深度值，高4位未使用；
			//	//注意这里需要转换，因为每个数据是2个字节，存储的同上面的颜色信息不一样，

			//	//ushort *ptr = depthmatt.ptr<ushort>(i);
			//	//unsigned char *ptr1 = imagem.ptr<unsigned char>(i);
			//	for (int j = 0; j<imageM.cols; j++)
			//	{
			//		//depmap[i][j] = ptr[j];
			//		//ptr1[j] =unsigned char( depmap[i][j] >> 0);
			//		if ((!mapp[i][j]) && (imageM.data[i * 512 + j] >0))
			//		{
			//			BFSp(i, j, &imageM);
			//		}
			//	}
			//}

		}



		//static double THCtime = GETTIME();
		//if (ShowIMAGEerroe)
		//{
		//	cout << "THCtime" << THCtime - THLastgettime << endl;
		//}
		//THCtime = GETTIME();



		Piclist[Picnum] = image.clone();
		Picnum++;
		if (Picnum > 29)
		{
			Picnum = 0;
		}


		//imshow("depthT", imageM);
		//waitKey(2);

		//cout << Gettime() << "  D" << endl;

		for (int i = 0; i < MAXTRACK; i++)
		{
			if (double(Gettime() - Poslist[i][0].T) / 1000 - Poslist[i][Posnum[i]].T > 0.3)
			{
				//if (Posnum[i] > 5)
				//{
				//	ofstream ofile("line.txt");
				//	for (int j = 1; j <= Posnum[i]; j++)
				//	{
				//		ofile << Poslist[i][j].T << "	" << Poslist[i][j].PX << "	" << Poslist[i][j].PY << "	" << Poslist[i][j].PZ << "	" << Poslist[i][j].VX << "	" << Poslist[i][j].VY << "	" << Poslist[i][j].VZ << endl;
				//	}
				//	for (int i = 1; i < savenum; i++)
				//	{
				//		stringstream ss;
				//		string s;
				//		ss << i;
				//		ss >> s;
				//		s = s + ".jpg";
				//		//imwrite(s, saveList[i]);
				//	}
				//	ofile.close();
				//	//return 0;
				//}

				if (Posnum[i] > 2)
				{
					flynow--;
				}
				//Poslist[1] = ball;
				Posnum[i] = 0;
			}

			if ((n > 2) && (Posnum[i] > 2) && (abs(Poslist[i][Posnum[i]].T - (timeList[DepthMatHead] - Poslist[i][0].T) / 1000) < 0.0001))
			{
				//cout << i << "   " << abs(Poslist[i][Posnum[i]].T - (timeList[DepthMatHead] - Poslist[i][0].T) / 1000) << endl;
				//int dept;
				//CvPoint point = FindTheCenterPoint(&imageM, 1, dept);
				//if (point.x*point.y > 0)
				//{
				ostringstream ss;
				string s;
				CvPoint point;
				point.x = int(Poslist[i][Posnum[i]].X);
				point.y = int(Poslist[i][Posnum[i]].Y);
				//int dept =Poslist[i][Posnum[i]].Z;

				//Posnum[i]--;

				putText(image1, "[ ]", cvPoint(point.x - 20, point.y + 10), 0, 1, CV_RGB(255, 255, 255));

				ss << "Track=" << i + 1;
				s = ss.str();
				putText(image1, s, cvPoint(point.x + 20, point.y + 10), 1, 0.8, CV_RGB(255, 255, 255));

				ss.str("");
				ss << "Num=" << Posnum[i] << " T=" << Poslist[i][Posnum[i]].T << " S=" << Poslist[i][Posnum[i]].S;
				s = ss.str();
				putText(image1, s, cvPoint(point.x + 20, point.y + 20), 1, 0.8, CV_RGB(255, 255, 255));


				/*ss.str("");
				ss << " X=" << Poslist[i][Posnum[i]].X << " Y=" << Poslist[i][Posnum[i]].Y << " Z=" << Poslist[i][Posnum[i]].Z;
				s = ss.str();
				putText(image1, s, cvPoint(point.x + 20, point.y + 30), 1, 0.8, CV_RGB(255, 255, 255));

				ss.str("");
				ss << "PX=" << Poslist[i][Posnum[i]].PX << " PY=" << Poslist[i][Posnum[i]].PY << " PZ=" << Poslist[i][Posnum[i]].PZ;
				s = ss.str();
				putText(image1, s, cvPoint(point.x + 20, point.y + 40), 1, 0.8, CV_RGB(255, 255, 255));

				ss.str("");
				ss << "VX=" << Poslist[i][Posnum[i]].VX << " VY=" << Poslist[i][Posnum[i]].VY << " VZ=" << Poslist[i][Posnum[i]].VZ << " V=" << Poslist[i][Posnum[i]].V;
				s = ss.str();
				putText(image1, s, cvPoint(point.x + 20, point.y + 50), 1, 0.8, CV_RGB(255, 255, 255));

				ss.str("");
				ss << "AX=" << Poslist[i][Posnum[i]].AX << " AY=" << Poslist[i][Posnum[i]].AY << " AZ=" << Poslist[i][Posnum[i]].AZ << " AC=" << Poslist[i][Posnum[i]].AC;
				s = ss.str();
				putText(image1, s, cvPoint(point.x + 20, point.y + 60), 1, 0.8, CV_RGB(255, 255, 255));*/

				//Posnum[i]++;

				//putText(image, "123", cvPoint(55, 200), 0, 0.3, CV_RGB(255, 255, 255));

				//saveList[Posnum[i]] = image1.clone();

				//for (int i = 0; i < 2; i++)
				//for (int j = 0; j < 4; j++)
				//{
				//	AX[i][j] = AX[i + 1][j];
				//}

				//AX[2][0] = dept* TanX / 212 * (point.y - 212) / 1000;
				//AX[2][1] = double(dept) / 1000 * SinZ - AX[2][0] * CosZ + AddZ;
				//AX[2][2] = dept* TanY / 256 * (point.x - 256) / 1000;
				//AX[2][0] = -AX[2][0] * SinZ + double(dept) / 1000 * CosZ;
				//AX[2][3] = Gettime();

				//cout << point.x << ' ' << point.y << ' ' << dept << endl;
				//cout << AX[2][0] << ' ' << AX[2][1] << ' ' << AX[2][2] << ' ' << AX[2][3] << endl;

				//if (Posnum[i] > 0)
				//{
				//	cout << endl;
				//	Posnum[i]--;
				//	cout << Posnum[i] << endl;
				//	cout << " X=" << Poslist[i][Posnum[i]].X << "   Y=" << Poslist[i][Posnum[i]].Y << "   Z=" << Poslist[i][Posnum[i]].Z << "   T=" << Poslist[i][Posnum[i]].T << "  S=" << Poslist[i][Posnum[i]].S << endl;
				//	cout << "PX=" << Poslist[i][Posnum[i]].PX << "  PY=" << Poslist[i][Posnum[i]].PY << "  PZ=" << Poslist[i][Posnum[i]].PZ << endl;
				//	cout << "VX=" << Poslist[i][Posnum[i]].VX << "  VY=" << Poslist[i][Posnum[i]].VY << "  VZ=" << Poslist[i][Posnum[i]].VZ << "  V=" << Poslist[i][Posnum[i]].V << endl;
				//	cout << "AX=" << Poslist[i][Posnum[i]].AX << "  AY=" << Poslist[i][Posnum[i]].AY << "  AZ=" << Poslist[i][Posnum[i]].AZ << "  AC=" << Poslist[i][Posnum[i]].AC<< endl;
				//	Posnum[i]++;
				//}

				//cout << Gettime() << "   " << endl;

				if (AX[0][3] > 0)
				{
					//double AV[3];
					//GetV(AX, AV);
					//GuessPos(AX[1][0], AX[1][1], AX[1][2], AV[0], AV[1], AV[2]);

					//SendPos(Poslist[i][int(Poslist[i][0][1])][2] * 100 * KKK + 300, (Poslist[i][int(Poslist[i][0][1])][0] * 100 - 35) *KKK);
					//return 0;

				}

			}
			//}
		}

		if (flynow)
		{
			//savenum++;
			//saveList[savenum] = image1.clone();
		}

		//cout << Gettime() << "  E" << endl;
		//putText(image1, "[ ]", cvPoint(-20, 10), 0, 1, CV_RGB(255, 255, 255));
		//putText(image1, "11", cvPoint(20, 20), 1, 1, CV_RGB(255, 255, 255));
		//imshow("depth123", image1);

		//waitKey(5);

		//static double THDtime = GETTIME();
		//if (ShowIMAGEerroe)
		//{
		//	cout << "THDtime" << THDtime - THLastgettime << endl;
		//}
		//THDtime = GETTIME();

		show = image1.clone();
		canshow = 1;

		{

			//dilate(depthMat, depthMatT, Mat(), Point(-1, -1), 3);

			//memset(mapb, 0, sizeof(mapb));
			for (int i = 0; i < imageM.rows; i++)
			{
				//深度图像数据含有两种格式，这里像素的低12位表示一个深度值，高4位未使用；
				//注意这里需要转换，因为每个数据是2个字节，存储的同上面的颜色信息不一样，

				ushort *ptr = depthMat.ptr<ushort>(i);
				//unsigned char *ptr1 = imageM.ptr<unsigned char>(i);
				for (int j = 0; j<imageM.cols; j++)
				{
					depmap[i][j] = ptr[j];
					if (depmap[i][j] == 0)
					{
						depmap[i][j] = 8192;
					}
					//ptr1[j] =unsigned char( depmap[i][j] >> 0);
					//if ((!mapb[i][j]) && (depmap[i][j] >0))
					//{
					//	BFSdep(i, j, &image);
					//}
				}
			}
		}

		//cout << Gettime() << "  F" << endl;
		//SendPos(Poslist[i][int(Poslist[i][0][1])][2] * 100  + 300, (Poslist[i][int(Poslist[i][0][1])][0] * 100 - 35) );

		imageC2 = imageC1.clone();
		image1 = image.clone();

		//cout << Gettime() << "  G" << endl;

		//imshow("dep", imageM);
		//imshow( "Depth", depthMat );

		//if( cv::waitKey( 5 ) == VK_ESCAPE )
		//{
		//	break;
		//}
		//cout << Gettime() << "  H" << endl;
	//	if (Gettime() - stime > 33)
	//	{
	//		cout << Gettime() - stime << endl;
	//	}



		/*if (WORKTIMEDELAY > 51)
		{
			cout << WORKTIMEDELAY << endl;
		}*/
		


		//static double THEtime = GETTIME();
		//if (ShowIMAGEerroe)
		//{
		//	cout << "THEtime" << THEtime - THLastgettime << endl;
		//}
		//THEtime = GETTIME();
		//

		if (!TrackNow)
		{
			CanErode = 0;
			CanErode2 = 0;
		}

	}

	//SafeRelease( pDepthSource );
	//SafeRelease( pDepthReader );
	//SafeRelease( pDescription );
	//if( pSensor ){
	//	pSensor->Close();
	//}
	//SafeRelease( pSensor );
	LA_ofile.close();
	return 0;
}
