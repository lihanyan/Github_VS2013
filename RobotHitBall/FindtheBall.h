#pragma once
#include<fstream>


#include "Parameter.h"
#include "MySinglePointTo3D.h"
#include "gettime.h"
#include "kalman.h"


void BFSdep(int x, int y, Mat *mapt, int listnum);

void BFS(int x, int y, Mat *mapt);