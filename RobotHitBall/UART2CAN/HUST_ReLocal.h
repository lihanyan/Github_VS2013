#pragma once
#include <opencv2\opencv.hpp>
#include <iostream>
#include <Windows.h>
#include "HUST_TYPE.h"
#include "HUST_CAN.h"
using namespace cv;


class HUST_ReLocal
{
public:
	HUST_ReLocal();
	HUST_ReLocal(HUST_CAN* SRCCAN);
	HUST_ReLocal(HUST_CAN* SRCCAN,int device);
	~HUST_ReLocal();
	void MySetColor(unsigned short ForeColor = 0, unsigned short BackGroundColor = 0)
	{
		HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hCon, ForeColor | BackGroundColor);
	}
	void initClass(HUST_CAN* SRCCAN, int device);//类初始化

public :
	int deviceNum = 0;//设备号
	HUST_CAN* CAN = NULL;
	MyCamState m_camState;				//当前的检测状态
	int canny_thres1_standrad, canny_thres2_standrad, hough_thres_standrad;
	CvPoint pt_init;//默认的中心点
	char m_command = 0;
	int threshold = 130;
	int canny_threshold1;
	int canny_threshold2;
	int hough_threshold;
	int hough_para1;
	int hough_para2;
	int tirckness;
	Mat frame;
	Mat srcRGB, dst, dst2;	//8U3
	Mat src, edge, mask;	//8U1
	Mat src2;		//灰度图
	CvMemStorage *storage = NULL;
	CvSeq *lines = NULL;
	vector<Vec4i> lines1;

	bool OpenCam(int device);
	VideoCapture *cap;

private:

	HANDLE m_HoughDealThread;
//	HANDLE m_CameraCapture;

	void InitPara();

	float ImageDealHoughStandard_Angle();
	MyDisErr ImageDealHoughProbabilistic_Position();

	MyConner FindConner(CvSeq *lines);		//寻找四个角点
	void Scan(MyLine *pFlag, CvSeq *lines);	//判断是否是符合要求的线
	MyConner DealLine(MyLine *pFlag, int len);//对符合要求的线处理
	CvPoint Equalxy(float y1, float theta1, float x2, float theta2);//计算求解
	void Otsu(IplImage* A, IplImage* B);		//自适应阈值，没用上
	MyDisErr CountErr(CvPoint now_pt, CvPoint init_pt);	//计算误差
	bool Judge(CvPoint VL, CvPoint VR, CvPoint HD, CvPoint HU);//判断是否满足条件的点
	bool JudgeLine(CvPoint line0, CvPoint line1);//判断某条线是否是符合要求的线
	MyConner ImageThresholdFind(IplImage* GrayImage);
	void M_print(MyConner my_pt);//打印某些信息
	int cvThresholdOtsu(IplImage* src);//自适应阈值

	//判断是否要发送数据
	void Send_PositionShifting(MyDisErr* err,int len);
	void Send_AngleShifting(float* angleShifting,int len);

	void CameraCapture();	//一个类中开两个线程，我感觉有点问题，这个暂时没有使用
	static DWORD CameraCapture(LPVOID lpParameter)
	{
		HUST_ReLocal *pG_ReLocal = (HUST_ReLocal *)lpParameter;
		pG_ReLocal->CameraCapture();
		return 0;
	}

	void HougtDeal_ThreadLoop();
	static DWORD ImageDealThread_Hough(LPVOID lpParameter)
	{
		HUST_ReLocal *pG_ReLocal = (HUST_ReLocal *)lpParameter;
		pG_ReLocal->HougtDeal_ThreadLoop();
		return 0;
	}

protected:
	

};

