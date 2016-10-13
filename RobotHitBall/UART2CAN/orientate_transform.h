#ifndef __ORIENTATE_TRANSFORM_H__
#define __ORIENTATE_TRANSFORM_H__

#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "HUST_CAN.h"
using namespace std;
struct Coordinate_inform
{
	//摄像机坐标转机器人坐标转换矩阵
	double T_camera_to_robot[4][4];
	//机器人坐标转全场坐标转换矩阵
	double T_robot_to_Generalized[4][4];
	//前三个分别为球对摄像机坐标
	double coordinate_camera[4];	 
	// 前三个分别为机器人全局坐标
	double coordinate_robot[4];
	// 前三个分别为球的全局X,Y,Z
	double coordinate_Generalized[4];
	//安装参数
	double x_camera_to_robot = 0;;
	double y_camera_to_robot = 0;
	double z_camera_to_robot = 0;
	double theta_camera_to_robot = 0;
	//实时坐标
	double x_robot[200];
	double y_robot[200];
	double theta_robot[200]; 
	//最终位置
	bool couldcalculate = false;
	double final_position_x;
	double final_position_y;
	double final_position_z;
	double final_position_tim = 0;
};

struct Coordinate_Get_From_Can
{
	int x;
	int y;
	double angle;
};

class Coordinate
{
public:
	HUST_CAN *CAN;
	Coordinate_inform Coordinate_robot;
	Coordinate_Get_From_Can Coordinate_from_can;
	void Coordinate_transformation();
	Coordinate(double x_camera_to_robot, double y_camera_to_robot, double z_camera_to_robot, double angle_camera_to_robot, HUST_CAN *can_src);
	~Coordinate(){};
	void MatrixMultiplication(double in1[4][4], double in2[4], double *out);
	void Coordinate_calculate(int listnumber);
	void Coordinate_set_robot_position(int x, int y, float angle); 
	void Coordinate_set_position_to_camera(double x, double y, double z);
	void Coordinate_calculate_position_to_court(double x_to_camerra, double y_to_camerra, double z_to_camerra, int listnumber);
	void Coordinate_printf_information(int listnum);
	void Coordinate_get_position_from_can(int listnum);
};

#endif