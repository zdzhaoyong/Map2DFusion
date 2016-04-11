#include <TooN/optimization/downhill_simplex.h>
#include <iomanip>
using namespace std;
using namespace TooN;


double sq(double x)
{
	return x*x;
}

double Rosenbrock(const Vector<2>& v)
{
		return sq(1 - v[0]) + 100 * sq(v[1] - sq(v[0]));
}

double Spiral(const Vector<2>& v)
{
	double x = v[0];
	double y = v[1];
	return sin(20.0*sqrt(x*x+y*y)+2.0*atan(y/x))+2.0*x*x+2.0*y*y;
}


int main()
{
	cout << setprecision(16);
	Vector<2> starting_point = makeVector(1.5, 1.5);

	DownhillSimplex<2> dh_fixed(Spiral, starting_point, .001);
	
	cout << "#> ignore" << endl;

	while(dh_fixed.iterate(Spiral))
	{
		cout << dh_fixed.get_simplex()[0] << dh_fixed.get_values()[0] << endl;
		cout << dh_fixed.get_simplex()[1] << dh_fixed.get_values()[1] << endl;
		cout << dh_fixed.get_simplex()[2] << dh_fixed.get_values()[2] << endl;
		cout << endl;
	}

	cout << "#> resume" << endl;

	cout << dh_fixed.get_simplex()[dh_fixed.get_best()] << endl
	     << dh_fixed.get_values()[dh_fixed.get_best()] << endl;

	DownhillSimplex<> dh_variable(Spiral, starting_point, .001);

	while(dh_variable.iterate(Spiral))
	{}

	cout << dh_variable.get_simplex()[dh_variable.get_best()] << endl
	     << dh_variable.get_values()[dh_variable.get_best()] << endl;
}


