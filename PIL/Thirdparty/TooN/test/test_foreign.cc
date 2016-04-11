#include <TooN/TooN.h>
#include <TooN/helpers.h>

using namespace std;
using namespace TooN;

int main()
{
	double data[]={1, 2, 3, 4, 5, 6};


	cout << Matrix<2,3,double, Reference::RowMajor> (data) << endl;
	cout << Matrix<2,-1,double, Reference::RowMajor> (data,2,3) << endl;
	cout << Matrix<-1,3,double, Reference::RowMajor> (data,2,3) << endl;
	cout << Matrix<-1,-1,double, Reference::RowMajor> (data,2,3) << endl;



	cout << Matrix<2,3,double, Reference::ColMajor> (data) << endl;
	cout << Matrix<2,-1,double, Reference::ColMajor> (data,2,3) << endl;
	cout << Matrix<-1,3,double, Reference::ColMajor> (data,2,3) << endl;
	cout << Matrix<-1,-1,double, Reference::ColMajor> (data,2,3) << endl;

	cout << wrapVector<6>(data) << endl;
	cout << wrapVector(data, 6) << endl;
	cout << Vector<6,double,Reference> (data) << endl;
	cout << Vector<-1,double,Reference> (data,6) << endl;

};
