#include <TooN/TooN.h>
using namespace std;
using namespace TooN;

template<class M> void row(const M& m)
{
	for(int r=0; r< m.num_rows(); r++)
		cout << m[r];
	cout << endl;
}

int main()
{
	Vector<6> v = makeVector(1, 2, 3, 4, 5, 6);
	Vector<>  u = v;

	cout << v.slice<0,3>() << endl;
	cout << u.slice<0,3>() << endl << endl;

	cout << v.slice<1,3>(1, 3) << endl;
	cout << u.slice<1,3>(1, 3) << endl;
	cout << v.slice<Dynamic,3>(1, 3) << endl;
	cout << u.slice<Dynamic,3>(1, 3) << endl;
	cout << v.slice<1,Dynamic>(1, 3) << endl;
	cout << u.slice<1,Dynamic>(1, 3) << endl;
	cout << v.slice<Dynamic,Dynamic>(1, 3) << endl;
	cout << u.slice<Dynamic,Dynamic>(1, 3) << endl << endl;

	cout << v.slice(2, 3) << endl;
	cout << u.slice(2, 3) << endl << endl;
	
	cout << project(v) << endl;
	cout << project(u) << endl;
	cout << project(v.slice<1,End<0> >()) << endl;
	cout << project(u.slice<1,End<0> >()) << endl;
	cout << unproject(v) << endl;
	cout << unproject(u) << endl;

	cout << v.slice<1, End<0> >() << endl;	
	cout << u.slice<1, End<0> >() << endl;	
	cout << v.slice<1, End<-1> >() << endl;	
	cout << u.slice<1, End<-1> >() << endl;	
	cout << v.slice(2, End) << endl;	
	cout << u.slice(2, End) << endl;	
	cout << v.slice(2, End(-1)) << endl << endl;	
	cout << u.slice(2, End(-1)) << endl << endl;	

	Vector<200> w = Zeros;

	cout << w.slice<100,End<-99> >() << endl;

	cout << endl << endl << endl;

	Matrix<3> m = Data(1, 2, 3, 4, 5, 6, 7, 8, 9);
	Matrix<>  n = m;
	
	row(m.slice<0,1,2,2>());
	row(n.slice<0,1,2,2>());
	row(m.slice(0,1,2,2));
	row(n.slice(0,1,2,2));
	row(m.slice<0      ,1      ,2      ,2      >(0, 1, 2, 2));
	row(n.slice<0      ,1      ,2      ,2      >(0, 1, 2, 2));
	row(m.slice<0      ,1      ,2      ,Dynamic>(0, 1, 2, 2));
	row(n.slice<0      ,1      ,2      ,Dynamic>(0, 1, 2, 2));
	row(m.slice<0      ,1      ,Dynamic,2      >(0, 1, 2, 2));
	row(n.slice<0      ,1      ,Dynamic,2      >(0, 1, 2, 2));
	row(m.slice<0      ,1      ,Dynamic,Dynamic>(0, 1, 2, 2));
	row(n.slice<0      ,1      ,Dynamic,Dynamic>(0, 1, 2, 2));
	row(m.slice<0      ,Dynamic,2      ,2      >(0, 1, 2, 2));
	row(n.slice<0      ,Dynamic,2      ,2      >(0, 1, 2, 2));
	row(m.slice<0      ,Dynamic,2      ,Dynamic>(0, 1, 2, 2));
	row(n.slice<0      ,Dynamic,2      ,Dynamic>(0, 1, 2, 2));
	row(m.slice<0      ,Dynamic,Dynamic,2      >(0, 1, 2, 2));
	row(n.slice<0      ,Dynamic,Dynamic,2      >(0, 1, 2, 2));
	row(m.slice<0      ,Dynamic,Dynamic,Dynamic>(0, 1, 2, 2));
	row(n.slice<0      ,Dynamic,Dynamic,Dynamic>(0, 1, 2, 2));
	row(m.slice<Dynamic,1      ,2      ,2      >(0, 1, 2, 2));
	row(n.slice<Dynamic,1      ,2      ,2      >(0, 1, 2, 2));
	row(m.slice<Dynamic,1      ,2      ,Dynamic>(0, 1, 2, 2));
	row(n.slice<Dynamic,1      ,2      ,Dynamic>(0, 1, 2, 2));
	row(m.slice<Dynamic,1      ,Dynamic,2      >(0, 1, 2, 2));
	row(n.slice<Dynamic,1      ,Dynamic,2      >(0, 1, 2, 2));
	row(m.slice<Dynamic,1      ,Dynamic,Dynamic>(0, 1, 2, 2));
	row(n.slice<Dynamic,1      ,Dynamic,Dynamic>(0, 1, 2, 2));
	row(m.slice<Dynamic,Dynamic,2      ,2      >(0, 1, 2, 2));
	row(n.slice<Dynamic,Dynamic,2      ,2      >(0, 1, 2, 2));
	row(m.slice<Dynamic,Dynamic,2      ,Dynamic>(0, 1, 2, 2));
	row(n.slice<Dynamic,Dynamic,2      ,Dynamic>(0, 1, 2, 2));
	row(m.slice<Dynamic,Dynamic,Dynamic,2      >(0, 1, 2, 2));
	row(n.slice<Dynamic,Dynamic,Dynamic,2      >(0, 1, 2, 2));
	row(m.slice<Dynamic,Dynamic,Dynamic,Dynamic>(0, 1, 2, 2));
	row(n.slice<Dynamic,Dynamic,Dynamic,Dynamic>(0, 1, 2, 2));
}
