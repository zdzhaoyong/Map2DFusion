#include "regressions/regression.h"

int main()
{
	Vector<5> v = makeVector(1, 2, 3, 4, 5);
	cout << v.slice<0,3>() << endl;

	Matrix<3> m = Data(1, 2, 3, 4, 5, 6, 7, 8, 9);
	const Matrix<3> n = m;

	cout << m.slice<0,0,2,2>() << endl;
	cout << n.slice<0,0,2,2>() << endl;

	cout << m.slice(0,0,2,2) << endl;
	cout << n.slice(0,0,2,2) << endl;

	const Vector<4> cv = makeVector(3,4,5,6);
	cout << cv.slice<0,2>() << endl;
	cout << cv.slice(0,2) << endl;
}
