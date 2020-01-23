#include "stdafx.h"

/*!
*	\brief	정수 리스트에서 최대공약수를 구한다.
*/
int get_gcd(std::vector<int> &IntegerList)
{
	if (IntegerList.empty())
		return 0;

	int min = IntegerList[0];
	int Num = (int)IntegerList.size();
	for (int i = 1; i < Num; ++i)
		min = MIN(min, IntegerList[i]);

	int gcd = 1;
	for (int i = 1; i < min; ++i)
	{
		bool common = true;
		for (int j = 0; j < Num; ++j)
		{
			if (IntegerList[j] % i)
				common = false;
		}

		if (common)
			gcd = i;
	}

	return gcd;
}

/*!
*	\brief Gift Wrap알고리즘을 이용하여 2차원 convex hull을 구한다.
*
*	\param InPts 입력 정점
*	\param OutPts convex hull의 정점들이 반시계 방향으로 저장됨
*/
static GPoint3 wrap(const std::vector<GPoint3> &InPts, const GPoint3 &PrevPt, const GPoint3 &CurrPt);
static bool left_turn(const GPoint3 &p, const GPoint3 &q, const GPoint3 &r);
void find_convexhull_2d(const std::vector<GPoint3> &InPts, std::vector<GPoint3> &OutPts)
{
	if (InPts.empty())
		return;

	// 시작점을 구한다
	GPoint3 StartPt = InPts[0];
	int NumPt = (int)InPts.size();
	for (int i = 1; i <NumPt; ++i)
	{
		if (InPts[i][1] < StartPt[1])
			StartPt = InPts[i];
	}
	OutPts.push_back(StartPt);

	GPoint3 PrevPt, CurrPt, NextPt;
	PrevPt = CurrPt = StartPt;
	while (true)
	{
		NextPt = wrap(InPts, PrevPt, CurrPt);
		if (NextPt == StartPt)
			break;

		OutPts.push_back(NextPt);
		PrevPt = CurrPt;
		CurrPt = NextPt;
	}
}

/*!
*	\brief Convex hull을 구성하는 다음 정점을 찾는다.
*
*	\param InPts 입력 정점
*	\param PrevPt 이전 정점
*	\param CurrPt 현재 정점
*
*	\return Gift Wrap 알고리즘에서 convex hull을 구성하는 다음 정점을 반환한다.
*/
GPoint3 wrap(const std::vector<GPoint3> &InPts, const GPoint3 &PrevPt, const GPoint3 &CurrPt)
{
	GPoint3 p, q, r;
	p = PrevPt;
	q = CurrPt;

	int NumPt = (int)InPts.size();
	for (int i = 0; i < NumPt; ++i)
	{
		r = InPts[i];
		if (r != PrevPt && r != CurrPt)
		{
			if (left_turn(p, q, r))
				p = r;
		}
	}
	return p;
}

/*!
*	\brief 정점 r이 선분 pq의 왼편에 있는지를 검사한다.
*
*	\param p 선분의 시작점
*	\param q 선분의 끝점
*	\param r 입력 정점
*
*	\return 정점 r이 선분 pq의 왼편에 존재할 경우(true), 그렇지 않으면(false)
*/
bool left_turn(const GPoint3 &p, const GPoint3 &q, const GPoint3 &r)
{
	double det;
	det = (q[0] - p[0]) * (r[1] - p[1]) - (q[1] - p[1]) * (r[0] - p[0]);

	if (det > 0)
		return true;
	else if (det < 0)
		return false;
	else // co-linear case.
	{
		double qr = dist(q, r);
		double qp = dist(q, p);
		if (qr >= qp)
			return true;
		else // pr < pq
			return false;
	}
}

/*!
*	\brief 주어진 정점이 다각형 내부에 포함되는지 여부를 판단
*	\warning 테스트 필요
*
*	\param Pts 다각형을 이루는 정점의 리스트(반시계 방향)
*	\param p 포함 여부를 조사할 정점
*
*	\return 다각형 내부에 포함될 경우 true, 그렇지 않으면 false를 반환
*/
bool pt_in_polygon(const std::vector<GPoint3> &Pts, GPoint3 const &p)
{
	int pnum = (int)Pts.size();
	
	bool ret = false;
	int j = pnum - 1;
	for (int i = 0; i < pnum; ++i) 
	{
		if (Pts[i][1] < p[1] && Pts[j][1] >= p[1] || Pts[j][1] < p[1] && Pts[i][1] >= p[1]) 
		{
			if (Pts[i][0] + (p[1] - Pts[i][1]) / (Pts[j][1] - Pts[i][1]) * (Pts[j][0] - Pts[i][0]) < p[0]) 
			{
				ret = !ret;
			}
		}
		j = i; 
	}

	return ret;
}

/*!
*	\brief 주어진 정점이 삼각형 내부에 포함되는지 여부를 판단
*
*	\param p 대상 정점
*	\param p1, p2, p2 삼각형 정점의 위치
*
*	\return 삼각형 내부에 포함될 경우 true, 그렇지 않으면 false를 반환
*/
bool pt_in_triangle(GVector2 p, GVector2 p1, GVector2 p2, GVector2 p3)
{
	double a, b, c, d, e, f;
	GVector2 v1 = p2 - p1;
	GVector2 v2 = p3 - p1;
	GVector2 v3 = p - p1;
	
	a = v1[0];
	c = v1[1];

	b = v2[0];
	d = v2[1];

	e = v3[0];
	f = v3[1];

	double alpha, beta;
	alpha = (d * e - b * f) / (a * d - b * c);
	beta = (a * f - c * e) / (a * d - b * c);

	if (alpha >= 0 && beta >= 0 && (alpha + beta) <= 1.0)
		return true;
	
	return false;
}
