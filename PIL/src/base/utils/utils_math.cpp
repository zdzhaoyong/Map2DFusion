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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_OPENMP
#include <omp.h>
#endif


// dSFMT (random number generator)
#define DSFMT_MEXP  19937
#define HAVE_SSE2
#include "dSFMT/dSFMT.h"


#include "base/debug/debug_config.h"

#include "utils_math.h"



namespace pi {

////////////////////////////////////////////////////////////////////////////////
/// angles
////////////////////////////////////////////////////////////////////////////////

double angleNormalize(double a)
{
    if( a < 0.0 ) {
        while(1) {
            a += 360.0;
            if( a >= 0 ) break;
        }
    } else if( a > 360.0 ) {
        while(1) {
            a -= 360.0;
            if( a < 360.0 ) break;
        }
    }

    return a;
}


////////////////////////////////////////////////////////////////////////////////
/// Vector
////////////////////////////////////////////////////////////////////////////////

double calc_dis3(double *a, double *b)
{
    double      d, v1, v2, v3;

    v1 = a[0] - b[0];
    v2 = a[1] - b[1];
    v3 = a[2] - b[2];

    d = sqrt(v1*v1 + v2*v2 + v3*v3);
    return d;
}

double vec3_dot_product(double *a, double *b)
{
    double  v;

    v = a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
    return v;
}

void vec3_cross_product(double *a, double *b, double *c)
{
    c[0] = a[1]*b[2] - a[2]*b[1];
    c[1] = a[2]*b[0] - a[0]*b[2];
    c[2] = a[0]*b[1] - a[1]*b[0];
}

////////////////////////////////////////////////////////////////////////////////
/// Geometry
////////////////////////////////////////////////////////////////////////////////

///
/// \brief Calculate triangle area
///
/// \param p1   - point 1 (3D)
/// \param p2   - point 2
/// \param p3   - point 3
///
/// \return     - triangle area
///
double triangle3_area(double *p1, double *p2, double *p3)
{
    double 	aa, bb, cc,
            s, S, q;

    aa=calc_dis3(p1, p2);
    bb=calc_dis3(p2, p3);
    cc=calc_dis3(p3, p1);

    s = (aa+bb+cc)/2;
    q = s*(s-aa)*(s-bb)*(s-cc);
    if( q < 0.0 )
        q = 0.0;
    S = sqrt(q);

    return S;
}


///
/// \brief Dijkstra algorithm (simple version)
///
/// \param n        - nodes number
/// \param v        - the source node index
/// \param dist     - the distance from the ith node to the source node
/// \param prev     - the previous node of the ith node
/// \param c        - every two nodes' distance
///
void Dijkstra(int n,
              int v,
              float *dist, int *prev,
              float *c)
{
    int     i, j;
    int     *s;

    float   tmp, newdist;
    int     u;

    s = new int[n];

    // init s, dist, prev array
    for(i=0; i<n; i++) {
        dist[i] = c[v*n+i];
        s[i] = 0;

        if( dist[i] == DIJKSTRA_MAX_DIS )
            prev[i] = 0;
        else
            prev[i] = v;
    }

    // set source node distance & visit
    dist[v] = 0;
    s[v] = 1;

    // find all un-visited node and calc nearest distance
    for(i=1; i<n; i++) {
        tmp = DIJKSTRA_MAX_DIS;
        u = v;

        // find nearest node which not visited
        for(j=0; j<n; j++) {
            if( s[j]==0 && dist[j]<tmp ) {
                u = j;
                tmp = dist[j];
            }
        }
        s[u] = 1;    // put node u to s

        // update dist array
        for(j=0; j<n; j++) {
            if( s[j]==0 && c[u*n+j]<DIJKSTRA_MAX_DIS ) {
                newdist = dist[u] + c[u*n+j];
                if( newdist < dist[j] ) {
                    dist[j] = newdist;
                    prev[j] = u;
                }
            }
        }
    }

    delete [] s;
}



////////////////////////////////////////////////////////////////////////////////
/// Normal random generator
////////////////////////////////////////////////////////////////////////////////

/**
 *  Generate a random seed by current time
 */
int GetRandSeed(void)
{
    unsigned int    ct;
    int             i, j, n;

    //ct = tm_get_millis();
    struct timeval  tm_val;
    unsigned int    isec;

    gettimeofday(&tm_val, NULL);
    isec = tm_val.tv_sec;
    ct = ((isec%1000) * 1000000 + tm_val.tv_usec);

    srand(ct);

    // skip first n random numbers
    n = ct % 9999;
    for(i=0; i<n; i++) rand();

    // return seed number
    j = rand();
    return j%9999;
}

/**
 *  Set random seed
 */
void RandSeed(int rseed)
{
    int     iseed, i;

    if( rseed == 0 ) iseed = GetRandSeed();
    else             iseed = rseed;

    srand(iseed);

    if( iseed >= 10000 )
        iseed = iseed % 9999;

    for(i=0; i<iseed; i++)
        rand();
}

/**
 *  Generate a random array in range [0:1]
 *
 *  Parameters:
 *      r           [out] random array
 *      n           [in] random data numbers
 *      rseed       [in] discard random data numbers
 *
 *  Return value:
 *      None
 *
 */
void RandomClose(double *r, int n, int rseed)
{
    double  rMax,rMin;
    int     i;

    // discard beginning random numbers
    for(i=0; i<rseed; i++)
        rand();

    // generate random number in range [rMin:rMax]
    r[0]=(double) rand();
    rMax=r[0];
    rMin=r[0];
    for(i=1;i<n;i++)
    {
        r[i] = (double)rand();
        if(rMax < r[i])
            rMax = r[i];
        if(rMin > r[i])
            rMin = r[i];
    }

    // chang the value range to [0:1]
    for(i=0;i<n;i++)
        r[i] = (r[i] - rMin)/(rMax - rMin);
}

/**
 *  Generate a random array (0:1)
 *
 *  Parameters:
 *      r           [out] random array
 *      n           [in] random data numbers
 *      rseed       [in] discard random data numbers
 *
 *  Return value:
 *      None
 *
 */
void RandomOpen(double *r,int n,int rseed)
{
    int i;

    RandomClose(r,n,rseed);
    for(i=0; i<n; i++){
       if(r[i] == 0)
           r[i] = 1.0e-5;
       if(r[i] == 1)
           r[i] = 0.99999;
    }
}

/**
 *  Generate a random array [-1:1]
 *
 *  Parameters:
 *      r           [out] random array
 *      n           [in] random data numbers
 *      rseed       [in] discard random data numbers
 *
 *  Return value:
 *      None
 *
 */
void RandomBoth(double *r, int n, int rseed)
{
    int i;

    //[0:1]
    RandomClose(r,n,rseed);
    //[0:1] -> [-1:1]
    for(i=0;i<n;i++)
        r[i] = 2*r[i] - 1.0;
}

/**
 *  Generate a random array (-1:1)
 *
 *  Parameters:
 *      r           [out] random array
 *      n           [in] random data numbers
 *      rseed       [in] discard random data numbers
 *
 *  Return value:
 *      None
 *
 */
void RandomBothOpen(double *r,int n,int rseed)
{
   int i;
   RandomClose(r,n,rseed);
   for(i=0;i<n;i++){
      r[i]=2*r[i]-1.0;
      if(r[i]==1)
          r[i]=0.99999999;
      if(r[i]==-1)
          r[i]=-0.99999999;
   }
}

/**
 *Generate Gauss distributed random numbers
 *
 * Parameters:
 *      r           [out] random number array
 *      n           [in] data number
 *      mean        [in] gauss mean
 *      sd          [in] gauss stander diff
 *      rseed       [in] seed number, get ride of fist rseed random number
 */
void RandomGauss_(double *r, int n, double mean, double sd, int rseed)
{
    int     k, i, j, nd;
    double  sum, *rr;

    k  = 6;
    nd = n+k-1;

    rr = (double*) malloc(sizeof(double)*nd);

    RandomClose(rr, nd, rseed);

    for(i=0; i<=nd-k; i++){
        sum=0;
        for(j=i; j<i+k; j++) sum += rr[j];
        sum /= k;

        rr[i] = (sum-0.5)/sqrt(1.0/(12.0*k))*sd + mean;
    }

    for(i=0; i<n; i++) r[i] = rr[i];

    free(rr);
}

/**
 * Generate Gauss distributed random numbers
 *      http://moby.ihme.washington.edu/bradbell/mat2cpp/randn.cpp.xml
 *
 * Parameters:
 *      r           [out] random number array
 *      n           [in] data number
 *      mean        [in] gauss mean
 *      sd          [in] gauss stander diff
 *      rseed       [in] seed number, get ride of fist rseed random number
 */
void RandomGauss(double *r, int n, double mean, double sd, int rseed)
{
    int     i, j, k, nd;
    double  *rr;
    double  square, amp, angle;


    nd = n+2;
    rr = (double*) malloc(sizeof(double)*nd);

    RandomClose(rr, nd, rseed);

    k = 0;
    for(i = 0; i < n; i++) {
        if( k % 2 == 0 ) {
            square = - 2. * log( rr[k] );
            if( square < 0. )
                square = 0.;
            amp = sqrt(square);
            angle = 2. * M_PI * rr[k+1];
            r[i] = amp * sin( angle );
        }
        else
            r[i] = amp * cos( angle );

        k++;

    }

    free(rr);
}


#ifdef __SSE2__
//////////////////////////////////////////////////////////////////////
//  SIMD-oriented Fast Mersenne Twister (SFMT)
//      http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/index.html
//
//  open  - ( or )
//  close - [ or ]
//////////////////////////////////////////////////////////////////////

/**
 *  Create new descriptior
 *
 *  Parameters:
 *      None
 *  Return value:
 *      dsfmt_t pointer
 */
void*   rand_new_des(void)
{
    dsfmt_t     *p;

    p = new dsfmt_t;
    return p;
}

void    rand_free_des(void *d)
{
    dsfmt_t     *p;

    p = (dsfmt_t *) d;
    if( p != NULL )
        delete p;
}

ru32  rand_time_seed(void *d)
{
    ru32  ct;
    ru32  i, n;
    dsfmt_t *p;

    ru32  s;

    //ct = tm_get_millis();
    struct timeval  tm_val;
    unsigned int    isec;

    gettimeofday(&tm_val, NULL);
    isec = tm_val.tv_sec;
    ct = ((isec%1000) * 1000000 + tm_val.tv_usec);

    srand(ct);

    // skip first n random numbers
    n = ct % 9999;
    for(i=0; i<n; i++) rand();

    // set seed number
    s = (ru32) rand();

    // return seed number
    return s;
}

void    rand_seed(ru32 s, void *d)
{
    // get time seed
    if( s == 0 ) {
        s = rand_time_seed(d);
    }

    // set seed
    if( d == NULL ) {
        dsfmt_gv_init_gen_rand(s);
    } else {
        dsfmt_t *p = (dsfmt_t*) d;
        dsfmt_init_gen_rand(p, s);
    }
}

ru32  rand_uint32(void *d)
{
    if( d == NULL ) {
        return dsfmt_genrand_uint32(&dsfmt_global_data);
    } else {
        dsfmt_t *p = (dsfmt_t*) d;
        return dsfmt_genrand_uint32(p);
    }
}

double  rand_c0o1(void *d)
{
    if( d == NULL ) {
        return dsfmt_genrand_close1_open2(&dsfmt_global_data) - 1.0;
    } else {
        dsfmt_t *p = (dsfmt_t*) d;
        return dsfmt_genrand_close1_open2(p) - 1.0;
    }
}

double  rand_o0c1(void *d)
{
    if( d == NULL ) {
        return 2.0 - dsfmt_genrand_close1_open2(&dsfmt_global_data);
    } else {
        dsfmt_t *p = (dsfmt_t*) d;
        return 2.0 - dsfmt_genrand_close1_open2(p);
    }
}

double  rand_o0o1(void *d)
{
    if( d == NULL ) {
        return dsfmt_genrand_open_open(&dsfmt_global_data);
    } else {
        dsfmt_t *p = (dsfmt_t*) d;
        return dsfmt_genrand_open_open(p);
    }
}

void    rand_c0o1(double *r, int n, void *d)
{
    if( n >= DSFMT_N64 ) {
        int     nn;

        if( n % 2 != 0 ) nn = n - 1;
        else             nn = n;

        if( d == NULL ) {
            dsfmt_fill_array_close_open(&dsfmt_global_data, r, nn);
        } else {
            dsfmt_t *p = (dsfmt_t*) d;
            dsfmt_fill_array_close_open(p, r, nn);
        }

        if( n % 2 != 0 ) r[n-1] = rand_c0o1(d);
    } else {
        for(int i=0; i<n; i++) r[i] = rand_c0o1(d);
    }
}

void    rand_o0c1(double *r, int n, void *d)
{
    if( n >= DSFMT_N64 ) {
        int     nn;

        if( n % 2 != 0 ) nn = n - 1;
        else             nn = n;

        if( d == NULL ) {
            dsfmt_fill_array_open_close(&dsfmt_global_data, r, nn);
        } else {
            dsfmt_t *p = (dsfmt_t*) d;
            dsfmt_fill_array_open_close(p, r, nn);
        }

        if( n % 2 != 0 ) r[n-1] = rand_o0c1(d);
    } else {
        for(int i=0; i<n; i++) r[i] = rand_o0c1(d);
    }
}

void    rand_o0o1(double *r, int n, void *d)
{
    if( n >= DSFMT_N64 ) {
        int     nn;

        if( n % 2 != 0 ) nn = n - 1;
        else             nn = n;

        if( d == NULL ) {
            dsfmt_fill_array_open_open(&dsfmt_global_data, r, nn);
        } else {
            dsfmt_t *p = (dsfmt_t*) d;
            dsfmt_fill_array_open_open(p, r, nn);
        }

        if( n % 2 != 0 ) r[n-1] = rand_o0o1(d);
    } else {
        for(int i=0; i<n; i++) r[i] = rand_o0o1(d);
    }
}

double rand_d(void *d)
{
    return rand_o0c1(d);
}

void rand_d(double *r, int n, void *d)
{
    rand_o0c1(r, n, d);
}

void random(double *r, int n, void *d)
{
    rand_o0c1(r, n, d);
}

/**
 *  Normalized random number
 *  FIXME: if the input n is short, it random will be not correct
 *
 *  Parameters:
 *      \param[out]         r       random number array
 *      \param[in]          n       length
 *      \param[in]          mean    mean value
 *      \param[in]          sd      sigma
 *      \param[in]          d       random handle
 *  Return Value:
 *      None
 */
void randn_(double *r, int n, double mean, double sd, void *d)
{
    int     k, i, j, nd;
    double  sum, *rr;

    k  = 6;
    nd = n+k-1;
    rr = (double*) malloc(sizeof(double)*nd);

    rand_o0c1(rr, nd, d);

    for(i=0; i<=nd-k; i++) {
        sum=0;
        for(j=i; j<i+k; j++) sum += rr[j];
        sum /= k;

        rr[i] = (sum-0.5)/sqrt(1.0/(12.0*k))*sd + mean;
    }

    for(i=0; i<n; i++) r[i] = rr[i];

    free(rr);
}

/**
 *  Normalized random number
 *      FIXME: if the input n is short, it random will be not correct
 *
 *      http://moby.ihme.washington.edu/bradbell/mat2cpp/randn.cpp.xml
 *
 *  Parameters:
 *      \param[out]         r       random number array
 *      \param[in]          n       length
 *      \param[in]          mean    mean value
 *      \param[in]          sd      sigma
 *      \param[in]          d       random handle
 *  Return Value:
 *      None
 */
void randn(double *r, int n, double mean, double sd, void *d)
{
    int     i, j, k, nd;
    double  *rr;
    double  square, amp, angle;


    nd = n+2;
    rr = (double*) malloc(sizeof(double)*nd);

    rand_o0c1(rr, nd, d);

    k = 0;
    for(i = 0; i < n; i++) {
        if( k % 2 == 0 ) {
            square = - 2. * log( rr[k] );
            if( square < 0. )
                square = 0.;
            amp = sqrt(square);
            angle = 2. * M_PI * rr[k+1];
            r[i] = amp * sin( angle );
        }
        else
            r[i] = amp * cos( angle );

        k++;
    }

    free(rr);
}



/**
 *  Generate random permutation
 *
 *  Parameters:
 *      \param[out]         r       random permutation
 *      \param[in]          n       array size
 */
void randperm(int *r, int n, void *d)
{
    int         i;
    double      *da;

    da = new double[n];
    for(i=0; i<n; i++) r[i] = i;

    rand_o0c1(da, n, d);
    quick_sort_di(da, r, 0, n-1);

    delete [] da;
}

#endif

////////////////////////////////////////////////////////////////////////////////
/// quick sort
////////////////////////////////////////////////////////////////////////////////

/**
 * double array quick sort
 *
 * Parameters:
 *      \param[in,out]      r       double type array
 *      \param[in]          first   array begin index
 *      \param[in]          last    array end index
 */
void quick_sort(double *r, int first, int last)
{
    double x,t;
    int i,j;

    x=r[(first+last)/2];
    i=first;
    j=last;
    for(;;) {
        while(r[i]<x) i++;
        while(r[j]>x) j--;
        if(i>=j) break;

        t=r[i]; r[i]=r[j]; r[j]=t;
        i++;
        j--;
    }

    if(first<i-1) quick_sort(r,first,i-1);
    if(j+1<last)  quick_sort(r,j+1,last);
}

/**
 *  Sort integer array i
 *
 *  Parameters:
 *      \param[in,out]      ia1     integer array
 *      \param[in]          first   array begin index
 *      \param[in]          last    array end index
 */
void quick_sort_i(int *ia1, ri64 first, ri64 last)
{
    int     x, t;
    ri64   i, j;

    x=ia1[(first+last)/2];
    i=first;
    j=last;
    for(;;) {
        while(ia1[i]<x) i++;
        while(ia1[j]>x) j--;
        if(i>=j) break;

        t=ia1[i]; ia1[i]=ia1[j]; ia1[j]=t;

        i++;
        j--;
    }

    if(first<i-1) quick_sort_i(ia1, first, i-1);
    if(j+1<last)  quick_sort_i(ia1, j+1, last);
}


/**
 *  Sort array ia1, meanwile da1 are also moved
 *
 *  Paramters:
 *      \param[in,out]      ia1     integer array 1 (master)
 *      \param[in,out]      da1     double array 1
 *      \param[in]          first   index of first item
 *      \param[in]          last    index of last item
 */
void quick_sort_id(int *ia1, double *da1, ri64 first, ri64 last)
{
    int     x, t;
    ri64   i, j;
    double  v;

    x=ia1[(first+last)/2];
    i=first;
    j=last;
    for(;;) {
        while(ia1[i]<x) i++;
        while(ia1[j]>x) j--;
        if(i>=j) break;

        t=ia1[i]; ia1[i]=ia1[j]; ia1[j]=t;
        v=da1[i]; da1[i]=da1[j]; da1[j]=v;

        i++;
        j--;
    }

    if(first<i-1) quick_sort_id(ia1, da1, first, i-1);
    if(j+1<last)  quick_sort_id(ia1, da1, j+1, last);
}


/**
 *  Sort array ia1, meanwile ia2, da1 are also moved
 *
 *  Paramters:
 *      \param[in,out]      ia1     integer array 1 (master)
 *      \param[in,out]      ia2     integer array 2
 *      \param[in,out]      da1     double array 1
 *      \param[in]          first   index of first item
 *      \param[in]          last    index of last item
 */
void quick_sort_iid(int *ia1, int *ia2, double *da1, ri64 first, ri64 last)
{
    int     x, t;
    ri64   i, j;
    double  v;

    x=ia1[(first+last)/2];
    i=first;
    j=last;
    for(;;) {
        while(ia1[i]<x) i++;
        while(ia1[j]>x) j--;
        if(i>=j) break;

        t=ia1[i]; ia1[i]=ia1[j]; ia1[j]=t;
        t=ia2[i]; ia2[i]=ia2[j]; ia2[j]=t;
        v=da1[i]; da1[i]=da1[j]; da1[j]=v;

        i++;
        j--;
    }

    if(first<i-1) quick_sort_iid(ia1, ia2, da1, first, i-1);
    if(j+1<last)  quick_sort_iid(ia1, ia2, da1, j+1, last);
}


/**
 *  sort array da1, meanwile ia1 is also moved (ascending order)
 *
 *  Parameters:
 *      \param[in,out]  da1         input double type array
 *      \param[in,out]  ia1         input integer type array
 *      \param[in]      first       index of first item
 *      \param[in]      last        index of last item
 */
void quick_sort_di(double *da1, int *ia1, ri64 first, ri64 last)
{
    double  x, t;
    ri64   i, j;
    int     v;

    x=da1[(first+last)/2];
    i=first;
    j=last;
    for(;;) {
        while(da1[i]<x) i++;
        while(da1[j]>x) j--;
        if(i>=j) break;

        t=da1[i]; da1[i]=da1[j]; da1[j]=t;
        v=ia1[i]; ia1[i]=ia1[j]; ia1[j]=v;

        i++;
        j--;
    }

    if(first<i-1) quick_sort_di(da1, ia1, first, i-1);
    if(j+1<last)  quick_sort_di(da1, ia1, j+1, last);
}


////////////////////////////////////////////////////////////////////////////////
/// interpolation
////////////////////////////////////////////////////////////////////////////////

/******************************************************************************\
 * 1-D spline interpolation ( calculate coefficients)
 *
 * Parameters:
 *      x           [in] X coordinates
 *      y           [in] Y values
 *      n           [in] Data number
 *      yp1         [in] First derivative of first point
 *      ypn         [in] First derivative of the last point
 *      y2          [out] Second derivatives of the interpolating function
\******************************************************************************/
void spline(double *_x, double *_y, int n, double yp1, double ypn, double *_y2)
{
    double  *x, *y, *y2;
    int     i, k;
    double  p, qn, sig, un, *u;

    x  = _x - 1;
    y  = _y - 1;
    y2 = _y2 - 1;

    u = (double*) malloc(sizeof(double)*n);

    // first point
    if (yp1 > 0.99e30)
        y2[1]=u[1]=0.0;
    else {
        y2[1] = -0.5;
        u[1]=(3.0/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
    }

    // 2~n-1 points
    for (i=2;i<=n-1;i++) {
        sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
        p=sig*y2[i-1]+2.0;
        y2[i]=(sig-1.0)/p;
        u[i]=(y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
        u[i]=(6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
    }

    // last point
    if (ypn > 0.99e30)
        qn=un=0.0;
    else {
        qn=0.5;
        un=(3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
    }

    y2[n]=(un-qn*u[n-1])/(qn*y2[n-1]+1.0);
    for (k=n-1;k>=1;k--)
        y2[k]=y2[k]*y2[k+1]+u[k];

    free(u);
}


/******************************************************************************\
 * 1-D Spline interpolation
 *
 * Prameters:
 *      xa          [in] x coordinate
 *      ya          [in] y values
 *      y2a         [in] second derivative
 *      n           [in] data lenght
 *      x           [in] sampling point x coordinate
 *      y           [out] output value at x
\******************************************************************************/
void splint_s(double *_xa, double *_ya, double *_y2a, int n, double x, double *y)
{
    double  *xa, *ya, *y2a;

    int     klo, khi, k;
    double  h, b, a;

    xa = _xa - 1;
    ya = _ya - 1;
    y2a = _y2a - 1;

    klo=1;
    khi=n;
    while (khi-klo > 1) {
        k=(khi+klo) >> 1;
        if (xa[k] > x) khi=k;
        else klo=k;
    }

    h=xa[khi]-xa[klo];
    if (h == 0.0) {
        dbg_pe("Bad xa input to routine splint");
        exit(1);
    }

    a=(xa[khi]-x)/h;
    b=(x-xa[klo])/h;
    *y= a*ya[klo] + b*ya[khi] +
        ((a*a*a-a)*y2a[klo] + (b*b*b-b)*y2a[khi])*(h*h)/6.0;
}

/******************************************************************************\
 * 1-D Spline interpolation
 *
 * Prameters:
 *      xa          [in] x coordinate
 *      ya          [in] y values
 *      xia         [in] interpolated x array
 *      yia         [out] interpolated y array
 *      n           [in] data length
 *      ni          [in] interpolated length
\******************************************************************************/
void splint(double *_xa, double *_ya,
            double *_xia, double *_yia,
            int n, int ni,
            double yp1, double ypn)
{
    double  *xa, *ya;
    double  *xia, *yia;
    double  *y2a, *_y2a;

    int     klo, khi, k, j;
    double  h, b, a;
    double  x;

    // calculate second derivtive
    _y2a = (double*) malloc(sizeof(double)*n);
    y2a = _y2a - 1;
    spline(_xa, _ya, n, yp1, ypn, _y2a);

    // interpolate each point
    xa  = _xa - 1;
    ya  = _ya - 1;
    xia = _xia - 1;
    yia = _yia - 1;

    klo = 1;
    khi = klo + 1;

    for(j=1; j<=ni; j++) {
        x = xia[j];

#if 1
        // update klo
        while( klo+1<=n && x>xa[klo+1]) {
            klo ++;
        }

        // update khi
        while( x>xa[khi] ) {
            khi ++;
        }
#else
        klo=1;
        khi=n;
        while (khi-klo > 1) {
            k=(khi+klo) >> 1;
            if (xa[k] > x) khi=k;
            else klo=k;
        }
#endif

        h=xa[khi]-xa[klo];
        if (h == 0.0) {
            dbg_pe("Bad xa input to routine splint");
            exit(1);
        }

        a = (xa[khi]-x)/h;
        b = (x-xa[klo])/h;
        yia[j] = a*ya[klo] + b*ya[khi] +
            ((a*a*a-a)*y2a[klo] + (b*b*b-b)*y2a[khi])*(h*h)/6.0;
    }

    free(_y2a);
}

/******************************************************************************\
 * 1-D Spline interpolation
 *
 * Prameters:
 *      ya          [in] input data
 *      y           [in] interpolated data
 *      n1          [in] input data length
 *      ni1         [in] interpolated length
\******************************************************************************/
void splint_a(double *ya, double *y, int n1, int ni1)
{
    double      *x1a, *xi1a;
    int         i;

    x1a = new double[n1];
    xi1a = new double[ni1];

    for(i=0; i<n1; i++)  x1a[i]  = i*1.0/(n1-1);
    for(i=0; i<ni1; i++) xi1a[i] = i*1.0/(ni1-1);

    splint( x1a,  ya,
            xi1a, y,
            n1, ni1);

    delete [] x1a;
    delete [] xi1a;
}

/******************************************************************************\
 * 2-D spline interpolation
 *
 * Parameters:
 *      x1a             [in] axis-1 coordinate
 *      x2a             [in] axis-2 coordinate
 *      ya              [in] values
 *      xi1a            [in] interpolate axis-1 coordinate
 *      xi2a            [in] interpolate axis-2 coordinate
 *      y               [out] interpolated values
 *      n1              [in] data number in axis-1
 *      n2              [in] data number in axis-2
 *      ni1             [in] data number in interpolated axis-1
 *      ni2             [in] data number in interpolated axis-2
\******************************************************************************/
void splint2(double *x1a, double *x2a, double *ya,
             double *xi1a, double *xi2a, double *y,
             int n1, int n2, int ni1, int ni2)
{
    int     i, j;
    double  *ytmp, *p1, *p2;

    // allocate buffer
    ytmp = new double[n1*ni2];
    p1   = new double[n1];
    p2   = new double[ni1];

    // interpolation in row
    for (j=0; j<n1; j++) {
        splint(x2a, ya+j*n2, xi2a, ytmp+j*ni2, n2, ni2);
    }

    // interpolation in col
    for (i=0; i<ni2; i++) {
        for(j=0; j<n1; j++)
            p1[j] = ytmp[j*ni2+i];

        splint(x1a, p1, xi1a, p2, n1, ni1);

        for(j=0; j<ni1; j++)
            y[j*ni2+i] = p2[j];
    }

    // free temp buffer
    delete [] ytmp;
    delete [] p1;
    delete [] p2;
}

void splint2_a(double *ya, double *y,
                int n1, int n2, int ni1, int ni2)
{
    double      *x1a, *x2a, *xi1a, *xi2a;
    int         i;

    x1a = new double[n1];
    x2a = new double[n2];
    xi1a = new double[ni1];
    xi2a = new double[ni2];

    for(i=0; i<n1; i++)  x1a[i]  = i*1.0/(n1-1);
    for(i=0; i<n2; i++)  x2a[i]  = i*1.0/(n2-1);
    for(i=0; i<ni1; i++) xi1a[i] = i*1.0/(ni1-1);
    for(i=0; i<ni2; i++) xi2a[i] = i*1.0/(ni2-1);

    splint2(x1a, x2a, ya,
            xi1a, xi2a, y,
            n1, n2, ni1, ni2);

    delete [] x1a;
    delete [] x2a;
    delete [] xi1a;
    delete [] xi2a;
}


/******************************************************************************\
 * 3-D spline interpolation
 *
 * Parameters:
 *      x1a             [in] axis-1 coordinate (chage slowest)
 *      x2a             [in] axis-2 coordinate
 *      x3a             [in] axis-3 coordinate (change fast)
 *      ya              [in] values
 *      xi1a            [in] interpolate axis-1 coordinate
 *      xi2a            [in] interpolate axis-2 coordinate
 *      xi3a            [in] interpolate axis-3 coordinate
 *      y               [out] interpolated values
 *      n1              [in] data number in axis-1
 *      n2              [in] data number in axis-2
 *      n3              [in] data number in axis-3
 *      ni1             [in] data number in interpolated axis-1
 *      ni2             [in] data number in interpolated axis-2
 *      ni3             [in] data number in interpolated axis-3
\******************************************************************************/
void splint3(double *x1a, double *x2a, double *x3a, double *ya,
             double *xi1a, double *xi2a, double *xi3a, double *y,
             int n1, int n2, int n3, int ni1, int ni2, int ni3)
{
    int     i, j, k;
    double  *ytmp, *p1, *p2;
    double  *pp1, *pp2;
    int     omp_tn, omp_ti;

#ifdef HAVE_OPENMP
    omp_tn = omp_get_max_threads();
#else
    omp_tn = 1;
#endif

    // allocate buffer
    ytmp = new double[n1*ni2*ni3];
    p1   = new double[n1*omp_tn];
    p2   = new double[ni1*omp_tn];

    // interpolation in axis-2 and axis-3
#ifdef HAVE_OPENMP
    #pragma omp parallel for private(k)
#endif
    for (k=0; k<n1; k++) {
        splint2(x2a, x3a, ya+k*n2*n3,
                xi2a, xi3a, ytmp+k*ni2*ni3,
                n2, n3, ni2, ni3);
    }

    // interpolation in axis1
#ifdef HAVE_OPENMP
    #pragma omp parallel for private(i,j,k,pp1,pp2,omp_ti)
#endif
    for (i=0; i<ni3; i++) {
        for (j=0; j<ni2; j++) {
#ifdef HAVE_OPENMP
            omp_ti = omp_get_thread_num();
#else
            omp_ti = 0;
#endif
            pp1 = p1 + n1*omp_ti;
            pp2 = p2 + ni1*omp_ti;

            for (k=0; k<n1; k++)
                pp1[k] = ytmp[k*ni2*ni3 + j*ni3 + i];

            splint(x1a, pp1, xi1a, pp2, n1, ni1);

            for (k=0; k<ni1; k++)
                y[k*ni2*ni3 + j*ni3 + i] = pp2[k];
        }
    }

    // free temp buffer
    delete [] ytmp;
    delete [] p1;
    delete [] p2;
}

void splint3_a(double *ya, double *y,
                int n1,  int n2,  int n3,
                int ni1, int ni2, int ni3)
{
    double      *x1a, *x2a, *x3a, *xi1a, *xi2a, *xi3a;
    int         i;

    x1a = new double[n1];
    x2a = new double[n2];
    x3a = new double[n3];
    xi1a = new double[ni1];
    xi2a = new double[ni2];
    xi3a = new double[ni3];

    for(i=0; i<n1; i++)  x1a[i]  = i*1.0/(n1-1);
    for(i=0; i<n2; i++)  x2a[i]  = i*1.0/(n2-1);
    for(i=0; i<n3; i++)  x3a[i]  = i*1.0/(n3-1);
    for(i=0; i<ni1; i++) xi1a[i] = i*1.0/(ni1-1);
    for(i=0; i<ni2; i++) xi2a[i] = i*1.0/(ni2-1);
    for(i=0; i<ni3; i++) xi3a[i] = i*1.0/(ni3-1);

    splint3(x1a, x2a, x3a, ya,
            xi1a, xi2a, xi3a, y,
            n1, n2, n3, ni1, ni2, ni3);

    delete [] x1a;
    delete [] x2a;
    delete [] x3a;
    delete [] xi1a;
    delete [] xi2a;
    delete [] xi3a;
}


} // end of namespace pi
