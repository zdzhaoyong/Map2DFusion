#include <TooN/TooN.h>
#include <TooN/helpers.h>

using namespace std;
using namespace TooN;

template<class C> void type(const C&)
{
	cout << __PRETTY_FUNCTION__ << endl;
}

#define V(a,N) (a==-1?-1:N)

template<int a, int b> void mmult_test()
{
	cout << "\n\n\n----------------------------------------------\n\n";
	cout << "Testing " << (a==-1?"dynamic":"static") << " * " << (b==-1?"dynamic":"static") << ":\n";

	Matrix<V(a,2),V(a,3)> m3(2,3);
	m3[0] = makeVector(0, 1, 2);
	m3[1] = makeVector(3, 4, 5);
	Matrix<V(b,3),V(b,2)> m4(3,2);
	m4[0] = makeVector(6, 7);
	m4[1] = makeVector(8, 9);
	m4[2] = makeVector(10, 11);

	Vector<V(a,3)> v(3);
	v = makeVector(6,8,10);

	cout << m3<<endl;
	cout << m4<<endl;
	cout << m3*m4;
	
	cout << "\n should be: \n    28    31\n  100   112\n";

	cout << endl << v << endl;
	cout << endl <<  m3*v << endl;

	cout << "\n should be: \n    28    100\n" << endl;

}

int main()
{
	mmult_test<1,1>();
	mmult_test<1,-1>();
	mmult_test<-1,1>();
	mmult_test<-1,-1>();
}

