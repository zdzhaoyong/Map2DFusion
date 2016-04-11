#include <TooN/SVD.h>
#include <TooN/helpers.h>
using namespace TooN;
using namespace std;

int main()
{
	Matrix<3, 4> m = Zeros;
	m[0] = makeVector(1, 2, 3, 4);
	m[1] = makeVector(1, 1, 1, 1);
	m[2] = makeVector(3, 2, 6, 7);	

	cout << m << endl;

	SVD<-1, -1> svdm(m);

	Matrix<3,4> temp =diagmult(svdm.get_diagonal(), svdm.get_VT());

	cout << svdm.get_U() * temp << endl;

	Matrix<4,3> m2 = m.T();
	SVD<-1,-1> svdm2(m2);

	cout << svdm2.get_U() * diagmult(svdm2.get_diagonal(), svdm2.get_VT()) << endl;





}
