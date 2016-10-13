#pragma once

#include "Parameter.h"
#include "guess.h"

//ifstream fin("temp.txt");
//ofstream fout("out.txt");


//double TT[1000];

int rinv(int n, double a[]);

int setkalman1(int num);

int setkalman(int num);

int kalman(int num);

badminton kalman(int num, badminton data);




//


//int kalman(int n, int m, int k, double f[], double q[], double r[], double h[], double y[], double x[], double p[], double g[])
//{
//	//int n, m, k;
//	//double f[], q[], r[], h[], y[], x[], p[], g[];
//	int i, j, kk, ii, l, jj, js, s;
//	double *e, *a, *b, *tmp;
//	double AX[3][4];
//
//	tmp = (double*)malloc(m*sizeof(double));
//	e = (double*)malloc(m*m*sizeof(double));
//	l = m;
//	if (l < n) l = n;
//	a = (double*)malloc(l*l*sizeof(double));
//	b = (double*)malloc(l*l*sizeof(double));
//
//	for (i = 0; i < 4; i++)
//	{
//		for (j = 0; j <= n - 1; j++)
//		{
//			x[i*n + j] = y[i*n + j];
//		}
//	}
//	ii = 3;
//	for (i = 0; i <= 2; i++)
//	{
//		for (j = 0; j <= 2; j++)
//		{
//			AX[i][j] = x[(ii - 3)*n + i * n + j];
//		}
//	}
//	AX[0][3] = TT[ii - 3];
//	AX[1][3] = TT[ii - 2];
//	AX[2][3] = TT[ii - 1];
//	for (j = 0; j <= 2; j++)
//	{
//		AX[2][j] = y[(ii - 1)*n + j];
//	}
//	GuessPos(AX, 3, tmp, (TT[ii - 1] - TT[0]) * 10000, (TT[ii - 2] - TT[0]) * 10000);
//
//
//	for (i = 0; i <= n - 4; i++)
//	{
//		jj = ii*n + i;
//		//x[jj] = x[jj] + GPlist[TT[jj] - TT[jj - 2]][i]; //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
//		//f[i*n + i] = x[jj*n + i] / x[(jj - 1)*n + i];
//		f[i*n + i] = 1;
//		f[i*n + i + 3] = (x[jj] - x[jj - n]) / x[jj - n + 3];
//	}
//	for (i = 6; i < n; i++)
//	{
//		jj = ii*n + i;
//		//f[i*n + i - 3] = x[jj*n + i] / x[(jj - 2)*n + i - 3];
//		//f[i*n + i - 3] = 1;
//		f[i*n + i] = x[jj] / x[jj - n];
//	}
//
//
//	for (i = 0; i <= n - 1; i++)
//	for (j = 0; j <= n - 1; j++)
//	{
//		ii = i*l + j;
//		a[ii] = 0.0;
//		for (kk = 0; kk <= n - 1; kk++)
//			a[ii] = a[ii] + p[i*n + kk] * f[j*n + kk];
//	}
//	for (i = 0; i <= n - 1; i++)
//	for (j = 0; j <= n - 1; j++)
//	{
//		ii = i*n + j; p[ii] = q[ii];
//		for (kk = 0; kk <= n - 1; kk++)
//			p[ii] = p[ii] + f[i*n + kk] * a[kk*l + j];//P(k/k-1)=FP(k-1/k-1)F'+q
//	}
//
//
//
//	for (ii = 4; ii <= k; ii++)
//	{
//		for (i = 0; i <= n - 1; i++)
//		for (j = 0; j <= m - 1; j++)
//		{
//			jj = i*l + j; a[jj] = 0.0;
//			for (kk = 0; kk <= n - 1; kk++)
//				a[jj] = a[jj] + p[i*n + kk] * h[j*n + kk]; //a = P(k/k-1)H'
//		}
//		for (i = 0; i <= m - 1; i++)
//		for (j = 0; j <= m - 1; j++)
//		{
//			jj = i*m + j; e[jj] = r[jj];
//			for (kk = 0; kk <= n - 1; kk++)
//				e[jj] = e[jj] + h[i*n + kk] * a[kk*l + j];//S=H*P*H'+R (a=p(k/k-1))
//		}
//		js = rinv(m, e);
//		if (js == 0)
//		{
//			free(e);
//			free(a);
//			free(b);
//			free(tmp);
//			return(js);
//		}
//		for (i = 0; i <= n - 1; i++)
//		for (j = 0; j <= m - 1; j++)
//		{
//			jj = i*m + j; g[jj] = 0.0;
//			for (kk = 0; kk <= m - 1; kk++)
//				g[jj] = g[jj] + a[i*l + kk] * e[j*m + kk];//Kg=a*S(-1)
//		}
//
//		//GuessV(x[(ii - 2)*n + 0], x[(ii - 2)*n + 1], x[(ii - 2)*n + 2], x[(ii - 2)*n + 3], x[(ii - 2)*n + 4], x[(ii - 2)*n + 5], 3, tmp, (TT[ii - 1] - TT[ii - 2]) * 10);
//		//GuessPos(AX, 3, tmp, (TT[ii - 1] - TT[ii - 2]) * 10, (TT[ii - 2] - TT[0]) * 10);
//		//cout << Gettime() << endl;
//
//		for (i = 0; i < 9; i++)
//		{
//			x[(ii - 1)*n + i] = GPlist[int((TT[(ii - 1)] - TT[0]) * 10000)][i];
//		}
//
//		for (i = 6; i < 9; i++)
//		{
//			//x[(ii - 1)*n + i] = x[(ii - 1)*n + i - n*2];
//		}
//		//for (i = 0; i <= n - 1; i++)
//		//{
//		//	jj = (ii - 1)*n + i;
//		//	x[jj] = 0.0;
//		//	x[jj] = x[jj] + GPlist[(TT[ii] - TT[ii - 2])*10][i];  //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
//		//	f[i*n + i] = x[jj] / x[jj - 2*n];
//		//}
//
//		for (i = 0; i < 3; i++)
//		{
//			jj = (ii - 1)*n + i;
//			//x[jj] = x[jj] + GPlist[TT[jj] - TT[jj - 2]][i]; //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
//			////f[i*n + i] = x[jj*n + i] / x[(jj - 1)*n + i];
//			f[i*n + i] = 1;
//			f[i*n + i + 3] = (x[jj] - x[(jj - 1 * n)]) / x[(jj - 1 * n) + 3];
//			//f[i*n + i + 3] = (TT[ii - 1] - TT[ii - 2]);
//		}
//
//		for (i = 3; i < 6; i++)
//		{
//			jj = (ii - 1)*n + i;
//			//x[jj] = x[jj] + GPlist[TT[jj] - TT[jj - 2]][i]; //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
//			//f[i*n + i] = x[jj] / x[jj - n];
//			f[i*n + i] = 1;
//			f[i*n + i + 3] = (x[jj] - x[(jj - 1 * n)]) / x[(jj - 1 * n) + 3];
//			//f[i*n + i + 3] = (TT[ii - 1] - TT[ii - 2]);
//		}
//
//		for (i = 6; i < n; i++)
//		{
//			jj = (ii - 1)*n + i;
//			//f[i*n + i - 3] = x[jj] / x[(jj - 1 * n) - 3];
//			f[i*n + i] = x[jj] / x[(jj - 1 * n)];
//			//f[i*n + i] = 1;
//			////f[i*n + i] = x[jj*n + i] / x[(jj - 1)*n + i];
//		}
//
//
//		//for (i = 0; i <= n - 1; i++)
//		//{
//		//	jj = (ii - 1)*n + i; x[jj] = 0.0;
//		//	for (j = 0; j <= n - 1; j++)
//		//		x[jj] = x[jj] + f[i*n + j] * x[(ii - 3)*n + j];   //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
//		//}
//
//		for (i = 0; i <= 2; i++)
//		{
//			for (j = 0; j <= 2; j++)
//			{
//				AX[i][j] = x[(ii - 3)*n + i * n + j];
//			}
//		}
//		AX[0][3] = TT[ii - 3];
//		AX[1][3] = TT[ii - 2];
//		AX[2][3] = TT[ii - 1];
//		for (j = 0; j <= 2; j++)
//		{
//			AX[2][j] = y[(ii - 1)*n + j];
//		}
//		//GuessPos(AX, 3, tmp, (TT[ii - 1]-TT[0]) * 10000, (TT[ii - 2] - TT[0]) * 10000);
//		GuessPos(x[(ii - 2)*n + 0], x[(ii - 2)*n + 1], x[(ii - 2)*n + 2], x[(ii - 2)*n + 3], x[(ii - 2)*n + 4], x[(ii - 2)*n + 5], 3, tmp, (TT[ii - 1] - TT[0]) * 10000, (TT[ii - 2] - TT[0]) * 10000);
//		for (i = 3; i <= m - 1; i++)
//		{
//			y[(ii - 1) * m + i] = tmp[i];
//		}
//
//		for (i = 0; i <= m - 1; i++)
//		{
//			jj = i*l; b[jj] = y[(ii - 1)*m + i];  //¹Û²âÖµ
//			for (j = 0; j <= n - 1; j++)
//				b[jj] = b[jj] - h[i*n + j] * x[(ii - 1)*n + j];   //y=z-H*X(k/k-1)
//		}
//		for (i = 0; i <= n - 1; i++)
//		{
//			jj = (ii - 1)*n + i;
//			for (j = 0; j <= m - 1; j++)
//				x[jj] = x[jj] + g[i*m + j] * b[j*l];//¸üÐÂÂË²¨ X(k/k)=X(k/k-1)+Kg*y
//			fout << x[jj] << "	";
//		}
//		fout << TT[ii - 1] << endl;
//
//		if (ii < k)
//		{
//			for (i = 0; i <= n - 1; i++)
//			for (j = 0; j <= n - 1; j++)
//			{
//				jj = i*l + j; a[jj] = 0.0;
//				for (kk = 0; kk <= m - 1; kk++)
//					a[jj] = a[jj] - g[i*m + kk] * h[kk*n + j]; //¸üÐÂÐ­·½²î P(k/k)=(I-Kg*H)*P(k/k-1)
//				if (i == j) a[jj] = 1.0 + a[jj];
//			}
//			for (i = 0; i <= n - 1; i++)
//			for (j = 0; j <= n - 1; j++)
//			{
//				jj = i*l + j; b[jj] = 0.0;
//				for (kk = 0; kk <= n - 1; kk++)
//					b[jj] = b[jj] + a[i*l + kk] * p[kk*n + j];
//			}
//			for (i = 0; i <= n - 1; i++)
//			for (j = 0; j <= n - 1; j++)
//			{
//				jj = i*l + j; a[jj] = 0.0;
//				for (kk = 0; kk <= n - 1; kk++)
//					a[jj] = a[jj] + b[i*l + kk] * f[j*n + kk];//Ô¤²â¹À¼ÆÐ­·½²î P(k/k-1)
//			}
//			for (i = 0; i <= n - 1; i++)
//			for (j = 0; j <= n - 1; j++)
//			{
//				jj = i*n + j; p[jj] = q[jj];
//				for (kk = 0; kk <= n - 1; kk++)
//					p[jj] = p[jj] + f[i*n + kk] * a[j*l + kk];
//			}
//		}
//	}
//	free(e);
//	free(a);
//	free(b);
//	free(tmp);
//	return(js);
//}

//
//void main()
//{
//	int n, m, k;
//	double f[81], q[81], r[81], h[81], y[8100], x[8100], p[81], g[81];
//
//	fin >> n >> m >> k;
//	for (int i = 0; i < k; i++)
//	{
//		fin >> TT[i];
//		//TT[i] /= 1000;
//		for (int j = 0; j < 6; j++)
//		{
//			fin >> y[i*n + j];
//		}
//		if (i>1)
//		{
//			//for (int j = 3; j < 6; j++)
//			//{
//			//	y[(i-1)*n + j] = (y[i*n + j - 3] - y[(i - 2)*n + j - 3]) / (TT[i] - TT[i - 2]);
//			//}
//			double ff[3];
//			Ff(y[(i - 1)*n + 3], y[(i - 1)*n + 4], y[(i - 1)*n + 5], ff);
//			for (int j = 6; j < 9; j++)
//			{
//				y[(i - 1)*n + j] = ff[j - 6];
//			}
//		}
//	}
//
//	//R¹Û²â PÔ¤²â
//
//	for (int i = 0; i < n*n; i++)
//	{
//		h[i] = 0.000;
//		p[i] = 0.000;
//		f[i] = 0.000;
//		q[i] = 0;
//		r[i] = 0;
//	}
//	for (int i = 0; i < n * 3; i += (n + 1))
//	{
//		h[i] = 1;
//		p[i] = 1;
//		q[i] = 0.1;  //0.001
//		r[i] = 0.5;   //0.01
//	}
//	for (int i = (3 * n + 3); i < n * 6; i += (n + 1))
//	{
//		h[i] = 1;
//		p[i] = 2;
//		q[i] = 0.5;  //0.01
//		r[i] = 2;     //1
//	}
//	for (int i = (6 * n + 6); i < n*n; i += (n + 1))
//	{
//		h[i] = 1;
//		p[i] = 3;
//		q[i] = 1;    //1
//		r[i] = 10;   //100
//	}
//
//	//h[0] = 0;
//
//	//for (int i = 3; i < n ; i++)
//	//{
//	//	for (int j = 3; j < n; j++)
//	//	{
//	//		q[i] = 0.001;
//	//		r[i] = 0.1;
//	//	}
//	//}
//
//
//	kalman(n, m, k, f, q, r, h, y, x, p, g);
//
//	cin.get();
//	cin.get();
//
//	fin.close();
//	fout.close();
//}