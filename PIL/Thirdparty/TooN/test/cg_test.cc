#include <TooN/optimization/golden_section.h>
#include <TooN/optimization/conjugate_gradient.h>
#include <iostream>
#include <cmath>
#include <cassert>
#include <cstdlib>

using namespace TooN;
using namespace std;

double sq(double d)
{
	return d*d;
}

double Rosenbrock(const Vector<2>& v)
{
		return sq(1 - v[0]) + 100 * sq(v[1] - sq(v[0]));
}

Vector<2> RosenbrockDerivatives(const Vector<2>& v)
{
	double x = v[0];
	double y = v[1];

	Vector<2> ret;
	ret[0] = -2+2*x-400*(y-sq(x))*x;
	ret[1] = 200*y-200*sq(x);

	return ret;
}

int evals=0;

double Spiral(const Vector<2>& v)
{
	double x = v[0];
	double y = v[1];
	evals++;
	
	return sin(20.0*sqrt(x*x+y*y)+2.0*atan(y/x))+2.0*x*x+2.0*y*y;
}


Vector<2> SpiralDerivatives(const Vector<2>& v)
{
	double x = v[0];
	double y = v[1];

	double dx =2.0*(10.0*cos(20.0*sqrt(x*x+y*y)+2.0*atan(y/x))*x*x*x+10.0*cos(20.0*sqrt(x*x+y*y)+2.0*atan(y/x))*x*y*y-cos(20.0*sqrt(x*x+y*y)+2.0*atan(y/x))*y*sqrt(x*x+y*y)+2.0*x*x*x*sqrt(x*x+y*y)+2.0*x*sqrt(x*x+y*y)*y*y)/sqrt(pow(x*x+y*y,3.0));
	double dy =  2.0*(10.0*cos(20.0*sqrt(x*x+y*y)+2.0*atan(y/x))*y*x*x+10.0*cos(20.0*sqrt(x*x+y*y)+2.0*atan(y/x))*y*y*y+cos(20.0*sqrt(x*x+y*y)+2.0*atan(y/x))*x*sqrt(x*x+y*y)+2.0*y*sqrt(x*x+y*y)*x*x+2.0*y*y*y*sqrt(x*x+y*y))/sqrt(pow(x*x+y*y,3.0));

	return makeVector(dx, dy);
}

int main()
{
	ConjugateGradient<2> cg(makeVector(1.5, 1.5), Spiral, SpiralDerivatives);
	cg.bracket_initial_lambda=1e-7; //Prevent the bracket from jumping over spiral arms for a prettier display

	cout << cg.x << " " << cg.y << endl;
	while(cg.iterate(Spiral, SpiralDerivatives))
		cout << cg.x << " " << cg.y << endl;
	cout << cg.x << " " << cg.y << endl;

	cerr << "Total evaluations: " << evals << endl;
}

