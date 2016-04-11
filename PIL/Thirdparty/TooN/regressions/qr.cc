#include "regressions/regression.h"
#include <TooN/QR.h>
using namespace TooN;
using namespace std;


template<class C> void test()
{
	double err=0;
	
	//Test a bunch of matrices 
	
	for(int i=0; i < 1000; i++)
	{
		int rows = xor128u() % 100 + 2;
		int cols = rows  + xor128u() % 20;

		Matrix<> m(rows, cols);

		for(int r=0; r < rows; r++)
			for(int c=0; c < cols; c++)
				m[r][c] = xor128d();

		C q(m);


		double edecomp = norm_fro(q.get_Q() * q.get_R() - m);
		double e_ortho = norm_fro(q.get_Q() * q.get_Q().T() - Matrix<>(Identity(rows)));

		for(int r=0; r < rows; r++)
			for(int c=0; c < r; c++)
				err = max(err, (q.get_R()[r][c] != 0)*1.0);
		
		err = max(err, max(edecomp, e_ortho));

	}


	cout << err << endl;
}

int main()
{
	Matrix<3,4> m;
	
	m = Data(5.4388593399963903e-01,
9.9370462412085203e-01,
1.0969746452319418e-01,
4.4837291206649532e-01,
7.2104662057981139e-01,
2.1867663239963386e-01,
6.3591370975105699e-02,
3.6581617683817125e-01,
5.2249530577710213e-01,
1.0579827325022817e-01,
4.0457999585762583e-01,
7.6350464084881342e-01);
	
	cout << setprecision(20) << scientific;
	cout << m << endl;

	QR<3, 4> q(m);

	cout << q.get_R() << endl;
	cout << q.get_Q() << endl;

	cout << q.get_Q() * q.get_R() - m << endl;

	
	test<QR<> >();
	test<QR_Lapack<> >();
}

