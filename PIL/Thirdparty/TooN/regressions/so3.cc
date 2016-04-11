#include <TooN/so3.h>
#include <cmath>

using namespace TooN;
using namespace std;

int main()
{
	Vector<3> v = makeVector(M_PI, 0, 0);

	cout << SO3<>::exp(v);

	const Vector<3>& u(v);

	cout << SO3<>::exp(u);

	cout << SO3<>::exp(u.as_slice());

}
