#undef TOON_CHECK_BOUNDS
#define TOON_CHECK_BOUNDS
#undef TOON_INITIALIZE_SNAN
#define TOON_INITIALIZE_NAN


#include <TooN/SymEigen.h>
#include "regressions/regression.h"
#include <iomanip>
#include <algorithm>

using namespace TooN;
using namespace std;

template<int Size, int Size2>
void test_(const Matrix<Size>& m)
{
	Matrix<Size2> M = m;
	SymEigen<Size2> e(M);
	cout << setprecision(15);
	cout << e.get_evectors() << endl;
	cout << e.get_evalues() << endl;
	cout << endl << endl;
}

template<int Size>
void test(const Matrix<Size>& m)
{
	test_<Size, Size>(m);
	test_<Size, Dynamic>(m);
}

template<int Size1> void test_matrix(const Matrix<Size1>& m, int& sorted, double& n)
{
	SymEigen<Size1> sm(m);

	//Check the results are sorted low to high
	for(int i=1; i < m.num_rows(); i++)
		if(sm.get_evalues()[i-1] > sm.get_evalues()[i])
		{
			sorted++;
			break;
		}
	
	//Check that  Mx = lX

	double e =0;
	
	for(int i=0; i < m.num_rows(); i++)
		e = max(e, norm_inf(m * sm.get_evectors()[i] - sm.get_evalues()[i] * sm.get_evectors()[i]));

	e = max(e, norm_inf(m - sm.get_evectors().T() * sm.get_evalues().as_diagonal() * sm.get_evectors())/m.num_rows());	
	e = max(e, norm_inf(Matrix<Size1>(Identity(m.num_rows())) - sm.get_evectors().T() * sm.get_evectors())/m.num_rows());	
	n = max(n, e);
}

template<int Size1>
void test_things_(int S2, int & sorted, double & n)
{
	Matrix<Size1> m = Zeros(S2);
	
	for(int r=0; r < m.num_rows(); r++)
		for(int c=r; c < m.num_cols(); c++)
			m[r][c] = m[c][r] = xor128d();

	test_matrix(m, sorted, n);

	for(int r=0; r < m.num_rows(); r++)
		for(int c=r; c < m.num_cols(); c++)
			m[r][c] = m[c][r] = floor(xor128d() * 5) - 2;

	test_matrix(m, sorted, n);
}


void test_things()
{
	int s=0;
	double e=0;

	for(int n=0; n < 20000; n++)
	{
		test_things_<2>(2, s, e);
		test_things_<3>(3, s, e);
	}

	for(int n=0; n < 200; n++)
	{
		test_things_<2>(2, s, e);
		test_things_<3>(3, s, e);
		test_things_<4>(4, s, e);
		test_things_<5>(5, s, e);
		test_things_<6>(6, s, e);
		test_things_<7>(7, s, e);
		test_things_<8>(8, s, e);
		test_things_<9>(9, s, e);

		test_things_<Dynamic>(2, s, e);
		test_things_<Dynamic>(3, s, e);
		test_things_<Dynamic>(4, s, e);
		test_things_<Dynamic>(5, s, e);
		test_things_<Dynamic>(6, s, e);
		test_things_<Dynamic>(7, s, e);
		test_things_<Dynamic>(8, s, e);
		test_things_<Dynamic>(9, s, e);


		test_things_<Dynamic>(n+9, s, e);
	}

	cout << "Sorted " << s << endl;
	cout << "Errors " << e << endl;

}


int main()
{
	test_things();
}
