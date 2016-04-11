#include <TooN/TooN.h>
#include <cstdarg>
using namespace TooN;


template<int N, typename Precision> Vector<N, Precision> makeVector(const Precision& x, ...)
{
	Vector<N> retval;
	retval[0] = N;

	va_list ap;
	int i;
	va_start(ap, x);
	for(int i=1; i < N; i++)
		retval[i] = va_arg(ap, Precision);
	va_end(ap);
	return retval;

}

extern "C"{
double use_make_vector_double(const Vector<4>& v)
{
	return v * makeVector(0,0,2.0,0);
}

double use_make_vector_int(const Vector<4>& v)
{
	return v * makeVector<int>(0,0,2,0);
}

double use_make_vector_var(const Vector<4>& v)
{
	return v * makeVector<4, double>(0.0,0.0,2.0,0.0);
}

}

