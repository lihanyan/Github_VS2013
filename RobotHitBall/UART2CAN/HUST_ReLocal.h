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
	void initClass(HUST_CAN* SRCCAN, int device);//���ʼ��

public :
	int deviceNum = 0;//�豸��
	HUST_CAN* CAN = NULL;
	MyCamState m_camState;				//��ǰ�ļ��״̬
	int canny_thres1_standrad, canny_thres2_standrad, hough_thres_standrad;
	CvPoint pt_init;//Ĭ�ϵ����ĵ�
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
	Mat src2;		//�Ҷ�ͼ
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

	MyConner FindConner(CvSeq *lines);		//Ѱ���ĸ��ǵ�
	void Scan(MyLine *pFlag, CvSeq *lines);	//�ж��Ƿ��Ƿ���Ҫ�����
	MyConner DealLine(MyLine *pFlag, int len);//�Է���Ҫ����ߴ���
	CvPoint Equalxy(float y1, float theta1, float x2, float theta2);//�������
	void Otsu(IplImage* A, IplImage* B);		//����Ӧ��ֵ��û����
	MyDisErr CountErr(CvPoint now_pt, CvPoint init_pt);	//�������
	bool Judge(CvPoint VL, CvPoint VR, CvPoint HD, CvPoint HU);//�ж��Ƿ����������ĵ�
	bool JudgeLine(CvPoint line0, CvPoint line1);//�ж�ĳ�����Ƿ��Ƿ���Ҫ�����
	MyConner ImageThresholdFind(IplImage* GrayImage);
	void M_print(MyConner my_pt);//��ӡĳЩ��Ϣ
	int cvThresholdOtsu(IplImage* src);//����Ӧ��ֵ

	//�ж��Ƿ�Ҫ��������
	void Send_PositionShifting(MyDisErr* err,int len);
	void Send_AngleShifting(float* angleShifting,int len);

	void CameraCapture();	//һ�����п������̣߳��Ҹо��е����⣬�����ʱû��ʹ��
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

