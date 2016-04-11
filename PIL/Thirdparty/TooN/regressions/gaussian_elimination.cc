#include "regressions/regression.h"
#include <TooN/gaussian_elimination.h>
using namespace TooN;
using namespace std;


int main()
{
	double err=0;
	
	//Test a bunch of matrices 
	
	for(int i=0; i < 10000; i++)
	{
		int rows = xor128u() % 100 + 2;
		int cols = rows  + xor128u() % 20;

		Matrix<> b(rows, cols);

		for(int r=0; r < rows; r++)
			for(int c=0; c < cols; c++)
				b[r][c] = xor128d();

		Matrix<> A(rows, rows);

		for(int r=0; r < rows; r++)
			for(int c=0; c < rows; c++)
				A[r][c] = xor128d();

		Matrix<> x = gaussian_elimination(A, b);

		Matrix<> e = A*x - b;
		
		for(int r=0; r < rows; r++)
			err = max(err, norm_inf(e[r]));
	}

	cout << err << endl;
}

