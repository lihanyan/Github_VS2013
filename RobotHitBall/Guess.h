#pragma once

#include <cmath>
#include <fstream>
#include <iostream>
#include"Parameter.h"



extern double GPlist[MAXTRACK][100000][9];


using namespace std;


//int GuessPos(double x, double y, double z, double vx, double vy, double vz, double tmax, double rex[], int ren)
//int GuessPos(double AX[3][4], double tmax, double rex[], int ren, int tol)

//int GuessPos(badminton AX, double tmax, double rex[], int ren, int tol,unsigned char n)

//int GuessPos(badminton AX, double tmax, double rex[], int ren, int tol, int n)

extern void Ff(double x, double y, double z, double *f);

int GuessPos(double x, double y, double z, double vx, double vy, double vz, double tmax, double rex[], int ren, int tol, int n);

int GuessPos(badminton AX, double tmax, int tol, int n);
int GuessPosnokal(badminton AX, double tmax, int tol, int n);