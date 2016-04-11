#include <TooN/helpers.h>

using namespace std;
using namespace TooN;

int main()
{
	{
		Vector<4> v;
		
		v = makeVector(1,1,1,1);
		normalize(v);
		cout << v << endl;

		v = makeVector(1,1,1,1);
		normalize(v.slice<0,2>());
		cout << v << endl;

		v = makeVector(1,1,1,1);
		normalize(v.slice(0,3));
		cout << v << endl;
	}
	
	{
		Vector<> v = makeVector(1,1,1,1);
		
		normalize(v);
		cout << v << endl;

		v = makeVector(1,1,1,1);
		normalize(v.slice<0,2>());
		cout << v << endl;

		v = makeVector(1,1,1,1);
		normalize(v.slice(0,3));
		cout << v << endl;
	}
}
