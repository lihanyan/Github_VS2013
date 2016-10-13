#pragma once
//这是学长写的，基于视觉原理变换，由左右两点点坐标得到 空间坐标的方法
/*************************************3D 坐标计算***********************************/
#include<iostream>

//struct BALL_3D_PARA
//{
//	double x;
//	double y;
//	double z;
//};
//
///*输出位置信息*/
//void txt_write_xyz(FILE *fp, double x, double y, double z){
//	fprintf(fp,"%lf ", x);
//	fprintf(fp,"%lf ", y);
//	fprintf(fp,"%lf\n ", z);
//	
//}
//
//bool MatrixInv(double *A, double *Ainv, int n)
//{//矩阵求逆
////A:输入矩阵
////Ainv:输出逆矩阵
////n,矩阵维数
//	int i,j,k;
//	double d=1.0,temp=0,lamda=0;
//	
//	double *a=(double*)malloc(sizeof(double)*2*n*n); 
///*初始化处理*/
//	for(i=0;i<n;i++)
//	   for(j=0;j<n;j++)
//	   {
//		a[i*2*n+j]=A[i*n+j];
//		a[i*2*n+n+j]=(i==j?1.0:0.0);
//	   }
///*根据初等变换原理,采用列消去法*/
//	   for(i=0;i<n;i++)
//	   { 
//		   double aii=a[i*2*n+i];
//		   if(aii+d==d)
//		   { 
//			   k=i+1;
//			   while(k<n&&a[k*2*n+i]+d==d) k++;
//			   if(k==n) 
//			   {   
//				   free(a);
//				   return false;
//			   }
//			   for(j=0;j<2*n;j++)/*换行*/
//			   { 
//				   temp=a[i*2*n+j];
//				   a[i*2*n+j]=a[k*2*n+j];
//				   a[k*2*n+j]=temp;
//			   }
//		   }
//		   aii=a[i*2*n+i];
//		   for (j=0;j<2*n;j++) a[i*2*n+j]/=aii;/*主行归一化*/ 
//		   for(k=0;k<n;k++)/*列消去*/                
//		   {
//			   if(k==i||a[k*2*n+i]+d==d) continue;
//			   lamda=a[k*2*n+i]/a[i*2*n+i];
//			   for(j=0;j<2*n;j++) a[k*2*n+j]=a[k*2*n+j]-lamda*a[i*2*n+j]; 
//		   }
//	   }
//	   
//	   for(i=0;i<n;i++)/*传出逆矩阵*/
//		   for(j=0;j<n;j++)
//			   Ainv[i*n+j]=a[i*2*n+n+j];
//	free(a);
//	return true;
//}
//
//bool MatrixMulti(double *A, double *B, double *C, int am, int an, int bm, int bn)
//{/*矩阵相乘,A,B为输入阵,C为积的矩阵,m,n为行和列*/ 
//	int i,j,l,u;
//
//	if(an!=bm) 
//	{  
//	   return false;
//	}
//    for (i=0; i<am; i++)
//		for (j=0; j<bn; j++)
//		  { 
//			u=i*bn+j; 
//			C[u]=0.0;
//			for (l=0; l<an; l++)
//			  C[u]=C[u]+A[i*an+l]*B[l*bn+j];
//
//			if (C[u]<0.00000001)
//			{
//			//	C[u]=0;
//			}
//		  }
//    return true;
//}
//
//bool MatrixTrans(double *matrixOrigin, double *matrixTrans, int row, int col)
//{//矩阵转置
//	int i,j;
//	for(i=0;i<col;i++)
//		for(j=0;j<row;j++)
//			matrixTrans[i*row+j]=matrixOrigin[j*col+i];
//	return true;
//}



//畸变校正函数
double DeDistortion(
	double *DistortionOfCamera,
	double *CameraIntr,
	double u_distort,
	double v_distort,
	double &u,
	double &v);