////////////////////////////////////////////////////////////////////////////
//	File:		    util.h
//	Author:		    Changchang Wu (ccwu@cs.washington.edu)
//	Description :   some utility functions for reading/writing SfM data
//
//  Copyright (c) 2011  Changchang Wu (ccwu@cs.washington.edu)
//    and the University of Washington at Seattle 
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public
//  License as published by the Free Software Foundation; either
//  Version 3 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  General Public License for more details.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILS_PBA__
#define __UTILS_PBA__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include <time.h>
#include <iomanip>
#include <algorithm>

using namespace std;
#include "DataInterface.h"

namespace pba {

typedef Point3D_<float>  Point3D;
typedef PBAPoint2f Point2D;

//File loader supports .nvm format and bundler format
bool LoadModelFile(const char* name, vector<CameraT>& camera_data, vector<Point3D>& point_data,
              vector<Point2D>& measurements, vector<int>& ptidx, vector<int>& camidx, 
              vector<string>& names, vector<int>& ptc); 
void SaveNVM(const char* filename, vector<CameraT>& camera_data, vector<Point3D>& point_data,
              vector<Point2D>& measurements, vector<int>& ptidx, vector<int>& camidx, 
              vector<string>& names, vector<int>& ptc);
void SaveBundlerModel(const char* filename, vector<CameraT>& camera_data, vector<Point3D>& point_data,
              vector<Point2D>& measurements, vector<int>& ptidx, vector<int>& camidx);

void SaveModelFile(const char* outpath, vector<CameraT>& camera_data, vector<Point3D>& point_data,
              vector<Point2D>& measurements, vector<int>& ptidx, vector<int>& camidx,
              vector<string>& names, vector<int>& ptc);
//////////////////////////////////////////////////////////////////
void AddNoise(vector<CameraT>& camera_data, vector<Point3D>& point_data, float percent);
void AddStableNoise(vector<CameraT>& camera_data, vector<Point3D>& point_data,
                    const vector<int>& ptidx, const vector<int>& camidx, float percent);
bool RemoveInvisiblePoints( vector<CameraT>& camera_data, vector<Point3D>& point_data,
                            vector<int>& ptidx, vector<int>& camidx, 
                            vector<Point2D>& measurements, vector<string>& names, vector<int>& ptc);

}

#endif
