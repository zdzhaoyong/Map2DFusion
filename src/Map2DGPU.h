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
#ifndef MAP2DGPU_H
#define MAP2DGPU_H
#include "Map2D.h"
#include "Map2DCPU.h"
#ifdef HAS_CUDA

#include <cuda_runtime.h>

typedef Map2DPrepare Map2DGPUPrepare;

class Map2DGPU:public Map2D,public pi::Thread
{

    struct Map2DGPUEle
    {
        Map2DGPUEle()
            :img(NULL),texName(0),Ischanged(false)
        {
        }
        ~Map2DGPUEle();

        bool updateTextureGPU();
        bool updateTextureCPU();

        float4* img;//BGRA
        uint    texName;

        bool    Ischanged;
        pi::MutexRW mutexData;
    };

    struct Map2DGPUData//change when spread and prepare
    {
        Map2DGPUData():_w(0),_h(0){}
        Map2DGPUData(double eleSize_,double lengthPixel_,pi::Point3d max_,pi::Point3d min_,
                     int w_,int h_,const std::vector<SPtr<Map2DGPUEle> >& d_)
            :_eleSize(eleSize_),_eleSizeInv(1./eleSize_),
              _lengthPixel(lengthPixel_),_lengthPixelInv(1./lengthPixel_),
              _min(min_),_max(max_),_w(w_),_h(h_),_data(d_){}

        bool   prepare(SPtr<Map2DGPUPrepare> prepared);// only done Once!

        double eleSize()const{return _eleSize;}
        double lengthPixel()const{return _lengthPixel;}
        double eleSizeInv()const{return _eleSizeInv;}
        double lengthPixelInv()const{return _lengthPixelInv;}
        const pi::Point3d& min()const{return _min;}
        const pi::Point3d& max()const{return _max;}
        const int w()const{return _w;}
        const int h()const{return _h;}

        std::vector<SPtr<Map2DGPUEle> > data()
        {pi::ReadMutex lock(mutexData);return _data;}

        SPtr<Map2DGPUEle> ele(uint idx)
        {
            pi::WriteMutex lock(mutexData);
            if(idx>_data.size()) return SPtr<Map2DGPUEle>();
            else if(!_data[idx].get())
            {
                _data[idx]=SPtr<Map2DGPUEle>(new Map2DGPUEle());
            }
            return _data[idx];
        }

    private:
        //IMPORTANT: everything should never changed after prepared!
        double      _eleSize,_lengthPixel,_eleSizeInv,_lengthPixelInv;
        pi::Point3d _max,_min;
        int         _w,_h;
        std::vector<SPtr<Map2DGPUEle> >  _data;
        pi::MutexRW mutexData;
    };

public:

    Map2DGPU(bool thread=true);

    virtual ~Map2DGPU(){_valid=false;}

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
    bool spreadMap(double xmin,double ymin,double xmax,double ymax);


    //source
    SPtr<Map2DGPUPrepare>             prepared;
    SPtr<Map2DGPUData>                data;
    pi::MutexRW                       mutex;

    bool                              _valid,_thread,_changed;
    cv::Mat                           weightImage;
    int&                              alpha;
};

#endif // HAS_GPU
#endif // MAP2DGPU_H
