#include <TooN/TooN.h>
#include <cstdlib>
using namespace TooN;
using namespace std;

int main()
{
	Matrix<2> m1(Data(1, 2, 3, 4));

	Vector<2> v = makeVector(5,6);

	m1 = v.as_diagonal();
	cout << m1 << endl;

	DiagonalMatrix<2> d(makeVector(3,4));

	Matrix<2> m2 = d;
	cout << m2 << endl;

	Matrix<2> m3(d);
	cout << m3 << endl;

	cout << d * v << endl;


	m2=-d;
	cout << m2 << endl;


	DiagonalMatrix<3> d3(Data(1, 2, 3));

	cout << Matrix<3>(d3) << endl;
}
