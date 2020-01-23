#include "stdafx.h"

/*!
*	\brief	���� ����Ʈ���� �ִ������� ���Ѵ�.
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
*	\brief Gift Wrap�˰����� �̿��Ͽ� 2���� convex hull�� ���Ѵ�.
*
*	\param InPts �Է� ����
*	\param OutPts convex hull�� �������� �ݽð� �������� �����
*/
static GPoint3 wrap(const std::vector<GPoint3> &InPts, const GPoint3 &PrevPt, const GPoint3 &CurrPt);
static bool left_turn(const GPoint3 &p, const GPoint3 &q, const GPoint3 &r);
void find_convexhull_2d(const std::vector<GPoint3> &InPts, std::vector<GPoint3> &OutPts)
{
	if (InPts.empty())
		return;

	// �������� ���Ѵ�
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
*	\brief Convex hull�� �����ϴ� ���� ������ ã�´�.
*
*	\param InPts �Է� ����
*	\param PrevPt ���� ����
*	\param CurrPt ���� ����
*
*	\return Gift Wrap �˰��򿡼� convex hull�� �����ϴ� ���� ������ ��ȯ�Ѵ�.
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
*	\brief ���� r�� ���� pq�� ���� �ִ����� �˻��Ѵ�.
*
*	\param p ������ ������
*	\param q ������ ����
*	\param r �Է� ����
*
*	\return ���� r�� ���� pq�� ���� ������ ���(true), �׷��� ������(false)
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
*	\brief �־��� ������ �ٰ��� ���ο� ���ԵǴ��� ���θ� �Ǵ�
*	\warning �׽�Ʈ �ʿ�
*
*	\param Pts �ٰ����� �̷�� ������ ����Ʈ(�ݽð� ����)
*	\param p ���� ���θ� ������ ����
*
*	\return �ٰ��� ���ο� ���Ե� ��� true, �׷��� ������ false�� ��ȯ
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
*	\brief �־��� ������ �ﰢ�� ���ο� ���ԵǴ��� ���θ� �Ǵ�
*
*	\param p ��� ����
*	\param p1, p2, p2 �ﰢ�� ������ ��ġ
*
*	\return �ﰢ�� ���ο� ���Ե� ��� true, �׷��� ������ false�� ��ȯ
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
