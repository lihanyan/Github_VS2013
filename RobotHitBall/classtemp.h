
class Ground
{
private:
	int Leftline;
	int Rightline;
	int Frontline;
	int Baseline;
	int serviceLeftline;
	int serviceRightline;
	int serviceFrontline;
	int serviceBaseline;

	int Allowance;
	int serviceAllowance;

public:
	Ground(int, int, int, int, int, int, int, int, int, int);
	~Ground(void);

	bool IsInService(int, int);
	bool IsInGround(int, int);

};

Ground::Ground(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j)
{
	Leftline;
	Rightline;
	Frontline;
	Baseline;
	serviceLeftline;
	serviceRightline;
	serviceFrontline;
	serviceBaseline;
	Allowance;
	serviceAllowance;
}


bool Ground::IsInService(int x, int y)
{
	if ((x > serviceLeftline - serviceAllowance) && (x<serviceRightline + serviceAllowance) && (y>serviceBaseline - serviceAllowance) && (y < serviceFrontline + serviceAllowance))
	{
		return 1;
	}
	return 0;
}

bool Ground::IsInService(int x, int y)
{
	if ((x > Leftline - Allowance) && (x<Rightline + Allowance) && (y>Baseline - Allowance) && (y < Frontline + Allowance))
	{
		return 1;
	}
	return 0;
}


class RobotCar
{
public:
	Kinect kinectV2;
	Chassis chassis;
	Racket racket;

public:

};

class Kinect
{


};

class Chassis
{

};

class Racket
{

};