#include "stdafx.h"
#include "HUST_ReLocal.h"
#include <time.h>
#include <iostream>

using namespace std;

#define _GKDEBUG
#define CAMERA_SPAN
#define ROBOT_A

#ifdef ROBOT_A
#define Kx 175			//X方向满像素距离
#define Ky 130			//Y方向满像素距离
#define KxP 1			//比例系数
#define KyP 1			//比例系数
#define WidthThesMin 120	//角点正方形宽度最小值
#define WidthThesMax 180	//角点正方形宽度最大值
#define POSDEPTH	5		//数组深度
#define AGLDEPTH	5		//数组深度
#else	//ROBOT_B
#define ROBOT_B
#define Kx 200			//X方向满像素距离
#define Ky 152			//Y方向满像素距
#define KxP 1			//比例系数
#define KyP 1			//比例系数
#define WidthThesMin 70	//角点正方形宽度最小值
#define WidthThesMax 200	//角点正方形宽度最大值
#define POSDEPTH	5		//数组深度
#define AGLDEPTH	5		//数组深度离
#endif



HUST_ReLocal::HUST_ReLocal()
{
	InitPara();
}

HUST_ReLocal::HUST_ReLocal(HUST_CAN *SRCCAN)
{
	InitPara();
	CAN = SRCCAN;
	CAN->pm_camState = &m_camState;
}

HUST_ReLocal::HUST_ReLocal(HUST_CAN *SRCCAN, int device)
{
	InitPara();
	CAN = SRCCAN;
	CAN->pm_camState = &m_camState;
	OpenCam(device);
	deviceNum = device;
}


HUST_ReLocal::~HUST_ReLocal()
{
	/*CloseHandle(m_HoughDealThread);
	CloseHandle(m_CameraCapture);*/
	
}

void HUST_ReLocal::initClass(HUST_CAN* SRCCAN, int device)	//类初始化
{
	InitPara();
	CAN = SRCCAN;
	CAN->pm_camState = &m_camState;
	OpenCam(device);
	deviceNum = device;
}

void HUST_ReLocal::InitPara()
{
	canny_thres1_standrad = 64;
	canny_thres2_standrad = 167;
	hough_thres_standrad = 225;
	canny_threshold1 = 91;
	canny_threshold2 = 165;
	hough_threshold = 10;
	hough_para1 = 66;
	hough_para2 = 60;
	tirckness = 3;
	m_camState = IDLE;
}

bool HUST_ReLocal::OpenCam(int device)
{
	//声明IplImage指针
	//VideoCapture cap(device);
	deviceNum = device;

	cap = new VideoCapture;

	cap->open(device);
	if (!cap->isOpened())
	{
		MySetColor(C_BLACKGROUND, C_WARNING);
		cout << "OPEN CAMERA ERROR!" << endl;
		MySetColor(C_BLACKGROUND, C_NOMAL);
		return false;
	}
	else
	{
		cout << "OPEN CAMERA SUCCESS!" << endl;
	}

	//获取摄像头
	pt_init.x = 320;
	pt_init.y = 240;

	storage = cvCreateMemStorage();
	(*cap) >> frame;
	cout <<  frame.cols << endl;
	while (frame.cols <= 0)
	{
		cout << "judge" << endl;
		(*cap) >> frame;
		waitKey(17);
	}

	srcRGB = Mat(frame.rows, frame.cols, CV_8UC3, Scalar(0, 0, 0));		//原始图
	dst = Mat(frame.rows, frame.cols, CV_8UC3, Scalar(0, 0, 0));		//霍夫图
	dst2 = Mat(frame.rows, frame.cols, CV_8UC3, Scalar(0, 0, 0));		//处理后的图
	src = Mat(frame.rows, frame.cols, CV_8UC1, Scalar(0, 0, 0));		//灰度图
	src2 = Mat(frame.rows, frame.cols, CV_8UC1, Scalar(0, 0, 0));		//
	edge = Mat(frame.rows, frame.cols, CV_8UC1, Scalar(0, 0, 0));		//边缘图
	
	//mask = Mat(frame.cols, frame.rows, CV_8UC1, Scalar(255,255,255));	//掩码图

	//	m_CameraCapture = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)m_CameraCapture, this, NULL, NULL);//开两个线程有问题
	cout << "启动线程" << endl;
	m_HoughDealThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ImageDealThread_Hough, this, NULL, NULL);
	return true;
}


void HUST_ReLocal::CameraCapture()
{
	while (1)
	{
		(*cap) >> frame;
		waitKey(17);
	}
}

//判断是否要发送数据
void HUST_ReLocal::Send_PositionShifting(MyDisErr* err, int len)
{
	MyDisErr err_t;
	char count = 0;
	err_t.x = 0.0;
	err_t.y = 0.0;
	for (int i = 1; i < len; i++)
	{
		if (err[i].isErrValue)
		{

			count++;
			err_t.x += err[i].x;
			err_t.y += err[i].y;
		}
	}
	if (count>1)
	{
		err_t.x /= count;
		err_t.y /= count;
		if (m_camState == RE_POS)
		{
			CAN->Send_RobotRelocalPosition_Frame(ROBOT_CONTROL_DEVICEID, err_t.x, err_t.y);		//发送矫正位置的CAN数据
		}
		std::cout << "CAN POSITION:" << err_t.x << "\t" << err_t.y << "*********************************" << endl;
	}
	else
	{
		std::cout << "CAN POSITION DATA ERR!********************************************************" << endl;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HUST_ReLocal::Send_AngleShifting(float* angleShifting, int len)
{
	float m_aglShifting=0.0;
	int count = 0;
	for (int i = 1; i < len; i++)
	{
		if (angleShifting[i]<20.0)
		{
			count++;
			m_aglShifting += angleShifting[i];
		}
	}
	if (count>1)
	{
		m_aglShifting /= count;
		if (m_camState == RE_AGL)
		{
			CAN->Send_RobotRelocalAngle_Frame(ROBOT_CONTROL_DEVICEID,(float) (m_aglShifting - 0.75));		//发送矫正姿态的数据
		}
		std::cout << "CAN ANGLE:" << m_aglShifting-0.75 <<"******************************************"<< endl;
	}
	else
	{
		std::cout << "CAN ANGLE DATA ERROR!******************************************************" << endl;
	}

}


//can中用指针传递，把m_camState的指针传过去。
void HUST_ReLocal::HougtDeal_ThreadLoop(){
	MyDisErr err[POSDEPTH];
	float m_aglShifting[AGLDEPTH];
	while (1){
		/*if (cap.isOpened())
		{
			cap >> frame;
		}*/
		(*cap) >> frame;
		cout  <<"frame:"<< frame.cols << "\t" << frame.rows << endl;
		cout << "cap  :" << cap->get(CV_CAP_PROP_FRAME_WIDTH) << "\t" << cap->get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
		while (640 != frame.cols || 480 != frame.rows)
		{
			//frame = Mat(480, 640, CV_8UC3, Scalar(0, 0, 0));
			//cap >> frame;
			cout << cap->get(CV_CAP_PROP_FRAME_WIDTH) << "\t" << cap->get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
			//VideoCapture capTemp;
			//capTemp.open(deviceNum);
			//cout << "cam error:" << capTemp.get(CV_CAP_PROP_FRAME_HEIGHT) << "\t" << capTemp.get(CV_CAP_PROP_FRAME_WIDTH) << endl;

			cap->release();


			//cap.retrieve(frame);

			cap = new VideoCapture;

			cap->open(deviceNum);

			/*while (1)
			{
				cap->release();
				cap = new VideoCapture;

				Sleep(600);
				cout << "**" << endl;
			}*/


			(*cap) >> frame;
			Sleep(500);
			//continue;
		}
		continue;
		switch (m_camState)
		{
		case IDLE:
			//ImageDealHoughProbabilistic_Position();
			//ImageDealHoughStandard_Angle();
			(*cap) >> frame;
			cv::imshow("video", frame);
			cvWaitKey(33);
			break;
		case RE_AGL:
		case TEST_AGL:
			for (int i = 0; i < AGLDEPTH; i++)
			{
				(*cap) >> frame;
				m_aglShifting[i] = ImageDealHoughStandard_Angle();
			}
			Send_AngleShifting(m_aglShifting, AGLDEPTH);
			Sleep(1);
			cvWaitKey(1);
			m_camState = IDLE;
			break;
		case RE_POS:
		case TEST_POS:
			for (int i = 0; i < POSDEPTH; i++)
			{
				(*cap) >> frame;
				err[i] = ImageDealHoughProbabilistic_Position();
				cvWaitKey(1);	
				//cap >> frame;
			}
			Send_PositionShifting(err, POSDEPTH);
			Sleep(1);
			cvWaitKey(1);
			m_camState = IDLE;
			break;
		default:
			(*cap) >> frame;
			ImageDealHoughProbabilistic_Position();
			cvWaitKey(1);
			Sleep(1);
			break;
		}
		//m_camState = IDLE;
	
	}
	cvWaitKey(10);
}

/*******************************************************************************
* Function Name  : ImageDealHoughStandard_Angle
* Description    : 重定位角度
* Input          : void
* Output         : m_angleShifting 角度偏差
* Return         : 角度偏差 单位 °（度）
*******************************************************************************/
float HUST_ReLocal::ImageDealHoughStandard_Angle()
{
	int start = clock();
	float m_angleShifting = 0.0;		//角度偏移
	int m_lineCount = 0;				//直线数量
	srcRGB = frame.clone();
	cvtColor(srcRGB, src, CV_RGB2GRAY);
	dilate(src, src2, Mat(5, 5, CV_8U), Point(-1, -1), 5);
	erode(src2, src, Mat(5, 5, CV_8U), Point(-1, -1), 5);
	
	Canny(src, edge, canny_thres1_standrad, canny_thres2_standrad);
	dst = srcRGB.clone();
	dst2 = srcRGB.clone();
	if (hough_thres_standrad){
		lines = cvHoughLines2(&IplImage(edge), storage, CV_HOUGH_STANDARD, 2, CV_PI / 180/10, hough_thres_standrad, 0, 0);
	}
	else
	{
		hough_thres_standrad = 200;
		lines = cvHoughLines2(&IplImage(edge), storage, CV_HOUGH_STANDARD, 2, CV_PI / 180/10, hough_thres_standrad, 0, 0);
	}
	std::cout << "线条个数" << lines->total << endl;
#ifdef _GKDEBUG
	for (int i = 0; i<MIN(lines->total, 100); i++)
	{
		float* line = (float*)cvGetSeqElem(lines, i);
		float rho = line[0];
		float theta = line[1];
		double a = cos(theta), b = sin(theta);
		double x0 = rho*a, y0 = rho*b;
		CvPoint p1, p2;
		p1.x = cvRound(x0 + 1000 * (-b));
		p1.y = cvRound(y0 + 1000 * a);
		p2.x = cvRound(x0 - 1000 * (-b));
		p2.y = cvRound(y0 - 1000 * a);
	/*	if (70.0 / 180 * CV_PI < theta && 110.0 / 180 * CV_PI>theta)
		{
			m_angleShifting += theta;
			m_lineCount++;
		}*/
		if (10.0 / 180 * CV_PI > theta || 170.0 / 180 * CV_PI < theta)
		{
			if (theta < 0.3)
			{
				m_angleShifting += theta;
			}
			else
			{
				m_angleShifting += (float)(theta-CV_PI);
			}
			m_lineCount++;

		}
		cvLine(&IplImage(dst), p1, p2, CV_RGB(255, 0, 0), 1, 8, 0);
		std::cout << "极轴长:\t" << rho << '\t' << "角度:\t" << theta * 180 / CV_PI << endl;
		std::cout << "(" << p1.x << "," << p1.y << ")" << '\t' << "(" << p2.x << "," << p2.y << ")" << '\t' << endl;
	}
	if (m_lineCount > 0)
	{
		m_angleShifting /= m_lineCount;
		std::cout << "angleShifting:" << m_angleShifting * 180 / CV_PI << endl;
		float rho = 240;
		double a = cos(m_angleShifting), b = sin(m_angleShifting);
		double x0 = rho*a, y0 = rho*b;
		CvPoint p1, p2;
		p1.x = cvRound(x0 + 1000 * (-b));
		p1.y = cvRound(y0 + 1000 * a);
		p2.x = cvRound(x0 - 1000 * (-b));
		p2.y = cvRound(y0 - 1000 * a);
		cvLine(&IplImage(dst), p1, p2, CV_RGB(255, 0, 0), tirckness, 8, 0);
//		CAN->Send_RobotRelocalAngle_Frame(ROBOT_CONTROL_DEVICEID, m_angleShifting * 180 / CV_PI);		//发送矫正姿态的数据
//		cout << "CAN ANGLE**************************************************************" << endl;
	}
	else
	{
		m_angleShifting = 90.0;
	}
#else
	for (int i = 0; i<MIN(lines->total, 100); i++)
	{
		float* line = (float*)cvGetSeqElem(lines, i);
		float rho = line[0];
		float theta = line[1];

		//if (70.0 / 180 * CV_PI < theta && 110.0 / 180 * CV_PI>theta)
		//{
		//	m_angleShifting += theta;
		//	m_lineCount++;
		//}
		if (10.0 / 180 * CV_PI > theta || 170.0 / 180 * CV_PI < theta)
		{
			if (theta < 0.3)
			{
				m_angleShifting += theta;
			}
			else
			{
				m_angleShifting += (float)(theta - CV_PI);
			}
			m_lineCount++;

}
	}
	if (m_lineCount > 0)
	{
		m_angleShifting /= m_lineCount;
		std::cout << "angleShifting:" << m_angleShifting * 180 / CV_PI << endl;
//		CAN->Send_RobotRelocalAngle_Frame(ROBOT_CONTROL_DEVICEID, m_angleShifting * 180 / CV_PI);		//发送矫正姿态的数据
//		cout << "CAN ANGLE**************************************************************" << endl;
	}
#endif		
#ifdef _GKDEBUG
	cv::imshow("hough", dst);
	cv::imshow("edge", edge);
	cv::imshow("src", src);
	cvCreateTrackbar("threshold1", "edge", &canny_thres1_standrad, 720, NULL);
	cvCreateTrackbar("threshold2", "edge", &canny_thres2_standrad, 720, NULL);
	cvCreateTrackbar("hough_threshold", "hough", &hough_thres_standrad, 640, NULL);
	cvCreateTrackbar("tirckness","hough",&tirckness,480,NULL);
#endif

	int end = clock();
	std::cout << "dealTime:\t" << end - start << "ms" << endl;
	std::cout << endl << endl << endl;
	cvWaitKey(1);
	return (float)(m_angleShifting * 180 / CV_PI);

}


/*******************************************************************************
* Function Name  : ImageDealHoughProbabilistic_Position
* Description    : 位置重定位
* Input          : void
* Output         : err 位置误差
* Return         : 位置偏差 单位（毫米）
*******************************************************************************/
MyDisErr HUST_ReLocal::ImageDealHoughProbabilistic_Position()
{
	MyDisErr err;
	int start = clock();
	srcRGB = frame.clone();
	cvtColor(srcRGB, src, CV_RGB2GRAY);
	dilate(src, src2, Mat(5, 5, CV_8U), Point(-1, -1), 5);
	erode(src2, src, Mat(5, 5, CV_8U), Point(-1, -1), 5);
	Canny(src, edge, canny_threshold1, canny_threshold2);
	dst = srcRGB.clone();
	dst2 = srcRGB.clone();
	lines = cvHoughLines2(&IplImage(edge), storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, hough_threshold, hough_para1, hough_para2);
	MyConner conner;
	conner = FindConner(lines);
#ifdef _GKDEBUG

	cvLine(&IplImage(dst), conner.leftup, conner.rightup, CV_RGB(255, 0, 0), 1, 8, 0);
	cvLine(&IplImage(dst), conner.leftup, conner.leftdown, CV_RGB(255, 0, 0), 1, 8, 0);
	cvLine(&IplImage(dst), conner.leftdown, conner.rightdown, CV_RGB(255, 0, 0), 1, 8, 0);
	cvLine(&IplImage(dst), conner.rightup, conner.rightdown, CV_RGB(255, 0, 0), 1, 8, 0);

#endif
	CvPoint pt;
	pt.x = (conner.leftup.x + conner.rightup.x + conner.leftdown.x + conner.rightdown.x) / 4;
	pt.y = (conner.leftup.y + conner.rightup.y + conner.leftdown.y + conner.rightdown.y) / 4;
#ifdef _GKDEBUG
	cvCircle(&IplImage(dst), pt, 5, CV_RGB(255, 0, 0), 1, 8, 3);
	std::cout << "霍夫中心点：（" << pt.x << "," << pt.y << ")" << endl;
#endif
	err = CountErr(pt, pt_init);
	//if (conner.isConner)
	//{
	//	CAN->Send_RobotRelocalPosition_Frame(ROBOT_CONTROL_DEVICEID, err.x, err.y);//发送矫正位置的CAN数据
	//	cout << "CAN POSITION**************************************************************" << endl;
	//}
	std::cout << endl;
#ifdef _GKDEBUG
	for (int i = 0; i<lines->total; ++i)
	{
		CvPoint *line = (CvPoint*)cvGetSeqElem(lines, i);
		if (JudgeLine(line[0], line[1]))
		{
			cvLine(&IplImage(dst2), line[0], line[1], CV_RGB(255, 0, 0), 1, 8);
		}
	}
	cv::imshow("video", frame);
	cv::imshow("edge", edge);
	cv::imshow("hough", dst);
	cv::imshow("hough2", dst2);
	cv::imshow("src", src);
	cvCreateTrackbar("threshold1", "edge", &canny_threshold1, 360, NULL);
	cvCreateTrackbar("threshold2", "edge", &canny_threshold2, 360, NULL);

	cvCreateTrackbar("hough_threshold", "hough2", &hough_threshold, 360, NULL);
	cvCreateTrackbar("hough_para1", "hough2", &hough_para1, 360, NULL);
	cvCreateTrackbar("hough_para2", "hough2", &hough_para2, 360, NULL);
#endif
	//if (m_command == 'a')
	//{
	//	pt_init = pt;
	//	m_command = 0;
	//}
	cv::waitKey(1);
	int end = clock();
	std::cout << "处理用时:\t" << end - start << "ms" << endl;
	return err;
}

MyConner HUST_ReLocal::FindConner(CvSeq *lines){
	MyConner conner;
	if (lines->total < 2){
		cout << "没有找到符合直线";
		return conner;
	}
	MyLine *pFlag = new MyLine[lines->total];//标记是否符合测定的曲线
	//memset(pFlag, -1,lines->total*sizeof(int));
	Scan(pFlag, lines);//填充pFlag

	conner = DealLine(pFlag, lines->total);
	delete[] pFlag;
	return conner;
}

void HUST_ReLocal::Scan(MyLine *pFlag, CvSeq *lines)
{

	for (int i = 0; i<lines->total; i++)
	{
		CvPoint *line = (CvPoint*)cvGetSeqElem(lines, i);
		float theta = (float)atan2((line[1].y - line[0].y), (line[1].x - line[0].x));

		//		cout << theta*180 / CV_PI << '\t';
		if (abs(theta * 180 / CV_PI) < 10)
		{
			pFlag[i].lineFlag = 0;	//表示可能是水平线
			pFlag[i].theta = theta;//角度
			pFlag[i].y = -((float)(line[1].y - line[0].y) / (line[1].x - line[0].x))*line[0].x + line[0].y;	//与Y交线
			pFlag[i].x = 0;
		}
		else if (abs(theta * 180 / CV_PI) < 80)
		{
			pFlag[i].lineFlag = -1; //干扰线
			pFlag[i].theta = -1;
			pFlag[i].x = -1;
			pFlag[i].y = -1;

		}
		else
		{
			pFlag[i].lineFlag = 1;	//表示可能是垂直线
			pFlag[i].theta = theta;//角度
			pFlag[i].x = -((float)(line[1].x - line[0].x)) / (line[1].y - line[0].y)*line[0].y + line[0].x;	//与X交线
			pFlag[i].y = 0;
		}



		/*	cout << "(" << line[0].x << "," << line[0].y << ")" << '\t' << "(" << line[1].x << "," << line[1].y << ")" << '\t'
		<< "权重:" << abs(line[1].x - line[0].x) + abs(line[1].y - line[0].y) << '\t'
		<< "theta:" << theta * 180 / CV_PI << "\t";
		pFlag[i].lineFlag == 0 ?
		cout << "与Y交线" << pFlag[i].y << endl :
		pFlag[i].lineFlag == 1 ?
		cout << "与X交线" << pFlag[i].x << endl :
		cout << "不满足要求,舍去" << endl;*/
	}
	//cout << endl << endl << endl;
	//	cout << endl;

}

MyConner HUST_ReLocal::DealLine(MyLine *pFlag, int len)
{
	MyConner conner;
	float thetaH = 0;
	float thetaV = 0;
	int thetaH_Num = 0;
	int thetaV_Num = 0;
	float x_count = 0;
	float y_count = 0;
	int x_count_Num = 0;
	int y_count_Num = 0;
	float x_count_VL = 0, x_count_VR = 0;
	float y_count_HU = 0, y_count_HD = 0;
	int x_count_Num_VL = 0, x_count_Num_VR = 0;
	int y_count_Num_HU = 0, y_count_Num_HD = 0;
	float left_V = 0, right_V = 0, up_H = 0, down_H = 0;

	for (int i = 0; i < len; i++){
		if (pFlag[i].lineFlag == 0)		//水平线
		{
			thetaH += pFlag[i].theta;
			thetaH_Num++;
			y_count += pFlag[i].y;
			y_count_Num++;
		}
		else if (pFlag[i].lineFlag == 1)	//垂直线
		{
			thetaV += pFlag[i].theta;
			if (pFlag[i].theta < 0){
				thetaV += (float)CV_PI;
			}
			thetaV_Num++;
			x_count += pFlag[i].x;
			x_count_Num++;
		}
		else{
			pFlag[i].theta = -1;
		}
	}


	//	cout << "水平数量：" << thetaH_Num << "\t\t" << "垂直数量:" << thetaV_Num << endl;
	if (thetaH_Num){
		thetaH = thetaH / thetaH_Num;
	}
	else
	{
		thetaH = -1;
		conner.isConner = false;
		return conner;
	}

	if (thetaV_Num){
		thetaV = thetaV / thetaV_Num;
		if (thetaV > CV_PI / 2){
			thetaV -= (float)CV_PI;
		}
	}
	else
	{
		thetaV = -1;
		conner.isConner = false;
		return conner;
	}

	//	cout << "角度：" << thetaH * 180 / CV_PI << "\t\t" << thetaV * 180 / CV_PI << "\t\t" << (thetaV - thetaH)*180/CV_PI<<endl;
	x_count = x_count / x_count_Num;
	y_count = y_count / y_count_Num;
	for (int i = 0; i < len; i++)
	{
		if (pFlag[i].lineFlag == 0){	//水平线
			if (pFlag[i].y < y_count)	//水平上
			{
				if (abs(pFlag[i].y - y_count)<150)
				{
					y_count_HU += pFlag[i].y;
					y_count_Num_HU++;
				}

			}
			else
			{
				if (abs(pFlag[i].y - y_count) < 150)
				{
					y_count_HD += pFlag[i].y;//水平下
					y_count_Num_HD++;
				}
			}
		}
		else if (pFlag[i].lineFlag == 1)//垂直线
		{
			if (pFlag[i].x < x_count){
				if (abs(pFlag[i].x - x_count) < 150)
				{
					x_count_VL += pFlag[i].x;
					x_count_Num_VL++;
				}
			}
			else
			{
				if (abs(pFlag[i].x - x_count) < 150){
					x_count_VR += pFlag[i].x;
					x_count_Num_VR++;
				}
			}
		}
	}
	if (x_count_Num_VL&&x_count_Num_VR&&y_count_Num_HD&&y_count_Num_HU){
		x_count_VL = x_count_VL / x_count_Num_VL;
		x_count_VR = x_count_VR / x_count_Num_VR;
		y_count_HD = y_count_HD / y_count_Num_HD;
		y_count_HU = y_count_HU / y_count_Num_HU;
	}
	else
	{
		return conner;
	}
	std::cout << x_count_VL << "\t" << x_count_VR << "\t" << y_count_HU << "\t" << y_count_HD << "\n";
	conner.mx_count_VL.x = (int)(x_count_VL + 0.5);
	conner.mx_count_VR.x = (int)(x_count_VR + 0.5);
	conner.my_count_HD.y = (int)(y_count_HD + 0.5);
	conner.my_count_HU.y = (int)(y_count_HU + 0.5);

	if (!Judge(conner.mx_count_VL, conner.mx_count_VR, conner.my_count_HD, conner.my_count_HU)){
		std::cout << "error lines" << endl;
		return conner;
	}

	conner.leftup = Equalxy(y_count_HU, thetaH, x_count_VL, thetaV);
	conner.leftdown = Equalxy(y_count_HD, thetaH, x_count_VL, thetaV);
	conner.rightup = Equalxy(y_count_HU, thetaH, x_count_VR, thetaV);
	conner.rightdown = Equalxy(y_count_HD, thetaH, x_count_VR, thetaV);
	conner.isConner = true;
	return conner;
}

CvPoint HUST_ReLocal::Equalxy(float y1, float theta1, float x2, float theta2)
{
	//1号水平，2号垂直
	CvPoint point;
	float k1, k2;
	if (abs(theta2 * 180 / CV_PI - 90) <= 0.01){	//线是垂直的
		if (abs(theta1) * 180 / CV_PI >= 0.01)
		{
			point.x = (int)(x2+0.5);
			point.y = (int)((float)(x2*tan(theta1) + y1+0.5));
		}
		else
		{
			point.x = (int)(x2+0.5);
			point.y = (int)(y1+0.5);
		}
		return point;
	}
	k1 = tan(theta1);
	k2 = tan(theta2);

	if (abs(theta1 * 180 / CV_PI) < 0.01)
	{
		point.y = (int)(y1 + 0.5);
		point.x = (int)((float)y1 / k2 + x2+0.5);
		return point;
	}
	point.x = (int)((float)(k2*x2 + y1) / (k2 - k1)+0.5);
	point.y = (int)((float)(k2*y1 + k1*k2*x2) / (k2 - k1)+0.5);
	return point;
}


void HUST_ReLocal::Otsu(IplImage* A, IplImage* B)
{
	int height = A->height;
	int width = A->width;
	long N = height * width;
	int h[256];
	double p[256], u[256], w[256];
	for (int i = 0; i < 256; i++)
	{
		h[i] = 0;
		p[i] = 0;
		u[i] = 0;
		w[i] = 0;
	}
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			for (int k = 0; k < 256; k++)
			{
				if (((uchar*)(A->imageData + A->widthStep*i))[j] == k)
					h[k]++;
			}

	for (int i = 0; i < 256; i++)
		p[i] = h[i] / double(N);

	int T = 0;
	double uT, thegma2fang;
	double thegma2fang_max = -10000;
	for (int k = 0; k < 256; k++)
	{
		uT = 0;
		for (int i = 0; i <= k; i++)
		{
			u[k] += i*p[i];
			w[k] += p[i];
		}

		for (int i = 0; i < 256; i++)
			uT += i*p[i];

		thegma2fang = (uT*w[k] - u[k])*(uT*w[k] - u[k]) / (w[k] * (1 - w[k]));
		if (thegma2fang > thegma2fang_max)
		{
			thegma2fang_max = thegma2fang;
			T = k;
		}
	}

	for (int i = 0; i < height; i++)   //根据最佳阈值，对原图像进行处理二值化处理
		for (int j = 0; j < width; j++)
			if (((uchar*)(A->imageData + A->widthStep*i))[j] > T)
				((uchar*)(B->imageData + B->widthStep*i))[j] = 0xff;
			else
				((uchar*)(B->imageData + B->widthStep*i))[j] = 0x00;
}


MyDisErr HUST_ReLocal::CountErr(CvPoint now_pt, CvPoint init_pt)
{
	MyDisErr err;
	if (now_pt.x<0 || now_pt.y<0){
		std::cout << "不合要求的数据\n\n";
		err.x = 0;
		err.y = 0;
		return err;
	}

#ifdef CAMERA_SPAN
	err.y = (float)(now_pt.x - init_pt.x) * Kx * KxP / 640;
	err.x = (float)(now_pt.y - init_pt.y) * Ky * KyP / 480;
	std::cout << "X偏差" << err.x << "mm \t\t" << "Y偏差：" << err.y << "mm\n\n";
#else
	err.x = -(float)(now_pt.x - init_pt.x) * Kx * KxP / 640;
	err.y = -(float)(now_pt.y - init_pt.y) * Ky * KyP / 480;
	std::cout << "X偏差" << err.x << "mm \t\t" << "Y偏差：" << err.y << "mm\n\n";
#endif
	err.isErrValue = true;
	return err;
}

bool HUST_ReLocal::Judge(CvPoint VL, CvPoint VR, CvPoint HD, CvPoint HU)
{
	if (/*true ||*/ (WidthThesMin <(VR.x - VL.x) && WidthThesMax >(VR.x - VL.x) && WidthThesMin <(HD.y - HU.y) && WidthThesMax >(HD.y - HU.y)))
		return true;
	else
		return false;
}

bool HUST_ReLocal::JudgeLine(CvPoint line0, CvPoint line1)
{
	double theta = atan2((line1.y - line0.y), (line1.x - line0.x));

	//		cout << theta*180 / CV_PI << '\t';
	if (abs(theta * 180 / CV_PI) < 10)
	{
		return true;
	}
	else if (abs(theta * 180 / CV_PI) < 80)
	{
		return false;

	}
	else
	{
		return true;
	}
}


MyConner HUST_ReLocal::ImageThresholdFind(IplImage* GrayImage)
{
	MyConner my_pt;
	int i, j;
	int count_width = 0;
	int count_height = 0;

	for (i = 0; i < GrayImage->width; i++)
	{
		for (j = 0; j < GrayImage->height; j++)
		{
			if (GrayImage->imageData[j*GrayImage->widthStep + i] && 0xff)
			{
				count_height++;
				if (count_height >= (GrayImage->height*0.6))
				{
					break;
				}
			}
		}
		if (count_height >= (GrayImage->height*0.6))
		{
			my_pt.leftup.x = i;
			my_pt.leftdown.x = i;
			break;
		}
		count_height = 0;
	}
	if (my_pt.leftup.x < 0)
	{
		return my_pt;
	}

	count_height = 0;
	for (i = GrayImage->width - 1; i >= my_pt.leftup.x; i--)
	{
		for (j = 0; j < GrayImage->height; j++)
		{
			if (GrayImage->imageData[j*GrayImage->widthStep + i] && 0xff)
			{
				count_height++;
				if (count_height >= (GrayImage->height*0.6))
				{
					break;
				}
			}
		}
		if (count_height >= (GrayImage->height*0.6)){
			//count_height = 0;
			my_pt.rightup.x = i;
			my_pt.rightdown.x = i;
			break;
		}
		count_height = 0;
	}

	if (my_pt.rightup.x < 0)
	{
		return my_pt;
	}


	for (j = 0; j < GrayImage->height; j++)
	{
		for (i = 0; i < GrayImage->width; i++)
		{
			if (GrayImage->imageData[j*GrayImage->widthStep + i] & 0xff)
			{
				count_width++;
				if (count_width >= (GrayImage->width / 2))
				{
					break;
				}
			}
		}
		if (count_width >= (GrayImage->width / 2)){
			my_pt.rightup.y = j;
			my_pt.leftup.y = j;
			break;
		}
		count_width = 0;
	}
	if (my_pt.rightup.y < 0)
	{
		return my_pt;
	}

	count_width = 0;
	for (j = GrayImage->height - 1; j >= my_pt.rightup.y; j--)
	{
		for (i = 0; i < GrayImage->width; i++)
		{
			if (GrayImage->imageData[j*GrayImage->widthStep + i] & 0xff)
			{
				count_width++;
				if (count_width >= (GrayImage->width / 2))
				{
					break;
				}
			}
		}
		if (count_width >= (GrayImage->width / 2)){
			my_pt.rightdown.y = j;
			my_pt.leftdown.y = j;
			break;
		}
		count_width = 0;
	}
	if (my_pt.rightup.y < 0)
	{
		return my_pt;
	}
	return my_pt;
}

void HUST_ReLocal::M_print(MyConner my_pt)
{
	std::cout << "conner leftup：" << my_pt.leftup.x << "  " << my_pt.leftup.y << endl;
	std::cout << "conner rightup：" << my_pt.rightup.x << "  " << my_pt.rightup.y << endl;
	std::cout << "conner leftdown：" << my_pt.leftdown.x << "  " << my_pt.leftdown.y << endl;
	std::cout << "conner rightdown：" << my_pt.rightdown.x << "  " << my_pt.rightdown.y << endl;

}

// implementation of otsu algorithm
// author: onezeros(@yahoo.cn)
// reference: Rafael C. Gonzalez. Digital Image Processing Using MATLAB
int HUST_ReLocal::cvThresholdOtsu(IplImage* src)
{
	int height = src->height;
	int width = src->width;

	//histogram
	float histogram[256] = { 0 };
	for (int i = 0; i<height; i++) {
		unsigned char* p = (unsigned char*)src->imageData + src->widthStep*i;
		for (int j = 0; j<width; j++) {
			histogram[*p++]++;
		}
	}
	//normalize histogram
	int size = height*width;
	for (int i = 0; i<256; i++) {
		histogram[i] = histogram[i] / size;
	}

	//average pixel value
	float avgValue = 0;
	for (int i = 0; i<256; i++) {
		avgValue += i*histogram[i];
	}

	int threshold;
	float maxVariance = 0;
	float w = 0, u = 0;
	for (int i = 0; i<256; i++) {
		w += histogram[i];
		u += i*histogram[i];

		float t = avgValue*w - u;
		float variance = t*t / (w*(1 - w));
		if (variance>maxVariance) {
			maxVariance = variance;
			threshold = i;
		}
	}

	return threshold;
}
