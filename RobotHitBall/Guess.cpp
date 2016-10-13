#include"guess.h"


double GPlist[MAXTRACK][100000][9];

//extern Coordinate *Coordinate_robot;
const double a = 0.0;
const double b = 0.001155;
const double t = 0.0001;
const double m = 0.005;
const double g = 9.8;

int qiuJiePosListNum = 0;

int GPnum = 0;


void GetV(double AX[3][4], double *V)
{
	double dt;
	dt = (AX[2][3] - AX[0][3]) / 1000;
	V[0] = (AX[2][0] - AX[0][0]) / dt;
	V[1] = (AX[2][1] - AX[0][1]) / dt;
	V[2] = (AX[2][2] - AX[0][2]) / dt;
	//return V;
}

void Ff(double x, double y, double z, double *f)
{
	double vv, v, temp;
	vv = x*x + y*y + z*z;
	v = sqrt(vv);
	temp = -abs((a + b*v) / m);
	f[0] = temp*x;
	f[1] = temp*y - g;
	f[2] = temp*z;
	//return(f);
}


int sheru(double x)
{
	if (x > 0)
	{
		x += 0.5;
	}
	else
	{
		x -= 0.5;
	}
	return(x);
}


int GuessPos(double x, double y, double z, double vx, double vy, double vz, double tmax, double rex[], int ren, int tol, int n)

{
	//double x, y, z, vx, vy, vz;
	double tvx, tvy, tvz;
	double k1[3], k2[3], k3[3], k4[3], f[3];
	double tim = 0, tmp;
	int i;
	//double dt;
	//ofstream fout;
	//fout.open("yumaoqiu.out");

	//GPlist[0][1] = 0;


	//dt = (AX[2][3] - AX[0][3]) / 1000;
	//vx = (AX[2][0] - AX[0][0]) / dt;
	//vy = (AX[2][1] - AX[0][1]) / dt;
	//vz = (AX[2][2] - AX[0][2]) / dt;
	//x = AX[1][0];
	//y = AX[1][1];
	//z = AX[1][2];
	/*
	vx = AX.VX;
	vy = AX.VY;
	vz = AX.VZ;
	x = AX.PX;
	y = AX.PY;
	z = AX.PZ;
	*/

	//tol++;
	GPlist[n][tol][0] = x;
	GPlist[n][tol][1] = y;
	GPlist[n][tol][2] = z;

	tvx = vx; tvy = vy; tvz = vz;

	while (((y >= HITPOINT) || (vy > 0)) && (tim < tmax))
	{

		Ff(vx, vy, vz, f);
		for (i = 0; i <= 2; i++){
			k1[i] = f[i];
		}

		tvx = vx + k1[0] * t / 2;
		tvy = vy + k1[1] * t / 2;
		tvz = vz + k1[2] * t / 2;
		if (vx*tvx < 0) tvx = 0;
		if (vz*tvz < 0) tvz = 0;
		Ff(tvx, tvy, tvz, f);
		for (i = 0; i <= 2; i++){
			k2[i] = f[i];
		}
		tvx = vx + k2[0] * t / 2;
		tvy = vy + k2[1] * t / 2;
		tvz = vz + k2[2] * t / 2;
		if (vx*tvx < 0) tvx = 0;
		if (vz*tvz < 0) tvz = 0;
		Ff(tvx, tvy, tvz, f);
		for (i = 0; i <= 2; i++){
			k3[i] = f[i];
		}
		tvx = vx + k3[0] * t;
		tvy = vy + k3[1] * t;
		tvz = vz + k3[2] * t;
		if (vx*tvx < 0) tvx = 0;
		if (vz*tvz < 0) tvz = 0;
		Ff(tvx, tvy, tvz, f);
		for (i = 0; i <= 2; i++){
			k4[i] = f[i];
		}
		tvx = vx + (k1[0] + 2 * k2[0] + 2 * k3[0] + k4[0])*t / 6;
		tvy = vy + (k1[1] + 2 * k2[1] + 2 * k3[1] + k4[1])*t / 6;
		tvz = vz + (k1[2] + 2 * k2[2] + 2 * k3[2] + k4[2])*t / 6;
		if (vx*tvx < 0) tvx = 0;
		if (vz*tvz < 0) tvz = 0;

		x = x + (vx + tvx)*t / 2;
		y = y + (vy + tvy)*t / 2;
		z = z + (vz + tvz)*t / 2;

		tol++;
		GPlist[n][tol][0] = x;
		GPlist[n][tol][1] = y;
		GPlist[n][tol][2] = z;
		GPlist[n][tol][3] = vx;
		GPlist[n][tol][4] = vy;
		GPlist[n][tol][5] = vz;
		GPlist[n][tol][6] = (tvx - vx) / t;
		GPlist[n][tol][7] = (tvy - vy) / t;
		GPlist[n][tol][8] = (tvz - vz) / t;


		if (tol == ren)
		{
			rex[0] = x;
			rex[1] = y;
			rex[2] = z;
			rex[3] = vx;
			rex[4] = vy;
			rex[5] = vz;
			rex[6] = (tvx - vx) / t;
			rex[7] = (tvy - vy) / t;
			rex[8] = (tvz - vz) / t;
		}

		vx = tvx; vy = tvy; vz = tvz;
		tim = tim + t;


		//if (abs(y - 2.1*x + 3.36) < 0.1)

		//for (i = 0; i <= 6; i++)
		//{
		//	tmp = (-18.0 + i * 6) / 100;
		//	if (abs(y - 1.07 + 1.732*(x + 0.49 + tmp)) < NeedToGo[i][4])
		//	{
		//		NeedToGo[i][4] = abs(y - 1.07 + 1.732*(x + 0.49 + tmp));
		//		//cout << "YES" << endl;
		//		NeedToGo[i][0] = sheru(-x * 1000);
		//		NeedToGo[i][1] = sheru(z * 1000 - 80);
		//		NeedToGo[i][2] = sheru(y * 1000);
		//		NeedToGo[i][3] = sheru(tim * 1000 - 80);
		//	}
		//}



		//if ((abs(x + 3.160) < 0.001) && (y > 0.45))
		//{
		//	NeedToGo[0][0] = sheru(x * 1000 + 3010);
		//	NeedToGo[0][1] = sheru(z * 1000);
		//	NeedToGo[0][2] = sheru(y * 1000);
		//	NeedToGo[0][3] = sheru(tim * 1000 - 150);
		//	canrun = 0;
		//	cout << "YES111" << endl;
		//	break;
		//}



		//for (i = 0; i <= 6; i++)
		//{
		//	tmp = (-18.0 + i * 6) / 100;
		//	if (abs(y - 1.07 - 1.732*(x - 2.03 - HitPos[1] + tmp)) < NeedToGo[i][4])
		//	{
		//		NeedToGo[i][4] = abs(y - 1.07 - 1.732*(x - 2.03 - HitPos[1] + tmp));
		//		//cout << "YES" << endl;
		//		NeedToGo[i][0] = sheru(-x * 1000 + 2030);
		//		NeedToGo[i][1] = sheru(z * 1000);
		//		NeedToGo[i][2] = sheru(y * 1000);
		//		NeedToGo[i][3] = sheru(tim * 1000 - 80);
		//	}
		//}


		//if (abs(y + 1.732*x - 0.7274) < 0.02)
		//{
		//	cout << "YES" << endl;
		//	NeedToGo[0] = sheru(-x * 1000);
		//	NeedToGo[1] = sheru(z * 1000);
		//	NeedToGo[2] = sheru(y * 1000);
		//	NeedToGo[3] = sheru(tim * 1000 - 160);
		//	canrun = 0;
		//	break;
		//}


		//fout << x << ' ' << z << ' ' << y << endl;
	}

	canrun = 0;

	cout << n << ' ' << x << ' ' << y << ' ' << z << ' ' << tim << endl;
	//Coordinate_robot->Coordinate_robot.final_position_x = -1000 * z;
	//Coordinate_robot->Coordinate_robot.final_position_y = 1000 * x;
	//Coordinate_robot->Coordinate_robot.final_position_z = 1000 * y;
	//Coordinate_robot->Coordinate_robot.final_position_tim = tim*1000;

	//if ((x < 6) && (x>0.5) && (abs(y - HITPOINT) < 0.001) && (abs(z) < 3) && (tim < 1.8))
	if (tim > 0.7)
	{

		//CAN.Send_RobotPosition_Frame(0x01, (short)(Coordinate_robot->Coordinate_robot.final_position_x - 200), (short)Coordinate_robot->Coordinate_robot.final_position_y,(short)Coordinate_robot->Coordinate_robot.final_position_tim);

		//SendPosZhaoYue(sheru(z * 100), sheru(-x * 100 + 207), sheru(y * 100), sheru(tim * 1000));
		//SendPosShenBo(sheru(z * 1000), sheru(-x * 1000 + 2100), sheru(y * 1000), sheru(tim * 1000-160));
		//SendPosBaShen(sheru(z * 1000), sheru(-x * 1000 + 2100), sheru(y * 1000), sheru(tim * 1000-250));
		//SendPosBaShen(sheru(90), sheru(-50), sheru(y * 1000), sheru(tim * 1000));

		//cout << "sended success!" << endl;
		//cout << "x=" << z * 100 << "  y=" << -x * 100 + 210 << endl;
	}

	//GPlist[n][0][1] = tol;
	//fout.close();
	return 0;
}



//double *Getpos(int GPnum)
//{
//	if (GPlist[0][0] == 1)
//	{
//		GPlist[0][0] = 0;
//		GPnum = 1;
//	}
//	return GPlist[GPnum];
//}
//


int GuessPos(badminton AX, double tmax, int tol, int n)

//int GuessPos(double x, double y, double z, double vx, double vy, double vz, double tmax, double rex[], int ren, int tol, int n)

{
	double x, y, z, vx, vy, vz;
	double tvx, tvy, tvz;
	double k1[3], k2[3], k3[3], k4[3], f[3];
	double tim = 0, tmp;
	int i, level;


#define THDecisionHight 0.92

	bool CanBeDecision = true;

	if (AX.PY < THDecisionHight&&AX.VY<0)
	{
		CanBeDecision = false;
	}



	double Dropx;
	double Dropy;
	double Dropt;


	//double dt;
	//ofstream fout;
	//fout.open("yumaoqiu.out");

	//GPlist[0][1] = 0;


	//dt = (AX[2][3] - AX[0][3]) / 1000;
	//vx = (AX[2][0] - AX[0][0]) / dt;
	//vy = (AX[2][1] - AX[0][1]) / dt;
	//vz = (AX[2][2] - AX[0][2]) / dt;
	//x = AX[1][0];
	//y = AX[1][1];
	//z = AX[1][2];

	for (int i = MINHITLEVEL; i <= MAXHITLEVEL; i++)
	{
		NeedToGo[i][0] = 0.00;
		NeedToGo[i][1] = 0.00;
		NeedToGo[i][2] = 0.00;
		NeedToGo[i][3] = 0.00;
		NeedToGo[i][4] = 0.009;
	}

	vx = AX.VX;
	vy = AX.VY;
	vz = AX.VZ;
	x = AX.PX;
	y = AX.PY;
	z = AX.PZ;

	level = MIN(MAXHITLEVEL, int(y * 100));

	//cout << level << endl;

	//tol++;
	GPlist[n][tol][0] = x;
	GPlist[n][tol][1] = y;
	GPlist[n][tol][2] = z;

	tvx = vx; tvy = vy; tvz = vz;

	while (((y >= HITPOINT) || (vy > 0)) && (tim < tmax))
	{

		Ff(vx, vy, vz, f);
		for (i = 0; i <= 2; i++){
			k1[i] = f[i];
		}

		tvx = vx + k1[0] * t / 2;
		tvy = vy + k1[1] * t / 2;
		tvz = vz + k1[2] * t / 2;
		if (vx*tvx < 0) tvx = 0;
		if (vz*tvz < 0) tvz = 0;
		Ff(tvx, tvy, tvz, f);
		for (i = 0; i <= 2; i++){
			k2[i] = f[i];
		}
		tvx = vx + k2[0] * t / 2;
		tvy = vy + k2[1] * t / 2;
		tvz = vz + k2[2] * t / 2;
		if (vx*tvx < 0) tvx = 0;
		if (vz*tvz < 0) tvz = 0;
		Ff(tvx, tvy, tvz, f);
		for (i = 0; i <= 2; i++){
			k3[i] = f[i];
		}
		tvx = vx + k3[0] * t;
		tvy = vy + k3[1] * t;
		tvz = vz + k3[2] * t;
		if (vx*tvx < 0) tvx = 0;
		if (vz*tvz < 0) tvz = 0;
		Ff(tvx, tvy, tvz, f);
		for (i = 0; i <= 2; i++){
			k4[i] = f[i];
		}
		tvx = vx + (k1[0] + 2 * k2[0] + 2 * k3[0] + k4[0])*t / 6;
		tvy = vy + (k1[1] + 2 * k2[1] + 2 * k3[1] + k4[1])*t / 6;
		tvz = vz + (k1[2] + 2 * k2[2] + 2 * k3[2] + k4[2])*t / 6;
		if (vx*tvx < 0) tvx = 0;
		if (vz*tvz < 0) tvz = 0;

		x = x + (vx + tvx)*t / 2;
		y = y + (vy + tvy)*t / 2;
		z = z + (vz + tvz)*t / 2;

		tol++;
		GPlist[n][tol][0] = x;
		GPlist[n][tol][1] = y;
		GPlist[n][tol][2] = z;
		GPlist[n][tol][3] = vx;
		GPlist[n][tol][4] = vy;
		GPlist[n][tol][5] = vz;
		GPlist[n][tol][6] = (tvx - vx) / t;
		GPlist[n][tol][7] = (tvy - vy) / t;
		GPlist[n][tol][8] = (tvz - vz) / t;




		if (y > THDecisionHight)
		{
			Dropx = -z;
			Dropy = x;
			Dropt = tim;
		}


		if (vx > 0.0)
		{
			return 0;
		}


		if (vx < 0.0)//飞来的球，击球的时候卡，是因为这个？
		{
			if ((abs(y - (level / 100.0)) < NeedToGo[level][4]))
			{
				NeedToGo[level][4] = abs(y - (level / 100.0));
				NeedToGo[level][0] = x * 1000;
				NeedToGo[level][1] = z * 1000;
				NeedToGo[level][2] = y * 1000;
				NeedToGo[level][3] = tim * 1000 -WORKTIMEDELAY;
			

			}
			else if (abs(y - (level / 100.0)) < 0.009)
			{
				//cout <<level<<"    "<< NeedToGo[level][4] << endl;

				//LA_ofile << level << "	" << NeedToGo[level][0] << "	" << NeedToGo[level][1] << "	" << NeedToGo[level][2] << "	"
				//	<< NeedToGo[level][3] << "	" << NeedToGo[level][4] << "	" << endl;
				//cout << "More than Minlevel" << endl;
				level--;
				if (level < MINHITLEVEL)
				{
					break;
				}
			}
		}


		vx = tvx; vy = tvy; vz = tvz;
		tim = tim + t;
	}

	if (WORKTIMEDELAY>60)
	{
		cout << "WORKTIMEERROR!!!    " << WORKTIMEDELAY << endl;
	}

	//cout << n << ' ' << x << ' ' << y << ' ' << z << ' ' << tim << endl;

	//canprint = 1;
	if (CAN.isFirstPlayBall)//
	{
		//cout << (-z * 1000) << "\t" << x * 1000 << endl;
		if (((-z * 1000)<350.0 || (-z * 1000)>2750.0 || (x * 1000) > -1745.0 || (x * 1000) < -3395.0))
		{
			CAN.isFirstPlayBallChujie = true;
			CAN.Send_RobotPosition_Frame(0x01,1550,-2600,100);
		}
		else
		{
			CAN.isFirstPlayBallChujie = false;
		}
	}







	
	//TH20151114
	//因蓝牙没有，注册决策机
	//#ifdef USETH
	//double ThisTime = GETTIME();

	////状态维持
	//GeStateOfABTime = ThisTime;

	//if (KinectSerialPort.InitSuccess)//判断决策机有没有初始化成功
	//{

	//	//单位转换
	//	Dropx = Dropx * 1000;
	//	Dropy = Dropy * 1000;
	//	Dropt = Dropt * 1000;


	//	if (CanBeDecision)//低于0.92的球就不进行决策了
	//	{
	//		//if (isPlayBall == false)//打完球之后就不进行决策和状态保持了
	//		{
	//			//第一次看到球，但是状态被置位
	//			if (m_StateOfAB == BGetheBall)
	//			{
	//				//发一帧让底盘停住
	//				CAN.Send_RobotPositionReset_Frame(0x01, CAN.m_robotState.robot_xNow, CAN.m_robotState.robot_yNow, 1200);


	//				cout << "TH Avoid The Car and return" << endl;



	//				//告诉另一台车球位置
	//				//状态维持
	//				KinectSerialPort.AddDataToSend(AimPosFrame, BGet_HighBall, (short)Dropx, (short)Dropy, (short)Dropt);
	//				return 0;
	//			}



	//			if (m_StateOfAB == AllWait)
	//			{
	//				if (CAN.m_robotState.robot_xNow == 0 && CAN.m_robotState.robot_yNow == 0)
	//				{
	//					gkout << "*********************" << endl;
	//				}

	//				if (KinectSerialPort.x == 0 && KinectSerialPort.y == 0)
	//				{
	//					gkout << "*********************" << endl;
	//				}


	//				

	//				////自己裁决
	//				//double Distance0 = sqrt(Mysquare(Dropx - CAN.m_robotState.robot_xNow) + Mysquare(Dropy - CAN.m_robotState.robot_yNow));
	//				//double Distance1 = sqrt(Mysquare(Dropx - KinectSerialPort.x) + Mysquare(Dropy - KinectSerialPort.y));



	//				//自己裁决左右比前后好
	//				double Distance0 = 0.7*abs(Dropx - CAN.m_robotState.robot_xNow) + abs(Dropy - CAN.m_robotState.robot_yNow);
	//				double Distance1 = 0.7*abs(Dropx - KinectSerialPort.x) + abs(Dropy - KinectSerialPort.y);



	//				gkout << "The First*********************" << Distance0 << "	" << Distance1 << endl;

	//				if (Distance0 + 10 > Distance1)//应该要自己主动去接球
	//				{
	//					gkout << "Another Car to get the ball!" << endl;

	//					m_StateOfAB = BGetheBall;

	//					cout << CAN.m_robotState.robot_xNow << "	" << CAN.m_robotState.robot_yNow << "	" << Dropx << "	" << Dropy << "	" << KinectSerialPort.x << "	" << KinectSerialPort.y << endl;

	//					//让另一台车玩
	//					KinectSerialPort.AddDataToSend(AimPosFrame, BGet_HighBall,(short)Dropx, (short)Dropy, (short)Dropt);
	//					return 0;
	//				}
	//				else
	//				{
	//					//自己玩
	//					gkout << "Get the ball Myself" << endl;
	//				}

	//			}


	//			m_StateOfAB = AGetheBall;


	//			cout << "Guess状态维持" << AGetheBall << endl;
	//		}
	//	}
	//	else 
	//	{
	//		if (BGetheBall == m_StateOfAB)
	//		{
	//			cout << "CanBeDecision return	BGetheBall " << AX.PY << endl;
	//			return 0;
	//		}
	//		else
	//		{
	//			m_StateOfAB = AGetheBall;
	//			cout << "CanBeDecision 	AGetheBall " << AX.PY << endl;
	//		}
	//	}


	//}
	//else
	//{
	//	//gkout << "决策机失效" << endl;
	//}


	////#endif



	//m_StateOfAB = AGetheBall;


	////自己玩
	////小状态维持
	//GetActionTypeTime = ThisTime;
	////李昂接手控制
	//m_ActionType = GetTheBallLA;











	//cout << "Canrun**************************************" << endl;

	for (i = MINHITLEVEL; i <= MAXHITLEVEL; i++)
	{
		if (NeedToGo[i][4] - 0.001 < 0)
		{
			canrun = 0;
			qiuJiePosListNum = i;
			//gkout << "CAN RUN and detectEnable:" << (int)CAN.detectEnable << endl;
			break;
		}
	}


	return 0;
}



int GuessPosnokal(badminton AX, double tmax, int tol, int n)

//int GuessPos(double x, double y, double z, double vx, double vy, double vz, double tmax, double rex[], int ren, int tol, int n)

{
	double x, y, z, vx, vy, vz;
	double tvx, tvy, tvz;
	double k1[3], k2[3], k3[3], k4[3], f[3];
	double tim = 0, tmp;
	int i, level;

	for (int i = MINHITLEVEL; i <= MAXHITLEVEL; i++)
	{
		NeedToGo[i][4] = 0.009;
	}


	

	vx = AX.VX;
	vy = AX.VY;
	vz = AX.VZ;
	x = AX.PX;
	y = AX.PY;
	z = AX.PZ;


	level = MIN(MAXHITLEVEL, int(y * 100));//！！！！！！！！！！！！！！！

	//tol++;
	GPlist[n][tol][0] = x;
	GPlist[n][tol][1] = y;
	GPlist[n][tol][2] = z;

	tvx = vx; tvy = vy; tvz = vz;

	while (((y >= HITPOINT) || (vy > 0)) && (tim < tmax))
	{

		Ff(vx, vy, vz, f);
		for (i = 0; i <= 2; i++){
			k1[i] = f[i];
		}

		tvx = vx + k1[0] * t / 2;
		tvy = vy + k1[1] * t / 2;
		tvz = vz + k1[2] * t / 2;
		if (vx*tvx < 0) tvx = 0;
		if (vz*tvz < 0) tvz = 0;
		Ff(tvx, tvy, tvz, f);
		for (i = 0; i <= 2; i++){
			k2[i] = f[i];
		}
		tvx = vx + k2[0] * t / 2;
		tvy = vy + k2[1] * t / 2;
		tvz = vz + k2[2] * t / 2;
		if (vx*tvx < 0) tvx = 0;
		if (vz*tvz < 0) tvz = 0;
		Ff(tvx, tvy, tvz, f);
		for (i = 0; i <= 2; i++){
			k3[i] = f[i];
		}
		tvx = vx + k3[0] * t;
		tvy = vy + k3[1] * t;
		tvz = vz + k3[2] * t;
		if (vx*tvx < 0) tvx = 0;
		if (vz*tvz < 0) tvz = 0;
		Ff(tvx, tvy, tvz, f);
		for (i = 0; i <= 2; i++){
			k4[i] = f[i];
		}
		tvx = vx + (k1[0] + 2 * k2[0] + 2 * k3[0] + k4[0])*t / 6;
		tvy = vy + (k1[1] + 2 * k2[1] + 2 * k3[1] + k4[1])*t / 6;
		tvz = vz + (k1[2] + 2 * k2[2] + 2 * k3[2] + k4[2])*t / 6;
		if (vx*tvx < 0) tvx = 0;
		if (vz*tvz < 0) tvz = 0;

		x = x + (vx + tvx)*t / 2;
		y = y + (vy + tvy)*t / 2;
		z = z + (vz + tvz)*t / 2;

		tol++;
		GPlist[n][tol][0] = x;
		GPlist[n][tol][1] = y;
		GPlist[n][tol][2] = z;
		GPlist[n][tol][3] = vx;
		GPlist[n][tol][4] = vy;
		GPlist[n][tol][5] = vz;
		GPlist[n][tol][6] = (tvx - vx) / t;
		GPlist[n][tol][7] = (tvy - vy) / t;
		GPlist[n][tol][8] = (tvz - vz) / t;



		if (vx > 0.0)
		{
			return 0;
		}


		if (vx < 0.0)//飞来的球，击球的时候卡，是因为这个？
		{
			if ((abs(y - (level / 100.0)) < NeedToGo[level][4]))
			{
				//NeedToGo[level][4] = abs(y - (level / 100.0));
				//NeedToGo[level][0] = x * 1000;
				//NeedToGo[level][1] = z * 1000;
				//NeedToGo[level][2] = y * 1000;
				NeedToGo[level][3] = tim * 1000 - WORKTIMEDELAY;
				level--;
				if (level < MINHITLEVEL)
				{
					break;
				}
			}
			//else if (abs(y - (level / 100.0)) < 0.009)
			//{
			//	//cout <<level<<"    "<< NeedToGo[level][4] << endl;

			//	//LA_ofile << level << "	" << NeedToGo[level][0] << "	" << NeedToGo[level][1] << "	" << NeedToGo[level][2] << "	"
			//	//	<< NeedToGo[level][3] << "	" << NeedToGo[level][4] << "	" << endl;


			//}
		}


		vx = tvx; vy = tvy; vz = tvz;
		tim = tim + t;
	}

	//if (WORKTIMEDELAY>60)
	//{
	//	cout << "WORKTIMEERROR!!!    " << WORKTIMEDELAY << endl;
	//}

	//cout << n << ' ' << x << ' ' << y << ' ' << z << ' ' << tim << endl;

	////cout << "Canrun**************************************" << endl;

	//for (i = MINHITLEVEL; i <= MAXHITLEVEL; i++)
	//{
	//	if (NeedToGo[i][4] - 0.001 < 0)
	//	{
	//		canrun = 0;
	//		qiuJiePosListNum = i;
	//		//gkout << "CAN RUN and detectEnable:" << (int)CAN.detectEnable << endl;
	//		break;
	//	}
	//}


	return 0;
}
