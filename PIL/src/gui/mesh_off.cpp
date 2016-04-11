
/* different algorithms; multiple sources and targets
    Danil Kirsanov, 01/2008
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "mesh_off.h"


#define sqr(x) ((x)*(x))


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
double calc_dis(double *a, double *b)
{
    double      d, v1, v2, v3;

    v1 = a[0] - b[0];
    v2 = a[1] - b[1];
    v3 = a[2] - b[2];

    d = sqrt(v1*v1 + v2*v2 + v3*v3);
    return d;
}

double vec_dot_product(double *a, double *b)
{
    double  v;

    v = a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
    return v;
}

void vec_cross_product(double *a, double *b, double *c)
{
    c[0] = a[1]*b[2] - a[2]*b[1];
    c[1] = a[2]*b[0] - a[0]*b[2];
    c[2] = a[0]*b[1] - a[1]*b[0];
}


double triangle_area(double *p1, double *p2, double *p3)
{
    double 	aa, bb, cc,
            s, S, q;

    aa=calc_dis(p1, p2);
    bb=calc_dis(p2, p3);
    cc=calc_dis(p3, p1);

    s = (aa+bb+cc)/2;
    q = s*(s-aa)*(s-bb)*(s-cc);
    if( q < 0.0 ) q = 0.0;

    S = sqrt(q);

    return S;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


MeshData::MeshData()
{
    init();
}

MeshData::~MeshData()
{
    free();
}

void MeshData::init(void)
{
    vex_num = 0;
    ele_num = 0;
    edge_num = 0;

    vex_arr  = NULL;
    vex_norm = NULL;
    ele_arr  = NULL;
    ele_norm = NULL;
}

void MeshData::free(void)
{
    if( vex_arr != NULL ) {
        delete vex_arr;
        vex_arr = NULL;
    }
    if( vex_norm != NULL ) {
        delete vex_norm;
        vex_norm = NULL;
    }

    if( ele_arr != NULL ) {
        delete ele_arr;
        ele_arr = NULL;
    }
    if( ele_norm != NULL ) {
        delete ele_norm;
        ele_norm = NULL;
    }


    vex_num = 0;
    ele_num = 0;
    edge_num = 0;
}

int MeshData::load(const char *fname)
{
    FILE    *fp;
    char    buf[2000];
    int     i;
    int     i1, i2, i3, i4;
    float   f1, f2, f3, f4;

    fp = fopen(fname, "r");
//    assert(fp);
    if(!fp)
    {
        std::cout<<"Can't find file "<<fname<<".\n";
        return 0;
    }

    // read file header
    fscanf(fp, "%s", buf);

    // read vertex, element, edege number
    fscanf(fp, "%d %d %d", &i1, &i2, &i3);

    this->free();
    vex_num = i1;
    ele_num = i2;
    vex_arr = new double[vex_num*3];
    ele_arr = new int[ele_num*3];

    // read vertex array
    for(i=0; i<vex_num; i++) {
        fscanf(fp, "%f %f %f", &f1, &f2, &f3);

        vex_arr[i*3+0] = f1;
        vex_arr[i*3+1] = f2;
        vex_arr[i*3+2] = f3;
    }

    // read element array
    for(i=0; i<ele_num; i++) {
        fscanf(fp, "%d %d %d %d", &i1, &i2, &i3, &i4);

        ele_arr[i*3+0] = i2;
        ele_arr[i*3+1] = i3;
        ele_arr[i*3+2] = i4;
    }

    fclose(fp);

    return ele_num;
}

/**
 *  Get elements which contain the vertex
 */
int MeshData::get_ele_by_vex(int vex_id, int *ele_n, int *ele)
{
    int     i, n;

    n = 0;
    for(i=0; i<ele_num*3; i++) {
        if( ele_arr[i] == vex_id ) {
            ele[n++] = i/3;
        }
    }

    *ele_n = n;

    return 0;
}

/**
 *  Get vertex data
 */
int MeshData::get_vex(int vex_id, double *v)
{
    v[0] = vex_arr[vex_id*3+0];
    v[1] = vex_arr[vex_id*3+1];
    v[2] = vex_arr[vex_id*3+2];

    return 0;
}

/**
 *  Calculate vertex normal vector
 */
int MeshData::calc_vex_norm(int vex_id, double *v)
{
    int     ele_n, *ele;
    double  v1[3], v2[3], v3[3];
    int     i1, i2, i3, ei;
    int     i, j;
    double  l;

    // clear v
    for(i=0; i<3; i++) v[i] = 0.0;

    // get elements which contain given vertex
    ele_n = 40;
    ele = new int[ele_n];
    get_ele_by_vex(vex_id, &ele_n, ele);

    // for each element
    for(i=0; i<ele_n; i++) {
        ei = ele[i];
        i1 = ele_arr[ei*3+0];
        i2 = ele_arr[ei*3+1];
        i3 = ele_arr[ei*3+2];

        // v2 - v1
        v1[0] = vex_arr[i2*3+0] - vex_arr[i1*3+0];
        v1[1] = vex_arr[i2*3+1] - vex_arr[i1*3+1];
        v1[2] = vex_arr[i2*3+2] - vex_arr[i1*3+2];

        // v3 - v1
        v2[0] = vex_arr[i3*3+0] - vex_arr[i1*3+0];
        v2[1] = vex_arr[i3*3+1] - vex_arr[i1*3+1];
        v2[2] = vex_arr[i3*3+2] - vex_arr[i1*3+2];

        // (v2-v1)x(v3-v1)
        vec_cross_product(v1, v2, v3);
        v[0] += v3[0];
        v[1] += v3[1];
        v[2] += v3[2];
    }

    // average normal vector
    v[0] /= ele_n;
    v[1] /= ele_n;
    v[2] /= ele_n;

    // normalize length
    l = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    v[0] /= l;
    v[1] /= l;
    v[2] /= l;

    delete ele;

    return 0;
}

int MeshData::calc_ele_norm(void)
{
    int     i;
    int     i1, i2, i3;
    double  v1[3], v2[3], v3[3];
    double  vl;

    if( ele_norm == NULL ) {
        ele_norm = new double[ele_num*3];
    }

    // for each element
    for(i=0; i<ele_num; i++) {
        i1 = ele_arr[i*3+0];
        i2 = ele_arr[i*3+1];
        i3 = ele_arr[i*3+2];

        // v2 - v1
        v1[0] = vex_arr[i2*3+0] - vex_arr[i1*3+0];
        v1[1] = vex_arr[i2*3+1] - vex_arr[i1*3+1];
        v1[2] = vex_arr[i2*3+2] - vex_arr[i1*3+2];

        // v3 - v1
        v2[0] = vex_arr[i3*3+0] - vex_arr[i1*3+0];
        v2[1] = vex_arr[i3*3+1] - vex_arr[i1*3+1];
        v2[2] = vex_arr[i3*3+2] - vex_arr[i1*3+2];

        // (v2-v1)x(v3-v1)
        vec_cross_product(v1, v2, v3);
        vl = sqrt(v3[0]*v3[0] + v3[1]*v3[1] + v3[2]*v3[2]);
        ele_norm[i*3+0] = v3[0] / vl;
        ele_norm[i*3+1] = v3[1] / vl;
        ele_norm[i*3+2] = v3[2] / vl;
    }

    return 0;
}

int MeshData::mesh_normalize(void)
{
    double  x_min, x_max, y_min, y_max, z_min, z_max;
    double  dx, dy, dz;
    double  fx, fy, fz;
    int     i;

    x_min = 1e30;   x_max = -1e30;
    y_min = 1e30;   y_max = -1e30;
    z_min = 1e30;   z_max = -1e30;

    for(i=0; i<vex_num; i++) {
        fx = vex_arr[i*3+0];
        fy = vex_arr[i*3+1];
        fz = vex_arr[i*3+2];

        if( fx < x_min ) x_min = fx;
        if( fx > x_max ) x_max = fx;
        if( fy < y_min ) y_min = fy;
        if( fy > y_max ) y_max = fy;
        if( fz < z_min ) z_min = fz;
        if( fz > z_max ) z_max = fz;
    }

    dx = 1.0 / (x_max - x_min);
    dy = 1.0 / (y_max - y_min);
    dz = 1.0 / (z_max - z_min);


    /*
    for(i=0; i<vex_num; i++) {
        fx = vex_arr[i*3+0];
        fy = vex_arr[i*3+1];
        fz = vex_arr[i*3+2];

        vex_arr[i*3+0] = (fx - x_min - (x_max-x_min)/2)*dx;
        vex_arr[i*3+1] = (fy - y_min - (y_max-y_min)/2)*dx;
        vex_arr[i*3+2] = (fz - z_min - (z_max-z_min)/2)*dx;
    }
    */

    calc_ele_norm();

    return 0;
}

double MeshData::mesh_bound_radius(void)
{
    double  x_min, x_max, y_min, y_max, z_min, z_max;
    double  fx, fy, fz;
    double  r;
    int     i;

    x_min = 1e30;   x_max = -1e30;
    y_min = 1e30;   y_max = -1e30;
    z_min = 1e30;   z_max = -1e30;

    for(i=0; i<vex_num; i++) {
        fx = vex_arr[i*3+0];
        fy = vex_arr[i*3+1];
        fz = vex_arr[i*3+2];

        if( fx < x_min ) x_min = fx;
        if( fx > x_max ) x_max = fx;
        if( fy < y_min ) y_min = fy;
        if( fy > y_max ) y_max = fy;
        if( fz < z_min ) z_min = fz;
        if( fz > z_max ) z_max = fz;
    }

    r = sqrt(sqr(x_max-x_min) + sqr(y_max-y_min)+sqr(z_max-z_min));

    return r;
}

double MeshData::mesh_area(void)
{
    double      s;
    int         i1, i2, i3;
    double      *p1, *p2, *p3;

    int         i;

    s = 0;
    for(i=0; i<ele_num; i++) {
        i1 = ele_arr[i*3+0];
        i2 = ele_arr[i*3+1];
        i3 = ele_arr[i*3+2];

        p1 = vex_arr+i1*3;
        p2 = vex_arr+i2*3;
        p3 = vex_arr+i3*3;

        s += triangle_area(p1, p2, p3);
    }

    return s;
}



