#include <TooN/TooN.h>
using namespace std;
using namespace TooN;

int main()
{
	Vector<Resizable> r = makeVector(4.);
	cout << r << endl;
	
	//Test simple resize
	Vector<> v1(makeVector(1, 2));
	r.resize(v1.size());
	r = v1;
	cout << r << endl;

	//Test automatic resize
	Vector<3> v2 = makeVector(2, 3, 4);
	r = v2;
	cout << r << endl;
	
	//Test non-sized operator
	r.resize(10);
	r = Ones;
	cout << r << endl;
	
	//Test sized operator
	r = Zeros(3);
	cout << r << endl;

	r = makeVector(6, 7, 8, 9, 0);
	cout << r << endl;

	r.resize(2);
	r[0]= 5; 
	r[1] = 6;

	cout << r << endl;


}
