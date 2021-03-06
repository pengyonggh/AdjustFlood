// Graphic.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <iterator>
#include <sstream>
#include <math.h>
#include <fstream>

#define ACCURACY 0.00001

using namespace std;

typedef vector<double>::iterator VecDouIt;

int flag = 1; //0:line    !0:oblique
double startZ, dt = 3 * 3600, DesignAccStartZ, CheckAccStartZ, q;

//D -> Design, C -> Check, R - Result, T -> Trail
VecDouIt ItDqBeg, ItDqEnd, ItCqBeg, ItCqEnd, ItVBeg, ItVEnd, ItDQBeg, ItDQEnd, ItCQBeg, ItCQEnd,
ItZBeg,ItZEnd, ItDesignAuxBeg, ItDesignAuxEnd, ItCheckAuxBeg,ItCheckAuxEnd,
Itq, ItV, ItQ, ItZ, ItAux,
ItDRTqBeg, ItDRTqEnd,
ItDRGqBeg, ItDRGqEnd,
ItCRTqBeg, ItCRTqEnd,
ItCRGqBeg, ItCRGqEnd;

vector<double> VecDesignq, VecCheckq, VecV, VecZ,
VecDesignQ, VecCheckQ, VecDesignAux, VecCheckAux,
VecDesignResTrialq, VecCheckResTrialq,
VecDesignResGraq, VecCheckResGraq,
VecDesignResTrialZ, VecCheckResTrialZ,
VecDesignResGraZ, VecCheckResGraZ,
VecDesignResGraAux, VecCheckResGraAux,
VecDesignAccQ, VecCheckAccQ,
VecDesignResAccq, VecCheckResAccq,
VecMaxq;


bool init();
void body();
void getAux();
void getMaxZ();
void accurate();
bool getRes();

bool Graphic(double, int);
double Discharge(double, int);
int getDataType(const string&);
bool TrialMethod(double, int, int);
double printRes(vector<double>&, string);
void getZ(vector<double>&, vector<double>&, double, int);
double WaterBalance(double, double, double, VecDouIt, int);
double LinearInterpolation(double d, VecDouIt it1Beg, VecDouIt it1End,
	VecDouIt it2Beg, VecDouIt it2Edn);

template <class Type>
Type stringToNum(const string&);
vector<string> split(const string&, const string&);


int main()
{
	body();
	getchar();
	return 0;
}

void body()
{
	int flag;

	init();

	//printf("%.4f\n", LinearInterpolation(1529.7628, VecCheckq.begin(), VecCheckq.end(), ItZBeg, ItZEnd));
	cout << "------------------------------" << endl;
	cout << "输入起调水位" << endl;
	cin >> startZ;
	double d = LinearInterpolation(startZ, ItZBeg, ItZEnd, ItDqBeg, ItDqEnd);
	printf("%.4f\n", d);
	getchar();
	TrialMethod(startZ, 0, 0);
	Graphic(startZ, 0);
	TrialMethod(startZ, 1, 0);
	Graphic(startZ, 1);

	getZ(VecDesignResTrialq, VecDesignResTrialZ, startZ, 0);
	getZ(VecCheckResTrialq, VecCheckResTrialZ, startZ, 1);
	getZ(VecDesignResGraq, VecDesignResGraZ, startZ, 0);
	getZ(VecCheckResGraq, VecCheckResGraZ, startZ, 1);

	accurate();

	getAux();

	getRes();

	getMaxZ();

	
	//cout << "------------------------------------" << endl;

	//cout << "试算法-输入设计洪水最大下泄流量: ";
	//cin >> startZ;
	//cout << "对应最大水位: " << LinearInterpolation(startZ, ItCQBeg, ItDQEnd, ItZBeg, ItZEnd);
	//cout << "半图解法-输入设计洪水最大下泄流量: ";
	//cin >> startZ;
	//cout << "对应最大水位: " << LinearInterpolation(startZ, ItCQBeg, ItDQEnd, ItZBeg, ItZEnd);
	//cout << "试算法-输入校核洪水最大下泄流量: ";
	//cin >> startZ;
	//cout << "对应最大水位: " << LinearInterpolation(startZ, ItCQBeg, ItDQEnd, ItZBeg, ItZEnd);
	//cout << "半图解法输入校核洪水最大下泄流量: ";
	//cin >> startZ;
	//cout << "对应最大水位: " << LinearInterpolation(startZ, ItCQBeg, ItDQEnd, ItZBeg, ItZEnd);

	getchar();
}

void getMaxZ()
{
	//Design Accq, Check Accq, Design Graq, Check Graq
	double d;
	VecDouIt it = VecMaxq.begin();
	d = LinearInterpolation(*it, ItDqBeg, ItDqEnd, ItZBeg, ItZEnd);
	printf("Design Accq: %.4f, and about Z: %.4f. \n", *(it++), d);

	d = LinearInterpolation(*it, ItCqBeg, ItCqEnd, ItZBeg, ItZEnd);
	printf("Check Accq: %.4f, and about Z: %.4f. \n", *(it++), d);

	d = LinearInterpolation(*it, ItDqBeg, ItDqEnd, ItZBeg, ItZEnd);
	printf("Design Graq: %.4f, and about Z: %.4f. \n", *(it++), d);

	d = LinearInterpolation(*it, ItCqBeg, ItCqEnd, ItZBeg, ItZEnd);
	printf("Check Graq: %.4f, and about Z: %.4f. \n", *(it++), d);
}

void getZ(vector<double>& v1, vector<double>& v2, double startZ, int i)
{
	double d;
	VecDouIt ItqBeg = ItDqBeg, ItqEnd = ItDqEnd, Itq = v1.begin();
	if (i == 1)
	{
		ItqBeg = ItCqBeg;
		ItqEnd = ItCqEnd;
	}

	for (; Itq != v1.end(); ++Itq)
	{
		d = LinearInterpolation(*Itq, ItqBeg, ItqEnd, ItZBeg, ItZEnd);
		if (d < startZ)
		{
			d = startZ;
		}
		v2.push_back(d);
	}
}

void getAux()
{
	double d, minaux;
	minaux = LinearInterpolation(startZ, ItZBeg, ItZEnd, ItDesignAuxBeg, ItDesignAuxEnd);
	VecDouIt it = VecDesignResGraq.begin();
	for (; it != VecDesignResGraq.end(); ++it)
	{
		if ((d = LinearInterpolation(*it, ItDqBeg, ItDqEnd, ItDesignAuxBeg, ItCheckAuxEnd)) < minaux)
		{
			VecDesignResGraAux.push_back(0);
		}
		else
		{
			VecDesignResGraAux.push_back(d);
		}
	}

	minaux = LinearInterpolation(startZ, ItZBeg, ItZEnd, ItCheckAuxBeg, ItCheckAuxEnd);
	it = VecCheckResGraq.begin();
	for (; it != VecCheckResGraq.end(); ++it)
	{
		if ((d = LinearInterpolation(*it, ItCqBeg, ItCqEnd, ItCheckAuxBeg, ItCheckAuxEnd)) < minaux)
		{
			VecCheckResGraAux.push_back(0);
		}
		else
		{
			VecCheckResGraAux.push_back(d);
		}
	}
}

bool getRes()
{
	printRes(VecDesignResTrialq,string("Design Trialq"));
	printRes(VecCheckResTrialq, string("Check Trialq"));

	VecMaxq.push_back(printRes(VecDesignResAccq, string("Design Accq")));
	VecMaxq.push_back(printRes(VecCheckResAccq, string("Check Accq")));
	VecMaxq.push_back(printRes(VecDesignResGraq, string("Dedign Graq")));
	VecMaxq.push_back(printRes(VecCheckResGraq, string("Check Graq")));

	printRes(VecDesignResGraAux, string("Design Aux"));
	printRes(VecCheckResGraAux, string("Check Aux"));

	printRes(VecDesignResTrialZ, string("Design TrialZ"));
	printRes(VecCheckResTrialZ, string("Check TrialZ"));
	printRes(VecDesignResGraZ, string("Design GraZ"));
	printRes(VecCheckResGraZ, string("Check GraZ"));

	return true;
}

void accurate()
{
	dt = 0.5 * 3600;
	int Dh = -2, Ch = -2;
	double Dqmax = 0, Cqmax = 0, startZ;
	VecDouIt Itq, ItDQ, ItCQ;
	ItDQ = VecDesignQ.begin();
	Itq = VecDesignResTrialq.begin();
	while ((*Itq) > Dqmax && (Itq != VecDesignResTrialq.end()))
	{
		Dqmax = *Itq;
		Dh++;
		ItDQ++;
		Itq++;
	}
	startZ = LinearInterpolation(*(Itq - 2), ItDqBeg, ItDqEnd, ItZBeg, ItZEnd);

	double d1 = *(ItDQ - 1) - *(ItDQ - 2), d2 = *(ItDQ) - *(ItDQ - 1);
	for (int i = 0; i < 13; ++i)
	{
		if (i < 7)
		{
			VecDesignAccQ.push_back(*(ItDQ - 2) + i * d1 / 6);
		}
		else
		{
			VecDesignAccQ.push_back(*(ItDQ - 1) + (i - 6)*d2 / 6);
		}
	}

	ItDQBeg = VecDesignAccQ.begin();
	ItDQEnd = VecDesignAccQ.end();

	cout << "The begin of Design Acc: " << 3 * Dh << "h" << endl;
	TrialMethod(startZ, 0, 1);


	ItCQ = VecCheckQ.begin();
	Itq = VecCheckResTrialq.begin();
	while ((*Itq > Cqmax) && (Itq != VecCheckResTrialq.end()))
	{
		Cqmax = *Itq;
		Ch++;
		ItCQ++;
		Itq++;
	}
	startZ = LinearInterpolation(*(Itq - 2), ItCqBeg, ItCqEnd, ItZBeg, ItZEnd);
	d1 = *(ItCQ - 1) - *(ItCQ - 2);
	d2 = *(ItCQ)-*(ItCQ - 1);
	for (int i = 0; i < 13; ++i)
	{
		if (i < 7)
		{
			VecCheckAccQ.push_back(*(ItCQ - 2) + i * d1 / 6);
		}
		else
		{
			VecCheckAccQ.push_back(*(ItCQ - 1) + (i - 6)*d2 / 6);
		}
	}

	ItCQBeg = VecCheckAccQ.begin();
	ItCQEnd = VecCheckAccQ.end();

	cout << "The begin of Check Acc: " << 3 * Ch << "h" << endl;
	TrialMethod(startZ, 1, 1);
}


double printRes(vector<double>& v, string s)
{
	double d = 0.0f;
	cout <<"size of "<< s <<": "<< v.size() << endl;
	VecDouIt Beg = v.begin();
	while (Beg != v.end())
	{
		if (*Beg > d)
		{
			d = *Beg;
		}
		printf("%.4f ", *Beg);
		Beg++;
	}
	cout << endl;
	return d;
}

bool init()
{
	vector<string> vs;
	string str;
	double d;
	ifstream ifstr_data("data.txt");

	while (true)
	{
		if (!ifstr_data.eof())
		{
			ifstr_data.clear();
			ifstr_data >> str;
			switch (getDataType(str))
			{
			case 0:
				while (ifstr_data >> d)
				{
					VecDesignq.push_back(d);
				}
				break;
			case 1:
				while (ifstr_data >> d)
				{
					VecCheckq.push_back(d);
				}
				break;
			case 2:
				while (ifstr_data >> d)
				{
					VecDesignAux.push_back(d);
				}
				break;
			case 3:
				while (ifstr_data >> d)
				{
					VecDesignQ.push_back(d);
				}
				break;
			case 4:
				while (ifstr_data >> d)
				{
					VecCheckQ.push_back(d);
				}
				break;
			case 5:
				while (ifstr_data >> d)
				{
					VecV.push_back(d);
				}
				break;
			case 6:
				while (ifstr_data >> d)
				{
					VecZ.push_back(d);
				}
				break;
			case 7:
				while (ifstr_data >> d)
				{
					VecCheckAux.push_back(d);
				}
				break;
			default:
				break;
			}
		}
		else
		{
			break;
		}
	}


	ItDqBeg = VecDesignq.begin(), ItDqEnd = VecDesignq.end(),
		ItCqBeg = VecCheckq.begin(), ItCqEnd = VecCheckq.end(),
		ItVBeg = VecV.begin(), ItVEnd = VecV.end(),
		ItDQBeg = VecDesignQ.begin(), ItDQEnd = VecDesignQ.end(),
		ItCQBeg = VecCheckQ.begin(), ItCQEnd = VecCheckQ.end(),
		ItZBeg = VecZ.begin(), ItZEnd = VecZ.end(),
		ItDesignAuxBeg = VecDesignAux.begin(), ItDesignAuxEnd = VecDesignAux.end(),
		ItCheckAuxBeg = VecCheckAux.begin(), ItCheckAuxEnd = VecCheckAux.end();

	ItV = ItVBeg, ItZ = ItZBeg;

	cout << "DesignqSize: " << VecDesignq.size() << endl;
	cout << "CheckqSize: " << VecCheckq.size() << endl;
	cout << "DesignAuxSize: " << VecDesignAux.size() << endl;
	cout << "DesignQSize: " << VecDesignQ.size() << endl;
	cout << "CheckQSize: " << VecCheckQ.size() << endl;
	cout << "VSize: " << VecV.size() << endl;
	cout << "ZSize: " << VecZ.size() << endl;
	cout << "CheckAuxSize: " << VecCheckAux.size() << endl;

	ifstr_data.close();

	if (((VecDesignq.size()) ^ (VecCheckq.size()) ^ (VecDesignAux.size()) ^ (VecV.size()) ^ (VecZ.size()) ^ (VecCheckAux.size())) == 0)
	{
		cout << "init has done" << endl;
		return true;
	}
	else
	{
		cout << "the input error" << endl;
		return false;
	}

	return false;
}

bool Graphic(double startZ, int i)
{

	VecDouIt ItQ, ItQBeg, ItQEnd, ItAux, ItAuxBeg, ItAuxEnd, Itq, ItqBeg, ItqEnd;

	if (i == 0)
	{
		ItqBeg = ItDqBeg, ItqEnd = ItDqEnd;
		ItQ = ItDQBeg, ItQBeg = ItDQBeg, ItQEnd = ItDQEnd,
		ItAux = ItDesignAuxBeg, ItAuxBeg = ItDesignAuxBeg, ItAuxEnd = ItDesignAuxEnd;
	}
	else
	{
		ItqBeg = ItCqBeg, ItqEnd = ItCqEnd;
		ItQ = ItCQBeg, ItQBeg = ItCQBeg, ItQEnd = ItCQEnd,
		ItAux = ItCheckAuxBeg, ItAuxBeg = ItCheckAuxBeg, ItAuxEnd = ItCheckAuxEnd;
	}


	double q1 = LinearInterpolation(startZ, ItZBeg, ItZEnd, ItqBeg, ItqEnd);
	double V1;
	double averQ, aux;

	//find Q > q
	while (*ItQ < q1)
	{
		i == 0 ? (VecDesignResGraq.push_back(*ItQ)) : (VecCheckResGraq.push_back(*ItQ));
		ItQ++;

	}
	q1 = q;
	V1 = LinearInterpolation(q1, ItqBeg, ItqEnd, ItVBeg, ItVEnd);

	i == 0 ? (VecDesignResGraq.push_back(q1)) : (VecCheckResGraq.push_back(q1));
	/*i == 0 ? (VecDesignResGraAux.push_back(LinearInterpolation(q1, ItqBeg, ItqEnd, ItAuxBeg, ItAuxEnd))) : 
		(VecCheckResGraAux.push_back(LinearInterpolation(q1, ItqBeg, ItqEnd, ItAuxBeg, ItAuxEnd)));*/
	
	for (; (ItQ + 1) != ItQEnd; ++ItQ)
	{
		averQ = (*ItQ + *(ItQ + 1)) / 2;
		aux = LinearInterpolation(q1, ItqBeg, ItqEnd, ItAuxBeg, ItAuxEnd) 
			+ averQ - q1;
		q1 = LinearInterpolation(aux, ItAuxBeg, ItAuxEnd, ItqBeg, ItqEnd);

		if (LinearInterpolation(q1, ItqBeg, ItqEnd, ItZBeg, ItZEnd) <= startZ)
		{
			ItQ++;
			while (ItQ != ItQEnd)
			{
				i == 0 ? (VecDesignResGraq.push_back(*ItQ)) : (VecCheckResGraq.push_back(*ItQ));
				ItQ++;
			}
			break;
		}

		i == 0 ? (VecDesignResGraq.push_back(q1)) : (VecCheckResGraq.push_back(q1));
		/*i == 0 ? (VecDesignResGraAux.push_back(LinearInterpolation(q1, ItqBeg, ItqEnd, ItAuxBeg, ItAuxEnd))) :
			(VecCheckResGraAux.push_back(LinearInterpolation(q1, ItqBeg, ItqEnd, ItAuxBeg, ItAuxEnd)));*/

	}

	if (i)
	{
		ItCRGqBeg = VecCheckResGraq.begin();
		ItCRGqEnd = VecCheckResGraq.end();
	}
	else {
		ItDRGqBeg = VecDesignResGraq.begin();
		ItDRGqEnd = VecDesignResGraq.end();
	}
	
	cout << endl;

	return true;
}

bool TrialMethod(double startZ, int i, int acc)
{

	VecDouIt Itq, ItqBeg, ItqEnd, ItQ, ItQBeg, ItQEnd;

	if (i == 0)
	{
		Itq = ItDqBeg, ItqBeg = ItDqBeg, ItqEnd = ItDqEnd;
		ItQ = ItDQBeg, ItQBeg = ItDQBeg, ItQEnd = ItDQEnd;
	}
	else
	{
		Itq = ItCqBeg, ItqBeg = ItCqBeg, ItqEnd = ItCqEnd;
		ItQ = ItCQBeg, ItQBeg = ItCQBeg, ItQEnd = ItCQEnd;
	}

	double V1 = LinearInterpolation(startZ, ItZBeg, ItZEnd, ItVBeg, ItVEnd);


	double q1 = LinearInterpolation(V1, ItVBeg, ItVEnd, ItqBeg, ItqEnd),
		q2x = 1240.0f, resq2x, V2x,
		q2y = 1240.0f, resq2y, V2y;

	
	if (acc)
	{
		i == 0 ? (VecDesignResAccq.push_back(q1)) : (VecCheckResAccq.push_back(q1));
	}
	else {
		//find Q > q
		while (*ItQ < q1)
		{
			
			i == 0 ? (VecDesignResTrialq.push_back(*ItQ)) : (VecCheckResTrialq.push_back(*ItQ));
			ItQ++;
		}

		if (flag)
		{
			double k = (*ItQ - q1) / (*ItQ - *(ItQ - 1)), temp = *(ItQ - 1);
			dt = k * dt;
			*(ItQ - 1) = q1;
			while ((abs(q2x - resq2x) > ACCURACY) && (abs(q2y - resq2y)) > ACCURACY)
			{

				if ((V2x = LinearInterpolation(q2x, ItqBeg, ItqEnd, ItVBeg, ItVEnd)) == -1)
				{
					q2x++;
				}

				if (V2x == -2)
				{
					q2x--;
				}
				else
				{
					resq2x = WaterBalance(V2x, q1, V1, ItQ - 1, 0);
					q2x = q2x - resq2x;
					resq2x = q2x + resq2x;
					q2x = resq2x - q2x;
				}

				if ((V2y = WaterBalance(q2y, q1, V1, ItQ - 1, 1)) < 0)	//赋值符号与小于符号的优先级
				{
					q2y--;
				}
				else
				{
					resq2y = LinearInterpolation(V2y, ItVBeg, ItVEnd, ItqBeg, ItqEnd);
					if (resq2y == -1)
					{
						q2y--;
					}
					else if (resq2y == -2)
					{
						q2y++;
					}
					else
					{
						q2y = q2y - resq2y;
						resq2y = q2y + resq2y;
						q2y = resq2y - q2y;
					}
				}

			}

			if (abs(q2x - resq2x) < ACCURACY)
			{
				q1 = q2x;
			}
			else
			{
				q1 = q2y;
			}
			dt = 3 * 3600;
			*(ItQ - 1) = temp;
		}
		else
		{
			double v = dt * (*(ItQ)-q1)* (*(ItQ)-q1) / (*ItQ - *(ItQ - 1)) / 20000;
			V1 += v;
			q1 = LinearInterpolation(V1, ItVBeg, ItVEnd, ItqBeg, ItqEnd);
		}


		q = q1;
		i == 0 ? (VecDesignResTrialq.push_back(q1)) : (VecCheckResTrialq.push_back(q1));
		V1 = LinearInterpolation(q1, ItqBeg, ItqEnd, ItVBeg, ItVEnd);
		resq2x = resq2y = 0;
	}
	
	

	V2x = LinearInterpolation(q2x, ItqBeg, ItqEnd, ItVBeg, ItVEnd);
	V2y = LinearInterpolation(q2y, ItqBeg, ItqEnd, ItVBeg, ItVEnd);

	resq2x = *(ItQ)+*(ItQ + 1) - 10000 * 2 * (V2x - V1) / dt - q1;
	resq2y = *(ItQ)+*(ItQ + 1) - 10000 * 2 * (V2y - V1) / dt - q1;

	for (; ((ItQ + 1) != ItQEnd); ++ItQ)
	{
		while ((abs(q2x - resq2x) > ACCURACY) && (abs(q2y - resq2y)) > ACCURACY )
		{

			if ((V2x = LinearInterpolation(q2x, ItqBeg, ItqEnd, ItVBeg, ItVEnd)) == -1)
			{
				q2x++;
			}
			
			if (V2x == -2)
			{
				q2x--;
			}
			else
			{
				resq2x = WaterBalance(V2x, q1, V1, ItQ, 0);
				q2x = q2x - resq2x;
				resq2x = q2x + resq2x;
				q2x = resq2x - q2x;
			}

			
			if ((V2y = WaterBalance(q2y, q1, V1, ItQ, 1)) < 0)	//赋值符号与小于符号的优先级
			{
				
				q2y--;
			}
			else
			{
				resq2y = LinearInterpolation(V2y, ItVBeg, ItVEnd, ItqBeg, ItqEnd);
				if (resq2y == -1)
				{
					break;
					q2y--;
				}
				else if (resq2y == -2)
				{
					q2y++;
				}
				else
				{
					q2y = q2y - resq2y;
					resq2y = q2y + resq2y;
					q2y = resq2y - q2y;
				}
			}

		}

		if (abs(q2x - resq2x) < ACCURACY)
		{
			q1 = q2x;
		}
		else
		{
			q1 = q2y;
		}


		if (acc)
		{
			i == 0 ? (VecDesignResAccq.push_back(q1)) : (VecCheckResAccq.push_back(q1));
		}
		else
		{
			if (LinearInterpolation(q1, ItqBeg, ItqEnd, ItZBeg, ItZEnd) <= startZ)
			{
				
				ItQ++;
				while (ItQ != ItQEnd)
				{
					i == 0 ? (VecDesignResTrialq.push_back(*ItQ)) : (VecCheckResTrialq.push_back(*ItQ));
					ItQ++;
				}
				break;			
			}
			i == 0 ? (VecDesignResTrialq.push_back(q1)) : (VecCheckResTrialq.push_back(q1));	
		}

		V1 = LinearInterpolation(q1, ItqBeg, ItqEnd, ItVBeg, ItVEnd);
		resq2x = resq2y = 0;
		
	}

	if (i)
	{
		ItCRTqBeg = VecCheckResTrialq.begin();
		ItCRTqEnd = VecCheckResTrialq.end();
	}
	else {
		ItDRTqBeg = VecDesignResTrialq.begin();
		ItDRTqEnd = VecDesignResTrialq.end();
	}
	
	return true;
}


vector<string> split(const string &s, const string &seperator) {
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}

template <class Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}


double LinearInterpolation(double d, VecDouIt it1Beg, VecDouIt it1End,
	VecDouIt it2Beg, VecDouIt it2Edn)
{
	VecDouIt it1 = it1Beg, it2 = it2Beg;
	if (d < *(it1))
	{
		return -1;
	}

	while ((it1 != it1End) && (d > *(it1)))
	{
		it1++;
		it2++;
	}

	if (it1 == it1End)
	{
		return -2;
	}

	if (d == *(it1))
	{
		return *(it2);
	}
	if (d < *(it1))
	{
		double res = *(it2 - 1) + (d - *(it1 - 1)) * (*it2 - *(it2 - 1)) / (*it1 - *(it1 - 1));
		return res;
	}

}

double WaterBalance(double para, double q1, double V1, VecDouIt ItDQ, int i)
{
	if (i == 0) //Solving q with V
	{
		return *(ItDQ)+*(ItDQ + 1) - 2 * 10000 * (para - V1) / dt - q1;
	}
	else //Solving V with q
	{
		return dt * (*(ItDQ)+*(ItDQ + 1) - q1 - para) / (2 * 10000) + V1;
	}
}

double Discharge(double V, int i)
{

	double Z = LinearInterpolation(V, VecV.begin(), VecV.end(), VecZ.begin(), VecZ.end());
	if (Z < 0)
	{
		return -1;
	}
	if (i == 0)		//Design
	{
		double H = Z - 2795 - 6.5 / 2;
		double q = 0.78 * 7 * 6.5 * sqrt(2 * 9.81 * H) + 103;
		return q;
	}
	else //Check
	{
		double H1 = Z - 2844;
		double H2 = Z - 2795 - 6.5 / 2;
		double q1 = 0.415 * 8 * sqrt(2 * 9.81) * pow(H1, 1.5);
		double q2 = 0.78 * 7 * 6.5 * sqrt(2 * 9.81 * H2);
		return q1 + q2;
	}
}

int getDataType(const string& str)
{
	if (str == "Designq:")
		return 0;
	if (str == "Checkq:")
		return 1;
	if (str == "DesignAux:")
		return 2;
	if (str == "DesignQ:")
		return 3;
	if (str == "CheckQ:")
		return 4;
	if (str == "V:")
		return 5;
	if (str == "Z:")
		return 6;
	if (str == "CheckAux:")
		return 7;
	return -1;
}

