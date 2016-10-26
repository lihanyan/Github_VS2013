//��˶���֡�������ȡ���Ų�ͬ��ͼ��ѭ������

#include<XnCppWrapper.h>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<fstream>

using namespace std;
using namespace cv;

//��������غ궨��
#define TanX 0.599911//0.425
#define TanY 0.413  //0.553  0.57

#define threshold_diff1 10 //���ü�֡���ֵ
#define threshold_diff2 10 //���ü�֡���ֵ

XnMapOutputMode mapMode;
XnMapOutputMode mapMode_RGB;

xn::Context context;
XnStatus result = XN_STATUS_OK;

xn::DepthMetaData depthMD;
xn::ImageMetaData imageMD;

const int cdepthwidth = 640;
const int cdepthheight = 480;
const int ccolorwidth = 640;
const int ccolorheight = 480;

//---------------------------------------------------------------------------
//Global
//---------------------------------------------------------------------------
Mat cvBGRImg, cvDepthImg;

int disMIN = 0000, disMAX = 2000;
int minarea = 12, maxarea = 190, minlen = 12, maxlen = 90;
Mat img_src1, img_src2, img_src3;//3֡����Ҫ3֡ͼƬ
Mat  gray1, gray2, gray3;
Mat gray_diff1, gray_diff2;//�洢2�������ͼƬ
Mat gray;//������ʾǰ����

//---------------------------------------------------------------------------
// Struct
//---------------------------------------------------------------------------
struct mapPointInform
{
	short xGlobe;
	short yGlobe;
	short zGlobe;
	short xImage;
	short yImage;
	short zImage;
};


//---------------------------------------------------------------------------
// Function
//---------------------------------------------------------------------------


int main()
{
	//**********************************��˶��в���***************************************************************
	result = context.Init();

	//��������������Ͳ�ɫ������
	xn::DepthGenerator depthGenerator;
	result = depthGenerator.Create(context);
	xn::ImageGenerator imageGenerator;
	result = imageGenerator.Create(context);

	mapMode.nXRes = cdepthwidth;
	mapMode.nYRes = cdepthheight;
	mapMode.nFPS = 60;
	result = depthGenerator.SetMapOutputMode(mapMode);

	mapMode_RGB.nXRes = ccolorwidth;
	mapMode_RGB.nYRes = ccolorheight;
	mapMode_RGB.nFPS = 60;
	result = depthGenerator.SetMapOutputMode(mapMode_RGB);

	//��������������ӽǶ��뵽��ɫ�������������������׼����ɫ����
	depthGenerator.GetAlternativeViewPointCap().SetViewPoint(imageGenerator);

	//����������������������������
	result = context.StartGeneratingAll();

	result = context.WaitNoneUpdateAll();
	//************************************************************************************************************
	namedWindow("BGRImg");
	createTrackbar("������С", "BGRImg", &disMIN, 5000, 0);
	createTrackbar("�������", "BGRImg", &disMAX, 8000, 0);
	//ifstream indat;
	//ofstream outdat;
	ofstream data("data.txt");
	long len = 0;
	long flen = 0;

	//vector<_Image_List>  Image_List;

	//HANDLE hThread0 = CreateThread(NULL, 0, GetImage, NULL, 0, NULL);//ͼ��ɼ�
	//HANDLE hThread1 = CreateThread(NULL, 0, Image, NULL, 0, NULL);//ͼ����

	//double absolute_start_t = (double)getTickCount();
	//double absolute_t = 0;
	//double last_absolute_t = 0;
	//double tm_absolute_t = 0;
	//TickMeter tm;
	//absolute_t = 1000 * (((double)getTickCount() - absolute_start_t) / getTickFrequency());
	//
	//tm.stop();
	//cout << "����" << tm.getCounter() << "��ʱ��" << tm.getTimeMilli() << "ms" << "    " << (absolute_t - last_absolute_t) << endl;
	//tm.reset();
	//tm.start();
	//last_absolute_t = absolute_t;

	while (1)
	{

		result = context.WaitOneUpdateAll(depthGenerator);
		if (XN_STATUS_OK == result)
		{
			//��ȡRGB��Ϣ
			imageGenerator.GetMetaData(imageMD);
			Mat cvRGBImg(imageMD.FullYRes(), imageMD.FullXRes(), CV_8UC3, (char *)imageMD.Data());
			cvtColor(cvRGBImg, cvBGRImg, CV_RGB2BGR);

			depthGenerator.GetMetaData(depthMD);
			Mat cvRawImg(depthMD.FullYRes(), depthMD.FullXRes(), CV_16UC1, (char *)depthMD.Data());
			cvRawImg.convertTo(cvDepthImg, CV_8UC3, 255.0 / (depthMD.ZRes()));

			imshow("DepthImg", cvDepthImg);
			imshow("RawImg", cvRawImg);
			//imshow("��ͼ", cvBGRImg);


			//����1.5m���µ�
			mapPointInform pointTemp;
			for (int y = 0; y < 240; y++)
			{
				ushort *pr = cvRawImg.ptr<ushort>(y);
				ushort *pd = cvDepthImg.ptr<ushort>(y);
				ushort *pb = cvBGRImg.ptr<ushort>(y);
				for (int x = 0; x < 320; x++)
				{
					pointTemp.zImage = pr[x];
					pointTemp.yImage = y;
					pointTemp.xImage = x;

					//ͼ������ת��
					//pointTemp.yGlobe = pointTemp.zImage* TanX / 120 * (120 - pointTemp.yImage);//(pointTemp.zImage* TanX / 120 * (120 - pointTemp.yImage) / 1000)*1000;
					pointTemp.zGlobe = pointTemp.zImage* TanY / 120 * (120 - pointTemp.yImage);// pointTemp.yGlobe;//(double(pointTemp.zImage) / 1000 * SinZ + pointTemp.yGlobe * CosZ + AddZ)*1000;
					pointTemp.xGlobe = pointTemp.zImage* TanX / 160 * (pointTemp.xImage - 160);// (pointTemp.zImage* TanY / 160 * (pointTemp.xImage - 160) / 1000) * 1000;
					pointTemp.yGlobe = pointTemp.zImage;// (-pointTemp.yGlobe * SinZ + double(pointTemp.zImage) / 1000 * CosZ) * 1000;

					/*			cout << "ͼ������" << pointTemp.xImage << "   " << pointTemp.yImage << "   " << pointTemp.zImage << endl;
					cout << "ȫ������" << pointTemp.xGlobe << "   " << pointTemp.yGlobe << "   " << pointTemp.zGlobe << endl;*/
					if (pointTemp.zGlobe <= 0 || pr[x]<disMIN || pr[x]>disMAX)		//���ƾ���
					{
						//*(cvDepthImg.data + cvDepthImg.step[0] * y + cvDepthImg.step[1] * x) = 0;
						cvDepthImg.at<uchar>(y, x) = 0;

						cvBGRImg.at<Vec3b>(y, x) = 0;
						//*(cvBGRImg.data + cvBGRImg.step[0] * y + cvBGRImg.step[1] * x) = 0;//[row, col]���صĵ� 1 ͨ����ַ�� * ����(blueͨ��) 
						//*(cvBGRImg.data + cvBGRImg.step[0] * y + cvBGRImg.step[1] * x + cvBGRImg.elemSize1()) = 0;//[row, col]���صĵ� 2 ͨ����ַ�� * ����(greenͨ��)
						//*(cvBGRImg.data + cvBGRImg.step[0] * y + cvBGRImg.step[1] * x + cvBGRImg.elemSize1() * 2) = 0;//[row, col]���صĵ� 3 ͨ����ַ�� * ����(redͨ��) 
					}
				}
			}//for,�õ�����1.5m�����ھ��뷶Χ�ڵ�ͼ
			imshow("BGRImg", cvBGRImg);

			cvBGRImg.copyTo(img_src1);
			cvtColor(img_src1, gray1, CV_BGR2GRAY);

			if (!img_src2.empty())
			{	
				cvtColor(img_src2, gray2, CV_BGR2GRAY);
				//֡��
				subtract(gray1, gray2, gray_diff1);
				for (int i = 0; i < gray_diff1.rows; i++)
				{
					for (int j = 0; j < gray_diff1.cols; j++)
					{
						if (abs(gray_diff1.at<unsigned char>(i, j)) >= threshold_diff1)
						{
							gray_diff1.at<unsigned char>(i, j) = 255;
						}
						else gray_diff1.at<unsigned char>(i, j) = 0;
					}
				}
				imshow("gray_diff1", gray_diff1);
		
			}//if (!img_src2.empty())

			if (!img_src3.empty())
			{
				bitwise_and(gray_diff1, gray_diff2, gray);
				imshow("foreground", gray);

				//��̬ѧ
				Mat img_D, img_E, img_O, img_C;
				Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
				//dilate(gray, img_D, element);//����
				//erode(gray, img_E, element);//��ʴ
				//imshow("����", img_D);
				//imshow("��ʴ", img_E);
				morphologyEx(gray, img_C, CV_MOP_CLOSE, element);//������
				//morphologyEx(img_C, img_O, CV_MOP_OPEN, element);//������
				//imshow("������", img_C);
				//imshow("������", img_O);

				//��˹�˲�����
				Mat img_Gauss;
				GaussianBlur(img_C, img_Gauss, Size(3, 3), 0, 0);
				imshow("����˹�˲���", img_Gauss);

				//������
				Mat img_canny, img_contours;
				vector<vector<Point>> contours;
				vector<Vec4i> Hierarchy;
				Canny(img_Gauss, img_canny, 3, 9, 3);
				imshow("�߼�ͼ", img_canny);
				findContours(img_canny, contours, Hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

				vector<Point2f> center(contours.size());
				vector<float> r(contours.size());
				vector<Moments>mu(contours.size());
				img_contours = Mat::zeros(img_canny.size(), CV_8UC3);
				createTrackbar("�����С", "����ͼ", &minarea, 1000, 0);
				createTrackbar("�ܳ���С", "����ͼ", &minlen, 1000, 0);
				createTrackbar("������", "����ͼ", &maxarea, 1000, 0);
				createTrackbar("�ܳ����", "����ͼ", &maxlen, 1000, 0);
				for (int i = 0; i < contours.size(); i++)
				{
					double area = contourArea(contours[i]);
					double len = arcLength(contours[i], true);
					if (area>minarea && area<maxarea && len>minlen && len < maxlen)
					{
					drawContours(img_contours, contours, i, Scalar(0, 255, 255), -1, 8, Hierarchy);
					cout << "��������" << i << "�����" << area << "���ܳ�" << len << endl;
					minEnclosingCircle(contours[i], center[i], r[i]);
					circle(img_contours, center[i], cvRound(r[i]), Scalar(255, 0, 0), 2, 8);

					//mu[i] = moments(contours[i], false);
					//center[i].x = cvRound(mu[i].m10 / mu[i].m00);
					//center[i].y = cvRound(mu[i].m01 / mu[i].m00);
					//cout << "����" << center[i].x << ", " << center[i].y << endl;
					}
				}
				imshow("����ͼ", img_contours);

		
				mapPointInform pT;
				int ax, ay;
				for (int i = 0; i < contours.size(); i++)
				{
					ax = saturate_cast<int>(center[i].x);
					ay = saturate_cast<int>(center[i].y);				
					ushort *pm = cvRawImg.ptr<ushort>(ay);

					pT.zImage = pm[ax];
					pT.yImage = ay;
					pT.xImage = ax;

					//ͼ������ת��
					//pT.yGlobe = pT.zImage* TanX / 120 * (120 - pT.yImage);//(pT.zImage* TanX / 120 * (120 - pT.yImage) / 1000)*1000;
					pT.zGlobe = pT.zImage* TanY / 120 * (120 - pT.yImage);// pT.yGlobe;//(double(pT.zImage) / 1000 * SinZ + pT.yGlobe * CosZ + AddZ)*1000;
					pT.xGlobe = pT.zImage* TanX / 160 * (pT.xImage - 160);// (pT.zImage* TanY / 160 * (pT.xImage - 160) / 1000) * 1000;
					pT.yGlobe = pT.zImage;// (-pT.yGlobe * SinZ + double(pT.zImage) / 1000 * CosZ) * 1000;
					//cout << "�������꣺" << pT.xGlobe << ", " << pT.yGlobe << ", " << pT.zGlobe << endl;

					//data << i << "   " << pT.xGlobe << ", " << pT.yGlobe << ", " << pT.zGlobe << endl;

					//outdat.open("out.dat");
					//if (!outdat)
					//{
					//	cout << "cannot open the outdat" << endl;
					//}
					//outdat.seekp(flen, ios::beg);
					//
					//outdat << i << endl;
					//outdat << pT.xGlobe << " , " << pT.yGlobe << " , " << pT.zGlobe << endl<<endl;
					//flen = outdat.tellp();
					////cout << "�ļ�β" << flen << endl;
					////outdat.seekp(0, ios::cur);
					////len = outdat.tellp();
					////cout << "��ǰλ��" << len << endl << endl;
					//
					//outdat.close();	

					//char buf[256];
					//indat.open("out.dat");
					//if (!indat)
					//{
					//	cout << "can not open the file:in.dat" << endl;
					//	return;
					//}
					//while (!indat.eof())
					//{
					//	indat.getline(buf, 100);
					//	cout << buf << endl;
					//}
				}//for

			}//if (!img_src3.empty())

			img_src2.copyTo(img_src3);
			img_src1.copyTo(img_src2);
			gray_diff1.copyTo(gray_diff2);

		
		}//if(XN_STATUS_OK)
	

		char c = (char)waitKey(1);
		if (c == 27)	break;
		if (c >= 0) waitKey(0);	
	}//while(1)


	data.close();
	return 0;
}


//#include <iostream>
//#include <fstream>
//using namespace std;
//void main()
//{
//	float x=0.1, y=0.2, z=0.3;
//	int  k=9;
//	ifstream indata;
//	ofstream outdata;
//
//	char buf[256];
//	indata.open("out.dat");
//	if (!indata)
//	{
//		cout << "can not open the file:in.dat" << endl;
//		return;
//	}
//	while (!indata.eof())
//	{
//		indata.getline(buf, 100);
//		cout << buf << endl;
//	}
//
//	outdata.open("out.dat");
//	if (!outdata)
//	{
//		cout << "can not open the file :out.dat" << endl;
//		return;
//	}
//	outdata << x << " " << y << endl;
//	outdata << z << " " << k << endl;
//
//	outdata.close();
//	indata.close();
//}


//��֡֡��

//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/core/core.hpp>
//#include"watershedSegmenter.h"
//#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>
//
//using namespace cv;
//using namespace std;
//
//#define threshold_diff1 10 //���ü�֡���ֵ
//#define threshold_diff2 10 //���ü�֡���ֵ
//
//char g_TrackbarName[50];
//int  g_pos = 0;
//long g_frameToStart = 0;
//int g_frameToStop = 1000;
//long g_currentFrame = 0;
//double g_rate = 0;
//int minarea = 12, maxarea = 190, minlen = 12, maxlen = 90;
//Mat img_src1, img_src2, img_src3;//3֡����Ҫ3֡ͼƬ
//Mat img_dst, gray1, gray2, gray3;
//Mat gray_diff1, gray_diff2;//�洢2�������ͼƬ
//Mat gray;//������ʾǰ����
//
//VideoCapture vido_file(0);//���������Ӧ���ļ���
//
//void on_Trackbar(int, void*)
//{
//	vido_file.set(CV_CAP_PROP_POS_FRAMES, g_pos);
//}
//
//int main(int argc, unsigned char* argv[])
//{
//
//	bool pause = false;
//
//	if (!vido_file.isOpened())
//	{
//		cout << "ERROR: Could not access the camera or video!" << endl;
//		exit(1);
//	}
//
//	long totalFrameNumber = (long)vido_file.get(CV_CAP_PROP_FRAME_COUNT);
//	sprintf(g_TrackbarName, "AF %d", totalFrameNumber);
//	vido_file.set(CV_CAP_PROP_POS_FRAMES, g_frameToStart);
//	if (g_frameToStop < g_frameToStart)
//	{
//		cout << "����֡С�ڿ�ʼ֡��������󣬼����˳���" << endl;
//		return -1;
//	}
//	else
//	{
//		cout << "����֡Ϊ����" << g_frameToStop << "֡" << endl;
//	}
//
//	g_currentFrame = g_frameToStart;
//	g_pos = g_frameToStart;
//	g_rate = vido_file.get(CV_CAP_PROP_FPS);
//	cout << "֡��Ϊ:" << g_rate << endl;
//
//	namedWindow("foreground", 0);
//	namedWindow("Extracted frame");
//
//	createTrackbar(g_TrackbarName, "Extracted frame", &g_pos, totalFrameNumber, on_Trackbar);
//	on_Trackbar(0, 0);
//
//	int CAMERA_CHECK_ITERATIONS = 10;
//
//	vido_file >> img_src1;
//	if (img_src1.empty())
//	{
//		printf("read img_src1 error ~!\n\n");
//		cout << "::: Accessing camera :::" << endl;
//		if (CAMERA_CHECK_ITERATIONS > 0) CAMERA_CHECK_ITERATIONS--;
//		else  return 0;
//	}
//	cvtColor(img_src1, gray1, CV_BGR2GRAY);
//	vido_file >> img_src2;
//	if (img_src2.empty())
//	{
//		printf("read img_src2 error ~!\n\n");
//		cout << "::: Accessing camera :::" << endl;
//		if (CAMERA_CHECK_ITERATIONS > 0) CAMERA_CHECK_ITERATIONS--;
//		else  return 0;
//	}
//	cvtColor(img_src2, gray2, CV_BGR2GRAY);
//	subtract(gray2, gray1, gray_diff1);
//	for (int i = 0; i < gray_diff1.rows; i++)
//	{
//		for (int j = 0; j < gray_diff1.cols; j++)
//		{
//
//			if (abs(gray_diff1.at<unsigned char>(i, j)) >= threshold_diff1)//����ģ�����һ��Ҫ��unsigned char�������һֱ����
//				gray_diff1.at<unsigned char>(i, j) = 255;            //��һ�������ֵ����
//			else gray_diff1.at<unsigned char>(i, j) = 0;
//		}
//	}
//	while (1)
//	{
//		vido_file >> img_src3;
//		if (!img_src3.empty())
//		{
//			g_currentFrame = (long)vido_file.get(CV_CAP_PROP_POS_FRAMES);
//			cout << "g_currentFrame : " << g_currentFrame << endl;    
//			setTrackbarPos(g_TrackbarName, "Extracted frame", g_currentFrame);
//			cvtColor(img_src3, gray3, CV_BGR2GRAY);
//			imshow("Extracted frame", img_src3);
//			subtract(gray3, gray2, gray_diff2);
//			for (int i = 0; i < gray_diff2.rows; i++)
//			{
//				for (int j = 0; j < gray_diff2.cols; j++)
//				{
//					if (abs(gray_diff2.at<unsigned char>(i, j)) >= threshold_diff2)//�ڶ��������ֵ����
//						gray_diff2.at<unsigned char>(i, j) = 255;
//					else gray_diff2.at<unsigned char>(i, j) = 0;
//				}
//			}
//			bitwise_and(gray_diff1, gray_diff2, gray);
//			imshow("foreground", gray);
//
//			gray_diff2.copyTo(gray_diff1);
//			gray3.copyTo(gray2);
//			img_src3.copyTo(img_src2);
//
//			Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
//			//dilate(gray, gray, element);
//			//imshow("����", gray);
//			//erode(gray, gray, element);
//			//imshow("��ʴ", gray);
//			morphologyEx(gray, gray, CV_MOP_CLOSE, element);
//			imshow("������", gray);
//			//morphologyEx(gray, gray, CV_MOP_OPEN, element);
//			//imshow("������", gray);
//			//morphologyEx(gray, gray, MORPH_TOPHAT, element);
//			//imshow("��ñ", gray);
//			//morphologyEx(gray, gray, MORPH_BLACKHAT, element);
//			//imshow("��ñ", gray);
//
//			//��˹�˲�����
//			Mat img_Gauss;
//			GaussianBlur(gray, img_Gauss, Size(3, 3), 0, 0);
//			imshow("����˹�˲���", img_Gauss);
//
//			//������
//			Mat img_canny, img_contours;
//			vector<vector<Point>> contours;
//			vector<Vec4i> Hierarchy;
//			Canny(img_Gauss, img_canny, 3, 9, 3);
//			imshow("�߼�ͼ", img_canny);
//			findContours(img_canny, contours, Hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
//
//			vector<Point2f> center(contours.size());
//			vector<float> r(contours.size());
//			img_contours = Mat::zeros(img_canny.size(), CV_8UC3);
//			createTrackbar("�����С", "����ͼ", &minarea, 1000, 0);
//			createTrackbar("�ܳ���С", "����ͼ", &minlen, 1000, 0);
//			createTrackbar("������", "����ͼ", &maxarea, 1000, 0);
//			createTrackbar("�ܳ����", "����ͼ", &maxlen, 1000, 0);
//			for (int i = 0; i < contours.size(); i++)
//			{
//				double area = contourArea(contours[i]);
//				double len = arcLength(contours[i], true);
//				if (area>minarea && area<maxarea && len>minlen && len<maxlen)
//				{
//					drawContours(img_contours, contours, i, Scalar(0, 255, 255), -1, 8, Hierarchy);
//					cout << "��������" << i << "�����" << area << "���ܳ�" << len << endl;
//					minEnclosingCircle(contours[i], center[i], r[i]);
//					circle(img_contours, center[i], cvRound(r[i]), Scalar(255, 0, 0), 2, 8);
//					cout << "Բ��" << center[i].x << ", " << center[i].y << endl;		
//				}
//			}			cout << endl;
//			imshow("����ͼ", img_contours);
//
//			char c = (char)waitKey(1);
//			if (c == 27 || g_currentFrame > g_frameToStop)
//			{
//				break;
//			}
//			if (c >= 0)
//			{
//				waitKey(0);
//			}
//		}
//		else
//		{
//			cout << "::: Accessing camera :::" << endl;
//			if (CAMERA_CHECK_ITERATIONS > 0) CAMERA_CHECK_ITERATIONS--;
//			else  break;
//		}
//	}
//	vido_file.release();
//	return 0;
//}
