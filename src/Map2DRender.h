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
#ifndef MAP2DRENDER_H
#define MAP2DRENDER_H
#include "Map2D.h"
#include <base/system/thread/ThreadBase.h>

class Map2DRender:public Map2D,public pi::Thread
{
    struct Map2DRenderPrepare//change when prepare
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

    struct Map2DRenderEle
    {
        Map2DRenderEle():texName(0),Ischanged(false){}
        ~Map2DRenderEle();
        cv::Mat img;//bgr
        cv::Mat mask;//weight
        uint    texName;
        bool    Ischanged;
        pi::MutexRW mutexData;
    };

    struct Map2DRenderData//change when spread and prepare
    {
        Map2DRenderData():_w(0),_h(0){}
        Map2DRenderData(double eleSize_,double lengthPixel_,pi::Point3d max_,pi::Point3d min_,
                     int w_,int h_,const std::vector<SPtr<Map2DRenderEle> >& d_)
            :_eleSize(eleSize_),_eleSizeInv(1./eleSize_),
              _lengthPixel(lengthPixel_),_lengthPixelInv(1./lengthPixel_),
              _min(min_),_max(max_),_w(w_),_h(h_),_data(d_){}

        bool   prepare(SPtr<Map2DRenderPrepare> prepared);// only done Once!

        double eleSize()const{return _eleSize;}
        double lengthPixel()const{return _lengthPixel;}
        double eleSizeInv()const{return _eleSizeInv;}
        double lengthPixelInv()const{return _lengthPixelInv;}
        const pi::Point3d& min()const{return _min;}
        const pi::Point3d& max()const{return _max;}
        const int w()const{return _w;}
        const int h()const{return _h;}

        std::vector<SPtr<Map2DRenderEle> > data()
        {pi::ReadMutex lock(mutexData);return _data;}

        SPtr<Map2DRenderEle> ele(uint idx)
        {
            pi::WriteMutex lock(mutexData);
            if(idx>_data.size()) return SPtr<Map2DRenderEle>();
            else if(!_data[idx].get())
            {
                _data[idx]=SPtr<Map2DRenderEle>(new Map2DRenderEle());
            }
            return _data[idx];
        }

    private:
        //IMPORTANT: everything should never changed after prepared!
        double      _eleSize,_lengthPixel,_eleSizeInv,_lengthPixelInv;
        pi::Point3d _max,_min;
        int         _w,_h;
        std::vector<SPtr<Map2DRenderEle> >  _data;
        pi::MutexRW mutexData;
    };

public:

    Map2DRender(bool thread=true);

    virtual ~Map2DRender(){_valid=false;}

    virtual bool prepare(const pi::SE3d& plane,const PinHoleParameters& camera,
                    const std::deque<std::pair<cv::Mat,pi::SE3d> >& frames);

    virtual bool feed(cv::Mat img,const pi::SE3d& pose);//world coordinate

    virtual void draw();

    virtual bool save(const std::string& filename);

    virtual uint queueSize(){
        if(prepared.get()) return prepared->queueSize();
        else               return 0;
    }

    virtual void run();

private:

    bool getFrame(std::pair<cv::Mat,pi::SE3d>& frame);
    bool renderFrame(const std::pair<cv::Mat,pi::SE3d>& frame);

    bool getFrames(std::deque<std::pair<cv::Mat,pi::SE3d> >& frames);
    bool renderFrames(std::deque<std::pair<cv::Mat,pi::SE3d> >& frames);

    bool spreadMap(double xmin,double ymin,double xmax,double ymax);


    //source
    SPtr<Map2DRenderPrepare>             prepared;
    SPtr<Map2DRenderData>                data;
    pi::MutexRW                          mutex;

    bool                              _valid,_thread,_changed;
    cv::Mat                           weightImage;
    int&                              alpha;
};
#endif // MAP2DRENDER_H
