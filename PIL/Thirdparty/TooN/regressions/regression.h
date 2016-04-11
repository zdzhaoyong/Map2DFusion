#include <TooN/TooN.h>
#include <TooN/LU.h>
#include <TooN/SVD.h>
#include <TooN/QR_Lapack.h>
#include <TooN/helpers.h>
#include <TooN/determinant.h>
#include <iomanip>
#include <stdint.h>
#include <utility>
using namespace std;
using namespace TooN;

//Simple, not bad and repeatable RNG from Marsaglia, George (July 2003)
uint32_t xor128u() 
{   
	static uint32_t x = 123456789;   
	static uint32_t y = 362436069;   
	static uint32_t z = 521288629;   
	static uint32_t w = 88675123;   
	uint32_t t;   
	t = x ^ (x << 11);   
	x = y; 
	y = z; 
	z = w;   
	w = w ^ (w >> 19) ^ (t ^ (t >> 8)); 
	return w;
}

double xor128d()
{
	return xor128u() * (1. / 4292967296.);
}


