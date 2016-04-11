#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <TooN/TooN.h>
#include <TooN/helpers.h>
#include <TooN/gaussian_elimination.h>
#include <tr1/random>

using namespace TooN;
using namespace std;
using namespace tr1;


int main()
{
	
	unsigned int s;
	ifstream("/dev/urandom").read((char*)&s, 4);
	
	std::tr1::mt19937 eng;
	std::tr1::uniform_real<double> rnd;
	eng.seed(s);
	Matrix<5,5> m(5,5);

	for(int i=0; i< m.num_rows(); i++)
		for(int j=0; j< m.num_rows(); j++)
			m[i][j] = rnd(eng);

	cout << m << endl;
	
	Matrix<5,5> i;
	Identity(i);

	Matrix<5,5> inv = gaussian_elimination(m, i);
	
	Matrix<5,5> a = m*inv;

	for(int i=0; i< m.num_rows(); i++)
		for(int j=0; j< m.num_rows(); j++)
		{
			if(round(a[i][j]) < 1e-10)
				a[i][j] = 0;
		}

	cout << a;



}
