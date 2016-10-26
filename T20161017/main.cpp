//2.帧差法2，三帧.分水岭效果比单独的轮廓填充好；三帧可以很大降低背景噪声；连通域函数还是不能识别小块连通域

//#include <stdio.h>
//#include <math.h> 
//#include<XnCppWrapper.h>
//#include"watershedSegmenter.h"
//#include<opencv2/opencv.hpp>
//#include<opencv2/highgui/highgui.hpp>
//#include<opencv2/core/core.hpp>
//#include<opencv2/imgproc/imgproc.hpp>
//#include<iostream>
//
//using namespace std;
//using namespace cv;
//
////********************************华硕体感部分*****************************************************************
//XnMapOutputMode mapMode;
//XnMapOutputMode mapMode_RGB;
//
//xn::Context context;
//XnStatus result = XN_STATUS_OK;
//
//xn::DepthMetaData depthMD;
//xn::ImageMetaData imageMD;
//
//const int cdepthwidth = 320;
//const int cdepthheight = 240;
//const int ccolorwidth = 320;
//const int ccolorheight = 240;
//
////相关宏定义
//#define TanX 0.599911//0.425
//#define TanY 0.413  //0.553  0.57
////#define threshold_diff1 100 //设置简单帧差法阈值
////#define threshold_diff2 100 //设置简单帧差法阈值
//#define CV_CVX_WHITE    CV_RGB(0xff,0xff,0xff)
//#define CV_CVX_BLACK    CV_RGB(0x00,0x00,0x00)
//
////---------------------------------------------------------------------------
////Global
////---------------------------------------------------------------------------
//VideoCapture file("RGB01.avi");
//int g_slider_position = 1180;
//bool pause = false;
//
//Mat img_src1, img_src2, img_src3;//3帧法需要3帧图片
//Mat img_dst, gray1, gray2, gray3;
//Mat gray_diff1, gray_diff2;//存储2次相减的图片
//
//int threshold_diff1 = 10;
//int threshold_diff2 = 10;
//
////---------------------------------------------------------------------------
//// Struct
////---------------------------------------------------------------------------
//struct mapPointInform
//{
//	short xGlobe;
//	short yGlobe;
//	short zGlobe;
//	short xImage;
//	short yImage;
//	short zImage;
//};
//
//
////---------------------------------------------------------------------------
//// Functions
////---------------------------------------------------------------------------
//void onTrackbar(int pos)
//{
//	file.set(CV_CAP_PROP_POS_FRAMES, g_slider_position);
//}
//
//void on_T1(int, void*){}
//void on_T2(int, void*){}
//
//void Morphology(Mat &mask_process)
//{
//	//接口
//	IplImage *mask = &mask_process.operator IplImage();
//
//	//开运算作用：平滑轮廓，去掉细节,断开缺口
//	cvMorphologyEx(mask, mask, NULL, NULL, CV_MOP_OPEN, 1);//对输入mask进行开操作，CVCLOSE_ITR为开操作的次数，输出为mask图像
//	//cvShowImage("开运算", mask);
//	//闭运算作用：平滑轮廓，连接缺口
//	cvMorphologyEx(mask, mask, NULL, NULL, CV_MOP_CLOSE, 1);//对输入mask进行闭操作，CVCLOSE_ITR为闭操作的次数，输出为mask图像
//	//cvShowImage("闭运算", mask);
//}
//
//void Contours(Mat img_binary)
//{
//	//提取轮廓，并作大小限制，填充
//	vector<vector<Point>> contours;
//	vector<Vec4i> Hierarchy;
//	Mat  img_canny, img_contours;
//
//	Morphology(img_binary);
//
//	blur(img_binary, img_binary, Size(3, 3));
//	imshow("blur", img_binary);
//	Canny(img_binary, img_canny, 3, 9, 3);
//	imshow("边检图", img_canny);
//
//	findContours(img_canny, contours, Hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
//
//	img_contours = Mat::zeros(img_canny.size(), CV_8UC3);
//	//createTrackbar("面积最小", "轮廓图", &minarea, 1000, on_minarea);
//	//createTrackbar("周长最小", "轮廓图", &minlen, 1000, on_minlen);
//	//createTrackbar("面积最大", "轮廓图", &maxarea, 1000, on_maxarea);
//	//createTrackbar("周长最大", "轮廓图", &maxlen, 1000, on_maxlen);
//
//	for (int i = 0; i < contours.size(); i++)
//	{
//		Scalar color(rand() & 255, rand() & 255, rand() & 255);
//
//		double area = contourArea(contours[i]);
//		double len = arcLength(contours[i], true);
//
//		/*	if (area>minarea && area<maxarea && len>minlen && len<maxlen)
//		{*/
//		drawContours(img_contours, contours, i, Scalar(0, 255, 255), -1, 8, Hierarchy);
//		//cout << "轮廓数" << i << "面积" << area << "周长" << len << endl;
//		//}
//	}
//	imshow("轮廓图", img_contours);
//}
//
//void Watershed(Mat img_binary)
//{
//	//分水岭算法
//	Mat fg;
//	erode(img_binary, fg, Mat(), Point(-1, -1), 1);
//	Mat bg;
//	dilate(img_binary, bg, Mat(), Point(-1, -1), 1);
//	threshold(bg, bg, 1, 128, THRESH_BINARY_INV);
//
//	Mat markers(img_binary.size(), CV_8U, Scalar(0));
//	markers = fg + bg;
//	imshow("前景", fg);
//	imshow("背景", bg);
//	imshow("标记", markers);
//
//	// Create watershed segmentation object
//	WatershedSegmenter segmenter;
//
//	// Set markers and process
//	segmenter.setMarkers(markers);
//	segmenter.process(img_src1);
//
//	Mat img_water = segmenter.getSegmentation();
//
//	imshow("分水岭图", img_water);
//	imshow("分割线", segmenter.getWatersheds());
//
//	Contours(img_water);
//}
//
//void ConnectedComponents(Mat &mask_process, int poly1_hull0, float perimScale, int minarea, int maxarea,
//		int number = 0, Rect &bounding_box = Rect(), Point &contour_centers = Point(-1, -1))
//	{
//		/*下面4句代码是为了兼容原函数接口，即内部使用的是c风格，但是其接口是c++风格的*/
//		IplImage *mask = &mask_process.operator IplImage();
//		int *num = &number;
//		CvRect *bbs = &bounding_box.operator CvRect();
//		CvPoint *centers = &contour_centers.operator CvPoint();
//		static CvMemStorage*    mem_storage = NULL;
//		static CvSeq*            contours = NULL;
//		//CLEAN UP RAW MASK
//		//开运算作用：平滑轮廓，去掉细节,断开缺口
//		cvMorphologyEx(mask, mask, NULL, NULL, CV_MOP_OPEN, 1);//对输入mask进行开操作，CVCLOSE_ITR为开操作的次数，输出为mask图像
//		cvShowImage("开运算", mask);
//		//闭运算作用：平滑轮廓，连接缺口
//		cvMorphologyEx(mask, mask, NULL, NULL, CV_MOP_CLOSE, 1);//对输入mask进行闭操作，CVCLOSE_ITR为闭操作的次数，输出为mask图像
//		cvShowImage("闭运算", mask);
//	
//	
//		//FIND CONTOURS AROUND ONLY BIGGER REGIONS
//		if (mem_storage == NULL) mem_storage = cvCreateMemStorage(0);
//		else cvClearMemStorage(mem_storage);
//		//CV_RETR_EXTERNAL=0是在types_c.h中定义的，CV_CHAIN_APPROX_SIMPLE=2也是在该文件中定义的
//		CvContourScanner scanner = cvStartFindContours(mask, mem_storage, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
//		CvSeq* c;
//		int numCont = 0;
//		//该while内部只针对比较大的轮廓曲线进行替换处理
//		while ((c = cvFindNextContour(scanner)) != NULL)
//		{
//			double len = cvContourPerimeter(c);
//			double q = (mask->height + mask->width) / perimScale;   //calculate perimeter len threshold
//			double area = cvContourArea(c);
//	
//			if (len < q || area<minarea || area>maxarea) //Get rid of blob if it's perimeter is too small or too big
//			{
//				cvSubstituteContour(scanner, NULL);    //用NULL代替原来的那个轮廓
//			}
//	
//			else //Smooth it's edges if it's large enough
//			{
//				CvSeq* c_new;
//				if (poly1_hull0) //Polygonal approximation of the segmentation
//					c_new = cvApproxPoly(c, sizeof(CvContour), mem_storage, CV_POLY_APPROX_DP, 2, 0);
//				else //Convex Hull of the segmentation
//					c_new = cvConvexHull2(c, mem_storage, CV_CLOCKWISE, 1);
//				cvSubstituteContour(scanner, c_new); //最开始的轮廓用凸包或者多项式拟合曲线替换
//				numCont++;
//	
//				cout << "轮廓数：" << numCont << ", " << "面积：" << area << endl;
//			}
//		}
//		contours = cvEndFindContours(&scanner);    //结束轮廓查找操作
//		// PAINT THE FOUND REGIONS BACK INTO THE IMAGE
//		cvZero(mask);
//		IplImage *maskTemp;
//		//CALC CENTER OF MASS AND OR BOUNDING RECTANGLES
//		if (*num != 0)
//		{
//			int N = *num, numFilled = 0, i = 0;
//			CvMoments moments;
//			double M00, M01, M10;
//			maskTemp = cvCloneImage(mask);
//			for (i = 0, c = contours; c != NULL; c = c->h_next, i++)        //h_next为轮廓序列中的下一个轮廓
//			{
//				if (i < N) //Only process up to *num of them
//				{
//					//CV_CVX_WHITE在本程序中是白色的意思
//					cvDrawContours(maskTemp, c, CV_CVX_WHITE, CV_CVX_WHITE, -1, CV_FILLED, 8);
//					//Find the center of each contour
//					if (centers != &cvPoint(-1, -1))
//					{
//						cvMoments(maskTemp, &moments, 1);    //计算mask图像的最高达3阶的矩
//						M00 = cvGetSpatialMoment(&moments, 0, 0); //提取x的0次和y的0次矩
//						M10 = cvGetSpatialMoment(&moments, 1, 0); //提取x的1次和y的0次矩
//						M01 = cvGetSpatialMoment(&moments, 0, 1); //提取x的0次和y的1次矩
//						centers[i].x = (int)(M10 / M00);    //利用矩的结果求出轮廓的中心点坐标
//						centers[i].y = (int)(M01 / M00);
//	
//						cout << centers[i].x << ", " << centers[i].y << endl;
//					}
//					//Bounding rectangles around blobs
//					if (bbs != &CvRect())
//					{
//						bbs[i] = cvBoundingRect(c); //算出轮廓c的外接矩形
//					}
//					cvZero(maskTemp);
//					numFilled++;
//				}
//				//Draw filled contours into mask
//				cvDrawContours(mask, c, CV_CVX_WHITE, CV_CVX_WHITE, -1, CV_FILLED, 8); //draw to central mask
//			} //end looping over contours
//			*num = numFilled;
//			cvReleaseImage(&maskTemp);
//		}
//		//ELSE JUST DRAW PROCESSED CONTOURS INTO THE MASK
//		else
//		{
//			for (c = contours; c != NULL; c = c->h_next)
//			{
//				cvDrawContours(mask, c, CV_CVX_WHITE, CV_CVX_BLACK, -1, CV_FILLED, 8);
//			}
//		}
//	
//		cvShowImage("连通域", mask);
//	}
//	
//
//int main()
//{//**********************************华硕体感部分***************************************************************
//	//result = context.Init();
//
//	////创建深度生成器和彩色生成器
//	//xn::DepthGenerator depthGenerator;
//	//result = depthGenerator.Create(context);
//	//xn::ImageGenerator imageGenerator;
//	//result = imageGenerator.Create(context);
//
//	//mapMode.nXRes = cdepthwidth;
//	//mapMode.nYRes = cdepthheight;
//	//mapMode.nFPS = 30;
//	//result = depthGenerator.SetMapOutputMode(mapMode);
//
//	//mapMode_RGB.nXRes = ccolorwidth;
//	//mapMode_RGB.nYRes = ccolorheight;
//	//mapMode_RGB.nFPS = 30;
//	//result = depthGenerator.SetMapOutputMode(mapMode_RGB);
//
//	////将深度生成器的视角对齐到彩色生成器，将深度数据配准到彩色数据
//	//depthGenerator.GetAlternativeViewPointCap().SetViewPoint(imageGenerator);
//
//	////启动所有生成器，即启动数据流
//	//result = context.StartGeneratingAll();
//
//	//result = context.WaitNoneUpdateAll();
//	//************************************************************************************************************
//	
//	//读视频,控制进度
//	namedWindow("BGR");
//	int frames = file.get(CV_CAP_PROP_FRAME_COUNT);
//	if (frames != 0)
//	{
//		cvCreateTrackbar("进度条", "BGR", &g_slider_position, frames, onTrackbar);
//	}
//
//	while (1)
//	{
//		//result = context.WaitOneUpdateAll(depthGenerator);
//		//if (XN_STATUS_OK == result)
//		//{
//		//	//获取RGB信息
//		//	imageGenerator.GetMetaData(imageMD);
//		//	Mat cvRGBImg(imageMD.FullYRes(), imageMD.FullXRes(), CV_8UC3, (char *)imageMD.Data());
//		//	cvtColor(cvRGBImg, cvBGRImg, CV_RGB2BGR);
//
//		//	depthGenerator.GetMetaData(depthMD);
//		//	Mat cvRawImg(depthMD.FullYRes(), depthMD.FullXRes(), CV_16UC1, (char *)depthMD.Data());
//		//	cvRawImg.convertTo(cvDepthImg, CV_8UC3, 255.0 / (depthMD.ZRes()));
//
//		//	imshow("RawImg", cvRawImg);
//		//	imshow("BGRImg", cvBGRImg);
//		//	imshow("DepthImg", cvDepthImg);
//
//		//	mapPointInform pointTemp;
//		//	for (int y = 0; y < 240; y++)
//		//	{
//		//		ushort *py = cvRawImg.ptr<ushort>(y);
//		//		for (int x = 0; x < 320; x++)
//		//		{
//		//			//if ((py[x] < disDetMax) && py[x] >DDETMIN)//限制距离
//		//			//{
//		//			pointTemp.zImage = py[x];
//		//			pointTemp.yImage = y;
//		//			pointTemp.xImage = x;
//
//		//			//图像坐标转换
//		//			//pointTemp.yGlobe = pointTemp.zImage* TanX / 120 * (120 - pointTemp.yImage);//(pointTemp.zImage* TanX / 120 * (120 - pointTemp.yImage) / 1000)*1000;
//		//			pointTemp.zGlobe = pointTemp.zImage* TanY / 120 * (120 - pointTemp.yImage);// pointTemp.yGlobe;//(double(pointTemp.zImage) / 1000 * SinZ + pointTemp.yGlobe * CosZ + AddZ)*1000;
//		//			pointTemp.xGlobe = pointTemp.zImage* TanX / 160 * (pointTemp.xImage - 160);// (pointTemp.zImage* TanY / 160 * (pointTemp.xImage - 160) / 1000) * 1000;
//		//			pointTemp.yGlobe = pointTemp.zImage;// (-pointTemp.yGlobe * SinZ + double(pointTemp.zImage) / 1000 * CosZ) * 1000;
//
//		//			/*			cout << "图像坐标" << pointTemp.xImage << "   " << pointTemp.yImage << "   " << pointTemp.zImage << endl;
//		//			cout << "全局坐标" << pointTemp.xGlobe << "   " << pointTemp.yGlobe << "   " << pointTemp.zGlobe << endl;*/
//		//			if (pointTemp.zGlobe <= 0)
//		//			{
//		//				*(cvDepthImg.data + cvDepthImg.step[0] * y + cvDepthImg.step[1] * x) = 0;
//		//				//cvDepthImg.at<uchar>(y, x) = 0;
//
//		//				cvBGRImg.at<Vec3b>(y, x) = 0;
//		//				//*(cvBGRImg.data + cvBGRImg.step[0] * y + cvBGRImg.step[1] * x) = 0;//[row, col]像素的第 1 通道地址被 * 解析(blue通道) 
//		//				//*(cvBGRImg.data + cvBGRImg.step[0] * y + cvBGRImg.step[1] * x + cvBGRImg.elemSize1()) = 0;//[row, col]像素的第 2 通道地址被 * 解析(green通道)
//		//				//*(cvBGRImg.data + cvBGRImg.step[0] * y + cvBGRImg.step[1] * x + cvBGRImg.elemSize1() * 2) = 0;//[row, col]像素的第 3 通道地址被 * 解析(red通道) 
//		//			}
//
//		//			//}//限制距离
//		//		}
//		//	}//for,得到化掉1.5m以下在距离范围内的图
//
//
//		namedWindow("BGR");
//		int frames = file.get(CV_CAP_PROP_FRAME_COUNT);
//		if (frames != 0)
//		{
//			cvCreateTrackbar("进度条", "BGR", &g_slider_position, frames, onTrackbar);
//		}
//
//		if (!pause)
//		{
//			file >> img_src1;
//			cvtColor(img_src1, gray1, CV_BGR2GRAY);
//
//			waitKey(5);
//			file >> img_src2;
//			cvtColor(img_src2, gray2, CV_BGR2GRAY);
//
//			waitKey(5);
//			file >> img_src3;
//			cvtColor(img_src3, gray3, CV_BGR2GRAY);
//
//			subtract(gray2, gray1, gray_diff1);//第二帧减第一帧
//			subtract(gray3, gray2, gray_diff2);//第三帧减第二帧
//
//			createTrackbar("T1", "foreground", &threshold_diff1, 255, on_T1);
//			createTrackbar("T2", "foreground", &threshold_diff2, 255, on_T2);
//
//			for (int i = 0; i<gray_diff1.rows; i++)
//			for (int j = 0; j<gray_diff1.cols; j++)
//			{
//				if (abs(gray_diff1.at<unsigned char>(i, j)) >= threshold_diff1)//这里模板参数一定要用unsigned char，否则就一直报错
//					gray_diff1.at<unsigned char>(i, j) = 255;            //第一次相减阈值处理
//				else gray_diff1.at<unsigned char>(i, j) = 0;
//
//				if (abs(gray_diff2.at<unsigned char>(i, j)) >= threshold_diff2)//第二次相减阈值处理
//					gray_diff2.at<unsigned char>(i, j) = 255;
//				else gray_diff2.at<unsigned char>(i, j) = 0;
//			}
//			bitwise_and(gray_diff1, gray_diff2, img_dst);
//
//			imshow("BGR", img_src1);
//			imshow("foreground", img_dst);
//
//			//Morphology(img_dst);
//			Watershed(img_dst);//漫水填充算法
//			//Contours(img_dst);
//			//ConnectedComponents(img_dst, 1, 8.0, 10, 300, 0, Rect(), Point(-1, -1));//多边形拟合
//
//		}
//		char c = waitKey(10);
//		if (c == ' ') pause = !pause;
//		if (c == 27) break;
//	}
//	return 0;
//}


//对运动物体的跟踪：  
//如果背景固定,可用帧差法 然后在计算下连通域 将面积小的去掉即可  
//如果背景单一,即你要跟踪的物体颜色和背景色有较大区别 可用基于颜色的跟踪 如CAMSHIFT 鲁棒性都是较好的  
//如果背景复杂,如背景中有和前景一样的颜色 就需要用到一些具有预测性的算法 如卡尔曼滤波等 可以和CAMSHIFT结合   

//模板匹配

#include <stdio.h>
#include <math.h> 
#include<XnCppWrapper.h>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/nonfree/features2d.hpp>
#include<opencv2/calib3d/calib3d.hpp>
#include<opencv2/video/tracking.hpp>
#include<iostream>

using namespace std;
using namespace cv;


//********************************华硕体感部分*****************************************************************
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

//体感相机相关宏定义
#define TanX 0.599911//0.425
#define TanY 0.413  //0.553  0.57

//**************************************************************************************************************

//---------------------------------------------------------------------------
//Global
//---------------------------------------------------------------------------
Mat src, cvBGRImg, cvDepthImg, cvRawImg, HSVImg, HSImg, out;
Mat temp=imread("12.jpg") ;
int MatchMethod;
int MaxTra = 5;

int g_slider_position = 1180;
VideoCapture file("RGB01.avi");
bool pause = false;

Rect rect;//鼠标画的矩形框
bool rect_flag = false;//标记位
Point origin;//初始坐标
Mat ROI;//感兴趣图

Point matchLoc;

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
// Functions
//---------------------------------------------------------------------------

void onTrackbar(int pos)
{
	file.set(CV_CAP_PROP_POS_FRAMES, g_slider_position);
}


void onMouse(int event, int x, int y, int, void*)
{
	switch (event)
	{
		//按下鼠标左键
	case CV_EVENT_LBUTTONDOWN:
		//点击鼠标图像时，清除之前ROI图像的显示窗口  
		cvDestroyWindow("ROI");
		//存放起始坐标  
		origin = Point(x, y);
		//初始化起始矩形框  
		rect = Rect(x, y, 0, 0);
		rect_flag = true;
		break;

		//松开鼠标左键      
	case CV_EVENT_LBUTTONUP:
		if (rect.height > 0 && rect.width > 0)
		{
			//将img中的矩形区域复制给roi，并显示
			ROI = HSImg(Rect(rect.x, rect.y, rect.width, rect.height));
			//rectangle(HSVImg, rect, Scalar(0, 0, 255), 1);

			//显示ROI图像
			namedWindow("ROI");
			imshow("ROI", ROI);
			//imwrite("1.jpg", ROI);
			//waitKey(0);
		}
		rect_flag = false;
		break;

		//移动光标
	case CV_EVENT_MOUSEMOVE:
		if (rect_flag)
		{
			//用MIN得到左上点作为矩形框的起始坐标，如果不加这个，画矩形时只能向一个方向进行  
			rect.x = MIN(x, origin.x);
			rect.y = MIN(y, origin.y);
			rect.width = abs(origin.x - x);
			rect.height = abs(origin.y - y);
			//防止矩形区域超出图像的范围  
			rect &= Rect(0, 0, HSImg.cols, HSImg.rows);
		}
		break;
	}

	//输出HSV值
	//unsigned char H, S, V;
	//for (int j = 0; j < ROI.rows; j++)
	//{
	//	uchar* data = ROI.ptr<uchar>(j);
	//	for (int i = 0; i < ROI.cols; i++)
	//	{
	//		//得到H,S,V值
	//		//H = *(ROI.data + ROI.step[0] * j + ROI.step[1] * i + ROI.elemSize1());
	//		//S = *(ROI.data + ROI.step[0] * j + ROI.step[1] * i + ROI.elemSize1() * 2);
	//		//V = *(ROI.data + ROI.step[0] * j + ROI.step[1] * i);
	//		//cout << H + 0 << ", " << S + 0 << ", " << V + 0 << endl;
	//		cout << data[i] + 0 << endl;
	//	}
	//}
}

void on_Matching(int, void*)
{
	Mat img;
	HSVImg.copyTo(img);

	int out_cols = HSVImg.cols - temp.cols + 1;
	int out_rows = HSVImg.rows - temp.rows + 1;
	out.create(out_cols, out_rows, CV_32FC1);

	//匹配和标准化
	matchTemplate(HSVImg, temp, out, MatchMethod);
	normalize(out, out, 0, 1, NORM_MINMAX, -1, Mat());

	//定位最匹配的位置
	double minVal, maxVal;
	Point minLoc, maxLoc;

	minMaxLoc(out, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	if (MatchMethod == CV_TM_SQDIFF || MatchMethod == CV_TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}

	rectangle(HSVImg, matchLoc, Point(matchLoc.x + temp.cols, matchLoc.y + temp.rows), Scalar(255, 255, 255), 2, 8, 0);
	rectangle(out, matchLoc, Point(matchLoc.x + temp.cols, matchLoc.y + temp.rows), Scalar(255, 255, 255), 2, 8, 0);

	//cout << "匹配位置" << matchLoc.x << "，" << matchLoc.y << endl;

	imshow("HSV", HSVImg);
	imshow("效果图", out);
}



int main()
{//**********************************华硕体感部分***************************************************************
	result = context.Init();

	//创建深度生成器和彩色生成器
	xn::DepthGenerator depthGenerator;
	result = depthGenerator.Create(context);
	xn::ImageGenerator imageGenerator;
	result = imageGenerator.Create(context);

	mapMode.nXRes = cdepthwidth;
	mapMode.nYRes = cdepthheight;
	mapMode.nFPS = 30;
	result = depthGenerator.SetMapOutputMode(mapMode);

	mapMode_RGB.nXRes = ccolorwidth;
	mapMode_RGB.nYRes = ccolorheight;
	mapMode_RGB.nFPS = 30;
	result = depthGenerator.SetMapOutputMode(mapMode_RGB);

	//将深度生成器的视角对齐到彩色生成器，将深度数据配准到彩色数据
	depthGenerator.GetAlternativeViewPointCap().SetViewPoint(imageGenerator);

	//启动所有生成器，即启动数据流
	result = context.StartGeneratingAll();

	result = context.WaitNoneUpdateAll();
	//************************************************************************************************************
	
	//读视频,控制进度
	//namedWindow("BGR");
	//int frames = file.get(CV_CAP_PROP_FRAME_COUNT);
	//if (frames != 0)
	//{
	//	cvCreateTrackbar("进度条", "BGR", &g_slider_position, frames, onTrackbar);
	//}

	while (1)
	{
		result = context.WaitOneUpdateAll(depthGenerator);
		if (XN_STATUS_OK == result)
		{
			//获取RGB信息
			imageGenerator.GetMetaData(imageMD);
			Mat cvRGBImg(imageMD.FullYRes(), imageMD.FullXRes(), CV_8UC3, (char *)imageMD.Data());
			cvtColor(cvRGBImg, cvBGRImg, CV_RGB2BGR);

			depthGenerator.GetMetaData(depthMD);
			Mat cvRawImg(depthMD.FullYRes(), depthMD.FullXRes(), CV_16UC1, (char *)depthMD.Data());
			cvRawImg.convertTo(cvDepthImg, CV_8UC3, 255.0 / (depthMD.ZRes()));

			imshow("RawImg", cvRawImg);
			imshow("BGRImg", cvBGRImg);
			imshow("DepthImg", cvDepthImg);

			mapPointInform pointTemp;
			for (int y = 0; y < 240; y++)
			{
				ushort *py = cvRawImg.ptr<ushort>(y);
				for (int x = 0; x < 320; x++)
				{
					//if ((py[x] < disDetMax) && py[x] >DDETMIN)//限制距离
					//{
					pointTemp.zImage = py[x];
					pointTemp.yImage = y;
					pointTemp.xImage = x;

					//图像坐标转换
					//pointTemp.yGlobe = pointTemp.zImage* TanX / 120 * (120 - pointTemp.yImage);//(pointTemp.zImage* TanX / 120 * (120 - pointTemp.yImage) / 1000)*1000;
					pointTemp.zGlobe = pointTemp.zImage* TanY / 120 * (120 - pointTemp.yImage);// pointTemp.yGlobe;//(double(pointTemp.zImage) / 1000 * SinZ + pointTemp.yGlobe * CosZ + AddZ)*1000;
					pointTemp.xGlobe = pointTemp.zImage* TanX / 160 * (pointTemp.xImage - 160);// (pointTemp.zImage* TanY / 160 * (pointTemp.xImage - 160) / 1000) * 1000;
					pointTemp.yGlobe = pointTemp.zImage;// (-pointTemp.yGlobe * SinZ + double(pointTemp.zImage) / 1000 * CosZ) * 1000;

					/*			cout << "图像坐标" << pointTemp.xImage << "   " << pointTemp.yImage << "   " << pointTemp.zImage << endl;
					cout << "全局坐标" << pointTemp.xGlobe << "   " << pointTemp.yGlobe << "   " << pointTemp.zGlobe << endl;*/
					if (pointTemp.zGlobe <= 0)
					{
						*(cvDepthImg.data + cvDepthImg.step[0] * y + cvDepthImg.step[1] * x) = 0;
						//cvDepthImg.at<uchar>(y, x) = 0;

						cvBGRImg.at<Vec3b>(y, x) = 0;
						//*(cvBGRImg.data + cvBGRImg.step[0] * y + cvBGRImg.step[1] * x) = 0;//[row, col]像素的第 1 通道地址被 * 解析(blue通道) 
						//*(cvBGRImg.data + cvBGRImg.step[0] * y + cvBGRImg.step[1] * x + cvBGRImg.elemSize1()) = 0;//[row, col]像素的第 2 通道地址被 * 解析(green通道)
						//*(cvBGRImg.data + cvBGRImg.step[0] * y + cvBGRImg.step[1] * x + cvBGRImg.elemSize1() * 2) = 0;//[row, col]像素的第 3 通道地址被 * 解析(red通道) 
					}

					//}//限制距离
				}
			}//for,得到化掉1.5m以下在距离范围内的图

			//file >> src;
			cvBGRImg.copyTo(src);
			imshow("BGR", src);

			//转换到HSV空间
			cvtColor(src, HSVImg, CV_BGR2HSV);
			//colorNormal(HSVImg);
			//imshow("HSV", HSVImg);

			//转化为HS两通道，减少光照影响
			vector<Mat> channels;
			Mat H;
			Mat S;
			split(HSVImg, channels);
			H = channels.at(0);
			S = channels.at(1);
			namedWindow("HS");
			addWeighted(H, 0.3, S, 0.7, 0.0, HSImg);
			imshow("HS", HSImg);

			////捕捉鼠标
			//setMouseCallback("HS", onMouse, 0);

			//选择方法匹配
			createTrackbar("方法", "HSV", &MatchMethod, MaxTra, on_Matching);
			on_Matching(0, 0);

			int b = matchLoc.y, a = matchLoc.x;
			ushort *pm = cvRawImg.ptr<ushort>(b);
			mapPointInform pT;
			pT.zImage = pm[a];
			pT.yImage = b;
			pT.xImage = a;

			//图像坐标转换
			//pT.yGlobe = pT.zImage* TanX / 120 * (120 - pT.yImage);//(pT.zImage* TanX / 120 * (120 - pT.yImage) / 1000)*1000;
			pT.zGlobe = pT.zImage* TanY / 120 * (120 - pT.yImage);// pT.yGlobe;//(double(pT.zImage) / 1000 * SinZ + pT.yGlobe * CosZ + AddZ)*1000;
			pT.xGlobe = pT.zImage* TanX / 160 * (pT.xImage - 160);// (pT.zImage* TanY / 160 * (pT.xImage - 160) / 1000) * 1000;
			pT.yGlobe = pT.zImage;// (-pT.yGlobe * SinZ + double(pT.zImage) / 1000 * CosZ) * 1000;

			cout << "中心坐标：" << pT.xGlobe << "," << pT.yGlobe << "," << pT.zGlobe << endl;

		}//if (XN_STATUS_OK == result)
		
		char c = waitKey(10);
		if (c >= 0) waitKey(0);
		if (c == 27) break;

	}//while(1)

	return 0;
}


//CamShift跟踪，效果不太好；算法挺快；要用鼠标选择

//bool backflag = false;
//bool rect_flag = false;//true 代表正在使用鼠标选择
//int trackNum = 0;
//bool histflag = true;
//
//Point origin;//鼠标第一点
//Rect rect;
//
//Mat hsv, hue, mask, back, hist, histImg = Mat::zeros(240, 320, CV_8UC3);
//int vmin = 10, vmax = 256, smin = 30;
//int hsize = 16;
//float hranges[] = { 0, 180 };
//const float *phranges = hranges;
//Rect trackWin;
//RotatedRect trackM;//旋转矩阵类对象
//
//void onMouse(int event, int x, int y, int, void*)
//{
//	switch (event)
//	{
//		//按下鼠标左键
//	case CV_EVENT_LBUTTONDOWN:
//		//点击鼠标图像时，清除之前ROI图像的显示窗口  
//		cvDestroyWindow("ROI");
//		//存放起始坐标  
//		origin = Point(x, y);
//		//初始化起始矩形框  
//		rect = Rect(x, y, 0, 0);
//		rect_flag = true;
//		break;
//
//		//松开鼠标左键      
//	case CV_EVENT_LBUTTONUP:
//		if (rect.height > 0 && rect.width > 0)
//		{
//			////将img中的矩形区域复制给roi，并显示
//			//ROI = HSImg(Rect(rect.x, rect.y, rect.width, rect.height));
//			////rectangle(HSVImg, rect, Scalar(0, 0, 255), 1);
//			trackNum = -1;
//		}
//		rect_flag = false;
//		break;
//
//		//移动光标
//	case CV_EVENT_MOUSEMOVE:
//		if (rect_flag)
//		{
//			//用MIN得到左上点作为矩形框的起始坐标，如果不加这个，画矩形时只能向一个方向进行  
//			rect.x = MIN(x, origin.x);
//			rect.y = MIN(y, origin.y);
//			rect.width = abs(origin.x - x);
//			rect.height = abs(origin.y - y);
//			//防止矩形区域超出图像的范围  
//			//rect &= Rect(0, 0, HSImg.cols, HSImg.rows);
//		}
//		break;
//	}
//}
//
//int main()
//{
//	VideoCapture cap;
//	cap.open(0);
//
//	namedWindow("Histogram", 0);
//	namedWindow("CamShift", 0);
//	setMouseCallback("CamShift", onMouse, 0);//消息响应机制
//	createTrackbar("Vmin", "CamShift", &vmin, 256, 0);//createTrackbar函数的功能是在对应的窗口创建滑动条，滑动条Vmin,vmin表示滑动条的值，最大为256
//	createTrackbar("Vmax", "CamShift", &vmax, 256, 0);//最后一个参数为0代表没有调用滑动拖动的响应函数
//	createTrackbar("Smin", "CamShift", &smin, 256, 0);//vmin,vmax,smin初始值分别为10,256,30
//
//	Mat frame,img;
//	bool pause = false;
//
//	while (1)
//	{
//		cap >> frame;
//		if (frame.empty())break;
//
//		frame.copyTo(img);
//		cvtColor(img, hsv, CV_BGR2HSV);
//
//		if (trackNum)
//		{
//			int _vmin = vmin, _vmax = vmax;
//
//			//检查HSV三通道是否分别在0-180，smin-256，vmin-vmax，之间。如果三通道都在范围内，则mask该点为1，否则为0
//			inRange(hsv, Scalar(0, smin, MIN(_vmin, _vmax)), Scalar(180, 256, MAX(_vmin, _vmax)), mask);
//			int ch[] = { 0, 0 };
//			hue.create(hsv.size(), hsv.depth());
//			mixChannels(&hsv, 1, &hue, 1, ch, 1);//第一个通道hue
//
//			if (trackNum < 0)
//			{
//				Mat ROI(hue, rect);
//				Mat maskROI(mask, rect);//mask保存HSV最小值
//
//				calcHist(&ROI, 1, 0, maskROI, hist, 1, &hsize, &phranges);
//				normalize(hist, hist, 0, 255, CV_MINMAX);//直方图hist归一化到0-255
//
//				trackWin = rect;
//				trackNum = 1;
//
//				histImg = Scalar::all(0);
//				int binW = histImg.cols / hsize;
//				Mat buf(1, hsize, CV_8UC3);
//				for (int i = 0; i < hsize; i++)
//				{
//					buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i * 180 / hsize), 255, 255);
//				}
//				cvtColor(buf, buf, CV_HSV2BGR);
//
//				for (int i = 0; i < hsize; i++)
//				{
//					int val = saturate_cast<int>(hist.at<float>(i)*histImg.rows / 255);
//					rectangle(histImg, 
//									Point(i*binW,
//									histImg.rows), 
//									Point((i + 1)*binW, histImg.rows - val), 
//									Scalar(buf.at<Vec3b>(i)),
//									-1, 
//									8);
//				}
//			}//	if (trackNum < 0)
//
//			calcBackProject(&hue, 1, 0, hist, back, &phranges);
//			back &= mask;//反向投影
//
//			RotatedRect trackM = CamShift(back, 
//					trackWin, //鼠标选择的区域
//					TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_EPS, 10, 1));//TermCriteria为确定迭代终止的准则
//																											//CV_TERMCRIT_EPS是通过forest_accuracy,
//																										//CV_TERMCRIT_ITER是通过max_num_of_trees_in_the_forest  
//			if (trackWin.area() <= 1)
//			{
//				int cols = back.cols, rows = back.rows, r = (MIN(cols, rows) + 5) / 6;
//				trackWin = Rect(trackWin.x - r, trackWin.y - r, trackWin.x + r, trackWin.y + r)
//								&Rect(0, 0, cols, rows);//Rect第一二个参数为矩阵的左上角点坐标，第三四个参数为矩阵的宽和高
//			}
//
//			if (backflag)
//			{
//				cvtColor(back, img, CV_GRAY2BGR);
//			}
//			ellipse(img, trackM, Scalar(0, 0, 255), 3, CV_AA);//画椭圆	
//		}//	if (trackNum)
//
//		else if (trackNum < 0)
//			pause = false;
//
//		if (rect_flag&&rect.width>0 && rect.height > 0)
//		{
//			Mat ROI(img, rect);
//			bitwise_not(ROI, ROI);//取反
//		}
//
//		imshow("CamShift", img);
//		imshow("Histogram", histImg);
//
//		char c = waitKey(10);
//		if (c == 27) break;
//		if (c >= 0) waitKey(0);
//	}
//	return 0;
//}



////试用Image Watch，可以看到两张图，以及像素

//int main()
//{
//	Mat src= imread("1.jpg", 1);
//	cvtColor(src, src, CV_BGR2GRAY);
//	Mat dst;
//	equalizeHist(src, dst);
//	imshow("shiyan", dst);
//	waitKey(0);
//}