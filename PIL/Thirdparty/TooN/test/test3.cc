#include <TooN/TooN.h>
#include <TooN/helpers.h>

using namespace std;
using namespace TooN;

template<class C> void type(const C&)
{
	cout << __PRETTY_FUNCTION__ << endl;
}

int main()
{
	Matrix<3> m1(Zeros);
	Matrix<3> m2(Zeros);

	m1.slice<0,0,2,2>()+=3;
	m2.slice<1,1,2,2>()+=2;

	cout << m1 << endl;
	cout << m2 << endl;
	
	m1+=m2;
	cout << m1 << endl;

	cout << (m1+m2) << endl;
	cout << (m1+m2) << endl;

	Matrix<2,3> m3;
	m3[0] = makeVector(0, 1, 2);
	m3[1] = makeVector(3, 4, 5);
	Matrix<3,2> m4;
	m4[0] = makeVector(6, 7);
	m4[1] = makeVector(8, 9);
	m4[2] = makeVector(10, 11);

	cout << m3<<endl;
	cout << m4<<endl;
	cout << m3*m4;

	
	cout << makeVector(1,2).as_col() * makeVector(3,4).as_row() << endl;
}

