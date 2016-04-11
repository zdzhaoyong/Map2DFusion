#include <TooN/helpers.h>

using namespace TooN;
using namespace std;

int main()
{
	Matrix<3> m = Identity;

	cout << m << endl;

	Matrix<3> n = 2 * Identity;

	cout << n << endl;

	n = Identity * 3;

	cout << n << endl;


	Matrix<> q = 5.5 * Identity(6) * 2;

	cout << q << endl;
	cout << q - 3.*Identity/4 << endl;

	Matrix<3> p = Ones * 2;

	cout <<  Identity - p << endl;

	Matrix<3> g = -Identity * 2;
	cout << g << endl;
}
