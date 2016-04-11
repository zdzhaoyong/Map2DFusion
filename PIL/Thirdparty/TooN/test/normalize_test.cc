#include <TooN/TooN.h>
#include <cmath>

using namespace TooN;
using namespace std;

extern Vector<4>& v;

inline void norm_in_place(Vector<4>& v)
{
	v/=sqrt(v*v);	
}


inline Vector<4> unit(const Vector<4>& v)
{
	return v/sqrt(v*v);	
}


void norm_v_1()
{
	norm_in_place(v);
}

void norm_v_2()
{
	v = unit(v);
}


void norm_v_3()
{
	v.slice<0,4>() = unit(v.slice<0,4>());
}

