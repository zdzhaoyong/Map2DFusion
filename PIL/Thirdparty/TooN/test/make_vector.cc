#include <TooN/TooN.h>
using namespace TooN;

Vector<5> return_a_vector()
{
	return makeVector(1, 2, 3, 4, 5);
}

double return_a_double()
{
	return makeVector(4, 5, 6, 7)[3] -6;
}
