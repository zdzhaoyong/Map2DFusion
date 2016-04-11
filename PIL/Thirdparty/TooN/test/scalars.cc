#include <TooN/TooN.h>
#include <TooN/helpers.h>
using namespace TooN;
using namespace std;
int main()
{
	Vector<> v = Ones(5);

	cout << v << endl;

	cout << v + Ones*3 << endl;
	
	cout << v.slice(2,3) + Ones*3 << endl;

	v+= Ones;
	cout << v << endl;
	v.slice(0,3) += Ones * 3;
	cout << v << endl;

	Matrix<> m = 3*Ones(4,4) * 2;
	cout << m << endl;
	cout << m.slice<0,0,2,3>() - 2*Ones << endl;

	m+= Ones;
	cout << m << endl;
	m.slice<0,0,3,2>() += Ones*2;
	cout << m << endl;


	Matrix<3> p = Identity;
	cout << p - Ones << endl;
	cout << Ones - p << endl;
}
