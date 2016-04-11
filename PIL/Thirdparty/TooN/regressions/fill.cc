#include <TooN/TooN.h>
using namespace TooN;
using namespace std;

int main()
{
	{
		Matrix <3,2> A;
		Fill(A) =  1,2,
				   3,4,
				   5,6;

		cout << A << endl;

		Matrix <2,3> B;
		Fill(B) =  1,2,3,
				   4,5,6;

		cout << B << endl;

		Matrix <3,2,float,ColMajor> C;
		Fill(C) =  1,2,
				   3,4,
				   5,6;

		cout << C << endl;

		Matrix <2,3, float,ColMajor> D;
		Fill(D) =  1,2,3,
				   4,5,6;

		cout << D << endl;
	}
	{
		Matrix<> A(3,2);
		Fill(A) =  1,2,
				   3,4,
				   5,6;

		cout << A << endl;

		Matrix<> B(2,3);
		Fill(B) =  1,2,3,
				   4,5,6;

		cout << B << endl;

		Matrix <Dynamic,Dynamic,float,ColMajor> C(3,2);
		Fill(C) =  1,2,
				   3,4,
				   5,6;

		cout << C << endl;

		Matrix <Dynamic,Dynamic, float,ColMajor> D(2,3);
		Fill(D) =  1,2,3,
				   4,5,6;

		cout << D << endl;
	}
}
