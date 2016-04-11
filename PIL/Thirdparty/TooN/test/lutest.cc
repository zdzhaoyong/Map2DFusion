#include <cstdlib>
#include <cmath>
#include <iostream>
#include <TooN/TooN.h>
#include <TooN/LU.h>

using namespace TooN;
using namespace std;


int main()
{
	Matrix<-1,5,float> m(5,5);

	for(int i=0; i< m.num_rows(); i++)
		for(int j=0; j< m.num_rows(); j++)
			m[i][j] = drand48();

	
	LU<-1,float> mlu(m);
	Matrix<5,5,float> a = m*mlu.get_inverse();

	for(int i=0; i< m.num_rows(); i++)
		for(int j=0; j< m.num_rows(); j++)
		{
			if(round(a[i][j]) < 1e-10)
				a[i][j] = 0;
		}

	cout << a;



}
