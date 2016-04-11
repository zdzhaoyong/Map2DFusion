#include <TooN/TooN.h>
#include <cstdlib>
using namespace TooN;
using namespace std;

template<int R, int C> void test(int r, int c)
{
	Matrix<R,C> m(r,c);
	for(int r=0; r < m.num_rows(); r++)
		for(int c=0; c < m.num_cols(); c++)
			m[r][c] = rand()*.1/RAND_MAX;
	
	cout << m << endl;
	cout << "Diag: " << m.diagonal_slice() << endl << endl;
}

int main()
{
	test<3,3>(3,3);
	test<3,2>(3,2);
	test<2,3>(2,3);

	test<3,Dynamic>(3,3);
	test<3,Dynamic>(3,2);
	test<2,Dynamic>(2,3);

	test<Dynamic,3>(3,3);
	test<Dynamic,2>(3,2);
	test<Dynamic,3>(2,3);


	test<Dynamic,Dynamic>(3,3);
	test<Dynamic,Dynamic>(3,2);
	test<Dynamic,Dynamic>(2,3);
}
