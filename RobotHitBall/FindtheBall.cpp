
#include "FindtheBall.h"
#include<iostream>

using namespace std;

double SinRobot, CosRobot, FSinRobot, FCosRobot;
double TmpX, TmpY, TmpZ, KinectZ, KinectX;


//extern double GPlist[MAXTRACK][100000][9];

//void BFS(int x, int y, Mat *mapt)
//{
//
//	int h = 0, t = 0;
//
//	listLT[0][0] = x;
//	listLT[0][1] = y;
//	mapb[x][y] = 1;
//
//	while (h <= t)
//	{
//
//		if ((listLT[h][0] + 1)<424)
//		if ((mapt->at<unsigned char>(listLT[h][0] + 1, listLT[h][1]) > 1) && (!mapb[listLT[h][0] + 1][listLT[h][1]]))
//		{
//			t++;
//			listLT[t][0] = listLT[h][0] + 1;
//			listLT[t][1] = listLT[h][1];
//			mapb[listLT[t][0]][listLT[t][1]] = 1;
//		}
//		if ((listLT[h][0] - 1) > 0)
//		if ((mapt->at<unsigned char>(listLT[h][0] - 1, listLT[h][1]) > 1) && (!mapb[listLT[h][0] - 1][listLT[h][1]]))
//		{
//			t++;
//			listLT[t][0] = listLT[h][0] - 1;
//			listLT[t][1] = listLT[h][1];
//			mapb[listLT[t][0]][listLT[t][1]] = 1;
//		}
//		if ((listLT[h][1] + 1)<512)
//		if ((mapt->at<unsigned char>(listLT[h][0], listLT[h][1] + 1) > 1) && (!mapb[listLT[h][0]][listLT[h][1] + 1]))
//		{
//			t++;
//			listLT[t][0] = listLT[h][0];
//			listLT[t][1] = listLT[h][1] + 1;
//			mapb[listLT[t][0]][listLT[t][1]] = 1;
//		}
//		if ((listLT[h][1] - 1) > 0)
//		if ((mapt->at<unsigned char>(listLT[h][0], listLT[h][1] - 1) > 1) && (!mapb[listLT[h][0]][listLT[h][1] - 1]))
//		{
//			t++;
//			listLT[t][0] = listLT[h][0];
//			listLT[t][1] = listLT[h][1] - 1;
//			mapb[listLT[t][0]][listLT[t][1]] = 1;
//		}
//		h++;
//	}
//	if ((t < 50) || (t>2000))
//	{
//		for (int i = 0; i <= t; i++)
//		{
//			//mapt->at<unsigned char>(listLT[i][0], listLT[i][1]) = 0;
//			mapt->data[listLT[i][0] * 512 + listLT[i][1]] = 0;
//		}
//	}
//
//}

double x_last = 0;
double y_last = 0;

void BFSdep(int x, int y, Mat *mapt, int listnum)
{



	int h = 0, t = 0;
	int sidelength = 0;
	int edge = 0;



	listLT[0][0] = x;
	listLT[0][1] = y;
	mapb[x][y] = 1;

	while (h <= t)
	{

		if ((listLT[h][0] + 1) < 424)
		{
			if ((abs(depmap[listLT[h][0] + 1][listLT[h][1]] - depmap[listLT[h][0]][listLT[h][1]]) < KKK) && (depmap[listLT[h][0] + 1][listLT[h][1]]>0))
			{
				if (!mapb[listLT[h][0] + 1][listLT[h][1]])
				{
					t++;
					listLT[t][0] = listLT[h][0] + 1;
					listLT[t][1] = listLT[h][1];
					mapb[listLT[t][0]][listLT[t][1]] = 1;
				}
			}
			else
			{
				sidelength++;
			}
		}
		else
		{
			edge = 1;
			sidelength++;
		}

		if ((listLT[h][0] - 1) >= 0)
		{
			if ((abs(depmap[listLT[h][0] - 1][listLT[h][1]] - depmap[listLT[h][0]][listLT[h][1]]) < KKK) && (depmap[listLT[h][0] - 1][listLT[h][1]]>0))
			{
				if (!mapb[listLT[h][0] - 1][listLT[h][1]])
				{
					t++;
					listLT[t][0] = listLT[h][0] - 1;
					listLT[t][1] = listLT[h][1];
					mapb[listLT[t][0]][listLT[t][1]] = 1;
				}
			}
			else
			{
				sidelength++;
			}
		}
		else
		{
			edge = 1;
			sidelength++;
		}

		if ((listLT[h][1] + 1) < 512)
		{
			if ((abs(depmap[listLT[h][0]][listLT[h][1] + 1] - depmap[listLT[h][0]][listLT[h][1]]) < KKK) && (depmap[listLT[h][0]][listLT[h][1] + 1]>0))
			{
				if (!mapb[listLT[h][0]][listLT[h][1] + 1])
				{
					t++;
					listLT[t][0] = listLT[h][0];
					listLT[t][1] = listLT[h][1] + 1;
					mapb[listLT[t][0]][listLT[t][1]] = 1;
				}
			}
			else
			{
				sidelength++;
			}
		}
		else
		{
			edge = 1;
			sidelength++;
		}

		if ((listLT[h][1] - 1) >= 0)
		{
			if ((abs(depmap[listLT[h][0]][listLT[h][1] - 1] - depmap[listLT[h][0]][listLT[h][1]]) < KKK) && (depmap[listLT[h][0]][listLT[h][1] - 1]>0))
			{
				if (!mapb[listLT[h][0]][listLT[h][1] - 1])
				{
					t++;
					listLT[t][0] = listLT[h][0];
					listLT[t][1] = listLT[h][1] - 1;
					mapb[listLT[t][0]][listLT[t][1]] = 1;
				}
			}
			else
			{
				sidelength++;
			}
		}
		else
		{
			edge = 1;
			sidelength++;
		}


		h++;
	}

	bool isball = 1;

	//edge = 0;
	if ((t < 10) || (t>1500)
		|| (edge)
		|| (sidelength*sidelength / (t + 1) > 55)
		|| (double(depmap[listLT[t][0]][listLT[t][1]]) * depmap[listLT[t][0]][listLT[t][1]] * t<80000000)
		|| (double(depmap[listLT[t][0]][listLT[t][1]]) * depmap[listLT[t][0]][listLT[t][1]] * t>750000000)
		)
	{


		for (int i = 0; i <= t; i++)
		{

			//mapt->at<unsigned char>(listLT[i][0], listLT[i][1]) = 0;
			mapt->data[listLT[i][0] * 512 + listLT[i][1]] = 0;
		}

		isball = 0;
	}

	if (!CanErode)
	{
		if (isball)
		{
			int tol = 0;
			for (int k = 0; k < 30; k++)
			{
				for (int i = 0; i <= t; i++)
				{
					if (abs(Piclist[k].data[listLT[i][0] * 512 + listLT[i][1]] - unsigned char(depmap[listLT[i][0]][listLT[i][1]] >> 5)) < 10)
					{
						tol++;
					}
				}
				if (tol > 1.5 * (t + 1)) break;
			}
			if (tol > 1.5 * (t + 1)) //(tol>3 * t)
			{
				for (int i = 0; i <= t; i++)
				{
					//mapt->at<unsigned char>(listLT[i][0], listLT[i][1]) = 0;
					mapt->data[listLT[i][0] * 512 + listLT[i][1]] = 0;
				}
				isball = 0;
			}
		}
	}

	if (isball)
	{
		badminton ball;

		//cout << DepthMatTail - DepthMatHead << endl;


		ball.S = t + 1;
		for (int i = 0; i <= t; i++)
		{
			//mapt->data[listLT[i][0] * 512 + listLT[i][1]] = unsigned char(depmap[listLT[i][0]][listLT[i][1]] >> 5);
			ball.Y += listLT[i][0];//xy不要弄反了
			ball.X += listLT[i][1];
			ball.Z += depmap[listLT[i][0]][listLT[i][1]];
		}

		ball.Y /= ball.S;
		ball.X /= ball.S;
		ball.Z /= ball.S;

		DeDistortion(Distortion, gIntr[0], ball.X, ball.Y, ball.X, ball.Y);



		ball.PX = ball.Z* TanX / 212 * (gIntr[1][2] - ball.Y) / 1000;
		ball.PY = double(ball.Z) / 1000 * SinZ + ball.PX * CosZ + AddZ;
		ball.PZ = ball.Z* TanY / 256 * (ball.X - gIntr[0][2]) / 1000;
		ball.PX = -ball.PX * SinZ + double(ball.Z) / 1000 * CosZ;


		//kinecct不能始终向前
		FSinRobot = sin(double(-RobotStateList[DepthMatHead][2]) / 180 * pi);
		FCosRobot = cos(double(-RobotStateList[DepthMatHead][2]) / 180 * pi);

		//kinect始终向前
		//FSinRobot = 0;
		//FCosRobot = 1;

		

		SinRobot = sin(double(RobotStateList[DepthMatHead][2]) / 180 * pi);
		CosRobot = cos(double(RobotStateList[DepthMatHead][2]) / 180 * pi);



		KinectZ = KinectToRobotZ*CosRobot - KinectToRobotX*SinRobot;
		KinectX = KinectToRobotZ*SinRobot + KinectToRobotX*CosRobot;

		//cout << KinectZ << "   " << KinectX << endl;


		TmpZ = ball.PZ*FCosRobot - ball.PX*FSinRobot - (KinectZ - KinectToRobotZ) * 1 - RobotStateList[DepthMatHead][0] / 1000;
		TmpX = ball.PZ*FSinRobot + ball.PX*FCosRobot + (KinectX - KinectToRobotX) * 1 + RobotStateList[DepthMatHead][1] / 1000;
		TmpY = ball.PY;
		
		//cout << Gettime() << "	" << CAN.m_robotState.robot_x << "	" << CAN.m_robotState.robot_y << "	" << CAN.m_robotState.angle << endl;
		//LA_ofile << Gettime() << "	" << CAN.m_robotState.robot_x << "	" << CAN.m_robotState.robot_y << "	" << CAN.m_robotState.angle << endl;

		TmpX += AddY;

		ball.PX = TmpX;
		ball.PY = TmpY;
		ball.PZ = TmpZ;


		//ball.PX = ball.Z* TanX / 212 * (212 - ball.Y) / 1000;
		//ball.PY = double(ball.Z) / 1000 * SinZ + ball.PX * CosZ + AddZ;
		//ball.PZ = ball.Z* TanY / 256 * (ball.X - 256) / 1000;
		//ball.PX = -ball.PX * SinZ + double(ball.Z) / 1000 * CosZ;

		//	cout << "s"; 
		//Coordinate_robot->Coordinate_calculate_position_to_court(-1000 * ball.PZ, 1000 * ball.PX, 1000 * ball.PY, listnum);
		//	cout << " e" << endl;

		{
			//	ball.PX = Coordinate_robot->Coordinate_robot.coordinate_Generalized[1] / 1000;
			//	ball.PY = Coordinate_robot->Coordinate_robot.coordinate_Generalized[2] / 1000;
			//	ball.PZ = -Coordinate_robot->Coordinate_robot.coordinate_Generalized[0] / 1000;
		}
		//cout << "X=" << ball.PX << " Y=" << ball.PY << " Z=" << ball.PZ << endl;
		//Coordinate_robot->Coordinate_printf_information(listnum);
		if (!CanErode)
		{
			if (isball)
			{
				int tol = 0;
				for (int i = -14; i < 15; i++)
				{
					for (int j = -14; j < 15; j++)
					{
						int p = (int)ball.Y + i;
						int q = (int)ball.X + j;
						if ((p >= 0) && (p < 424) && (q >= 0) && (q < 512))
						{
							if (abs(ball.Z - depmap[p][q]) < KKK)
							{
								tol++;
							}
						}
					}
				}

				if (tol > 1.5 * (t + 1))  // (tol>3*t) 
				{
					for (int i = 0; i <= t; i++)
					{
						//mapt->at<unsigned char>(listLT[i][0], listLT[i][1]) = 0;
						mapt->data[listLT[i][0] * 512 + listLT[i][1]] = 0;

					}
		//			cout << "BBB out!" << endl;
					isball = 0;
				}
			}
		}


		if ((ball.PY > 5) && (isball))
		{
			for (int i = 0; i <= t; i++)
			{

				//mapt->at<unsigned char>(listLT[i][0], listLT[i][1]) = 0;
				mapt->data[listLT[i][0] * 512 + listLT[i][1]] = 0;
			}
			//cout << "PY out!" << endl;
			isball = 0;
		}


		if ((ball.PX > 8) && (isball))
		{
			for (int i = 0; i <= t; i++)
			{

				//mapt->at<unsigned char>(listLT[i][0], listLT[i][1]) = 0;
				mapt->data[listLT[i][0] * 512 + listLT[i][1]] = 0;
			}
			//cout << "PX out!" << endl;
			isball = 0;
		}




		if (!isball)
		{
			return;
		}

		int numflag = -1;
		double minPEX = 10000;
		double minVEX = 10000;
		double minAEX = 10000;

		for (int i = 0; i<MAXTRACK; i++)
		{
			isball = 1;
			if ((Posnum[i] > 0) && (isball))
			{
				if (double(Gettime() - Poslist[i][0].T) / 1000 - Poslist[i][Posnum[i]].T > 0.3)
				{
				//	if (Posnum[i] > 15)
				//	{
				//		for (int i = 1; i < savenum; i++)
				//		{
				//			stringstream ss;
				//			string s;
				//			ss << i;
				//			ss >> s;
				//			s = s + ".jpg";
				//			//imwrite(s, saveList[i]);
				//		}
				//	}

					if (Posnum[i] > 2)
					{
						flynow--;
					}
					//Poslist[1] = ball;
					Posnum[i] = 0;
					//isball = 0;

				}
			}

			if (Posnum[i] == 0)
			{
				//starttime = Gettime();
				Poslist[i][0].T = timeList[DepthMatHead];
				//minVEX = 5;
				//numflag = i;
				//continue;
				//cout << Poslist[i][0].T << endl;
			}


			//ball.T = double(Gettime() - starttime) / 1000;
			ball.T = double(timeList[DepthMatHead] - Poslist[i][0].T) / 1000;
			if (abs(ball.T - Poslist[i][Posnum[i]].T)<0.0001)
			{
				continue;
			}
			//cout << ball.T << endl;

			if ((Posnum[i] > 0) && (isball))
			{
				double dt;
				dt = (ball.T - Poslist[i][Posnum[i]].T);
				ball.VX = (ball.PX - Poslist[i][Posnum[i]].PX) / dt;
				ball.VY = (ball.PY - Poslist[i][Posnum[i]].PY) / dt;
				ball.VZ = (ball.PZ - Poslist[i][Posnum[i]].PZ) / dt;
				ball.V = sqrt(ball.VX*ball.VX + ball.VY*ball.VY + ball.VZ*ball.VZ);
				//if (Posnum[i] == 3)
				//{
				//	Poslist[i][0] = Poslist[i][2];
				//}

				if (ball.V > 70)// && (flynow))
				{
					//for (int i = 0; i <= t; i++)
					//{
					//	//mapt->at<unsigned char>(listLT[i][0], listLT[i][1]) = 0;
					//	mapt->data[listLT[i][0] * 512 + listLT[i][1]] = 0;
					//}
					isball = 0;
					//cout << i << "     11111111111111111111" << endl;
				}
				//regression();
			}

			if ((Posnum[i] > 1) && (isball))
			{
				double dt;
				double lastPosnum = 2 * Poslist[i][Posnum[i]].T - ball.T;

				if (lastPosnum <= Poslist[i][2].T)
				{
					dt = (ball.T - Poslist[i][Posnum[i] - 1].T);
					Poslist[i][Posnum[i]].VX = (ball.PX - Poslist[i][Posnum[i] - 1].PX) / dt;
					Poslist[i][Posnum[i]].VY = (ball.PY - Poslist[i][Posnum[i] - 1].PY) / dt;
					Poslist[i][Posnum[i]].VZ = (ball.PZ - Poslist[i][Posnum[i] - 1].PZ) / dt;
					Poslist[i][Posnum[i]].V = sqrt(Poslist[i][Posnum[i]].VX*Poslist[i][Posnum[i]].VX + Poslist[i][Posnum[i]].VY*Poslist[i][Posnum[i]].VY + Poslist[i][Posnum[i]].VZ*Poslist[i][Posnum[i]].VZ);
				}
				else
				{

					dt = (ball.T - Poslist[i][Posnum[i] - 1].T);
					Poslist[i][Posnum[i]].VX = (ball.PX - Poslist[i][Posnum[i] - 1].PX) / dt;
					Poslist[i][Posnum[i]].VY = (ball.PY - Poslist[i][Posnum[i] - 1].PY) / dt;
					Poslist[i][Posnum[i]].VZ = (ball.PZ - Poslist[i][Posnum[i] - 1].PZ) / dt;
					Poslist[i][Posnum[i]].V = sqrt(Poslist[i][Posnum[i]].VX*Poslist[i][Posnum[i]].VX + Poslist[i][Posnum[i]].VY*Poslist[i][Posnum[i]].VY + Poslist[i][Posnum[i]].VZ*Poslist[i][Posnum[i]].VZ);

					//cout << "VX=" << Poslist[i][Posnum[i]].VX << "  VY=" << Poslist[i][Posnum[i]].VY << "  VZ=" << Poslist[i][Posnum[i]].VZ << "  V=" << Poslist[i][Posnum[i]].V << endl;


					dt = (ball.T - lastPosnum);
					Poslist[i][Posnum[i]].VX = (ball.PX - GPlist[i][int(lastPosnum * 10000)][0]) / dt;
					Poslist[i][Posnum[i]].VY = (ball.PY - GPlist[i][int(lastPosnum * 10000)][1]) / dt;
					Poslist[i][Posnum[i]].VZ = (ball.PZ - GPlist[i][int(lastPosnum * 10000)][2]) / dt;
					Poslist[i][Posnum[i]].V = sqrt(Poslist[i][Posnum[i]].VX*Poslist[i][Posnum[i]].VX + Poslist[i][Posnum[i]].VY*Poslist[i][Posnum[i]].VY + Poslist[i][Posnum[i]].VZ*Poslist[i][Posnum[i]].VZ);

					//cout << "VX=" << Poslist[i][Posnum[i]].VX << "  VY=" << Poslist[i][Posnum[i]].VY << "  VZ=" << Poslist[i][Posnum[i]].VZ << "  V=" << Poslist[i][Posnum[i]].V << endl;

				}

				//if (Posnum[i] == 3)
				//{
				//	Poslist[i][0] = Poslist[i][2];
				//}

				if (Posnum[i] > 2)
				{
					double EX;

					EX = (GPlist[i][int(Poslist[i][Posnum[i]].T * 10000)][3] - Poslist[i][Posnum[i]].VX)*(GPlist[i][int(Poslist[i][Posnum[i]].T * 10000)][3] - Poslist[i][Posnum[i]].VX) +
						(GPlist[i][int(Poslist[i][Posnum[i]].T * 10000)][4] - Poslist[i][Posnum[i]].VY)*(GPlist[i][int(Poslist[i][Posnum[i]].T * 10000)][4] - Poslist[i][Posnum[i]].VY) +
						(GPlist[i][int(Poslist[i][Posnum[i]].T * 10000)][5] - Poslist[i][Posnum[i]].VZ)*(GPlist[i][int(Poslist[i][Posnum[i]].T * 10000)][5] - Poslist[i][Posnum[i]].VZ);

					//cout << i << "  EX= " << EX << endl;

					if ((EX<5) && (minVEX > EX))
					{
						minVEX = EX;
						numflag = i;
					}
					else
					{
						isball = 0;
					}

				}
			}

			if ((Posnum[i] > 1) && (isball))
			{
				{
					double f[3];
					Ff(Poslist[i][Posnum[i]].VX, Poslist[i][Posnum[i]].VY, Poslist[i][Posnum[i]].VZ, f);
					Poslist[i][Posnum[i]].AX = f[0]; // (Poslist[i][Posnum[i]].VX - Poslist[i][Posnum[i] - 1].VX) / dt;
					Poslist[i][Posnum[i]].AY = f[1]; // (Poslist[i][Posnum[i]].VY - Poslist[i][Posnum[i] - 1].VY) / dt;
					Poslist[i][Posnum[i]].AZ = f[2]; // (Poslist[i][Posnum[i]].VZ - Poslist[i][Posnum[i] - 1].VZ) / dt;
					Poslist[i][Posnum[i]].AC = sqrt(Poslist[i][Posnum[i]].AX*Poslist[i][Posnum[i]].AX + Poslist[i][Posnum[i]].AY*Poslist[i][Posnum[i]].AY + Poslist[i][Posnum[i]].AZ*Poslist[i][Posnum[i]].AZ);

					//Poslist[i][Posnum[i]].AC = (Poslist[i][Posnum[i]].V - Poslist[i][Posnum[i] - 1].V) / dt;
				}


/*
				double dt;
				dt = (Poslist[i][Posnum[i]].T - Poslist[i][Posnum[i] - 1].T);
				Poslist[i][Posnum[i]].AX = (Poslist[i][Posnum[i]].VX - Poslist[i][Posnum[i] - 1].VX) / dt;
				Poslist[i][Posnum[i]].AY = (Poslist[i][Posnum[i]].VY - Poslist[i][Posnum[i] - 1].VY) / dt;
				Poslist[i][Posnum[i]].AZ = (Poslist[i][Posnum[i]].VZ - Poslist[i][Posnum[i] - 1].VZ) / dt;
				Poslist[i][Posnum[i]].AC = (Poslist[i][Posnum[i]].V - Poslist[i][Posnum[i] - 1].V) / dt;*/





				if ((Poslist[i][Posnum[i]].AC < -200) || (Poslist[i][Posnum[i]].AC > 200))// && (flynow))
				{
					//for (int i = 0; i <= t; i++)
					//{
					//	//mapt->at<unsigned char>(listLT[i][0], listLT[i][1]) = 0;
					//	mapt->data[listLT[i][0] * 512 + listLT[i][1]] = 0;

					//}
					isball = 0;
					//cout << "222222222222222222222222222222222222" << endl;
				}
				//Poslist[i][Posnum[i] - 1].AC = sqrt(Poslist[i][Posnum[i] - 1].AX*Poslist[i][Posnum[i] - 1].AX + Poslist[i][Posnum[i] - 1].AY*Poslist[i][Posnum[i] - 1].AY + Poslist[i][Posnum[i] - 1].AZ*Poslist[i][Posnum[i] - 1].AZ);
			}

			if ((minVEX > 5) && (isball))
			{
				minVEX = 5;
				numflag = i;
			}

		}

		//cout << "minVEX= " << minVEX << endl;


		if (numflag >= 0)
		{
			
			//cout << WORKTIMEDELAY << endl;
			if (Posnum[numflag] > 1)
			{
				Drowlist[0][1] += 1;
				Drowlist[int(Drowlist[0][1])][0] = Poslist[numflag][Posnum[numflag]].PX + 5;
				Drowlist[int(Drowlist[0][1])][1] = Poslist[numflag][Posnum[numflag]].PY;
				Drowlist[int(Drowlist[0][1])][2] = -Poslist[numflag][Posnum[numflag]].PZ;

				if (Posnum[numflag] == 2)

					//GuessPos(Poslist[0][Posnum[0] - 1].PX, Poslist[num][Posnum[num] - 1].PY, Poslist[num][Posnum[num] - 1].PZ, Poslist[num][Posnum[num] - 1].VX, Poslist[num][Posnum[num] - 1].VY, Poslist[num][Posnum[num] - 1].VZ, 3, tmp, int((Poslist[num][Posnum[num]].T - Poslist[num][1].T) * 10000), int((Poslist[num][Posnum[num] - 1].T - Poslist[num][1].T) * 10000), num);
				{
					//GuessPos(x[(ii - 2)*n + 0], x[(ii - 2)*n + 1], x[(ii - 2)*n + 2], x[(ii - 2)*n + 3], x[(ii - 2)*n + 4], x[(ii - 2)*n + 5], 3, tmp, int((Poslist[num][Posnum[num]].T - Poslist[num][1].T) * 10000), int((Poslist[num][Posnum[num] - 1].T - Poslist[num][1].T) * 10000), num);
					//GuessPosnokal(Poslist[numflag][Posnum[numflag]], 3, int((Poslist[numflag][Posnum[numflag]].T - Poslist[numflag][1].T) * 10000), numflag);
					GuessPos(Poslist[numflag][Posnum[numflag]], 3, int((Poslist[numflag][Posnum[numflag]].T - Poslist[numflag][1].T) * 10000), numflag);
					setkalman(numflag);
				}
				else
				{


					//Drowlist[0][1] += 1;
					//Drowlist[int(Drowlist[0][1])][0] = TmpX;
					//Drowlist[int(Drowlist[0][1])][1] = TmpY;
					//Drowlist[int(Drowlist[0][1])][2] = -TmpZ;

					//LA_ofile << Poslist[numflag][Posnum[numflag]].PX << "	" << Poslist[numflag][Posnum[numflag]].PY << "	" << Poslist[numflag][Posnum[numflag]].PZ << "	" << Poslist[numflag][Posnum[numflag]].T << endl;
					//GuessPosnokal(Poslist[numflag][Posnum[numflag]], 3, int((Poslist[numflag][Posnum[numflag]].T - Poslist[numflag][1].T) * 10000), numflag);

					kalman(numflag);

					//ball.T = double(timeList[DepthMatHead] - Poslist[numflag][0].T) / 1000;
					//ball.T = double(timeList[DepthMatHead] - Poslist[numflag][0].T) / 1000;

					//Posnum[numflag]--;

					//{
					//	double dt;
					//	double lastPosnum = 2 * Poslist[numflag][Posnum[numflag]].T - ball.T;


					//	if (lastPosnum <= Poslist[numflag][2].T)
					//	{
					//		dt = (ball.T - Poslist[numflag][Posnum[numflag] - 1].T);
					//		Poslist[numflag][Posnum[numflag]].VX = (ball.PX - Poslist[numflag][Posnum[numflag] - 1].PX) / dt;
					//		Poslist[numflag][Posnum[numflag]].VY = (ball.PY - Poslist[numflag][Posnum[numflag] - 1].PY) / dt;
					//		Poslist[numflag][Posnum[numflag]].VZ = (ball.PZ - Poslist[numflag][Posnum[numflag] - 1].PZ) / dt;
					//		Poslist[numflag][Posnum[numflag]].V = sqrt(Poslist[numflag][Posnum[numflag]].VX*Poslist[numflag][Posnum[numflag]].VX + Poslist[numflag][Posnum[numflag]].VY*Poslist[numflag][Posnum[numflag]].VY + Poslist[numflag][Posnum[numflag]].VZ*Poslist[numflag][Posnum[numflag]].VZ);
					//	}
					//	else
					//	{
					//		dt = (ball.T - lastPosnum);
					//		Poslist[numflag][Posnum[numflag]].VX = (ball.PX - GPlist[numflag][int(lastPosnum * 10000)][0]) / dt;
					//		Poslist[numflag][Posnum[numflag]].VY = (ball.PY - GPlist[numflag][int(lastPosnum * 10000)][1]) / dt;
					//		Poslist[numflag][Posnum[numflag]].VZ = (ball.PZ - GPlist[numflag][int(lastPosnum * 10000)][2]) / dt;
					//		Poslist[numflag][Posnum[numflag]].V = sqrt(Poslist[numflag][Posnum[numflag]].VX*Poslist[numflag][Posnum[numflag]].VX + Poslist[numflag][Posnum[numflag]].VY*Poslist[numflag][Posnum[numflag]].VY + Poslist[numflag][Posnum[numflag]].VZ*Poslist[numflag][Posnum[numflag]].VZ);

					//		cout << "VX=" << Poslist[numflag][Posnum[numflag]].VX << "  VY=" << Poslist[numflag][Posnum[numflag]].VY << "  VZ=" << Poslist[numflag][Posnum[numflag]].VZ << "  V=" << Poslist[numflag][Posnum[numflag]].V << endl;

					//	}
					//}

					//{
					//	double dt;
					//	dt = (Poslist[numflag][Posnum[numflag]].T - Poslist[numflag][Posnum[numflag] - 1].T);
					//	Poslist[numflag][Posnum[numflag]].AX = (Poslist[numflag][Posnum[numflag]].VX - Poslist[numflag][Posnum[numflag] - 1].VX) / dt;
					//	Poslist[numflag][Posnum[numflag]].AY = (Poslist[numflag][Posnum[numflag]].VY - Poslist[numflag][Posnum[numflag] - 1].VY) / dt;
					//	Poslist[numflag][Posnum[numflag]].AZ = (Poslist[numflag][Posnum[numflag]].VZ - Poslist[numflag][Posnum[numflag] - 1].VZ) / dt;
					//	Poslist[numflag][Posnum[numflag]].AC = (Poslist[numflag][Posnum[numflag]].V - Poslist[numflag][Posnum[numflag] - 1].V) / dt;

					//	//Poslist[i][Posnum[i]].AC = (Poslist[i][Posnum[i]].V - Poslist[i][Posnum[i] - 1].V) / dt;
					//}

					//Posnum[numflag]++;
					//Poslist[numflag][Posnum[numflag]] = ball;
					//Poslist[numflag][Posnum[numflag]].T = double(timeList[DepthMatHead] - Poslist[numflag][0].T) / 1000;

					GuessPos(Poslist[numflag][Posnum[numflag]], 3, int((Poslist[numflag][Posnum[numflag]].T - Poslist[numflag][1].T) * 10000), numflag);
				}
				//GuessPos(Poslist[0][Posnum[0] - 1], 3, Poslist[0][Posnum[0] - 1].T * 10000, 0);
				//LA_ofile << Poslist[numflag][Posnum[numflag]].PX << "	" << Poslist[numflag][Posnum[numflag]].PY << "	" << Poslist[numflag][Posnum[numflag]].PZ <<"	kal"<<endl;
			}


			Drowlist[0][1] += 1;
			Drowlist[int(Drowlist[0][1])][0] = Poslist[numflag][Posnum[numflag]].PX;
			Drowlist[int(Drowlist[0][1])][1] = Poslist[numflag][Posnum[numflag]].PY;
			Drowlist[int(Drowlist[0][1])][2] = Poslist[numflag][Posnum[numflag]].PZ;

			//LA_ofile << timeList[DepthMatHead] << "	" << RobotStateList[DepthMatHead][0] << "	" << RobotStateList[DepthMatHead][1] << "	" << RobotStateList[DepthMatHead][2] << "	" << ball.PZ << endl;

			//cout << Poslist[numflag][Posnum[numflag]].PX << "	" << Poslist[numflag][Posnum[numflag]].PY << "	" << Poslist[numflag][Posnum[numflag]].PZ << endl;

			Posnum[numflag]++;
			Poslist[numflag][Posnum[numflag]] = ball;
			Poslist[numflag][Posnum[numflag]].T = double(timeList[DepthMatHead] - Poslist[numflag][0].T) / 1000;

			TrackNow = 1;
			//cout << Poslist[numflag][Posnum[numflag]].S << endl;
			if (Poslist[numflag][Posnum[numflag]].S > 50)
			{
				CanErode = 1;
				if (Poslist[numflag][Posnum[numflag]].S > 100)
				{
					CanErode2 = 1;
				}
			}
			//else
			//{
			//	CanErode = 0;
			//	CanErode2 = 0;
			//}

		}
		else
		{
			for (int i = 0; i <= t; i++)
			{
				//mapt->at<unsigned char>(listLT[i][0], listLT[i][1]) = 0;
				mapt->data[listLT[i][0] * 512 + listLT[i][1]] = 0;
			}
		}
	}
}


		//	if (Posnum[numflag] > 2)
		//	{
		//		flynow++;
		//		//double rex[9];
		//		if (Posnum[numflag] == 3)
		//		{
		//			setkalman(numflag);
		//		}
		//		else
		//		{
		//			kalman(numflag);
		//		}
		//		//if (((Coordinate_robot->Coordinate_robot.final_position_x - 200)*(Coordinate_robot->Coordinate_robot.final_position_x - 200) + Coordinate_robot->Coordinate_robot.final_position_y*Coordinate_robot->Coordinate_robot.final_position_y) < 3000 * 3000)
		//		//{
		//		//	if (((x_last - Coordinate_robot->Coordinate_robot.final_position_x)*(x_last - Coordinate_robot->Coordinate_robot.final_position_x) + (y_last - Coordinate_robot->Coordinate_robot.final_position_y)*(y_last - Coordinate_robot->Coordinate_robot.final_position_y)) > 150*150)
		//		//	{
		//		//		x_last = Coordinate_robot->Coordinate_robot.final_position_x;
		//		//		y_last = Coordinate_robot->Coordinate_robot.final_position_y;
		//		//		CAN.Send_RobotPosition_Frame(0x01, (short)Coordinate_robot->Coordinate_robot.final_position_tim,(short)(Coordinate_robot->Coordinate_robot.final_position_x - 200), (short)Coordinate_robot->Coordinate_robot.final_position_y);
		//		//	}
		//		//}	//GuessPos(Poslist[numflag][Posnum[numflag] - 1], 3, Poslist[numflag][Posnum[numflag] - 1].T * 10000, numflag);
		//	}

		//	Drowlist[0][1] += 1;
		//	Drowlist[int(Drowlist[0][1])][0] = Poslist[numflag][Posnum[numflag]].PX;
		//	Drowlist[int(Drowlist[0][1])][1] = Poslist[numflag][Posnum[numflag]].PY;
		//	Drowlist[int(Drowlist[0][1])][2] = Poslist[numflag][Posnum[numflag]].PZ;
		//}
		//else
		//{
		//	for (int i = 0; i <= t; i++)
		//	{
		//		//mapt->at<unsigned char>(listLT[i][0], listLT[i][1]) = 0;
		//		mapt->data[listLT[i][0] * 512 + listLT[i][1]] = 0;
		//	}
		//}




		//cout << Posnum[numflag] << endl;


		//	if (Posnum[i] > 8)
		//	{
		//regression();
		//}

		//if (Posnum[i] > 0)
		//{
		//	cout << endl;
		//	Posnum[i]--;
		//	cout << " X=" << Poslist[i][Posnum[i]].X << "   Y=" << Poslist[i][Posnum[i]].Y << "   Z=" << Poslist[i][Posnum[i]].Z << "   T=" << Poslist[i][Posnum[i]].T << endl;
		//	cout << "PX=" << Poslist[i][Posnum[i]].PX << "  PY=" << Poslist[i][Posnum[i]].PY << "  PZ=" << Poslist[i][Posnum[i]].PZ << endl;
		//	cout << "VX=" << Poslist[i][Posnum[i]].VX << "  VY=" << Poslist[i][Posnum[i]].VY << "  VZ=" << Poslist[i][Posnum[i]].VZ << endl;
		//	cout << "AX=" << Poslist[i][Posnum[i]].AX << "  AY=" << Poslist[i][Posnum[i]].AY << "  AZ=" << Poslist[i][Posnum[i]].AZ << endl;
		//	Posnum[i]++;
		//}


		//if (Posnum[i] > 1)
		//{
		//	double dt;
		//	dt = (Poslist[i][Posnum[i]].T - Poslist[i][Posnum[i] - 1].T) / 1000;
		//	Poslist[i][Posnum[i]].VX = (Poslist[i][Posnum[i]].PX - Poslist[i][Posnum[i] - 1].PX) / dt;
		//	Poslist[i][Posnum[i]].VY = (Poslist[i][Posnum[i]].PY - Poslist[i][Posnum[i] - 1].PY) / dt;
		//	Poslist[i][Posnum[i]].VZ = (Poslist[i][Posnum[i]].PZ - Poslist[i][Posnum[i] - 1].PZ) / dt;
		//	Poslist[i][Posnum[i]].V = sqrt(Poslist[i][Posnum[i]].VX*Poslist[i][Posnum[i]].VX + Poslist[i][Posnum[i]].VY*Poslist[i][Posnum[i]].VY + Poslist[i][Posnum[i]].VZ*Poslist[i][Posnum[i]].VZ);
		//	cout << "VX=" << Poslist[i][Posnum[i]].VX << "  VY=" << Poslist[i][Posnum[i]].VY << "  VZ=" << Poslist[i][Posnum[i]].VZ << endl;
		//}
		//if (Posnum[i] > 2)
		//{
		//	double dt;
		//	dt = (Poslist[i][Posnum[i]].T - Poslist[i][Posnum[i] - 1].T) / 1000;
		//	Poslist[i][Posnum[i]].AX = (Poslist[i][Posnum[i]].VX - Poslist[i][Posnum[i] - 1].VX) / dt;
		//	Poslist[i][Posnum[i]].AY = (Poslist[i][Posnum[i]].VY - Poslist[i][Posnum[i] - 1].VY) / dt;
		//	Poslist[i][Posnum[i]].AZ = (Poslist[i][Posnum[i]].VZ - Poslist[i][Posnum[i] - 1].VZ) / dt;
		//	Poslist[i][Posnum[i]].AC = sqrt(Poslist[i][Posnum[i]].AX*Poslist[i][Posnum[i]].AX + Poslist[i][Posnum[i]].AY*Poslist[i][Posnum[i]].AY + Poslist[i][Posnum[i]].AZ*Poslist[i][Posnum[i]].AZ);
		//	cout << "AX=" << Poslist[i][Posnum[i]].AX << "  AY=" << Poslist[i][Posnum[i]].AY << "  AZ=" << Poslist[i][Posnum[i]].AZ << endl;
		//}
		//if (Posnum[i] > 3)
		//{
