#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include "HUST_TYPE.h"
#include "ReverseTheta.h"
using namespace std;


void QuickSort_Z(vector<enumTable>& a, int p, int r);
int FindZPos(vector<enumTable>& a, float z, int num);
bool InsertCorrcetResultFirst(vector<enumTable>& a, vector<enumTable>& vector, float x, float y, float z, int xThreshold, int yThreshold, int zThreshold);
void InsertCorrcetResult(vector<enumTable>& a, vector<enumTable>& vector, enumTable mBestResultLast, float x, float y, float z, int xThreshold, int yThreshold, int zThreshold);
void FindFinalResult(enumTable& mBestResult, enumTable mBestResultLast, vector<enumTable>& vector);
bool FindBetterResult(vector<enumTable>& v, enumTable& mBestResult, enumTable mBestResultLast);
void CreateTable(vector<enumTable>& mTable, vector<enumTable>& mTableL, vector<enumTable>& mTableLN, vector<enumTable>& mTableLW, vector<enumTable>& mTableR, vector<enumTable>& mTableRN, vector<enumTable>& mTableRW);
void QuickSort_BYaw(vector<enumTable>& a, int p, int r);
void ResetYawPos(vector<enumTable>& tableIn, int* yawPos);


