#include <TooN/functions/derivatives.h>

using namespace TooN;


double f(const Vector<2>& x)
{
	return (1 + x[1]*x[1])*cos(x[0]);
}

Vector<2> grad(const Vector<2>& x)
{
   return makeVector(-sin(x[0])*(x[1]*x[1] + 1), 2*x[1]*cos(x[0]));
}

Matrix<2> hess(const Vector<2>& x)
{

  return Data(-cos(x[0])*(x[1]*x[1] + 1), (-2)*x[1]*sin(x[0]),
                     (-2)*x[1]*sin(x[0]),      2*cos(x[0]));
}

template<int N> ostream& operator<<(ostream& o, const pair<Matrix<N>, Matrix<N> >& m)
{
	o << m.first << endl << m.second << endl;
	return o;
}

int main()
{
	cout << grad(Zeros) << endl;
	cout << numerical_gradient_with_errors(f, Vector<2>(Zeros)).T() << endl << endl;

	cout << hess(Zeros) << endl;
	cout << numerical_hessian_with_errors(f, Vector<2>(Zeros)) << endl;
	

	Vector<2> v;
	for(v[0] = -10; v[0] < 10; v[0] += 4.3)
		for(v[1] = -10; v[1] < 10; v[1] += 4.3)
		{
			cout << grad(v) << endl;
			cout << numerical_gradient(f, v) << endl;
			cout << norm_fro(hess(v) - numerical_hessian(f, v)) << endl << endl;
		}
}
