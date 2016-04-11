/******************************************************************************

  This file is part of Map2DFusion.

  Copyright 2016 (c)  Yong Zhao <zd5945@126.com> http://www.zhaoyong.adv-ci.com

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
#include "Map2D.h"
#include "Map2DCPU.h"
#include "Map2DRender.h"
#include "Map2DGPU.h"
#include "MultiBandMap2DCPU.h"
#include <iostream>

using namespace std;

bool Map2DPrepare::prepare(const pi::SE3d& plane,const PinHoleParameters& camera,
                                        const std::deque<std::pair<cv::Mat,pi::SE3d> >& frames)
{
    if(frames.size()==0||camera.w<=0||camera.h<=0||camera.fx==0||camera.fy==0)
    {
        cerr<<"Map2D::prepare:Not valid prepare!\n";
        return false;
    }
    _camera=camera;_fxinv=1./camera.fx;_fyinv=1./camera.fy;
    _plane =plane;
    _frames=frames;
    for(std::deque<std::pair<cv::Mat,pi::SE3d> >::iterator it=_frames.begin();it!=_frames.end();it++)
    {
        pi::SE3d& pose=it->second;
        pose=plane.inverse()*pose;//plane coordinate
    }
    return true;
}

SPtr<Map2D> Map2D::create(int type,bool thread)
{
    if(type==NoType) return SPtr<Map2D>();
    else if(type==TypeCPU)    return SPtr<Map2D>(new Map2DCPU(thread));
    else if(type==TypeMultiBandCPU) return SPtr<MultiBandMap2DCPU>(new MultiBandMap2DCPU(thread));
    else if(type==TypeRender)    return SPtr<Map2D>(new Map2DRender(thread));
    else if(type==TypeGPU)
    {
#ifdef HAS_CUDA
        return SPtr<Map2D>(new Map2DGPU(thread));
#else
        std::cout<<"Warning: CUDA is not enabled, switch to CPU implimentation.\n";
        return SPtr<Map2D>(new Map2DCPU(thread));
#endif
    }
}
