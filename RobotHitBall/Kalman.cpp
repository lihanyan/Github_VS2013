
#include"kalman.h"

using namespace std;

kal kalarr[MAXTRACK];


const double rr[81] = { 0.0001, 0.000001, 0.000001, 0.003, 0.000001, 0.000001, 0.2, 0.000001, 0.000001,
0.000001, 0.0001, 0.000001, 0.000001, 0.003, 0.000001, 0.000001, 0.2, 0.000001,
0.000001, 0.000001, 0.0001, 0.000001, 0.000001, 0.003, 0.000001, 0.000001, 0.2,
0.003, 0.000001, 0.000001, 0.1, 0.000001, 0.000001, 6.0, 0.000001, 0.000001,
0.000001, 0.003, 0.000001, 0.000001, 0.1, 0.000001, 0.000001, 6.0, 0.000001,
0.000001, 0.000001, 0.003, 0.000001, 0.000001, 0.1, 0.000001, 0.000001, 6.0,
0.2, 0.000001, 0.000001, 6.0, 0.000001, 0.000001, 400, 0.000001, 0.000001,
0.000001, 0.2, 0.000001, 0.000001, 6.0, 0.000001, 0.000001, 400, 0.000001,
0.000001, 0.000001, 0.2, 0.000001, 0.000001, 6.0, 0.000001, 0.000001, 400 };


int rinv(int n, double a[])
{
	int  *is, *js, i, j, k, l, u, v;
	double d, p;
	is = (int*)malloc(n*sizeof(int));
	js = (int*)malloc(n*sizeof(int));
	for (k = 0; k <= n - 1; k++)
	{
		d = 0.0;
		for (i = k; i <= n - 1; i++)
		for (j = k; j <= n - 1; j++)
		{
			l = i*n + j; p = fabs(a[l]);
			if (p > d)
			{
				d = p; is[k] = i; js[k] = j;
			}
		}
		if (d + 1.0 == 1.0)
		{
			free(is);
			free(js);
			printf("err**not inv\n");
			return(0);
		}
		if (is[k] != k)
		for (j = 0; j <= n - 1; j++)
		{
			u = k*n + j; v = is[k] * n + j;
			p = a[u]; a[u] = a[v]; a[v] = p;
		}
		if (js[k] != k)
		for (i = 0; i <= n - 1; i++)
		{
			u = i*n + k; v = i*n + js[k];
			p = a[u]; a[u] = a[v]; a[v] = p;
		}
		l = k*n + k;
		a[l] = 1.0 / a[l];
		for (j = 0; j <= n - 1; j++)
		if (j != k)
		{
			u = k*n + j; a[u] = a[u] * a[l];
		}
		for (i = 0; i <= n - 1; i++)
		if (i != k)
		for (j = 0; j <= n - 1; j++)
		if (j != k)
		{
			u = i*n + j;
			a[u] = a[u] - a[i*n + k] * a[k*n + j];
		}
		for (i = 0; i <= n - 1; i++)
		if (i != k)
		{
			u = i*n + k; a[u] = -a[u] * a[l];
		}
	}
	for (k = n - 1; k >= 0; k--)
	{
		if (js[k] != k)
		for (j = 0; j <= n - 1; j++)
		{
			u = k*n + j; v = js[k] * n + j;
			p = a[u]; a[u] = a[v]; a[v] = p;
		}
		if (is[k] != k)
		for (i = 0; i <= n - 1; i++)
		{
			u = i*n + k; v = i*n + is[k];
			p = a[u]; a[u] = a[v]; a[v] = p;
		}
	}
	free(is);
	free(js);
	return(1);
}



int setkalman1(int num)
{
	double *f, *q, *r, *h, *y, *x, *p, *g;
	double *e, *a, *b, *tmp;
	int i, j, kk, ii, l, jj, js, n = 9, m = 9;

	f = kalarr[num].f;
	q = kalarr[num].q;
	r = kalarr[num].r;
	h = kalarr[num].h;
	y = kalarr[num].y;
	x = kalarr[num].x;
	p = kalarr[num].p;
	g = kalarr[num].g;
	e = kalarr[num].e;
	a = kalarr[num].a;
	b = kalarr[num].b;
	tmp = kalarr[num].tmp;

	l = m;
	if (l < n) l = n;

	//R¹Û²â PÔ¤²â
	for (int i = 0; i < n*n; i++)
	{
		h[i] = 0.000;
		p[i] = 0.000;
		f[i] = 0.000;
		q[i] = 0;
		r[i] = 0;
	}
	for (int i = 0; i < n * 3; i += (n + 1))
	{
		h[i] = 1;
		p[i] = 0.05;
		q[i] = 0.01;  //0.001
		r[i] = 0.05;   //0.01
	}
	for (int i = (3 * n + 3); i < n * 6; i += (n + 1))
	{
		h[i] = 1;
		p[i] = 1;
		q[i] = 0.1;  //0.01
		r[i] = 1;     //1
	}
	for (int i = (6 * n + 6); i < n*n; i += (n + 1))
	{
		h[i] = 0;
		p[i] = 5;
		q[i] = 5;    //1
		r[i] = 100;   //100
	}


	for (i = 0; i <= 1; i++)
	{
		x[i*n + 0] = Poslist[num][Posnum[num] - 1 + i].PX;
		x[i*n + 1] = Poslist[num][Posnum[num] - 1 + i].PY;
		x[i*n + 2] = Poslist[num][Posnum[num] - 1 + i].PZ;
		x[i*n + 3] = Poslist[num][Posnum[num] - 1 + i].VX;
		x[i*n + 4] = Poslist[num][Posnum[num] - 1 + i].VY;
		x[i*n + 5] = Poslist[num][Posnum[num] - 1 + i].VZ;
		x[i*n + 6] = Poslist[num][Posnum[num] - 1 + i].AX;
		x[i*n + 7] = Poslist[num][Posnum[num] - 1 + i].AY;
		x[i*n + 8] = Poslist[num][Posnum[num] - 1 + i].AZ;
	}


	//GuessPos(Poslist[num][Posnum[num] - 1], 3, tmp, (TT[ii - 1] - TT[0]) * 10000, (TT[ii - 2] - TT[0]) * 10000);

	//GuessPos(Poslist[num][Posnum[num] - 1], 3, tmp, int((Poslist[num][Posnum[num]].T - Poslist[num][1].T) * 10000), int((Poslist[num][Posnum[num] - 1].T - Poslist[num][1].T) * 10000), num);
	GuessPos(Poslist[num][Posnum[num] - 1].PX, Poslist[num][Posnum[num] - 1].PY, Poslist[num][Posnum[num] - 1].PZ, Poslist[num][Posnum[num] - 1].VX, Poslist[num][Posnum[num] - 1].VY, Poslist[num][Posnum[num] - 1].VZ, 3, tmp, int((Poslist[num][Posnum[num]].T - Poslist[num][1].T) * 10000), int((Poslist[num][Posnum[num] - 1].T - Poslist[num][1].T) * 10000), num);

	ii = 1;
	for (i = 0; i <3; i++)
	{
		jj = ii*n + i;
		//x[jj] = x[jj] + GPlist[TT[jj] - TT[jj - 2]][i]; //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
		//f[i*n + i] = x[jj*n + i] / x[(jj - 1)*n + i];
		f[i*n + i] = 1;
		f[i*n + i + 3] = (x[jj] - x[jj - n]) / x[jj - n + 3];
	}
	for (i = 3; i <6; i++)
	{
		jj = ii*n + i;
		//x[jj] = x[jj] + GPlist[TT[jj] - TT[jj - 2]][i]; //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
		//f[i*n + i] = x[jj*n + i] / x[(jj - 1)*n + i];
		f[i*n + i] = 1;
		//f[i*n + i + 3] = (x[jj] - x[jj - n]) / x[jj - n + 3];
		f[i*n + i + 3] = Poslist[num][Posnum[num]].T - Poslist[num][Posnum[num] - 1].T;
	}
	for (i = 6; i < n; i++)
	{
		jj = ii*n + i;
		//f[i*n + i - 3] = x[jj*n + i] / x[(jj - 2)*n + i - 3];
		f[i*n + i - 3] = 1;
		//f[i*n + i] = x[jj] / x[jj - n];
	}


	for (i = 0; i <= n - 1; i++)
	for (j = 0; j <= n - 1; j++)
	{
		ii = i*l + j;
		a[ii] = 0.0;
		for (kk = 0; kk <= n - 1; kk++)
			a[ii] = a[ii] + p[i*n + kk] * f[j*n + kk];
	}
	for (i = 0; i <= n - 1; i++)
	for (j = 0; j <= n - 1; j++)
	{
		ii = i*n + j; p[ii] = q[ii];
		for (kk = 0; kk <= n - 1; kk++)
			p[ii] = p[ii] + f[i*n + kk] * a[kk*l + j];//P(k/k-1)=FP(k-1/k-1)F'+q
	}

	return 0;

}


//
//int kalman(int num)
//{
//
//	double *f, *q, *r, *h, *y, *x, *p, *g;
//	double *e, *a, *b, *tmp;
//	int i, j, kk, ii, l, jj, js, n = 9, m = 9;
//
//	f = kalarr[num].f;
//	q = kalarr[num].q;
//	r = kalarr[num].r;
//	h = kalarr[num].h;
//	y = kalarr[num].y;
//	x = kalarr[num].x;
//	p = kalarr[num].p;
//	g = kalarr[num].g;
//	e = kalarr[num].e;
//	a = kalarr[num].a;
//	b = kalarr[num].b;
//	tmp = kalarr[num].tmp;
//
//	l = m;
//	if (l < n) l = n;
//
//	for (i = 0; i < 9; i++)
//	{
//		x[i] = x[i + 9];
//	}
//
//	if (Posnum[num] == 4)
//	{
//
//		for (i = 0; i <= 1; i++)
//		{
//			x[i*n + 0] = Poslist[num][Posnum[num] - 1 + i].PX;
//			x[i*n + 1] = Poslist[num][Posnum[num] - 1 + i].PY;
//			x[i*n + 2] = Poslist[num][Posnum[num] - 1 + i].PZ;
//			x[i*n + 3] = Poslist[num][Posnum[num] - 1 + i].VX;
//			x[i*n + 4] = Poslist[num][Posnum[num] - 1 + i].VY;
//			x[i*n + 5] = Poslist[num][Posnum[num] - 1 + i].VZ;
//			x[i*n + 6] = Poslist[num][Posnum[num] - 1 + i].AX;
//			x[i*n + 7] = Poslist[num][Posnum[num] - 1 + i].AY;
//			x[i*n + 8] = Poslist[num][Posnum[num] - 1 + i].AZ;
//		}
//
//	}
//
//
//	ii = 2;
//
//	for (i = 0; i <= n - 1; i++)
//	for (j = 0; j <= m - 1; j++)
//	{
//		jj = i*l + j; a[jj] = 0.0;
//		for (kk = 0; kk <= n - 1; kk++)
//			a[jj] = a[jj] + p[i*n + kk] * h[j*n + kk]; //a = P(k/k-1)H'
//	}
//	for (i = 0; i <= m - 1; i++)
//	for (j = 0; j <= m - 1; j++)
//	{
//		jj = i*m + j; e[jj] = r[jj];
//		for (kk = 0; kk <= n - 1; kk++)
//			e[jj] = e[jj] + h[i*n + kk] * a[kk*l + j];//S=H*P*H'+R (a=p(k/k-1))
//	}
//	js = rinv(m, e);
//	if (js == 0)
//	{
//		return(js);
//	}
//	for (i = 0; i <= n - 1; i++)
//	for (j = 0; j <= m - 1; j++)
//	{
//		jj = i*m + j; g[jj] = 0.0;
//		for (kk = 0; kk <= m - 1; kk++)
//			g[jj] = g[jj] + a[i*l + kk] * e[j*m + kk];//Kg=a*S(-1)
//	}
//
//	//GuessV(x[(ii - 2)*n + 0], x[(ii - 2)*n + 1], x[(ii - 2)*n + 2], x[(ii - 2)*n + 3], x[(ii - 2)*n + 4], x[(ii - 2)*n + 5], 3, tmp, (TT[ii - 1] - TT[ii - 2]) * 10);
//	//GuessPos(AX, 3, tmp, (TT[ii - 1] - TT[ii - 2]) * 10, (TT[ii - 2] - TT[0]) * 10);
//	//cout << Gettime() << endl;
//
//	for (i = 0; i < 9; i++)
//	{
//		x[(ii - 1)*n + i] = GPlist[num][int(Poslist[num][Posnum[num]].T * 10000)][i];
//	}
//
//	//	for (i = 6; i < 9; i++)
//	//	{
//	//x[(ii - 1)*n + i] = x[(ii - 1)*n + i - n*2];
//	//	}
//	//for (i = 0; i <= n - 1; i++)
//	//{
//	//	jj = (ii - 1)*n + i;
//	//	x[jj] = 0.0;
//	//	x[jj] = x[jj] + GPlist[(TT[ii] - TT[ii - 2])*10][i];  //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
//	//	f[i*n + i] = x[jj] / x[jj - 2*n];
//	//}
//
//
//	for (i = 0; i < 3; i++)
//	{
//		jj = (ii - 1)*n + i;
//		//x[jj] = x[jj] + GPlist[TT[jj] - TT[jj - 2]][i]; //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
//		////f[i*n + i] = x[jj*n + i] / x[(jj - 1)*n + i];
//		//f[i*n + i] = x[jj] / x[jj - n];
//		f[i*n + i] = 1;
//		f[i*n + i + 3] = (x[jj] - x[(jj - 1 * n)]) / x[(jj - 1 * n) + 3];
//		//f[i*n + i + 3] = Poslist[num][Posnum[num]].T - Poslist[num][Posnum[num] - 1].T;
//	}
//
//	for (i = 3; i < 6; i++)
//	{
//		jj = (ii - 1)*n + i;
//		//x[jj] = x[jj] + GPlist[TT[jj] - TT[jj - 2]][i]; //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
//		//f[i*n + i] = x[jj] / x[jj - n];
//		f[i*n + i] = 1;
//		f[i*n + i + 3] = (x[jj] - x[(jj - 1 * n)]) / x[(jj - 1 * n) + 3];
//		//f[i*n + i + 3] = Poslist[num][Posnum[num]].T - Poslist[num][Posnum[num] - 1].T;
//		//f[i*n + i + 3] = (TT[ii - 1] - TT[ii - 2]);
//	}
//
//	for (i = 6; i < n; i++)
//	{
//		jj = (ii - 1)*n + i;
//		//f[i*n + i - 3] = x[jj] / x[(jj - 1 * n) - 3];
//		f[i*n + i] = x[jj] / x[(jj - 1 * n)];
//		//f[i*n + i] = 1;
//		////f[i*n + i] = x[jj*n + i] / x[(jj - 1)*n + i];
//	}
//
//
//	//for (i = 0; i <= n - 1; i++)
//	//{
//	//	jj = (ii - 1)*n + i; x[jj] = 0.0;
//	//	for (j = 0; j <= n - 1; j++)
//	//		x[jj] = x[jj] + f[i*n + j] * x[(ii - 3)*n + j];   //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
//	//}
//
//	//for (i = 0; i <= 2; i++)
//	//{
//	//	for (j = 0; j <= 2; j++)
//	//	{
//	//		AX[i][j] = x[(ii - 3)*n + i * n + j];
//	//	}
//	//}
//	//AX[0][3] = TT[ii - 3];
//	//AX[1][3] = TT[ii - 2];
//	//AX[2][3] = TT[ii - 1];
//
//	//for (j = 0; j <= 2; j++)
//	//{
//	//	AX[2][j] = y[(ii - 1)*n + j];
//	//}
//
//
//
//	GuessPos(x[(ii - 2)*n + 0], x[(ii - 2)*n + 1], x[(ii - 2)*n + 2], x[(ii - 2)*n + 3], x[(ii - 2)*n + 4], x[(ii - 2)*n + 5], 3, tmp, int((Poslist[num][Posnum[num]].T - Poslist[num][1].T) * 10000), int((Poslist[num][Posnum[num] - 1].T - Poslist[num][1].T) * 10000), num);
//	//GuessPos(Poslist[num][Posnum[num] - 1], 3, tmp, int((Poslist[num][Posnum[num]].T - Poslist[num][1].T) * 10000), int((Poslist[num][Posnum[num] - 1].T - Poslist[num][1].T) * 10000), num);
//	//GuessPos(x[(ii - 2)*n + 0], x[(ii - 2)*n + 1], x[(ii - 2)*n + 2], x[(ii - 2)*n + 3], x[(ii - 2)*n + 4], x[(ii - 2)*n + 5], 3, tmp, (TT[ii - 1] - TT[0]) * 10000, (TT[ii - 2] - TT[0]) * 10000);
//
//
//	{
//		y[(ii - 1) * m + 0] = Poslist[num][Posnum[num]].PX;
//		y[(ii - 1) * m + 1] = Poslist[num][Posnum[num]].PY;
//		y[(ii - 1) * m + 2] = Poslist[num][Posnum[num]].PZ;
//	}
//
//	for (i = 3; i <= m - 1; i++)
//	{
//		y[(ii - 1) * m + i] = tmp[i];
//	}
//
//	for (i = 0; i <= m - 1; i++)
//	{
//		jj = i*l; b[jj] = y[(ii - 1)*m + i];  //¹Û²âÖµ
//		for (j = 0; j <= n - 1; j++)
//			b[jj] = b[jj] - h[i*n + j] * x[(ii - 1)*n + j];   //y=z-H*X(k/k-1)
//	}
//
//	//fout << Poslist[num][Posnum[num]].T << "	";
//	for (i = 0; i <= n - 1; i++)
//	{
//		jj = (ii - 1)*n + i;
//		for (j = 0; j <= m - 1; j++)
//			x[jj] = x[jj] + g[i*m + j] * b[j*l];//¸üÐÂÂË²¨ X(k/k)=X(k/k-1)+Kg*y
//		//fout << x[jj] << "	";
//	}
//	//fout << endl;
//
//	Poslist[num][Posnum[num]].PX = x[(ii - 1)*n + 0];
//	Poslist[num][Posnum[num]].PY = x[(ii - 1)*n + 1];
//	Poslist[num][Posnum[num]].PZ = x[(ii - 1)*n + 2];
//	Poslist[num][Posnum[num]].VX = x[(ii - 1)*n + 3];
//	Poslist[num][Posnum[num]].VY = x[(ii - 1)*n + 4];
//	Poslist[num][Posnum[num]].VZ = x[(ii - 1)*n + 5];
//	Poslist[num][Posnum[num]].AX = x[(ii - 1)*n + 6];
//	Poslist[num][Posnum[num]].AY = x[(ii - 1)*n + 7];
//	Poslist[num][Posnum[num]].AZ = x[(ii - 1)*n + 8];
//
//	for (i = 0; i <= n - 1; i++)
//	for (j = 0; j <= n - 1; j++)
//	{
//		jj = i*l + j; a[jj] = 0.0;
//		for (kk = 0; kk <= m - 1; kk++)
//			a[jj] = a[jj] - g[i*m + kk] * h[kk*n + j]; //¸üÐÂÐ­·½²î P(k/k)=(I-Kg*H)*P(k/k-1)
//		if (i == j) a[jj] = 1.0 + a[jj];
//	}
//	for (i = 0; i <= n - 1; i++)
//	for (j = 0; j <= n - 1; j++)
//	{
//		jj = i*l + j; b[jj] = 0.0;
//		for (kk = 0; kk <= n - 1; kk++)
//			b[jj] = b[jj] + a[i*l + kk] * p[kk*n + j];
//	}
//	for (i = 0; i <= n - 1; i++)
//	for (j = 0; j <= n - 1; j++)
//	{
//		jj = i*l + j; a[jj] = 0.0;
//		for (kk = 0; kk <= n - 1; kk++)
//			a[jj] = a[jj] + b[i*l + kk] * f[j*n + kk];//Ô¤²â¹À¼ÆÐ­·½²î P(k/k-1)
//	}
//	for (i = 0; i <= n - 1; i++)
//	for (j = 0; j <= n - 1; j++)
//	{
//		jj = i*n + j; p[jj] = q[jj];
//		for (kk = 0; kk <= n - 1; kk++)
//			p[jj] = p[jj] + f[i*n + kk] * a[j*l + kk];
//	}
//
//	return(js);
//}



int setkalman(int num)
{
	double *f, *q, *r, *h, *y, *x, *p, *g;
	double *e, *a, *b, *tmp;
	int i, j, kk, ii, l, jj, js, s, n = 9, m = 9;

	f = kalarr[num].f;
	q = kalarr[num].q;
	r = kalarr[num].r;
	h = kalarr[num].h;
	y = kalarr[num].y;
	x = kalarr[num].x;
	p = kalarr[num].p;
	g = kalarr[num].g;
	e = kalarr[num].e;
	a = kalarr[num].a;
	b = kalarr[num].b;
	tmp = kalarr[num].tmp;

	l = m;
	if (l < n) l = n;

	//R¹Û²â PÔ¤²â

	for (int i = 0; i < n*n; i++)
	{
		h[i] = 0.000;
		p[i] = 0.000;
		f[i] = 0.000;
		q[i] = 0.0000;
		r[i] = 0.0000;
	}




	//for (int i = 0; i < n * 3; i += (n + 1))
	//{
	//	h[i] = 1;
	//	p[i] = 0.1;
	//	q[i] = 0.00001;  //0.001
	//	r[i] = 0.057;   //0.01
	//}
	//for (int i = (3 * n + 3); i < n * 6; i += (n + 1))
	//{
	//	h[i] = 1;
	//	p[i] = 1;
	//	q[i] = 0.001;  //0.01
	//	r[i] = 1;     //1
	//}
	//for (int i = (6 * n + 6); i < n*n; i += (n + 1))
	//{
	//	h[i] = 1;
	//	p[i] = 5;
	//	q[i] = 1;    //1
	//	r[i] = 100;   //100
	//}



	for (int i = 0; i < n * 3; i += (n + 1))
	{
		h[i] = 1;
		p[i] = 0.1;
		q[i] = 0.01;  //0.001
		r[i] = 0.1;   //0.01
	}
	for (int i = (3 * n + 3); i < n * 6; i += (n + 1))
	{
		h[i] = 1;
		p[i] = 1;
		q[i] = 0.1;  //0.01
		r[i] = 1;     //1
	}
	for (int i = (6 * n + 6); i < n*n; i += (n + 1))
	{
		h[i] = 1;
		p[i] = 10;
		q[i] = 1;    //1
		r[i] = 100;   //100
	}


	for (int i = 0; i < 81; i++)
	{
		//q[i] = rr[i];
		//r[i] = rr[i];
	}


	for (int i = 0; i < n * 3; i += (n + 1))
	{
		//r[i + 3] = 0.001;
		//q[i + 3] = 0.001;
	}



	for (i = 0; i <= 1; i++)
	{
		x[i*n + 0] = Poslist[num][Posnum[num] - 1 + i].PX;
		x[i*n + 1] = Poslist[num][Posnum[num] - 1 + i].PY;
		x[i*n + 2] = Poslist[num][Posnum[num] - 1 + i].PZ;
		x[i*n + 3] = Poslist[num][Posnum[num] - 1 + i].VX;
		x[i*n + 4] = Poslist[num][Posnum[num] - 1 + i].VY;
		x[i*n + 5] = Poslist[num][Posnum[num] - 1 + i].VZ;
		x[i*n + 6] = Poslist[num][Posnum[num] - 1 + i].AX;
		x[i*n + 7] = Poslist[num][Posnum[num] - 1 + i].AY;
		x[i*n + 8] = Poslist[num][Posnum[num] - 1 + i].AZ;
	}


	//GuessPos(Poslist[num][Posnum[num] - 1], 3, tmp, (TT[ii - 1] - TT[0]) * 10000, (TT[ii - 2] - TT[0]) * 10000);

	//GuessPos(Poslist[num][Posnum[num] - 1].PX, Poslist[num][Posnum[num] - 1].PY, Poslist[num][Posnum[num] - 1].PZ, Poslist[num][Posnum[num] - 1].VX, Poslist[num][Posnum[num] - 1].VY, Poslist[num][Posnum[num] - 1].VZ, 3, tmp, int((Poslist[num][Posnum[num]].T - Poslist[num][1].T) * 10000), int((Poslist[num][Posnum[num] - 1].T - Poslist[num][1].T) * 10000), num);


	//ii = 2;
	//for (i = 0; i < 9; i++)
	//{
	//	x[(ii - 1)*n + i] = GPlist[num][int(Poslist[num][Posnum[num]].T * 10000)][i];
	//}

	ii = 2;

	for (i = 3; i < 9; i++)
	{
		//x[(ii)*n + i] = GPlist[num][int(Poslist[num][Posnum[num]].T * 10000)][i];
	}

	for (i = 0; i <3; i++)
	{
		jj = ii*n + i;
		//x[jj] = x[jj] + GPlist[TT[jj] - TT[jj - 2]][i]; //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
		//f[i*n + i] = x[jj*n + i] / x[(jj - 1)*n + i];
		f[i*n + i] = 1;
		//f[i*n + i + 3] = (x[jj] - x[jj - 2*n]) / x[jj - n + 3];
		f[i*n + i + 3] = Poslist[num][Posnum[num]].T - Poslist[num][Posnum[num] - 1].T;
		//f[i*n + i + 6] = (Poslist[num][Posnum[num]].T - Poslist[num][Posnum[num] - 1].T)*(Poslist[num][Posnum[num]].T - Poslist[num][Posnum[num] - 1].T)/2;
		//f[i*n + i] = x[jj] / x[jj - n];
	}
	for (i = 3; i <6; i++)
	{
		jj = ii*n + i;
		//x[jj] = x[jj] + GPlist[TT[jj] - TT[jj - 2]][i]; //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
		//f[i*n + i] = x[jj*n + i] / x[(jj - 1)*n + i];
		f[i*n + i] = 1;
		//f[i*n + i + 3] = (x[jj] - x[jj - n]) / x[jj - n + 3];
		//f[i*n + i] = x[jj] / x[jj - n];
		f[i*n + i + 3] = Poslist[num][Posnum[num]].T - Poslist[num][Posnum[num] - 1].T;
	}
	for (i = 6; i < n; i++)
	{
		jj = ii*n + i;
		//f[i*n + i - 3] = x[jj] / x[(jj - 1 * n) - 3];
		//f[i*n + i - 3] = x[jj*n + i] / x[(jj - 2)*n + i - 3];
		//f[i*n + i - 3] = 1;
		//f[i*n + i] = x[jj] / x[jj - n];
		f[i*n + i] = 0.9;
	}

	for (i = 0; i < 9; i++)
	{
		x[i] = x[i + 9];
	}

	for (i = 0; i <= n - 1; i++)
		for (j = 0; j <= n - 1; j++)
		{
			ii = i*l + j;
			a[ii] = 0.0;
			for (kk = 0; kk <= n - 1; kk++)
				a[ii] = a[ii] + p[i*n + kk] * f[j*n + kk];
		}
	for (i = 0; i <= n - 1; i++)
		for (j = 0; j <= n - 1; j++)
		{
			ii = i*n + j; p[ii] = q[ii];
			for (kk = 0; kk <= n - 1; kk++)
				p[ii] = p[ii] + f[i*n + kk] * a[kk*l + j];//P(k/k-1)=FP(k-1/k-1)F'+q
		}

	return 0;

}


int kalman(int num)
{

	double *f, *q, *r, *h, *y, *x, *p, *g;
	double *e, *a, *b, *tmp;
	int i, j, kk, ii, l, jj, js, s, n = 9, m = 9;

	f = kalarr[num].f;
	q = kalarr[num].q;
	r = kalarr[num].r;
	h = kalarr[num].h;
	y = kalarr[num].y;
	x = kalarr[num].x;
	p = kalarr[num].p;
	g = kalarr[num].g;
	e = kalarr[num].e;
	a = kalarr[num].a;
	b = kalarr[num].b;
	tmp = kalarr[num].tmp;

	l = m;
	if (l < n) l = n;

	//for (i = 0; i < 9; i++)
	//{
	//	x[i] = x[i + 9];
	//}

	//if (Posnum[num] == 4)
	//{

	//	for (i = 0; i <= 1; i++)
	//	{
	//		x[i*n + 0] = Poslist[num][Posnum[num] - 1 + i].PX;
	//		x[i*n + 1] = Poslist[num][Posnum[num] - 1 + i].PY;
	//		x[i*n + 2] = Poslist[num][Posnum[num] - 1 + i].PZ;
	//		x[i*n + 3] = Poslist[num][Posnum[num] - 1 + i].VX;
	//		x[i*n + 4] = Poslist[num][Posnum[num] - 1 + i].VY;
	//		x[i*n + 5] = Poslist[num][Posnum[num] - 1 + i].VZ;
	//		x[i*n + 6] = Poslist[num][Posnum[num] - 1 + i].AX;
	//		x[i*n + 7] = Poslist[num][Posnum[num] - 1 + i].AY;
	//		x[i*n + 8] = Poslist[num][Posnum[num] - 1 + i].AZ;
	//	}

	//}

	ii = 2;

	for (i = 0; i <= n - 1; i++)
		for (j = 0; j <= m - 1; j++)
		{
			jj = i*l + j; a[jj] = 0.0;
			for (kk = 0; kk <= n - 1; kk++)
				a[jj] = a[jj] + p[i*n + kk] * h[j*n + kk]; //a = P(k/k-1)H'
		}
	for (i = 0; i <= m - 1; i++)
		for (j = 0; j <= m - 1; j++)
		{
			jj = i*m + j; e[jj] = r[jj];
			for (kk = 0; kk <= n - 1; kk++)
				e[jj] = e[jj] + h[i*n + kk] * a[kk*l + j];//S=H*P*H'+R (a=p(k/k-1))
		}
	js = rinv(m, e);
	if (js == 0)
	{
		return(0);
	}
	for (i = 0; i <= n - 1; i++)
		for (j = 0; j <= m - 1; j++)
		{
			jj = i*m + j; g[jj] = 0.0;
			for (kk = 0; kk <= m - 1; kk++)
				g[jj] = g[jj] + a[i*l + kk] * e[j*m + kk];//Kg=a*S(-1)
		}

	//GuessV(x[(ii - 2)*n + 0], x[(ii - 2)*n + 1], x[(ii - 2)*n + 2], x[(ii - 2)*n + 3], x[(ii - 2)*n + 4], x[(ii - 2)*n + 5], 3, tmp, (TT[ii - 1] - TT[ii - 2]) * 10);
	//GuessPos(AX, 3, tmp, (TT[ii - 1] - TT[ii - 2]) * 10, (TT[ii - 2] - TT[0]) * 10);
	//cout << clock() << endl;

	for (i = 0; i < 9; i++)
	{
		x[(ii - 1)*n + i] = GPlist[num][int(Poslist[num][Posnum[num]].T * 10000)][i];
	}

	//	for (i = 6; i < 9; i++)
	//	{
	//x[(ii - 1)*n + i] = x[(ii - 1)*n + i - n*2];
	//	}
	//for (i = 0; i <= n - 1; i++)
	//{
	//	jj = (ii - 1)*n + i;
	//	x[jj] = 0.0;
	//	x[jj] = x[jj] + GPlist[(TT[ii] - TT[ii - 2])*10][i];  //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
	//	f[i*n + i] = x[jj] / x[jj - 2*n];
	//}

	for (i = 0; i < 3; i++)
	{
		jj = (ii - 1)*n + i;
		//x[jj] = x[jj] + GPlist[TT[jj] - TT[jj - 2]][i]; //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
		////f[i*n + i] = x[jj*n + i] / x[(jj - 1)*n + i];
		//f[i*n + i] = x[jj] / x[jj - n];
		f[i*n + i] = 1;
		f[i*n + i + 3] = (x[jj] - x[(jj - 1 * n)]) / x[(jj - 1 * n) + 3];
		//cout << f[i*n + i + 3] << endl;
		//f[i*n + i + 3] = Poslist[num][Posnum[num]].T - Poslist[num][Posnum[num] - 1].T;
		//f[i*n + i + 6] = (Poslist[num][Posnum[num]].T - Poslist[num][Posnum[num] - 1].T)*(Poslist[num][Posnum[num]].T - Poslist[num][Posnum[num] - 1].T) / 2;

	}

	for (i = 3; i < 6; i++)
	{
		jj = (ii - 1)*n + i;
		//x[jj] = x[jj] + GPlist[TT[jj] - TT[jj - 2]][i]; //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
		//f[i*n + i] = x[jj] / x[jj - n];
		f[i*n + i] = 1;
		//f[i*n + i + 3] = (x[jj] - x[(jj - 1 * n)]) / (x[(jj - 1 * n) + 3] + x[jj + 3]) * 2;
		f[i*n + i + 3] = (x[jj] - x[(jj - 1 * n)]) / x[(jj - 1 * n) + 3];
		//cout << f[i*n + i + 3] << endl;
		//f[i*n + i + 3] = Poslist[num][Posnum[num]].T - Poslist[num][Posnum[num] - 1].T;
		//f[i*n + i + 3] = (TT[ii - 1] - TT[ii - 2]);
	}

	for (i = 6; i < n; i++)
	{
		jj = (ii - 1)*n + i;
		//f[i*n + i - 3] = x[jj] / x[(jj - 1 * n) - 3];
		//cout << f[i*n + i - 3] << endl;
		f[i*n + i] = x[jj] / x[(jj - 1 * n)];
		//cout << f[i*n + i] << endl;
		//f[i*n + i] = 1;
		////f[i*n + i] = x[jj*n + i] / x[(jj - 1)*n + i];
	}

	//for (i = 0; i <= n - 1; i++)
	//{
	//	jj = (ii - 1)*n + i; x[jj] = 0.0;
	//	for (j = 0; j <= n - 1; j++)
	//		x[jj] = x[jj] + f[i*n + j] * x[(ii - 2)*n + j];   //Ô¤²â X(k/k-1)=F*X(k-1/k-1)
	//}

	//for (i = 0; i <= 2; i++)
	//{
	//	for (j = 0; j <= 2; j++)
	//	{
	//		AX[i][j] = x[(ii - 3)*n + i * n + j];
	//	}
	//}
	//AX[0][3] = TT[ii - 3];
	//AX[1][3] = TT[ii - 2];
	//AX[2][3] = TT[ii - 1];

	//for (j = 0; j <= 2; j++)
	//{
	//	AX[2][j] = y[(ii - 1)*n + j];
	//}

	//GuessPos(Poslist[num][Posnum[num] - 1], 3, tmp, int((Poslist[num][Posnum[num]].T - Poslist[num][1].T) * 10000), int((Poslist[num][Posnum[num] - 1].T - Poslist[num][1].T) * 10000), num);
	//GuessPos(x[(ii - 2)*n + 0], x[(ii - 2)*n + 1], x[(ii - 2)*n + 2], x[(ii - 2)*n + 3], x[(ii - 2)*n + 4], x[(ii - 2)*n + 5], 3, tmp, (TT[ii - 1] - TT[0]) * 10000, (TT[ii - 2] - TT[0]) * 10000);
	//GuessPos(x[(ii - 2)*n + 0], x[(ii - 2)*n + 1], x[(ii - 2)*n + 2], x[(ii - 2)*n + 3], x[(ii - 2)*n + 4], x[(ii - 2)*n + 5], 3, tmp, int((Poslist[num][Posnum[num]].T - Poslist[num][1].T) * 10000), int((Poslist[num][Posnum[num] - 1].T - Poslist[num][1].T) * 10000), num);




	{
		y[(ii - 1) * m + 0] = Poslist[num][Posnum[num]].PX;
		y[(ii - 1) * m + 1] = Poslist[num][Posnum[num]].PY;
		y[(ii - 1) * m + 2] = Poslist[num][Posnum[num]].PZ;
		y[(ii - 1) * m + 3] = Poslist[num][Posnum[num]].VX;
		y[(ii - 1) * m + 4] = Poslist[num][Posnum[num]].VY;
		y[(ii - 1) * m + 5] = Poslist[num][Posnum[num]].VZ;
		y[(ii - 1) * m + 6] = Poslist[num][Posnum[num]].AX;
		y[(ii - 1) * m + 7] = Poslist[num][Posnum[num]].AY;
		y[(ii - 1) * m + 8] = Poslist[num][Posnum[num]].AZ;
	}

	//for (i = 3; i <= m - 1; i++)
	//{
	//	y[(ii - 1) * m + i] = tmp[i];
	//}

	for (i = 0; i <= m - 1; i++)
	{
		jj = i*l; b[jj] = y[(ii - 1)*m + i];  //¹Û²âÖµ
		for (j = 0; j <= n - 1; j++)
			b[jj] = b[jj] - h[i*n + j] * x[(ii - 1)*n + j];   //y=z-H*X(k/k-1)
	}

	//fout << data.T << "	";
	for (i = 0; i <= n - 1; i++)
	{
		jj = (ii - 1)*n + i;
		for (j = 0; j <= m - 1; j++)
			x[jj] = x[jj] + g[i*m + j] * b[j*l];//¸üÐÂÂË²¨ X(k/k)=X(k/k-1)+Kg*y
		//fout << x[jj] << "	";
	}
	//fout << endl;

	//cout << endl;
	//cout << Poslist[num][Posnum[num]].PX << "	" << Poslist[num][Posnum[num]].PY << "	" << Poslist[num][Posnum[num]].PZ << endl;
	//cout << Poslist[num][Posnum[num]].VX << "	" << Poslist[num][Posnum[num]].VY << "	" << Poslist[num][Posnum[num]].VZ << endl;
	//cout << Poslist[num][Posnum[num]].AX << "	" << Poslist[num][Posnum[num]].AY << "	" << Poslist[num][Posnum[num]].AZ << endl;
	//cout << endl;
	//cout << x[(ii - 1)*n + 0] << "	" << x[(ii - 1)*n + 1] << "	" << x[(ii - 1)*n + 2] << endl;
	//cout << endl;

	if ((Poslist[num][Posnum[num]].PX - x[(ii - 1)*n + 0])*(Poslist[num][Posnum[num]].PX - x[(ii - 1)*n + 0]) +
		(Poslist[num][Posnum[num]].PY - x[(ii - 1)*n + 1])*(Poslist[num][Posnum[num]].PY - x[(ii - 1)*n + 1]) +
		(Poslist[num][Posnum[num]].PZ - x[(ii - 1)*n + 2])*(Poslist[num][Posnum[num]].PZ - x[(ii - 1)*n + 2]) > 0.05)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), C_WARNING | 0);
		cout << "Kalman failed!" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), C_NOMAL | 0);
		//cout << Poslist[num][Posnum[num]].PX << "	" << Poslist[num][Posnum[num]].PY << "	" << Poslist[num][Posnum[num]].PZ << endl;
		//cout << x[(ii - 1)*n + 0] << "	" << x[(ii - 1)*n + 1] << "	" << x[(ii - 1)*n + 2] << endl;


		for (int i = 0; i < n*n; i++)
		{
			h[i] = 0.000;
			p[i] = 0.000;
			f[i] = 0.000;
			q[i] = 0.0000;
			r[i] = 0.0000;
		}


		for (int i = 0; i < n * 3; i += (n + 1))
		{
			h[i] = 1;
			p[i] = 0.1;
			q[i] = 0.01;  //0.001
			r[i] = 0.1;   //0.01
		}
		for (int i = (3 * n + 3); i < n * 6; i += (n + 1))
		{
			h[i] = 1;
			p[i] = 1;
			q[i] = 0.1;  //0.01
			r[i] = 1;     //1
		}
		for (int i = (6 * n + 6); i < n*n; i += (n + 1))
		{
			h[i] = 1;
			p[i] = 10;
			q[i] = 1;    //1
			r[i] = 100;   //100
		}

		return 0;

	}
	else
	{

		Poslist[num][Posnum[num]].PX = x[(ii - 1)*n + 0];
		Poslist[num][Posnum[num]].PY = x[(ii - 1)*n + 1];
		Poslist[num][Posnum[num]].PZ = x[(ii - 1)*n + 2];
		Poslist[num][Posnum[num]].VX = x[(ii - 1)*n + 3];
		Poslist[num][Posnum[num]].VY = x[(ii - 1)*n + 4];
		Poslist[num][Posnum[num]].VZ = x[(ii - 1)*n + 5];
		Poslist[num][Posnum[num]].AX = x[(ii - 1)*n + 6];
		Poslist[num][Posnum[num]].AY = x[(ii - 1)*n + 7];
		Poslist[num][Posnum[num]].AZ = x[(ii - 1)*n + 8];
	}




	//{
	//	double dt;
	//	double lastPosnum = 2 * Poslist[num][Posnum[num] - 1].T - Poslist[num][Posnum[num]].T;
	//	if (lastPosnum <= Poslist[num][2].T)
	//	{
	//		dt = (Poslist[num][Posnum[num]].T - Poslist[num][Posnum[num] - 2].T);
	//		Poslist[num][Posnum[num] - 1].VX = (Poslist[num][Posnum[num]].PX - Poslist[num][Posnum[num] - 2].PX) / dt;
	//		Poslist[num][Posnum[num] - 1].VY = (Poslist[num][Posnum[num]].PY - Poslist[num][Posnum[num] - 2].PY) / dt;
	//		Poslist[num][Posnum[num] - 1].VZ = (Poslist[num][Posnum[num]].PZ - Poslist[num][Posnum[num] - 2].PZ) / dt;
	//	}
	//	else
	//	{
	//		dt = (Poslist[num][Posnum[num]].T - lastPosnum);
	//		Poslist[num][Posnum[num] - 1].VX = (Poslist[num][Posnum[num]].PX - GPlist[num][int(lastPosnum * 10000)][0]) / dt;
	//		Poslist[num][Posnum[num] - 1].VY = (Poslist[num][Posnum[num]].PY - GPlist[num][int(lastPosnum * 10000)][1]) / dt;
	//		Poslist[num][Posnum[num] - 1].VZ = (Poslist[num][Posnum[num]].PZ - GPlist[num][int(lastPosnum * 10000)][2]) / dt;
	//	}
	//	Poslist[num][Posnum[num] - 1].V = sqrt(Poslist[num][Posnum[num] - 1].VX*Poslist[num][Posnum[num] - 1].VX + Poslist[num][Posnum[num] - 1].VY*Poslist[num][Posnum[num] - 1].VY + Poslist[num][Posnum[num] - 1].VZ*Poslist[num][Posnum[num] - 1].VZ);
	//	cout << "VX=" << Poslist[num][Posnum[num]].VX << "  VY=" << Poslist[num][Posnum[num]].VY << "  VZ=" << Poslist[num][Posnum[num]].VZ << "  V=" << Poslist[num][Posnum[num]].V << endl;
	//}

	//{
	//	double f[3];
	//	Ff(Poslist[num][Posnum[num] - 1].VX, Poslist[num][Posnum[num] - 1].VY, Poslist[num][Posnum[num] - 1].VZ, f);
	//	Poslist[num][Posnum[num] - 1].AX = f[0]; // (Poslist[i][Posnum[i]].VX - Poslist[i][Posnum[i] - 1].VX) / dt;
	//	Poslist[num][Posnum[num] - 1].AY = f[1]; // (Poslist[i][Posnum[i]].VY - Poslist[i][Posnum[i] - 1].VY) / dt;
	//	Poslist[num][Posnum[num] - 1].AZ = f[2]; // (Poslist[i][Posnum[i]].VZ - Poslist[i][Posnum[i] - 1].VZ) / dt;
	//	Poslist[num][Posnum[num] - 1].AC = sqrt(Poslist[num][Posnum[num] - 1].AX*Poslist[num][Posnum[num] - 1].AX + Poslist[num][Posnum[num] - 1].AY*Poslist[num][Posnum[num] - 1].AY + Poslist[num][Posnum[num] - 1].AZ*Poslist[num][Posnum[num] - 1].AZ);
	//	//Poslist[i][Posnum[i]].AC = (Poslist[i][Posnum[i]].V - Poslist[i][Posnum[i] - 1].V) / dt;
	//}

	//ans.PX = x[(ii - 1)*n + 0];
	//ans.PY = x[(ii - 1)*n + 1];
	//ans.PZ = x[(ii - 1)*n + 2];
	//ans.VX = x[(ii - 1)*n + 3];
	//ans.VY = x[(ii - 1)*n + 4];
	//ans.VZ = x[(ii - 1)*n + 5];
	//ans.AX = x[(ii - 1)*n + 6];
	//ans.AY = x[(ii - 1)*n + 7];
	//ans.AZ = x[(ii - 1)*n + 8];
	//ans.T = data.T;

	for (i = 0; i < 9; i++)
	{
		x[i] = x[i + 9];
	}
	


	//for (i = 0; i < 9; i++)
	//{
	//	for (j = 0; j < 9; j++)
	//	{
	//	//	PrintP[i][j] = p[9 * i + j];
	//	}
	//}

	//canprint = 1;



	for (i = 0; i <= n - 1; i++)
	for (j = 0; j <= n - 1; j++)
	{
		jj = i*l + j; a[jj] = 0.0;
		for (kk = 0; kk <= m - 1; kk++)
			a[jj] = a[jj] - g[i*m + kk] * h[kk*n + j]; //¸üÐÂÐ­·½²î P(k/k)=(I-Kg*H)*P(k/k-1)
		if (i == j) a[jj] = 1.0 + a[jj];
	}
	for (i = 0; i <= n - 1; i++)
	for (j = 0; j <= n - 1; j++)
	{
		jj = i*l + j; b[jj] = 0.0;
		for (kk = 0; kk <= n - 1; kk++)
			b[jj] = b[jj] + a[i*l + kk] * p[kk*n + j];
	}
	for (i = 0; i <= n - 1; i++)
	for (j = 0; j <= n - 1; j++)
	{
		jj = i*l + j; a[jj] = 0.0;
		for (kk = 0; kk <= n - 1; kk++)
			a[jj] = a[jj] + b[i*l + kk] * f[j*n + kk];//Ô¤²â¹À¼ÆÐ­·½²î P(k/k-1)
	}
	for (i = 0; i <= n - 1; i++)
	for (j = 0; j <= n - 1; j++)
	{
		jj = i*n + j; p[jj] = q[jj];
		for (kk = 0; kk <= n - 1; kk++)
			p[jj] = p[jj] + f[i*n + kk] * a[j*l + kk];
	}

	return(0);
}




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