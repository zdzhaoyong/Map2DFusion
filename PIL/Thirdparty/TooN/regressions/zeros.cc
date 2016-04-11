#include "regressions/regression.h"

int main()
{
	Vector<5> v1 = Zeros;
	cout << v1 << endl;

	Vector<> v2 = Zeros(3);
	cout << v2 << endl;

	Matrix<3,6> m1 = Zeros;
	cout << m1 << endl;

	Matrix<> m2 = Zeros(2,3);
	cout << m2 << endl;


	Vector<4> v3 = Zeros;
	cout << (v3 != Zeros) << endl;

	v3 = Ones;
	cout << (v3 != Zeros) << endl;

	Matrix<4> m3 = Zeros;
	cout << (m3 != Zeros) << endl;
	m3 = Ones;
	cout << (m3 != Zeros) << endl;
}
