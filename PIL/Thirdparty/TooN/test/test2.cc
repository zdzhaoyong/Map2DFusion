#include <TooN/TooN.h>
#include <TooN/helpers.h>

using namespace std;
using namespace TooN;

template<class C> void type(const C&)
{
	cout << __PRETTY_FUNCTION__ << endl;
}

void make_a_copy_constructor_happen(const Vector<4>& v)
{
	cout << "Pre CC\n";
	Vector<4> v2(v);
	cout << "Post CC\n";
}

int main()
{
	Vector<4> v1 = makeVector(1, 2, 3, 4);
	Vector<4> v2 = makeVector(5, 6, 7, 8);

	make_a_copy_constructor_happen(v1);

	cout << Ones + (v1 + v2)+2*Ones << endl;

	v1.slice<0, 2>() /= 2;
	cout << v1 << endl;

	type(Vector<2>() + Vector<2, int>());
}

