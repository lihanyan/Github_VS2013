#pragma once
#include "math.h"
#include "Parameter.h"

#define PI 3.1415926
#define xalpha (120/180.0*PI)
#define DegreeToRadian 0.01745329252

#define L1_R   0
#define L2_R   R_BATSHIFTING
#define L3_R   R_BATLENGTH

#define L1_L   0
#define L2_L   L_BATSHIFTING
#define L3_L   L_BATLENGTH

#define ZY_R 8000.0

const double sa = sin(xalpha);
const double ca = cos(xalpha);

typedef  struct
{
	double theta1;
	double theta2;
}Theta;

typedef  struct
{
	double x;
	double y;
	double z;
}ZY_Vector;

typedef  struct
{
	double x;
	double y;
	double z;
}Position;

/*角度制三角函数*/
double cosd(double angle);
double sind(double angle);
double tand(double angle);
double acosd(double angle);
double asind(double angle);
double atand(double angle);

int GetYawSpeed(int xTarget, int yTarget, int xNow, int yNow);
ZY_Vector ForwardSolution(Theta theta);
ZY_Vector ForwardSolutionL(Theta theta);
ZY_Vector ForwardSolutionR(Theta theta);
Position ForwardSolutionPositionL(Theta theta);
Position ForwardSolutionPositionR(Theta theta);
Theta ReverseSolution(ZY_Vector vector, bool reserve);
