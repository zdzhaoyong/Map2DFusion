#include <TooN/TooN.h>

using namespace TooN;
extern "C"{

Vector<3> add_static(const Vector<3>& a, const Vector<3>& b)
{
	Vector<3> t;
	for(int i=0; i < 3; i++)
		t[i] = a[i] + b[i];

	Vector<3> t1 = t;
	Vector<3> t2 = t1;
	return t2;
}

Vector<-1> add_dynamic(const Vector<-1>& a, const Vector<-1>& b)
{
	Vector<> t(a.size());
	for(int i=0; i < 3; i++)
		t[i] = a[i] + b[i];
	Vector<> t1 = t;
	Vector<> t2 = t1;
	return t2;
}

}
