#include <TooN/TooN.h>
using namespace TooN;
using namespace std;

int main()
{
	Vector<3> v = makeVector(1, 2, 3);

	cout << v.as_row() << endl;
	cout << v.as_col() << endl;
	cout << v.as_slice() << endl;

	Vector<> u = makeVector(1, 2, 3);

	cout << u.as_row() << endl;
	cout << u.as_col() << endl;

	cout << v.as_slice() << endl;

}
