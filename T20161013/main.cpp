// meanshift_segmentation.cpp : �������̨Ӧ�ó������ڵ㡣�д�������ٿ�����

//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <iostream>
//
//using namespace cv;
//using namespace std;
//
// 
//Mat src, dst;
//int spatialRad = 10, colorRad = 10, maxPryLevel = 1;
//const Scalar& colorDiff=Scalar::all(1);
//
//void meanshift_seg(int, void *)
//{
//	//����meanshiftͼ����������зָ�
//	pyrMeanShiftFiltering(src, dst, spatialRad, colorRad, maxPryLevel);
//	RNG rng = theRNG();
//	Mat mask(dst.rows + 2, dst.cols + 2, CV_8UC1, Scalar::all(0));
//	for (int i = 0; i<dst.rows; i++)    //opencvͼ��Ⱦ���Ҳ�ǻ���0������
//	for (int j = 0; j<dst.cols; j++)
//	if (mask.at<uchar>(i + 1, j + 1) == 0)
//	{
//		Scalar newcolor(rng(256), rng(256), rng(256));
//		//floodFill(dst, mask, Point(i, j), newcolor, 0, Scalar::all(1), Scalar::all(1));
//		        floodFill(dst,mask,Point(i,j),newcolor,0,colorDiff,colorDiff);
//	}
//	imshow("dst", dst);
//}
//
//
//int main(int argc, uchar* argv[])
//{
//
//	namedWindow("src", WINDOW_AUTOSIZE);
//	namedWindow("dst", WINDOW_AUTOSIZE);
//
//	src = imread("disk1.jpg");
//	//CV_Assert(!src.empty());
//
//	//��ȻcreateTrackbar�����Ĳ���onChange����Ҫ����2��������ʽΪonChange(int,void*)
//	//����������ϵͳ��Ӧ��������ʹ��createTrackbar����ʱ������õĺ������Բ���д����������
//	//���Ŷ�����д����������ú�����ʵ�ֹ����л�����Ҫ����(int,void*)2����������
//	createTrackbar("spatialRad", "dst", &spatialRad, 80, meanshift_seg);
//	createTrackbar("colorRad", "dst", &colorRad, 60, meanshift_seg);
//	createTrackbar("maxPryLevel", "dst", &maxPryLevel, 5, meanshift_seg);
//
//	//    meanshift_seg(0,0);
//
//	imshow("src", src);
//	/*char c=(char)waitKey();
//	if(27==c)
//	return 0;*/
//	imshow("dst", src);
//	waitKey();//���޵ȴ��û�������Ӧ
//	//    while(1);//���ﲻ����while(1)��ԭ������Ҫ�ȴ��û��Ľ�������while(1)û�иù��ܡ���Ȼ2�߶������޵ȴ������á�
//	return 0;
//}


//��˶���

#include <stdio.h>
#include <math.h> 
#include<XnCppWrapper.h>
#include"watershedSegmenter.h"
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>

using namespace std;
using namespace cv;

//********************************��˶��в���*****************************************************************
XnMapOutputMode mapMode;
XnMapOutputMode mapMode_RGB;

xn::Context context;
XnStatus result = XN_STATUS_OK;

xn::DepthMetaData depthMD;
xn::ImageMetaData imageMD;

const int cdepthwidth = 320;
const int cdepthheight = 240;
const int ccolorwidth = 320;
const int ccolorheight = 240;

//��������غ궨��
#define TanX 0.599911//0.425
#define TanY 0.413  //0.553  0.57

//#define disMIN 2000
//#define disMAX 8000
//**************************************************************************************************************


//---------------------------------------------------------------------------
//Global
//---------------------------------------------------------------------------
Mat cvBGRImg, cvDepthImg, HSVImg, img_gray, img_binary, img_contours;
vector<vector<Point>> contours;

int T = 100;
int minarea = 60, maxarea = 800, minlen = 30, maxlen = 100;
int disMIN = 0000, disMAX = 2000;
//int g_slider_position = 1180;
//VideoCapture file("RGB01.avi");
bool pause = false;
Rect rect;//��껭�ľ��ο�
bool rect_flag = false;//���λ
Point origin;//��ʼ����
Mat ROI;//����Ȥͼ

//---------------------------------------------------------------------------
// Struct
//---------------------------------------------------------------------------
struct mapPointInform
{
	int xGlobe;
	int yGlobe;
	int zGlobe;
	int xImage;
	int yImage;
	int zImage;
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------
void HSVWrite(Mat HSVImg)
{
	Mat pImg = Mat(240, 320, CV_8UC3);
	long x, y, z;
	FILE  *fp;
	unsigned char H, S, V;
	unsigned char HSV[64][64][64] = { 0 };

	//����ɫ��
	if ((fp = fopen("ColourTable.txt", "r+")) == NULL)
		cout << "can't open the file\n" << endl;
	for (x = 0; x < 64; x++)
	{
		for (y = 0; y < 64; y++)
		{
			for (z = 0; z < 64; z++)
			{
				rewind(fp);
				fseek(fp, 4 * long(x * 64 * 64 + y * 64 + z), 0);
				fread(&HSV[x][y][z], sizeof(int), 1, fp);
			}
		}
	}
	fclose(fp);
	for (y = 0; y < HSVImg.rows; y++)
	{
		for (x = 0; x < HSVImg.cols; x++)
		{
			//�õ�H,S,Vֵ
			H = *(HSVImg.data + HSVImg.step[0] * y + HSVImg.step[1] * x + HSVImg.elemSize1());
			S = *(HSVImg.data + HSVImg.step[0] * y + HSVImg.step[1] * x + HSVImg.elemSize1() * 2);
			V = *(HSVImg.data + HSVImg.step[0] * y + HSVImg.step[1] * x);
			H = H / 4; S = S / 4; V = V / 4;
			//cout << H +0<< ", " << S+0 << ", " << V +0<< endl;

			////���
			switch (HSV[H][S][V])
			{
			case 0:
				pImg.at<Vec3b>(y, x) = 0;//��ɫ
				cout << "colour=" << HSV[H][S][V] + 0 << endl;
				break;
			case 1:
				pImg.at<Vec3b>(x, y)[0] = 255;
				pImg.at<Vec3b>(x, y)[1] = 0;
				pImg.at<Vec3b>(x, y)[2] = 0;//��ɫ
				cout << "colour=" << HSV[H][S][V] + 0 << endl;
				break;
			case 2:
				pImg.at<Vec3b>(x, y)[0] = 0;
				pImg.at<Vec3b>(x, y)[1] = 0;
				pImg.at<Vec3b>(x, y)[2] = 255;//��ɫ
				cout << "colour=" << HSV[H][S][V] + 0 << endl;
				break;
			default:
				break;
			}
		}
	}

	imshow("��ɫƥ��ͼ", pImg);
}

//void Morphology(Mat &mask_process)
//{
//	//�ӿ�
//	IplImage *mask = &mask_process.operator IplImage();
//
//	//���������ã�ƽ��������ȥ��ϸ��,�Ͽ�ȱ��
//	cvMorphologyEx(mask, mask, NULL, NULL, CV_MOP_OPEN, 1);//������mask���п�������CVCLOSE_ITRΪ�������Ĵ��������Ϊmaskͼ��
//	cvShowImage("������", mask);
//	//���������ã�ƽ������������ȱ��.//������mask���бղ�����CVCLOSE_ITRΪ�ղ����Ĵ��������Ϊmaskͼ��
//	cvMorphologyEx(mask, mask, NULL, NULL, CV_MOP_CLOSE, 1);
//	cvShowImage("������", mask);
//}
//void on_minarea(int, void*){}
//void on_maxarea(int, void*){}
//void on_minlen(int, void*){}
//void on_maxlen(int, void*){}
//void on_disMIN(int, void*){}
//void on_disMAX(int, void*){}

//void onTrackbar(int pos)
//{
//	file.set(CV_CAP_PROP_POS_FRAMES, g_slider_position);
//}

void on_Threshold(int, void*)
{
	threshold(img_gray, img_binary, T, 255, THRESH_BINARY);
	imshow("��ֵͼ", img_binary);
}

void onMouse(int event, int x, int y, int, void*)
{
	switch (event)
	{
		//����������
	case CV_EVENT_LBUTTONDOWN:
		//������ͼ��ʱ�����֮ǰROIͼ�����ʾ����  
		cvDestroyWindow("ROI");
		//�����ʼ����  
		origin = Point(x, y);
		//��ʼ����ʼ���ο�  
		rect = Rect(x, y, 0, 0);
		rect_flag = true;
		break;

		//�ɿ�������      
	case CV_EVENT_LBUTTONUP:
		if (rect.height > 0 && rect.width > 0)
		{
			//��img�еľ��������Ƹ�roi������ʾ
			ROI = HSVImg(Rect(rect.x, rect.y, rect.width, rect.height));
			//rectangle(HSVImg, rect, Scalar(0, 0, 255), 1);

			//��ʾROIͼ��
			namedWindow("ROI");
			imshow("ROI", ROI);
			//waitKey(0);
		}
		rect_flag = false;
		break;

		//�ƶ����
	case CV_EVENT_MOUSEMOVE:
		if (rect_flag)
		{
			//��MIN�õ����ϵ���Ϊ���ο����ʼ���꣬������������������ʱֻ����һ���������  
			rect.x = MIN(x, origin.x);
			rect.y = MIN(y, origin.y);
			rect.width = abs(origin.x - x);
			rect.height = abs(origin.y - y);
			//��ֹ�������򳬳�ͼ��ķ�Χ  
			rect &= Rect(0, 0, HSVImg.cols, HSVImg.rows);
		}
		break;
	}
	unsigned char H, S, V;
	for (int j = 0; j < ROI.rows; j++)
	{
		uchar* data = ROI.ptr<uchar>(j);
		for (int i = 0; i < ROI.cols; i++)
		{
			//�õ�H,S,Vֵ
			//H = *(ROI.data + ROI.step[0] * j + ROI.step[1] * i + ROI.elemSize1());
			//S = *(ROI.data + ROI.step[0] * j + ROI.step[1] * i + ROI.elemSize1() * 2);
			//V = *(ROI.data + ROI.step[0] * j + ROI.step[1] * i);
			//cout << H + 0 << ", " << S + 0 << ", " << V + 0 << endl;
			cout << data[i] + 0 << endl;
		}
	}
}


void Contours(Mat img_binary)
{
	//��ȡ������������С���ƣ����
	//vector<vector<Point>> contours;
	vector<Vec4i> Hierarchy;
	Mat  img_canny;

	//��̬ѧ
	Mat img_D, img_E, img_O, img_C;
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	morphologyEx(img_binary, img_C, CV_MOP_CLOSE, element);//������
	morphologyEx(img_C, img_O, CV_MOP_OPEN, element);//������
	
	//��˹�˲�����
	Mat img_Gauss;
	GaussianBlur(img_O, img_Gauss, Size(3, 3), 0, 0);
	imshow("����˹�˲���", img_Gauss);

	Canny(img_Gauss, img_canny, 3, 9, 3);
	imshow("�߼�ͼ", img_canny);

	findContours(img_canny, contours, Hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	img_contours = Mat::zeros(img_canny.size(), CV_8UC3);
	createTrackbar("�����С", "����ͼ", &minarea, 1000, 0);
	createTrackbar("�ܳ���С", "����ͼ", &minlen, 1000, 0);
	createTrackbar("������", "����ͼ", &maxarea, 1000, 0);
	createTrackbar("�ܳ����", "����ͼ", &maxlen, 1000, 0);

	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color(rand() & 255, rand() & 255, rand() & 255);

		double area = contourArea(contours[i]);
		double len = arcLength(contours[i], true);

		/*	if (area>minarea && area<maxarea && len>minlen && len<maxlen)
		{*/
		drawContours(img_contours, contours, i, Scalar(255, 255, 255), -1, 8, Hierarchy);
		cout << "������" << i << "���" << area << "�ܳ�" << len << endl;
		//}
	}
	//imshow("����ͼ", img_contours);
}

void Watershed(Mat img_binary)
{
	//subtract(gray1, gray2, gray_diff);
	//��ˮ���㷨
	Mat fg;
	erode(img_binary, fg, Mat(), Point(-1, -1), 1);
	Mat bg;
	dilate(img_binary, bg, Mat(), Point(-1, -1), 1);
	threshold(bg, bg, 1, 128, THRESH_BINARY_INV);

	Mat markers(img_binary.size(), CV_8U, Scalar(0));
	markers = fg + bg;
	imshow("ǰ��", fg);
	imshow("����", bg);
	imshow("���", markers);

	// Create watershed segmentation object
	WatershedSegmenter segmenter;

	// Set markers and process
	segmenter.setMarkers(markers);
	segmenter.process(cvBGRImg);

	Mat img_water = segmenter.getSegmentation();

	imshow("��ˮ��ͼ", img_water);
	imshow("�ָ���", segmenter.getWatersheds());

	//Contours(img_water);
}


int main()
{//**********************************��˶��в���***************************************************************
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

	//namedWindow("BGR");
	//int frames =file.get(CV_CAP_PROP_FRAME_COUNT);
	//if (frames != 0)
	//{
	//	cvCreateTrackbar("������","BGR",&g_slider_position,frames,onTrackbar);
	//}
	namedWindow("BGRImg");
	createTrackbar("������С", "BGRImg", &disMIN, 5000, 0);
	createTrackbar("�������", "BGRImg", &disMAX, 8000, 0);
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

	/*		imshow("RawImg", cvRawImg);
			imshow("DepthImg", cvDepthImg);*/

			mapPointInform pointTemp;
			for (int y = 0; y < 240; y++)
			{
				ushort *py = cvRawImg.ptr<ushort>(y);
				for (int x = 0; x < 320; x++)
				{			
					pointTemp.zImage = py[x];
					pointTemp.yImage = y;
					pointTemp.xImage = x;

					//ͼ������ת��
					//pointTemp.yGlobe = pointTemp.zImage* TanX / 120 * (120 - pointTemp.yImage);//(pointTemp.zImage* TanX / 120 * (120 - pointTemp.yImage) / 1000)*1000;
					pointTemp.zGlobe = pointTemp.zImage* TanY / 120 * (120 - pointTemp.yImage);// pointTemp.yGlobe;//(double(pointTemp.zImage) / 1000 * SinZ + pointTemp.yGlobe * CosZ + AddZ)*1000;
					pointTemp.xGlobe = pointTemp.zImage* TanX / 160 * (pointTemp.xImage - 160);// (pointTemp.zImage* TanY / 160 * (pointTemp.xImage - 160) / 1000) * 1000;
					pointTemp.yGlobe = pointTemp.zImage;// (-pointTemp.yGlobe * SinZ + double(pointTemp.zImage) / 1000 * CosZ) * 1000;

					/*			cout << "ͼ������" << pointTemp.xImage << "   " << pointTemp.yImage << "   " << pointTemp.zImage << endl;
					cout << "ȫ������" << pointTemp.xGlobe << "   " << pointTemp.yGlobe << "   " << pointTemp.zGlobe << endl;*/

					if (pointTemp.zGlobe <= 0 || (py[x] > disMAX) ||( py[x] <disMIN))
					{
						*(cvDepthImg.data + cvDepthImg.step[0] * y + cvDepthImg.step[1] * x) = 0;
						//cvDepthImg.at<uchar>(y, x) = 0;

						cvBGRImg.at<Vec3b>(y, x) = 0;
						//*(cvBGRImg.data + cvBGRImg.step[0] * y + cvBGRImg.step[1] * x) = 0;//[row, col]���صĵ� 1 ͨ����ַ�� * ����(blueͨ��) 
						//*(cvBGRImg.data + cvBGRImg.step[0] * y + cvBGRImg.step[1] * x + cvBGRImg.elemSize1()) = 0;//[row, col]���صĵ� 2 ͨ����ַ�� * ����(greenͨ��)
						//*(cvBGRImg.data + cvBGRImg.step[0] * y + cvBGRImg.step[1] * x + cvBGRImg.elemSize1() * 2) = 0;//[row, col]���صĵ� 3 ͨ����ַ�� * ����(redͨ��) 
					}	
				}
			}//for,�õ�����1.5m�����ھ��뷶Χ�ڵ�ͼ
			imshow("BGRImg", cvBGRImg);

			//	file >> cvBGRImg;
			//	imshow("BGR", cvBGRImg);
			//		//�����°벿��
			//	for (int y = cvBGRImg.rows / 2; y < cvBGRImg.rows; y++)
			//	{
			//		for (int x = 0; x < cvBGRImg.cols; x++)
			//		{
			//			cvBGRImg.at<Vec3b>(y, x) = 0;
			//		}
			//	}

			//ת����HSV�ռ�----------------------------------------------------------------------------------
			//cvtColor(cvBGRImg, HSVImg, CV_BGR2HSV);
			//imshow("HSV", HSVImg);
			//HSVWrite(HSVImg);//��ɫƥ��
			//��׽���
			//setMouseCallback("HSV", onMouse, 0);

			//�Ҷ�ͼ����ֵͼ-----------------------------------------------------------------------------------
			//namedWindow("�Ҷ�ͼ");
			cvtColor(cvBGRImg, img_gray, CV_BGR2GRAY);

			namedWindow("��ֵͼ");
			//threshold(img_gray, img_binary, T, 255, THRESH_BINARY);
			//imshow("��ֵͼ", img_binary);
			createTrackbar("��ֵ", "��ֵͼ", &T, 255, on_Threshold);
			on_Threshold(0, 0);//��ֵ��

			//Watershed(img_binary);//��ˮ����㷨
			Contours(img_binary);

			//����������,����
			vector<Moments> mu(contours.size());
			vector<Point2f> mc(contours.size());
			vector<float> r(contours.size());
			mapPointInform pT;
			for (int i = 0; i < contours.size(); i++)
			{
				minEnclosingCircle(contours[i], mc[i], r[i]);
				circle(img_contours, mc[i], cvRound(r[i]), Scalar(255, 0, 0), 2, 8);
				imshow("����ͼ", img_contours);

			/*	mu[i] = moments(contours[i], false);
				mc[i].x = saturate_cast<ushort>(mu[i].m10 / mu[i].m00);
				mc[i].y = saturate_cast<ushort>(mu[i].m01 / mu[i].m00);
				cout << "����" << mc[i].x << ", " << mc[i].y << endl;
*/
				int b = saturate_cast<int>(mc[i].y), a = saturate_cast<int>(mc[i].x);
					ushort *pm = cvRawImg.ptr<ushort>(b);

					pT.zImage = pm[a];
					pT.yImage = b;
					pT.xImage = a;	

					//ͼ������ת��
					//pT.yGlobe = pT.zImage* TanX / 120 * (120 - pT.yImage);//(pT.zImage* TanX / 120 * (120 - pT.yImage) / 1000)*1000;
					pT.zGlobe = pT.zImage* TanY / 120 * (120 - pT.yImage);// pT.yGlobe;//(double(pT.zImage) / 1000 * SinZ + pT.yGlobe * CosZ + AddZ)*1000;
					pT.xGlobe = pT.zImage* TanX / 160 * (pT.xImage - 160);// (pT.zImage* TanY / 160 * (pT.xImage - 160) / 1000) * 1000;
					pT.yGlobe = pT.zImage;// (-pT.yGlobe * SinZ + double(pT.zImage) / 1000 * CosZ) * 1000;

					cout << "�������꣺" << pT.xGlobe << "," << pT.yGlobe << "," << pT.zGlobe << endl;
			}//for

		
			char c = waitKey(10);
			if (c == 27) break;  
			if (c >= 0) waitKey(0);
		}//if (XN_STATUS_OK == result)
	}//while (1)
}


//#include<opencv2/opencv.hpp>
//#include<opencv2/highgui/highgui.hpp>
//#include<iostream>
//
//using namespace std;
//using namespace cv;
//
//int main()
//{
//	VideoCapture file("RGB02.avi");
//	Mat cvBGRImg;
//	while (1)
//	{
//		file >> cvBGRImg;
//		imshow("", cvBGRImg);
//		char c = cvWaitKey(30);
//				if (c == 27) break;
//	}
//	return 0;
//}