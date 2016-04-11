#ifndef __MESH_OFF_H__
#define __MESH_OFF_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class MeshData {
public:
    int     vex_num, ele_num, edge_num;

    double  *vex_arr;
    double  *vex_norm;
    int     *ele_arr;
    double  *ele_norm;

public:
    MeshData();
    ~MeshData();

    void init(void);
    void free(void);

    int load(const char *fname);
    //int load_haveedge(HalfEdge *);

    int get_ele_by_vex(int vex_id, int *ele_n, int *ele);
    int calc_vex_norm(int vex_id, double *v);
    int calc_ele_norm(void);

    int get_vex_num(void) { return vex_num; }
    int get_ele_num(void) { return ele_num; }
    int get_vex(int vex_id, double *v);

    double *get_vex(void) { return vex_arr; }
    int    *get_ele(void) { return ele_arr; }

    int mesh_normalize(void);
    double mesh_bound_radius(void);

    double mesh_area(void);
};


#endif // MESH_OFF_H
