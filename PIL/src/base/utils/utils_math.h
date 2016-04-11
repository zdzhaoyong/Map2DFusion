/******************************************************************************

  Robot Toolkit ++ (RTK++)

  Copyright (c) 2007-2013 Shuhui Bu <bushuhui@nwpu.edu.cn>
  http://www.adv-ci.com

  ----------------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

#ifndef __UTILS_MATH__
#define __UTILS_MATH__

#include "base/types/types.h"

namespace pi {

////////////////////////////////////////////////////////////////////////////////
/// angles
////////////////////////////////////////////////////////////////////////////////
#define DEG2RAD         M_PI/180.0
#define RAD2DEG         180.0/M_PI

// normalize angle to [0, 360]
double angleNormalize(double a);

////////////////////////////////////////////////////////////////////////////////
/// Vector
////////////////////////////////////////////////////////////////////////////////
double calc_dis3(double *a, double *b);
double vec3_dot_product(double *a, double *b);
void   vec3_cross_product(double *a, double *b, double *c);


////////////////////////////////////////////////////////////////////////////////
/// Geometry
////////////////////////////////////////////////////////////////////////////////
double triangle3_area(double *p1, double *p2, double *p3);

// Simple Dijkstra algorithm
#define DIJKSTRA_MAX_DIS    1e60L        // max distance

void Dijkstra(int n, int v,
              float *dist, int *prev,
              float *c);



////////////////////////////////////////////////////////////////////////////////
/// Normal random generator
////////////////////////////////////////////////////////////////////////////////

// Generate a random seed by current time
int GetRandSeed(void);

// Set random seed
void RandSeed(int rseed=0);

// Generate a random array in range [0:1]
void RandomClose(double *r, int n, int rseed=0);

// Generate a random array (0:1)
void RandomOpen(double *r,int n,int rseed=0);

// Generate a random array [-1:1]
void RandomBoth(double *r, int n, int rseed=0);

// Generate a random array (-1:1)
void RandomBothOpen(double *r,int n,int rseed=0);

// Generate Gauss distributed random numbers
void RandomGauss(double *r, int n, double mean=0.0, double sd=1.0, int rseed=0);



////////////////////////////////////////////////////////////////////////////////
///  SIMD-oriented Fast Mersenne Twister (SFMT) random number generator
///      http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/index.html
///
///  open  - ( or )
///  close - [ or ]
////////////////////////////////////////////////////////////////////////////////

void*   rand_new_des(void);
void    rand_free_des(void *d);

ru32    rand_time_seed(void *d=NULL);
void    rand_seed(ru32 s=0, void *d=NULL);

ru32    rand_uint32(void *d=NULL);

double  rand_c0o1(void *d=NULL);
double  rand_o0c1(void *d=NULL);
double  rand_o0o1(void *d=NULL);

void    rand_c0o1(double *r, int n, void *d=NULL);
void    rand_o0c1(double *r, int n, void *d=NULL);
void    rand_o0o1(double *r, int n, void *d=NULL);

double  rand_d(void *d=NULL);
void    rand_d(double *r, int n, void *d=NULL);
void    random(double *r, int n, void *d=NULL);
void    randn(double *r, int n, double mean=0.0, double sd=1.0, void *d=NULL);

// Generate random permutation
void    randperm(int *r, int n, void *d=NULL);



////////////////////////////////////////////////////////////////////////////////
/// quick sort
////////////////////////////////////////////////////////////////////////////////
// double array quick sort
void quick_sort(double *r, int first, int last);

// sort array ia
void quick_sort_i(int *ia1, ri64 first, ri64 last);

// Sort array ia1, meanwile da1 are also moved
void quick_sort_id(int *ia1, double *da1, ri64 first, ri64 last);

// Sort array ia1, meanwile ia2, da1 are also moved
void quick_sort_iid(int *ia1, int *ia2, double *da1, ri64 first, ri64 last);

// sort array da1, meanwile ia1 is also moved
void quick_sort_di(double *da1, int *ia1, ri64 first, ri64 last);



////////////////////////////////////////////////////////////////////////////////
/// interpolation
////////////////////////////////////////////////////////////////////////////////

// 1-D spline interpolation ( calculate coefficients)
void spline(double *_x, double *_y, int n, double yp1, double ypn, double *_y2);

// 1-D Spline interpolation ( for single point )
void splint_s(double *_xa, double *_ya, double *_y2a, int n, double x, double *y);

// 1-D Spline interpolation
void splint(double *_xa, double *_ya,
            double *_xia, double *_yia,
            int n, int ni,
            double yp1=1e31, double ypn=1e31);
void splint_a(double *ya, double *y, int n1, int ni1);

// 2-D spline interpolation
void splint2(double *x1a, double *x2a, double *ya,
             double *xi1a, double *xi2a, double *y,
             int n1, int n2, int ni1, int ni2);
// 2-D spline interpolation (automatic)
void splint2_a(double *ya, double *y,
                int n1, int n2, int ni1, int ni2);

// 3-D spline interpolation
void splint3(double *x1a, double *x2a, double *x3a, double *ya,
             double *xi1a, double *xi2a, double *xi3a, double *y,
             int n1, int n2, int n3, int ni1, int ni2, int ni3);
// 3-D spline interpolation (automatic)
void splint3_a(double *ya, double *y,
                int n1,  int n2,  int n3,
                int ni1, int ni2, int ni3);


////////////////////////////////////////////////////////////////////////////////
/// min/max and other
////////////////////////////////////////////////////////////////////////////////

// square value of x
template<class T>
T inline sqr(T x) { return x*x; }

template<class T>
T& max(T &a1, T &a2)
{
    if( a1 > a2 )
        return a1;
    else
        return a2;
}

template<class T>
T& max(T &a1, T &a2, T &a3)
{
    if( a1 > a2 ) {
        if( a1 > a3 )
            return a1;
        else
            return a3;
    } else {
        if( a2 > a3 )
            return a2;
        else
            return a3;
    }
}

template<class T>
T& min(T &a1, T &a2)
{
    if( a1 > a2 )
        return a2;
    else
        return a1;
}

template<class T>
T& min(T &a1, T &a2, T &a3)
{
    if( a1 > a2 ) {
        if( a2 > a3 )
            return a3;
        else
            return a2;
    } else {
        if( a1 > a3 )
            return a3;
        else
            return a1;
    }
}


} // end of namespace pi

#endif // end of __UTILS_MATH__
