#pragma once
//����ѧ��д�ģ������Ӿ�ԭ��任�����������������õ� �ռ�����ķ���
/*************************************3D �������***********************************/
#include "MySinglePointTo3D.h"
using namespace std;

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
	double &v)
{
	//Learning Opencv P442
	//����˵�� 
	//DistortionOfCamera	����������		1X5
	//CameraIntr			����ڲ�			3X4

	//2010��8��2��
	//double u0 = gRightIntr[0][2];     
	//double v0 = gRightIntr[1][2]; 
	//double kx = gRightIntr[0][0]; 
	//double ky = gRightIntr[1][1];
	double u0 = *(CameraIntr + 2);	// gRightIntr[0][2];	(ͼ������ԭ��)
	double v0 = *(CameraIntr + 4 + 2);	// grightintr[1][2];	(ͼ������ԭ��)
	double kx = *(CameraIntr);		// grightintr[0][0];	fx
	double ky = *(CameraIntr + 4 + 1);	// grightintr[1][1];	fy


	//    ��������
	double k1 = DistortionOfCamera[0];
	double k2 = DistortionOfCamera[1];
	double p1 = DistortionOfCamera[2];
	double p2 = DistortionOfCamera[3];
	double k3 = DistortionOfCamera[4];

	// ��һ������	
	double x_dist, y_dist;
	x_dist = (u_distort - u0) / kx;
	y_dist = (v_distort - v0) / ky;

	// ��ʼ���䲹��
	int k = 3;  // ����������ʵ��ֵΪ3�ξͲ���ˣ����Զ��Դռ���

	double r2;   // x*x + y*y
	double x, y;	// ��������
	double k_radial; // ����
	double k_tang_x, k_tang_y;   // ����

	double delta_;  // �в�
	double x0, y0;  // ÿ�ֵ�����ʼʱx,y��ֵ������


	x = x_dist;  // ������ֵ
	y = y_dist;
	// ��ʼ����
	for (int i = 0; i<k; i++)  // ����3�κ�,[x,y]����Ա仯��Ϊ0.0003949
	{
		x0 = x; y0 = y;

		r2 = x*x + y*y;
		k_radial = 1 + k1 * r2 + k2 * r2*r2 + k3 * r2*r2*r2;
		k_tang_x = 2 * p1*x*y + p2*(r2 + 2 * x*x);
		k_tang_y = 2 * p2*x*y + p1*(r2 + 2 * y*y);

		x = (x_dist - k_tang_x) / k_radial;
		y = (y_dist - k_tang_y) / k_radial;

		// �����ʹ�òв�����Ƿ��������������Բ���������if�еĴ���
		// ����������ٶ�
		if (1)
		{
			double delta = sqrt((x - x0)*(x - x0) + (y - y0)*(y - y0));
			double normal = sqrt((x0)*(x0)+(y0)*(y0));
			delta_ = delta / normal;
			if (delta_ < 0.0001)
				break;
		}

	}
	delta_ = 0;
	// ���
	u = x * kx + u0;
	v = y * ky + v0;
	return delta_;
}


//
//
//bool Compute3D(
//	double *RightIntr,
//	double *LeftIntr,
//	double *RightExtr,
//	double *LeftExtr,
//	double x1,
//	double y1, 
//	double x2, 
//	double y2, 
//	double &x, 
//	double &y, 
//	double &z)
//{//������ά����
////	printf("x1:%d,y1:%d,x2:%d,y2:%d\n",x1,y1,x2,y2);
//
//
//
//	double gLeftMat[3][4];
//	double gRightMat[3][4];
//
//	double gTempMatA[4][3];
//	double gTempMatAT[3][4];
//
//	double gTempMatB[4];
//	double gTempMatC[3];
//
//	double gTempMatD[3][3];
//	double gTempMatE[3][3];
//	double gTempMatF[3][4];
//
//	MatrixMulti(RightIntr,RightExtr,gRightMat[0],3,4,4,4);
//	MatrixMulti(LeftIntr,LeftExtr,gLeftMat[0],3,4,4,4);
//
//	gTempMatA[0][0] = x1*gLeftMat[2][0] - gLeftMat[0][0];
//	gTempMatA[0][1] = x1*gLeftMat[2][1] - gLeftMat[0][1];
//	gTempMatA[0][2] = x1*gLeftMat[2][2] - gLeftMat[0][2];
//	gTempMatA[1][0] = y1*gLeftMat[2][0] - gLeftMat[1][0];
//	gTempMatA[1][1] = y1*gLeftMat[2][1] - gLeftMat[1][1];
//	gTempMatA[1][2] = y1*gLeftMat[2][2] - gLeftMat[1][2];
//	gTempMatA[2][0] = x2*gRightMat[2][0] - gRightMat[0][0];
//	gTempMatA[2][1] = x2*gRightMat[2][1] - gRightMat[0][1];
//	gTempMatA[2][2] = x2*gRightMat[2][2] - gRightMat[0][2];
//	gTempMatA[3][0] = y2*gRightMat[2][0] - gRightMat[1][0];
//	gTempMatA[3][1] = y2*gRightMat[2][1] - gRightMat[1][1];
//	gTempMatA[3][2] = y2*gRightMat[2][2] - gRightMat[1][2];
//	
//	gTempMatB[0] = gLeftMat[0][3] - x1*gLeftMat[2][3];
//	gTempMatB[1] = gLeftMat[1][3] - y1*gLeftMat[2][3];
//	gTempMatB[2] = gRightMat[0][3] - x2*gRightMat[2][3];
//	gTempMatB[3] = gRightMat[1][3] - y2*gRightMat[2][3];
//	
////����D = A^T*A
//	MatrixTrans(gTempMatA[0],gTempMatAT[0],4,3);
//	MatrixMulti(gTempMatAT[0],gTempMatA[0],gTempMatD[0],3,4,4,3);
//
////����E = D^-1
//	if (!MatrixInv(gTempMatD[0],gTempMatE[0],3))
//	{
//		printf("��������ʧ�ܣ��������!\n");
//		return false;
//	}
////����F = E*A^T
//	MatrixMulti(gTempMatE[0],gTempMatAT[0],gTempMatF[0],3,3,3,4);
////������C = F*B
//	MatrixMulti(gTempMatF[0],gTempMatB,gTempMatC,3,4,4,1);
//
//
//	//��λ���Ǳ궨���̵ı߳�  һ����mm
//	//��λת�� mm����m
//	//�����λ m
//	//x = gTempMatC[0]/1000.0;
//	//y = gTempMatC[1]/1000.0;
//	//z = gTempMatC[2]/1000.0;
//
//
//	//�����λ��λ mm
//	x = gTempMatC[0];
//	y = gTempMatC[1];
//	z = gTempMatC[2];
//
//
////������������ϵ
////	x = gTempMatC[0] - 1226;
////	y = gTempMatC[1] - 300;
////	z = gTempMatC[2] +15;
//	if (fabs(x) > 4 || fabs(y) > 2 || fabs(z) > 2)
//	{
//		//printf("Compute3D Failed! ");
//		//printf("x1:%d,y1:%d,x2:%d,y2:%d\n");
//		return false;
//	}
//	return true;
//}
//
//
//
//
//bool processCam(
//	double *DistortionOfLeftCamera,
//	double *LeftCameraIntr,
//	double *LeftCameraExtr,
//	double *DistortionOfRightCamera,
//	double *RightCameraIntr,
//	double *RightCameraExtr,
//	double Left_x,
//	double Left_y,
//	double Right_x,
//	double Right_y,
//	BALL_3D_PARA *ball,
//	double sita
//	)
//{
//	//�������
//	double x10 = Left_x;
//	double y10 = Left_y;
//	double x20 = Right_x;
//	double y20 = Right_y;
//
//	DeDistortion(DistortionOfLeftCamera, LeftCameraIntr, x10, y10, Left_x, Left_y);
//	DeDistortion(DistortionOfRightCamera, RightCameraIntr, x20, y20, Right_x, Right_y);
//
//	//cout << "����У����ĵ�" << endl;
//	//cout<< Left_x << "	" << Left_y << endl;
//	//cout << Right_x << "	" << Right_y << endl;
//
//	BALL_3D_PARA Temp;
//	//������ά����
//	//�����ά�������������ϵ��ԭ����  ��ѡ������һ��ͼƬ������ϵ�������������趨�� RT��
//	Compute3D(RightCameraIntr, LeftCameraIntr, RightCameraExtr, LeftCameraExtr, Left_x, Left_y, Right_x, Right_y, Temp.x, Temp.y, Temp.z);
//	//Compute3D(RightCameraIntr, LeftCameraIntr, RightCameraExtr, LeftCameraExtr, Left_x, Left_y, Right_x, Right_y, Temp.x, Temp.y, Temp.z);
//
//
//	//�����б������ת��
//	ball->x = Temp.x; 
//	ball->y = Temp.y;
//	ball->z = Temp.z;
//
//
//	//ball->y = (445.0 / 482)*Temp.y + 0.3842321411*Temp.z;
//	//ball->z = -0.3842321411*Temp.y + (445.0 / 482)*Temp.z;
//
//	//ball->y = 0.8916543234*Temp.y - 0.4527168*Temp.z;
//	//ball->z = + 0.4527168*Temp.y + 0.8916543234*Temp.z;
//
//	/*double sita_PI = (sita / 180)*3.141592653;
//	ball->y = cos(-sita_PI)*Temp.y + sin(-sita_PI)*Temp.z;
//	ball->z = -sin(-sita_PI)*Temp.y + cos(-sita_PI)*Temp.z;*/
//
//	return true;
//}