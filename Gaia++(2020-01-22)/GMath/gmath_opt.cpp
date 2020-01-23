#include "stdafx.h"

#define NRANSI
#define ITMAX 20

// Function used in Numerical Recipe in C.
#define NR_END 1
#define FREE_ARG char*


// Function used in Numerical Recipe in C.
static float maxarg1, maxarg2;
#define FMAX(a,b)  (maxarg1 = (a), maxarg2 = (b), (maxarg1) >  (maxarg2) ?  (maxarg1) :  (maxarg2))

static float *alloc_vector(long nl, long nh)
/* allocate a float std::vector with subscript range v[nl..nh] */
{
	float *v;
	v = (float *)malloc((size_t)((nh - nl + 1 + NR_END) * sizeof(float)));
	//if (!v) nrerror("allocation failure in std::vector()");
	return v - nl + NR_END;
}

static void free_vector(float *v, long nl, long nh)
/* free a float std::vector allocated with std::vector() */
{
	free((FREE_ARG)(v + nl - NR_END));
}

static void nrerror(char error_text[])
/* Numerical Recipes standard error handler */
{
	fprintf(stderr, "Numerical Recipes run-time error...\n");
	fprintf(stderr, "%s\n",error_text);
	fprintf(stderr, "...now exiting to system...\n");
	exit(1);
}

bool powell(float p[], float **xi, int n, float ftol, int *iter, float *fret, float (*func)(float []))
{
	int i, ibig, j;
	float del, fp, fptt, t, *pt, *ptt, *xit;

	pt = alloc_vector(1, n);
	ptt = alloc_vector(1, n);
	xit = alloc_vector(1, n);
	*fret = (*func)(p);
	for (j = 1; j <= n; j++) 
		pt[j] = p[j];

	for (*iter = 1; ;++(*iter)) 
	{
		fp = (*fret);
		ibig = 0;
		del = 0.0f;
		for (i = 1;i <= n; i++) 
		{
			for (j = 1; j <= n; j++) 
				xit[j] = xi[j][i];

			fptt = (*fret);
			linmin(p, xit, n, fret, func);
			if (fabs(fptt - (*fret)) > del) 
			{
				del = (float)fabs(fptt - (*fret));
				ibig = i;
			}
		}
		if (2.0f * fabs(fp - (*fret)) <= ftol * (fabs(fp) + fabs(*fret))) 
		{
			free_vector(xit, 1, n);
			free_vector(ptt, 1, n);
			free_vector(pt, 1, n);
			return true;
		}
		if (*iter == ITMAX)
		{
			return false;
			//nrerror("powell exceeding maximum iterations.");
		}
		for (j = 1; j <= n; j++) 
		{
			ptt[j] = 2.0f * p[j] - pt[j];
			xit[j] = p[j] - pt[j];
			pt[j] = p[j];
		}
		fptt = (*func)(ptt);
		if (fptt < fp) 
		{
			t = 2.0f * (fp - 2.0f * (*fret) + fptt) * SQR(fp - (*fret) - del) - del * SQR(fp - fptt);
			if (t < 0.0f) 
			{
				linmin(p, xit, n, fret, func);
				for (j = 1; j <= n; j++) 
				{
					xi[j][ibig] = xi[j][n];
					xi[j][n] = xit[j];
				}
			}
		}
	}
	return true;
}
#undef ITMAX


#define TOL 2.0e-4
int ncom;
float *pcom,*xicom,(*nrfunc)(float []);
void linmin(float p[], float xi[], int n, float *fret, float (*func)(float []))
{
	int j;
	float xx, xmin, fx, fb, fa, bx, ax;

	ncom = n;
	pcom = alloc_vector(1, n);
	xicom = alloc_vector(1, n);
	nrfunc = func;
	for (j = 1; j <= n; j++) 
	{
		pcom[j] = p[j];
		xicom[j] = xi[j];
	}
	ax = 0.0f;
	xx = 1.0f;
	mnbrak(&ax, &xx, &bx, &fa, &fx, &fb, f1dim);
	*fret = brent(ax, xx, bx, f1dim, (float)TOL, &xmin);
	for (j = 1; j <= n; j++) 
	{
		xi[j] *= xmin;
		p[j] += xi[j];
	}
	free_vector(xicom, 1, n);
	free_vector(pcom, 1, n);
}
#undef TOL


#define ITMAX 100
#define CGOLD 0.3819660f
#define ZEPS 1.0e-10
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);
float brent(float ax, float bx, float cx, float (*f)(float), float tol,	float *xmin)
{
	int iter;
	float a, b, d, etemp, fu, fv, fw, fx, p, q, r, tol1, tol2, u, v, w, x, xm;
	float e = 0.0;

	a = (ax < cx ? ax : cx);
	b = (ax > cx ? ax : cx);
	x = w = v = bx;
	fw = fv = fx = (*f)(x);
	for (iter = 1; iter <= ITMAX; iter++) 
	{
		xm = 0.5f * (a + b);
		tol2 = 2.0f * (tol1 = tol * (float)fabs(x) + (float)ZEPS);
		if (fabs(x - xm) <= (tol2 - 0.5 * (b - a))) 
		{
			*xmin = x;
			return fx;
		}
		if (fabs(e) > tol1) 
		{
			r = (x - w) * (fx - fv);
			q = (x - v) * (fx - fw);
			p = (x - v) * q - (x - w) * r;
			q = 2.0f * (q - r);
			if (q > 0.0) p = -p;
			q = (float)fabs(q);
			etemp = e;
			e = d;
			if (fabs(p) >= fabs(0.5 * q * etemp) || p <= q * (a - x) || p >= q * (b - x))
				d = CGOLD * (e = (x >= xm ? a-x : b-x));
			else {
				d = p / q;
				u = x + d;
				if (u - a < tol2 || b - u < tol2)
					d = SIGN1(tol1, xm-x);
			}
		} 
		else 
		{
			d = CGOLD * (e = (x >= xm ? a-x : b-x));
		}
		u = (fabs(d) >= tol1 ? x+d : x + SIGN1(tol1, d));
		fu = (*f)(u);
		if (fu <= fx) 
		{
			if (u >= x) a = x; else b = x;
			SHFT(v, w, x, u)
			SHFT(fv, fw, fx, fu)
		} 
		else 
		{
			if (u < x) a = u; else b = u;
			if (fu <= fw || w == x) 
			{
				v = w;
				w = u;
				fv = fw;
				fw = fu;
			} 
			else if (fu <= fv || v == x || v == w) 
			{
				v = u;
				fv = fu;
			}
		}
	}
	//nrerror("Too many iterations in brent");
	*xmin = x;
	return fx;
}
#undef ITMAX
#undef CGOLD
#undef ZEPS
#undef SHFT

extern int ncom;
extern float *pcom,*xicom,(*nrfunc)(float []);
float f1dim(float x)
{
	int j;
	float f, *xt;

	xt = alloc_vector(1, ncom);
	for (j = 1; j <= ncom; j++) xt[j] = pcom[j] + x * xicom[j];
	f = (*nrfunc)(xt);
	free_vector(xt, 1, ncom);
	return f;
}


#define GOLD 1.618034f
#define GLIMIT 100.0f
#define TINY 1.0e-20
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);
void mnbrak(float *ax, float *bx, float *cx, float *fa, float *fb, float *fc, float (*func)(float))
{
	float ulim, u, r, q, fu, dum;

	*fa = (*func)(*ax);
	*fb = (*func)(*bx);
	if (*fb > *fa) 
	{
		SHFT(dum, *ax, *bx, dum)
		SHFT(dum, *fb, *fa, dum)
	}
	*cx = (*bx) + GOLD * (*bx - *ax);
	*fc = (*func)(*cx);
	while (*fb > *fc) 
	{
		r = (*bx - *ax) * (*fb - *fc);
		q = (*bx - *cx) * (*fb - *fa);
		u = (*bx) - ((*bx - *cx) * q - (*bx - *ax) * r) /
			(2.0f * (float)SIGN1(FMAX((float)fabs(q - r), (float)TINY), q - r));
		ulim = (*bx) + GLIMIT * (*cx - *bx);
		if ((*bx -  u) * (u - *cx) > 0.0) 
		{
			fu = (*func)(u);
			if (fu < *fc) {
				*ax = (*bx);
				*bx = u;
				*fa = (*fb);
				*fb = fu;
				return;
			} 
			else if (fu > *fb) 
			{
				*cx = u;
				*fc = fu;
				return;
			}
			u = (*cx) + GOLD * (*cx - *bx);
			fu = (*func)(u);
		} else if ((*cx - u) * (u - ulim) > 0.0) 
		{
			fu = (*func)(u);
			if (fu < *fc) 
			{
				SHFT(*bx, *cx, u, *cx + GOLD * (*cx - *bx))
				SHFT(*fb, *fc, fu, (*func)(u))
			}
		} 
		else if ((u - ulim) * (ulim - *cx) >= 0.0) 
		{
			u = ulim;
			fu = (*func)(u);
		} 
		else 
		{
			u = (*cx) + GOLD * (*cx - *bx);
			fu = (*func)(u);
		}
		SHFT(*ax, *bx, *cx, u)
		SHFT(*fa, *fb, *fc, fu)
	}
}
#undef GOLD
#undef GLIMIT
#undef TINY
#undef SHFT

#undef NRANSI


