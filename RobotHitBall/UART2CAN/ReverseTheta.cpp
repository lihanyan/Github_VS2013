#include "ReverseTheta.h"

#include <math.h>

/*角度制三角函数*/
double cosd(double angle)
{
	double COSA = cos(angle * DegreeToRadian);
	return COSA;
}

double acosd(double angle)
{
	double COSA = acos(angle) / DegreeToRadian;
	return COSA;
}

double sind(double angle)
{
	double SINA = sin(angle * DegreeToRadian);
	return SINA;
}
double asind(double angle)
{
	double SINA = asin(angle) / DegreeToRadian;
	return SINA;
}

double tand(double angle)
{
	double TANA = tan(angle * DegreeToRadian);
	return TANA;
}

double atand(double angle)
{
	double TANA = atan(angle) / DegreeToRadian;
	return TANA;
}
ZY_Vector ForwardSolution(Theta theta)
{
	ZY_Vector vector;
	if (theta.theta2<0)
	{
		vector.x = -cos(theta.theta1)*cos(theta.theta2);
		vector.y =  sin(theta.theta1)*cos(theta.theta2)*ca - sin(theta.theta2)*sa;
		vector.z =  sin(theta.theta1)*cos(theta.theta2)*sa + sin(theta.theta2)*ca;
	}
	else
	{
		vector.x =  cos(theta.theta1)*cos(theta.theta2);
		vector.y = -sin(theta.theta1)*cos(theta.theta2)*ca + sin(theta.theta2)*sa;
		vector.z = -sin(theta.theta1)*cos(theta.theta2)*sa - sin(theta.theta2)*ca;
	}
	return vector;
}


ZY_Vector ForwardSolutionL(Theta theta)
{
	ZY_Vector vector;
	vector.x = -cos(theta.theta1)*cos(theta.theta2);
	vector.y = sin(theta.theta1)*cos(theta.theta2)*ca - sin(theta.theta2)*sa;
	vector.z = sin(theta.theta1)*cos(theta.theta2)*sa + sin(theta.theta2)*ca;
	return vector;
}

ZY_Vector ForwardSolutionR(Theta theta)
{
	ZY_Vector vector;
	vector.x = cos(theta.theta1)*cos(theta.theta2);
	vector.y = -sin(theta.theta1)*cos(theta.theta2)*ca + sin(theta.theta2)*sa;
	vector.z = -sin(theta.theta1)*cos(theta.theta2)*sa - sin(theta.theta2)*ca;
	return vector;
}

Theta ReverseSolution(ZY_Vector vector, bool FowardBack)
{
	Theta theta;
	double a, b, c;
	if (FowardBack) //  -180-0
	{
		a = -vector.x;
		b =  vector.y*ca + vector.z*sa;
		c = -vector.y*sa + vector.z*ca;
	}
	else            //   0-180
	{
		a =  vector.x;
		b = -vector.y*ca - vector.z*sa;
		c =  vector.y*sa - vector.z*ca;
	}
	if (vector.x<0)  //-90-0  90-180
	{
		if (FowardBack)  // -90-0
		{
			theta.theta2 = asin(c);
			theta.theta1 = atan((b/a));
		}
		else
		{
			theta.theta2 = PI-asin(c);
			theta.theta1 = atan((b/a));
		}
	}
	else if (vector.x>0)           //-180- -90  0-90
	{
		if (FowardBack)  // -180- -90
		{
			theta.theta2 = asin(c);
			theta.theta1 = atan((b / a));
		}
		else             // 0-90
		{
			theta.theta2 = PI - asin(c);
			theta.theta1 = atan((b / a));
		}
	}
	else
	{
		if (FowardBack)  // -180- -90
		{
			theta.theta2 = asin(c);
			theta.theta1 = PI/2;
		}
		else             // 0-90
		{
			theta.theta2 = PI - asin(c);
			theta.theta1 = PI/2;
		}
	}
	return theta;
}

//zuotaqiujie jiapianzhi
#define X_L_DIS L_SHIFTING
#define X_R_DIS R_SHIFTING
#define Y_DIS (UPSIDESHIFTING)
#define Z_L_HEIGHT L_HEIGHT
#define Z_R_HEIGHT R_HEIGHT
Position ForwardSolutionPositionL(Theta theta)
{
	Position position;
	position.x = L2_L*sin(theta.theta1) + L3_L*cos(theta.theta1)*sin(theta.theta2)-X_L_DIS;
	position.y = sa*(L1_L - L3_L*cos(theta.theta2)) + ca*(L2_L*cos(theta.theta1) - L3_L*sin(theta.theta1)*sin(theta.theta2)) + Y_DIS;
	position.z = sa*(L2_L*cos(theta.theta1) - L3_L*sin(theta.theta1)*sin(theta.theta2)) - ca*(L1_L - L3_L*cos(theta.theta2)) + Z_L_HEIGHT;
	return position;
}

//youtaqiujie jiapianzhi
Position ForwardSolutionPositionR(Theta theta)
{
	Position position;
	position.x = L2_R*sin(theta.theta1) + L3_R*cos(theta.theta1)*sin(theta.theta2)+X_R_DIS;
	position.y = sa*(L1_R - L3_R*cos(theta.theta2)) + ca*(L2_R*cos(theta.theta1) - L3_R*sin(theta.theta1)*sin(theta.theta2)) + Y_DIS;
	position.z = sa*(L2_R*cos(theta.theta1) - L3_R*sin(theta.theta1)*sin(theta.theta2)) - ca*(L1_R - L3_R*cos(theta.theta2)) + Z_R_HEIGHT;
	return position;
}

int GetYawSpeed(int xTarget, int yTarget, int xNow, int yNow)
{
	const int disSpeed = 333;
	double dX = abs(xTarget - xNow);
	double dY = abs(yTarget - yNow);
	double alpha1;
	double alpha2;
	double distance;
	uchar speed;
	uchar yawMin;
	uchar yawMax;
	distance = sqrt(dX*dX+dY*dY);
	speed = (int)((distance-3000) / disSpeed) ;
	if (!(speed%2))
	{
		speed = speed + 1;
	}
	speed = speed + 8;
	if (yNow<-1500)
	{
		speed = 17;
	}

	if (abs(xTarget - xNow)<10)//中间击球
	{
		yawMax = 95;
		yawMin = 85;
	}
	else if (xTarget>xNow)//向右击球
	{
		alpha1 = atand(dY / dX);
		yawMin = alpha1 - 5;
		yawMax = alpha1 + 5;
	}
	else //向左击球
	{
		alpha1 = atand(dY / dX);
		yawMin = 180 - (alpha1 + 5);
		yawMax = 180 - (alpha1 - 5);
	}

	speed = speed < 5 ? 5 : speed;
	speed = speed > 17 ? 17 : speed;
	yawMin = yawMin < 60 ? 60 : yawMin;
	yawMax = yawMax > 120 ? 120 : yawMax;

	return ((speed << 16) | (yawMax << 8) | yawMin);
}