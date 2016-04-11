#include <TooN/optimization/brent.h>
#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace TooN;
using namespace std;



double f(double x)
{
	return abs(x-1.2) + .2 * sin(5*x);
}


int main()
{
	cout << brent_line_search(-3., .5, 3., f(.5), f, 100) << endl;

	cout << "Value should be: " << "  1.2000  -0.0559 " << endl;
}

