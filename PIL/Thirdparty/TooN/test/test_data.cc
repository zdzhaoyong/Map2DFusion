#include <TooN/TooN.h>
using namespace TooN;
using namespace std;

void foo(Matrix<2>& foo)
{
	foo = Data(1, 2, 3, 4);
}

int main()
{
	Matrix<2> m = Data(1, 2, 3, 4);

	cout << m << endl;

	Matrix<4> n = Zeros;
	n.slice<0,0,2,2>() = Data(2, 3, 4, 5);

	cout << n << endl;
}
