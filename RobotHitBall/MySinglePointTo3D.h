#pragma once
//����ѧ��д�ģ������Ӿ�ԭ��任�����������������õ� �ռ�����ķ���
/*************************************3D �������***********************************/
#include<iostream>

//struct BALL_3D_PARA
//{
//	double x;
//	double y;
//	double z;
//};
//
///*���λ����Ϣ*/
//void txt_write_xyz(FILE *fp, double x, double y, double z){
//	fprintf(fp,"%lf ", x);
//	fprintf(fp,"%lf ", y);
//	fprintf(fp,"%lf\n ", z);
//	
//}
//
//bool MatrixInv(double *A, double *Ainv, int n)
//{//��������
////A:�������
////Ainv:��������
////n,����ά��
//	int i,j,k;
//	double d=1.0,temp=0,lamda=0;
//	
//	double *a=(double*)malloc(sizeof(double)*2*n*n); 
///*��ʼ������*/
//	for(i=0;i<n;i++)
//	   for(j=0;j<n;j++)
//	   {
//		a[i*2*n+j]=A[i*n+j];
//		a[i*2*n+n+j]=(i==j?1.0:0.0);
//	   }
///*���ݳ��ȱ任ԭ��,��������ȥ��*/
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
//			   for(j=0;j<2*n;j++)/*����*/
//			   { 
//				   temp=a[i*2*n+j];
//				   a[i*2*n+j]=a[k*2*n+j];
//				   a[k*2*n+j]=temp;
//			   }
//		   }
//		   aii=a[i*2*n+i];
//		   for (j=0;j<2*n;j++) a[i*2*n+j]/=aii;/*���й�һ��*/ 
//		   for(k=0;k<n;k++)/*����ȥ*/                
//		   {
//			   if(k==i||a[k*2*n+i]+d==d) continue;
//			   lamda=a[k*2*n+i]/a[i*2*n+i];
//			   for(j=0;j<2*n;j++) a[k*2*n+j]=a[k*2*n+j]-lamda*a[i*2*n+j]; 
//		   }
//	   }
//	   
//	   for(i=0;i<n;i++)/*���������*/
//		   for(j=0;j<n;j++)
//			   Ainv[i*n+j]=a[i*2*n+n+j];
//	free(a);
//	return true;
//}
//
//bool MatrixMulti(double *A, double *B, double *C, int am, int an, int bm, int bn)
//{/*�������,A,BΪ������,CΪ���ľ���,m,nΪ�к���*/ 
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
//{//����ת��
//	int i,j;
//	for(i=0;i<col;i++)
//		for(j=0;j<row;j++)
//			matrixTrans[i*row+j]=matrixOrigin[j*col+i];
//	return true;
//}



//����У������
double DeDistortion(
	double *DistortionOfCamera,
	double *CameraIntr,
	double u_distort,
	double v_distort,
	double &u,
	double &v);