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
#ifndef MAP2D_H
#define MAP2D_H
#include <deque>
#include <opencv2/features2d/features2d.hpp>

#include <base/types/SPtr.h>
#include <base/types/SE3.h>
#include <base/system/thread/ThreadBase.h>
#include <gui/gl/GL_Object.h>

#define  ELE_PIXELS 256

struct PinHoleParameters
{
    PinHoleParameters(){}
    PinHoleParameters(int _w,int _h,double _fx,double _fy,double _cx,double _cy)
        :w(_w),h(_h),fx(_fx),fy(_fy),cx(_cx),cy(_cy){}
    double w,h,fx,fy,cx,cy;
};

struct Map2DPrepare//change when prepare
{
    uint queueSize(){pi::ReadMutex lock(mutexFrames);
                  return _frames.size();}

    bool prepare(const pi::SE3d& plane,const PinHoleParameters& camera,
                 const std::deque<std::pair<cv::Mat,pi::SE3d> >& frames);

    pi::Point2d Project(const pi::Point3d& pt)
    {
        double zinv=1./pt.z;
        return pi::Point2d(_camera.fx*pt.x*zinv+_camera.cx,
                           _camera.fy*pt.y*zinv+_camera.cy);
    }

    pi::Point3d UnProject(const pi::Point2d& pt)
    {
        return pi::Point3d((pt.x-_camera.cx)*_fxinv,
                           (pt.y-_camera.cy)*_fyinv,1.);
    }

    std::deque<std::pair<cv::Mat,pi::SE3d> > getFrames()
    {
        pi::ReadMutex lock(mutexFrames);
        return _frames;
    }

    PinHoleParameters                        _camera;
    double                                   _fxinv,_fyinv;
    pi::SE3d                                 _plane;//all fixed
    std::deque<std::pair<cv::Mat,pi::SE3d> > _frames;//camera coordinate
    pi::MutexRW                              mutexFrames;
};

class Map2D:public pi::gl::GL_Object
{

public:
    enum Map2DType{NoType=0,TypeCPU=1,TypeGPU=2,TypeMultiBandCPU=3,TypeRender=4};
    static SPtr<Map2D> create(int type=TypeCPU,bool thread=true);

    virtual ~Map2D(){}

    virtual bool prepare(const pi::SE3d& plane,const PinHoleParameters& camera,
                    const std::deque<std::pair<cv::Mat,pi::SE3d> >& frames){return false;}

    virtual bool feed(cv::Mat img,const pi::SE3d& pose){return false;}

    virtual void draw(){}

    virtual bool save(const std::string& filename){return false;}

    virtual uint queueSize(){return 0;}
};

#endif // MAP2D_H
