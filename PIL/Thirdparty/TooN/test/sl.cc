#include <TooN/TooN.h>
#include <TooN/sl.h>

#include <iostream>
#include <iomanip>

using namespace TooN;
using namespace std;

int main(int , char ** ){
	SL<3> h(makeVector(1,0,-1,0,0,0,0,0));
	cout << h << endl;
	cout << h.inverse() << endl;
	cout << SL<3>::exp(makeVector(-1,0,1,0,0,0,0,0)) << endl;
	cout << h * h.inverse() << endl;
	h *= h.inverse();
	cout << h << endl;
	
	for(int i = 0; i < SL<3>::dim; ++i)
		cout << "generator " << i << "\n" << SL<3>::generator(i) << endl;

	for(int i = 0; i < SL<2>::dim; ++i)
		cout << "generator " << i << "\n" << SL<2>::generator(i) << endl;
	
	cout << SL<2>::exp(makeVector(1,2,3)) << endl;
	
	h = SL<3>::exp(makeVector(1,0,-1,0,0,0,1,0));
	
	cout << h << "\n";
	Vector<3> t = makeVector(0,1,2);
	cout << "with vector " << t << "\n";
	cout << h * t << "\n";
	cout << t * h << "\n";
	
	Matrix<3,5> m = Zeros;
	m[0] = makeVector(0,1,2,3,4);
	m[1] = makeVector(1,2,3,4,-5);
	m[2] = makeVector(2,3,4,5,8);
	
	cout << "with matrix " << m << "\n";
	cout << h * m << "\n";
	cout << m.T() * h << "\n";
	
	cout << endl;
	
	cout << "log(h)\t" << h.ln() << endl;
	cout << "diff\n" << h.get_matrix() - SL<3>::exp(h.ln()).get_matrix() << endl;
	
/*
	SO3<> so3(makeVector(1,0,1));
	h = so3;
	cout << so3.get_matrix() << h.get_matrix() << "\n";
*/	
	return 0;
}
