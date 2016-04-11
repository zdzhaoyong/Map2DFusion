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
#include "MultiBandMap2DCPU.h"

#include <gui/gl/glHelper.h>
#include <GL/gl.h>
#include <base/Svar/Svar.h>
#include <base/time/Global_Timer.h>
#include <gui/gl/SignalHandle.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/stitching/stitcher.hpp>

#define HAS_GOOGLEMAP
#ifdef HAS_GOOGLEMAP
#include <hardware/Gps/utils_GPS.h>
#include <base/Svar/Scommand.h>
#endif

using namespace std;

/**

  __________max
  |    |    |
  |____|____|
  |    |    |
  |____|____|
 min
 */

MultiBandMap2DCPU::MultiBandMap2DCPUEle::~MultiBandMap2DCPUEle()
{
    if(texName) pi::gl::Signal_Handle::instance().delete_texture(texName);
}

bool MultiBandMap2DCPU::MultiBandMap2DCPUEle::normalizeUsingWeightMap(const cv::Mat& weight, cv::Mat& src)
{
    if(!(src.type()==CV_32FC3&&weight.type()==CV_32FC1)) return false;
    pi::Point3f* srcP=(pi::Point3f*)src.data;
    float*    weightP=(float*)weight.data;
    for(float* Pend=weightP+weight.cols*weight.rows;weightP!=Pend;weightP++,srcP++)
        *srcP=(*srcP)/(*weightP+1e-5);
    return true;
}

bool MultiBandMap2DCPU::MultiBandMap2DCPUEle::mulWeightMap(const cv::Mat& weight, cv::Mat& src)
{
    if(!(src.type()==CV_32FC3&&weight.type()==CV_32FC1)) return false;
    pi::Point3f* srcP=(pi::Point3f*)src.data;
    float*    weightP=(float*)weight.data;
    for(float* Pend=weightP+weight.cols*weight.rows;weightP!=Pend;weightP++,srcP++)
        *srcP=(*srcP)*(*weightP);
    return true;
}

cv::Mat MultiBandMap2DCPU::MultiBandMap2DCPUEle::blend(const std::vector<SPtr<MultiBandMap2DCPUEle> >& neighbors)
{
    if(!pyr_laplace.size()) return cv::Mat();
    if(neighbors.size()==9)
    {
        //blend with neighbors, this obtains better visualization
        int flag=0;
        for(int i=0;i<neighbors.size();i++)
        {
            flag<<=1;
            if(neighbors[i].get()&&neighbors[i]->pyr_laplace.size())
                flag|=1;
        }
        switch (flag) {
        case 0X01FF:
        {
            vector<cv::Mat> pyr_laplaceClone(pyr_laplace.size());
            for(int i=0;i<pyr_laplace.size();i++)
            {
                int borderSize=1<<(pyr_laplace.size()-i-1);
                int srcrows=pyr_laplace[i].rows;
                int dstrows=srcrows+(borderSize<<1);
                pyr_laplaceClone[i]=cv::Mat(dstrows,dstrows,pyr_laplace[i].type());

                for(int y=0;y<3;y++)
                    for(int x=0;x<3;x++)
                {
                    const SPtr<MultiBandMap2DCPUEle>& ele=neighbors[3*y+x];
                    pi::ReadMutex lock(ele->mutexData);
                    if(ele->pyr_laplace[i].empty())
                        continue;
                    cv::Rect      src,dst;
                    src.width =dst.width =(x==1)?srcrows:borderSize;
                    src.height=dst.height=(y==1)?srcrows:borderSize;
                    src.x=(x==0)?(srcrows-borderSize):0;
                    src.y=(y==0)?(srcrows-borderSize):0;
                    dst.x=(x==0)?0:((x==1)?borderSize:(dstrows-borderSize));
                    dst.y=(y==0)?0:((y==1)?borderSize:(dstrows-borderSize));
                    ele->pyr_laplace[i](src).copyTo(pyr_laplaceClone[i](dst));
                }
            }

            cv::detail::restoreImageFromLaplacePyr(pyr_laplaceClone);

            {
                cv::Mat result;
                int borderSize=1<<(pyr_laplace.size()-1);
                pyr_laplaceClone[0](cv::Rect(borderSize,borderSize,ELE_PIXELS,ELE_PIXELS)).copyTo(result);
                return  result.setTo(cv::Scalar::all(0),weights[0]==0);
            }
        }
            break;
        default:
            break;
        }
    }

    {
        //blend by self
        vector<cv::Mat> pyr_laplaceClone(pyr_laplace.size());
        for(int i=0;i<pyr_laplace.size();i++)
        {
            pyr_laplaceClone[i]=pyr_laplace[i].clone();
        }

        cv::detail::restoreImageFromLaplacePyr(pyr_laplaceClone);

        return  pyr_laplaceClone[0].setTo(cv::Scalar::all(0),weights[0]==0);
    }
}

// this is a bad idea, just for test
bool MultiBandMap2DCPU::MultiBandMap2DCPUEle::updateTexture(const std::vector<SPtr<MultiBandMap2DCPUEle> >& neighbors)
{
    cv::Mat tmp=blend(neighbors);
    uint type=0;
    if(tmp.empty()) return false;
    else if(tmp.type()==CV_16SC3)
    {
        tmp.convertTo(tmp,CV_8UC3);
        type=GL_UNSIGNED_BYTE;
    }
    else if(tmp.type()==CV_32FC3)
        type=GL_FLOAT;
    if(!type) return false;

    {
        if(texName==0)// create texture
        {
            glGenTextures(1, &texName);
            glBindTexture(GL_TEXTURE_2D,texName);
            glTexImage2D(GL_TEXTURE_2D, 0,
                         GL_RGB,tmp.cols,tmp.rows, 0,
                         GL_BGR, type,tmp.data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D,texName);
            glTexImage2D(GL_TEXTURE_2D, 0,
                         GL_RGB,tmp.cols,tmp.rows, 0,
                         GL_BGR, type,tmp.data);
        }
    }

    SvarWithType<cv::Mat>::instance()["LastTexMat"]=tmp;
    SvarWithType<cv::Mat>::instance()["LastTexMatWeight"]=weights[0].clone();

    Ischanged=false;
    return true;
}

MultiBandMap2DCPU::MultiBandMap2DCPUData::MultiBandMap2DCPUData(double eleSize_,double lengthPixel_,pi::Point3d max_,pi::Point3d min_,
             int w_,int h_,const std::vector<SPtr<MultiBandMap2DCPUEle> >& d_)
    :_eleSize(eleSize_),_eleSizeInv(1./eleSize_),
      _lengthPixel(lengthPixel_),_lengthPixelInv(1./lengthPixel_),
      _min(min_),_max(max_),_w(w_),_h(h_),_data(d_)
{
    _gpsOrigin=svar.get_var("GPS.Origin",_gpsOrigin);
}

bool MultiBandMap2DCPU::MultiBandMap2DCPUData::prepare(SPtr<MultiBandMap2DCPUPrepare> prepared)
{
    if(_w||_h) return false;//already prepared
    {
        _max=pi::Point3d(-1e10,-1e10,-1e10);
        _min=-_max;
        for(std::deque<std::pair<cv::Mat,pi::SE3d> >::iterator it=prepared->_frames.begin();
            it!=prepared->_frames.end();it++)
        {
            pi::SE3d& pose=it->second;
            pi::Point3d& t=pose.get_translation();
            _max.x=t.x>_max.x?t.x:_max.x;
            _max.y=t.y>_max.y?t.y:_max.y;
            _max.z=t.z>_max.z?t.z:_max.z;
            _min.x=t.x<_min.x?t.x:_min.x;
            _min.y=t.y<_min.y?t.y:_min.y;
            _min.z=t.z<_min.z?t.z:_min.z;
        }
        if(_min.z*_max.z<=0) return false;
        cout<<"Box:Min:"<<_min<<",Max:"<<_max<<endl;
    }
    //estimate w,h and bonding box
    {
        double maxh;
        if(_max.z>0) maxh=_max.z;
        else maxh=-_min.z;
        pi::Point3d line=prepared->UnProject(pi::Point2d(prepared->_camera.w,prepared->_camera.h))
                -prepared->UnProject(pi::Point2d(0,0));
        double radius=0.5*maxh*sqrt((line.x*line.x+line.y*line.y));
        _lengthPixel=svar.GetDouble("Map2D.Resolution",0);
        if(!_lengthPixel)
        {
            cout<<"Auto resolution from max height "<<maxh<<"m.\n";
            _lengthPixel=2*radius/sqrt(prepared->_camera.w*prepared->_camera.w
                                       +prepared->_camera.h*prepared->_camera.h);

            _lengthPixel/=svar.GetDouble("Map2D.Scale",1);
        }
        cout<<"Map2D.Resolution="<<_lengthPixel<<endl;
        _lengthPixelInv=1./_lengthPixel;
        _min=_min-pi::Point3d(radius,radius,0);
        _max=_max+pi::Point3d(radius,radius,0);
        pi::Point3d center=0.5*(_min+_max);
        _min=2*_min-center;_max=2*_max-center;
        _eleSize=ELE_PIXELS*_lengthPixel;
        _eleSizeInv=1./_eleSize;
        {
            _w=ceil((_max.x-_min.x)/_eleSize);
            _h=ceil((_max.y-_min.y)/_eleSize);
            _max.x=_min.x+_eleSize*_w;
            _max.y=_min.y+_eleSize*_h;
            _data.resize(_w*_h);
        }
    }
    _gpsOrigin=svar.get_var("GPS.Origin",_gpsOrigin);
    return true;
}

MultiBandMap2DCPU::MultiBandMap2DCPU(bool thread)
    :alpha(svar.GetInt("Map2D.Alpha",0)),
     _valid(false),_thread(thread),
     _bandNum(svar.GetInt("MultiBandMap2DCPU.BandNumber",5)),
     _highQualityShow(svar.GetInt("MultiBandMap2DCPU.HighQualityShow",1))
{
    _bandNum=min(_bandNum, static_cast<int>(ceil(log(ELE_PIXELS) / log(2.0))));
}

bool MultiBandMap2DCPU::prepare(const pi::SE3d& plane,const PinHoleParameters& camera,
                const std::deque<std::pair<cv::Mat,pi::SE3d> >& frames)
{
    //insert frames
    SPtr<MultiBandMap2DCPUPrepare> p(new MultiBandMap2DCPUPrepare);
    SPtr<MultiBandMap2DCPUData>    d(new MultiBandMap2DCPUData);

    if(p->prepare(plane,camera,frames))
        if(d->prepare(p))
        {
            pi::WriteMutex lock(mutex);
            prepared=p;
            data=d;
            weightImage.release();
            if(_thread&&!isRunning())
                start();
            _valid=true;
            return true;
        }
    return false;
}

bool MultiBandMap2DCPU::feed(cv::Mat img,const pi::SE3d& pose)
{
    if(!_valid) return false;
    SPtr<MultiBandMap2DCPUPrepare> p;
    SPtr<MultiBandMap2DCPUData>    d;
    {
        pi::ReadMutex lock(mutex);
        p=prepared;d=data;
    }
    std::pair<cv::Mat,pi::SE3d> frame(img,p->_plane.inverse()*pose);
    if(_thread)
    {
        pi::WriteMutex lock(p->mutexFrames);
        p->_frames.push_back(frame);
        if(p->_frames.size()>20) p->_frames.pop_front();
        return true;
    }
    else
    {
        return renderFrame(frame);
    }
}

bool MultiBandMap2DCPU::renderFrame(const std::pair<cv::Mat,pi::SE3d>& frame)
{
    SPtr<MultiBandMap2DCPUPrepare> p;
    SPtr<MultiBandMap2DCPUData>    d;
    {
        pi::ReadMutex lock(mutex);
        p=prepared;d=data;
    }
    if(frame.first.cols!=p->_camera.w||frame.first.rows!=p->_camera.h||frame.first.type()!=CV_8UC3)
    {
        cerr<<"MultiBandMap2DCPU::renderFrame: frame.first.cols!=p->_camera.w||frame.first.rows!=p->_camera.h||frame.first.type()!=CV_8UC3\n";
        return false;
    }
    // 1. pose->pts
    std::vector<pi::Point2d>          imgPts;
    {
        imgPts.reserve(4);
        imgPts.push_back(pi::Point2d(0,0));
        imgPts.push_back(pi::Point2d(p->_camera.w,0));
        imgPts.push_back(pi::Point2d(0,p->_camera.h));
        imgPts.push_back(pi::Point2d(p->_camera.w,p->_camera.h));
    }
    vector<pi::Point2d> pts;
    pts.reserve(imgPts.size());
    pi::Point3d downLook(0,0,-1);
    if(frame.second.get_translation().z<0) downLook=pi::Point3d(0,0,1);
    for(int i=0;i<imgPts.size();i++)
    {
        pi::Point3d axis=frame.second.get_rotation()*p->UnProject(imgPts[i]);
        if(axis.dot(downLook)<0.4)
        {
            return false;
        }
        axis=frame.second.get_translation()
                -axis*(frame.second.get_translation().z/axis.z);
        pts.push_back(pi::Point2d(axis.x,axis.y));
    }
    // 2. dest location?
    double xmin=pts[0].x;
    double xmax=xmin;
    double ymin=pts[0].y;
    double ymax=ymin;
    for(int i=1;i<pts.size();i++)
    {
        if(pts[i].x<xmin) xmin=pts[i].x;
        if(pts[i].y<ymin) ymin=pts[i].y;
        if(pts[i].x>xmax) xmax=pts[i].x;
        if(pts[i].y>ymax) ymax=pts[i].y;
    }
    if(xmin<d->min().x||xmax>d->max().x||ymin<d->min().y||ymax>d->max().y)
    {
        if(p!=prepared)//what if prepare called?
        {
            return false;
        }
        if(!spreadMap(xmin,ymin,xmax,ymax))
        {
            return false;
        }
        else
        {
            pi::ReadMutex lock(mutex);
            if(p!=prepared)//what if prepare called?
            {
                return false;
            }
            d=data;//new data
        }
    }
    int xminInt=floor((xmin-d->min().x)*d->eleSizeInv());
    int yminInt=floor((ymin-d->min().y)*d->eleSizeInv());
    int xmaxInt= ceil((xmax-d->min().x)*d->eleSizeInv());
    int ymaxInt= ceil((ymax-d->min().y)*d->eleSizeInv());
    if(xminInt<0||yminInt<0||xmaxInt>d->w()||ymaxInt>d->h()||xminInt>=xmaxInt||yminInt>=ymaxInt)
    {
        cerr<<"MultiBandMap2DCPU::renderFrame:should never happen!\n";
        return false;
    }
    {
        xmin=d->min().x+d->eleSize()*xminInt;
        ymin=d->min().y+d->eleSize()*yminInt;
        xmax=d->min().x+d->eleSize()*xmaxInt;
        ymax=d->min().y+d->eleSize()*ymaxInt;
    }
    // 3.prepare weight and warp images
    cv::Mat weight_src;
    if(weightImage.empty()||weightImage.cols!=frame.first.cols||weightImage.rows!=frame.first.rows)
    {
        pi::WriteMutex lock(mutex);
        int w=frame.first.cols;
        int h=frame.first.rows;
        weightImage.create(h,w,CV_32FC1);
        float *p=(float*)weightImage.data;
        float x_center=w/2;
        float y_center=h/2;
        float dis_max=sqrt(x_center*x_center+y_center*y_center);
        int weightType=svar.GetInt("Map2D.WeightType",0);
        for(int i=0;i<h;i++)
            for(int j=0;j<w;j++)
            {
                float dis=(i-y_center)*(i-y_center)+(j-x_center)*(j-x_center);
                dis=1-sqrt(dis)/dis_max;
                if(0==weightType)
                    *p=dis;
                else *p=dis*dis;
                if(*p<=1e-5) *p=1e-5;
                p++;
            }
        weight_src=weightImage.clone();
    }
    else
    {
        pi::ReadMutex lock(mutex);
        weight_src=weightImage.clone();
    }

    std::vector<cv::Point2f>          imgPtsCV;
    {
        imgPtsCV.reserve(imgPts.size());
        for(int i=0;i<imgPts.size();i++)
            imgPtsCV.push_back(cv::Point2f(imgPts[i].x,imgPts[i].y));
    }
    std::vector<cv::Point2f> destPoints;
    destPoints.reserve(imgPtsCV.size());
    for(int i=0;i<imgPtsCV.size();i++)
    {
        destPoints.push_back(cv::Point2f((pts[i].x-xmin)*d->lengthPixelInv(),
                             (pts[i].y-ymin)*d->lengthPixelInv()));
    }

    cv::Mat transmtx = cv::getPerspectiveTransform(imgPtsCV, destPoints);

    cv::Mat img_src;
    if(svar.GetInt("MultiBandMap2DCPU.ForceFloat",0))
        frame.first.convertTo(img_src,CV_32FC3,1./255.);
    else
        frame.first.convertTo(img_src,CV_16SC3);

    cv::Mat weight_warped((ymaxInt-yminInt)*ELE_PIXELS,(xmaxInt-xminInt)*ELE_PIXELS,CV_32FC1);
    cv::Mat image_warped((ymaxInt-yminInt)*ELE_PIXELS,(xmaxInt-xminInt)*ELE_PIXELS,img_src.type());
    cv::warpPerspective(img_src, image_warped, transmtx, image_warped.size(),cv::INTER_LINEAR,cv::BORDER_REFLECT);
    cv::warpPerspective(weight_src, weight_warped, transmtx, weight_warped.size(),cv::INTER_NEAREST);

    if(svar.GetInt("ShowWarped",0))
    {
        cv::imshow("image_warped",image_warped);
        cv::imshow("weight_warped",weight_warped);
        if(svar.GetInt("SaveImageWarped"))
        {
            cout<<"Saving warped image.\n";
            cv::imwrite("image_warped.png",image_warped);
            cv::imwrite("weight_warped.png",weight_warped);
        }
        cv::waitKey(0);
    }

    // 4. blender dst to eles
    std::vector<cv::Mat> pyr_laplace;
    cv::detail::createLaplacePyr(image_warped, _bandNum, pyr_laplace);

    std::vector<cv::Mat> pyr_weights(_bandNum+1);
    pyr_weights[0]=weight_warped;
    for (int i = 0; i < _bandNum; ++i)
        cv::pyrDown(pyr_weights[i], pyr_weights[i + 1]);

    pi::timer.enter("MultiBandMap2DCPU::Apply");
    std::vector<SPtr<MultiBandMap2DCPUEle> > dataCopy=d->data();
    for(int x=xminInt;x<xmaxInt;x++)
        for(int y=yminInt;y<ymaxInt;y++)
        {
            SPtr<MultiBandMap2DCPUEle> ele=dataCopy[y*d->w()+x];
            if(!ele.get())
            {
                ele=d->ele(y*d->w()+x);
            }
            {
                pi::WriteMutex lock(ele->mutexData);
                if(!ele->pyr_laplace.size())
                {
                    ele->pyr_laplace.resize(_bandNum+1);
                    ele->weights.resize(_bandNum+1);
                }

                int width=ELE_PIXELS,height=ELE_PIXELS;

                for (int i = 0; i <= _bandNum; ++i)
                {
                    if(ele->pyr_laplace[i].empty())
                    {
                        //fresh
                        cv::Rect rect(width*(x-xminInt),height*(y-yminInt),width,height);
                        pyr_laplace[i](rect).copyTo(ele->pyr_laplace[i]);
                        pyr_weights[i](rect).copyTo(ele->weights[i]);
                    }
                    else
                    {
                        if(pyr_laplace[i].type()==CV_32FC3)
                        {
                            int org =(x-xminInt)*width+(y-yminInt)*height*pyr_laplace[i].cols;
                            int skip=pyr_laplace[i].cols-ele->pyr_laplace[i].cols;

                            pi::Point3f *srcL=((pi::Point3f*)pyr_laplace[i].data)+org;
                            float       *srcW=((float*)pyr_weights[i].data)+org;

                            pi::Point3f *dstL=(pi::Point3f*)ele->pyr_laplace[i].data;
                            float       *dstW=(float*)ele->weights[i].data;

                            for(int eleY=0;eleY<height;eleY++,srcL+=skip,srcW+=skip)
                                for(int eleX=0;eleX<width;eleX++,srcL++,dstL++,srcW++,dstW++)
                                {
                                    if((*srcW)>=(*dstW))
                                    {
                                        *dstL=(*srcL);
                                        *dstW=*srcW;
                                    }
                                }
                        }
                        else if(pyr_laplace[i].type()==CV_16SC3)
                        {
                            int org =(x-xminInt)*width+(y-yminInt)*height*pyr_laplace[i].cols;
                            int skip=pyr_laplace[i].cols-ele->pyr_laplace[i].cols;

                            pi::Point3_<short> *srcL=((pi::Point3_<short>*)pyr_laplace[i].data)+org;
                            float       *srcW=((float*)pyr_weights[i].data)+org;

                            pi::Point3_<short> *dstL=(pi::Point3_<short>*)ele->pyr_laplace[i].data;
                            float       *dstW=(float*)ele->weights[i].data;

                            for(int eleY=0;eleY<height;eleY++,srcL+=skip,srcW+=skip)
                                for(int eleX=0;eleX<width;eleX++,srcL++,dstL++,srcW++,dstW++)
                                {
                                    if((*srcW)>=(*dstW))
                                    {
                                        *dstL=(*srcL);
                                        *dstW=*srcW;
                                    }
                                }
                        }
                    }
                    width/=2;height/=2;
                }
                ele->Ischanged=true;
            }
        }
    pi::timer.leave("MultiBandMap2DCPU::Apply");

    return true;
}


bool MultiBandMap2DCPU::spreadMap(double xmin,double ymin,double xmax,double ymax)
{
    pi::timer.enter("MultiBandMap2DCPU::spreadMap");
    SPtr<MultiBandMap2DCPUData> d;
    {
        pi::ReadMutex lock(mutex);
        d=data;
    }
    int xminInt=floor((xmin-d->min().x)*d->eleSizeInv());
    int yminInt=floor((ymin-d->min().y)*d->eleSizeInv());
    int xmaxInt= ceil((xmax-d->min().x)*d->eleSizeInv());
    int ymaxInt= ceil((ymax-d->min().y)*d->eleSizeInv());
    xminInt=min(xminInt,0); yminInt=min(yminInt,0);
    xmaxInt=max(xmaxInt,d->w()); ymaxInt=max(ymaxInt,d->h());
    int w=xmaxInt-xminInt;
    int h=ymaxInt-yminInt;
    pi::Point2d min,max;
    {
        min.x=d->min().x+d->eleSize()*xminInt;
        min.y=d->min().y+d->eleSize()*yminInt;
        max.x=min.x+w*d->eleSize();
        max.y=min.y+h*d->eleSize();
    }
    std::vector<SPtr<MultiBandMap2DCPUEle> > dataOld=d->data();
    std::vector<SPtr<MultiBandMap2DCPUEle> > dataCopy;
    dataCopy.resize(w*h);
    {
        for(int x=0,xend=d->w();x<xend;x++)
            for(int y=0,yend=d->h();y<yend;y++)
            {
                dataCopy[x-xminInt+(y-yminInt)*w]=dataOld[y*d->w()+x];
            }
    }
    //apply
    {
        pi::WriteMutex lock(mutex);
        data=SPtr<MultiBandMap2DCPUData>(new MultiBandMap2DCPUData(d->eleSize(),d->lengthPixel(),
                                                 pi::Point3d(max.x,max.y,d->max().z),
                                                 pi::Point3d(min.x,min.y,d->min().z),
                                                 w,h,dataCopy));
    }
    pi::timer.leave("MultiBandMap2DCPU::spreadMap");
    return true;
}

bool MultiBandMap2DCPU::getFrame(std::pair<cv::Mat,pi::SE3d>& frame)
{
    pi::ReadMutex lock(mutex);
    pi::ReadMutex lock1(prepared->mutexFrames);
    if(prepared->_frames.size())
    {
        frame=prepared->_frames.front();
        prepared->_frames.pop_front();
        return true;
    }
    else return false;
}

void MultiBandMap2DCPU::run()
{
    std::pair<cv::Mat,pi::SE3d> frame;
    while(!shouldStop())
    {
        if(_valid)
        {
            if(getFrame(frame))
            {
                pi::timer.enter("MultiBandMap2DCPU::renderFrame");
                renderFrame(frame);
                pi::timer.leave("MultiBandMap2DCPU::renderFrame");
            }
        }
        sleep(10);
    }
}

void MultiBandMap2DCPU::draw()
{
    if(!_valid) return;

    SPtr<MultiBandMap2DCPUPrepare> p;
    SPtr<MultiBandMap2DCPUData>    d;
    {
        pi::ReadMutex lock(mutex);
        p=prepared;d=data;
    }
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrix(p->_plane);
    //draw deque frames
    pi::TicTac ticTac;
    ticTac.Tic();
    {
        std::deque<std::pair<cv::Mat,pi::SE3d> > frames=p->getFrames();
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        for(std::deque<std::pair<cv::Mat,pi::SE3d> >::iterator it=frames.begin();it!=frames.end();it++)
        {
            pi::SE3d& pose=it->second;
            glColor3ub(255,0,0);
            glVertex(pose.get_translation());
            glVertex(pose*pi::Point3d(1,0,0));
            glColor3ub(0,255,0);
            glVertex(pose.get_translation());
            glVertex(pose*pi::Point3d(0,1,0));
            glColor3ub(0,0,255);
            glVertex(pose.get_translation());
            glVertex(pose*pi::Point3d(0,0,1));
        }
        glEnd();
    }
    //draw global area
    if(svar.GetInt("Map2D.DrawArea"))
    {
        pi::Point3d _min=d->min();
        pi::Point3d _max=d->max();
        glColor3ub(255,0,0);
        glBegin(GL_LINES);
        glVertex3d(_min.x,_min.y,0);
        glVertex3d(_min.x,_max.y,0);
        glVertex3d(_min.x,_min.y,0);
        glVertex3d(_max.x,_min.y,0);
        glVertex3d(_max.x,_min.y,0);
        glVertex3d(_max.x,_max.y,0);
        glVertex3d(_min.x,_max.y,0);
        glVertex3d(_max.x,_max.y,0);
        glEnd();
    }

    //draw textures
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
//    glEnable(GL_LIGHTING);
    if(alpha)
    {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.1f);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    }
    GLint last_texture_ID;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
    std::vector<SPtr<MultiBandMap2DCPUEle> > dataCopy=d->data();
    int wCopy=d->w(),hCopy=d->h();
    glColor3ub(255,255,255);
    for(int x=0;x<wCopy;x++)
        for(int y=0;y<hCopy;y++)
        {
            int idxData=y*wCopy+x;
            float x0=d->min().x+x*d->eleSize();
            float y0=d->min().y+y*d->eleSize();
            float x1=x0+d->eleSize();
            float y1=y0+d->eleSize();
            SPtr<MultiBandMap2DCPUEle> ele=dataCopy[idxData];
            if(!ele.get())  continue;
            {
                {
                    pi::ReadMutex lock(ele->mutexData);
                    if(!(ele->pyr_laplace.size()&&ele->weights.size()
                         &&ele->pyr_laplace.size()==ele->weights.size())) continue;
                    if(ele->Ischanged)
                    {
                        pi::timer.enter("MultiBandMap2DCPU::updateTexture");
                        bool updated=false,inborder=false;
                        if(_highQualityShow)
                        {
                            vector<SPtr<MultiBandMap2DCPUEle> > neighbors;
                            neighbors.reserve(9);
                            for(int yi=y-1;yi<=y+1;yi++)
                                for(int xi=x-1;xi<=x+1;xi++)
                                {
                                    if(yi<0||yi>=hCopy||xi<0||xi>=wCopy)
                                    {
                                        neighbors.push_back(SPtr<MultiBandMap2DCPUEle>());
                                        inborder=true;
                                    }
                                    else neighbors.push_back(dataCopy[yi*wCopy+xi]);
                                }
                            updated=ele->updateTexture(neighbors);
                        }
                        else
                            updated=ele->updateTexture();
                        pi::timer.leave("MultiBandMap2DCPU::updateTexture");

                        if(updated&&!inborder&&svar.GetInt("Fuse2Google"))
                        {
                            pi::timer.enter("MultiBandMap2DCPU::fuseGoogle");
                            stringstream cmd;
                            pi::Point3d  worldTl=p->_plane*pi::Point3d(x0,y0,0);
                            pi::Point3d  worldBr=p->_plane*pi::Point3d(x1,y1,0);
                            pi::Point3d  gpsTl,gpsBr;
                            pi::calcLngLatFromDistance(d->gpsOrigin().x,d->gpsOrigin().y,worldTl.x,worldTl.y,gpsTl.x,gpsTl.y);
                            pi::calcLngLatFromDistance(d->gpsOrigin().x,d->gpsOrigin().y,worldBr.x,worldBr.y,gpsBr.x,gpsBr.y);
//                            cout<<"world:"<<worldBr<<"origin:"<<d->gpsOrigin()<<endl;
                            cmd<<"Map2DUpdate LastTexMat "<< setiosflags(ios::fixed)
                              << setprecision(9)<<gpsTl<<" "<<gpsBr;
//                            cout<<cmd.str()<<endl;
                            scommand.Call("MapWidget",cmd.str());
                            pi::timer.leave("MultiBandMap2DCPU::fuseGoogle");

                        }
                    }
                }
                if(ele->texName)
                {
                    glBindTexture(GL_TEXTURE_2D,ele->texName);
                    glBegin(GL_QUADS);
                    glTexCoord2f(0.0f, 0.0f); glVertex3f(x0,y0,0);
                    glTexCoord2f(0.0f, 1.0f); glVertex3f(x0,y1,0);
                    glTexCoord2f(1.0f, 1.0f); glVertex3f(x1,y1,0);
                    glTexCoord2f(1.0f, 0.0f); glVertex3f(x1,y0,0);
                    glEnd();
                }
            }
        }
    glBindTexture(GL_TEXTURE_2D, last_texture_ID);
    glPopMatrix();
}

bool MultiBandMap2DCPU::save(const std::string& filename)
{
    // determin minmax
    SPtr<MultiBandMap2DCPUPrepare> p;
    SPtr<MultiBandMap2DCPUData>    d;
    {
        pi::ReadMutex lock(mutex);
        p=prepared;d=data;
    }
    if(d->w()==0||d->h()==0) return false;

    pi::Point2i minInt(1e6,1e6),maxInt(-1e6,-1e6);
    int contentCount=0;
    for(int x=0;x<d->w();x++)
        for(int y=0;y<d->h();y++)
        {
            SPtr<MultiBandMap2DCPUEle> ele=d->data()[x+y*d->w()];
            if(!ele.get()) continue;
            {
                pi::ReadMutex lock(ele->mutexData);
                if(!ele->pyr_laplace.size()) continue;
            }
            contentCount++;
            minInt.x=min(minInt.x,x); minInt.y=min(minInt.y,y);
            maxInt.x=max(maxInt.x,x); maxInt.y=max(maxInt.y,y);
        }

    maxInt=maxInt+pi::Point2i(1,1);
    pi::Point2i wh=maxInt-minInt;
    vector<cv::Mat> pyr_laplace(_bandNum+1);
    vector<cv::Mat> pyr_weights(_bandNum+1);
    for(int i=0;i<=0;i++)
        pyr_weights[i]=cv::Mat::zeros(wh.y*ELE_PIXELS,wh.x*ELE_PIXELS,CV_32FC1);

    for(int x=minInt.x;x<maxInt.x;x++)
        for(int y=minInt.y;y<maxInt.y;y++)
        {
            SPtr<MultiBandMap2DCPUEle> ele=d->data()[x+y*d->w()];
            if(!ele.get()) continue;
            {
                pi::ReadMutex lock(ele->mutexData);
                if(!ele->pyr_laplace.size()) continue;
                int width=ELE_PIXELS,height=ELE_PIXELS;

                for (int i = 0; i <= _bandNum; ++i)
                {
                    cv::Rect rect(width*(x-minInt.x),height*(y-minInt.y),width,height);
                    if(pyr_laplace[i].empty())
                        pyr_laplace[i]=cv::Mat::zeros(wh.y*height,wh.x*width,ele->pyr_laplace[i].type());
                    ele->pyr_laplace[i].copyTo(pyr_laplace[i](rect));
                    if(i==0)
                        ele->weights[i].copyTo(pyr_weights[i](rect));
                    height>>=1;width>>=1;
                }
            }
        }

    cv::detail::restoreImageFromLaplacePyr(pyr_laplace);

    cv::Mat result=pyr_laplace[0];
    if(result.type()==CV_16SC3) result.convertTo(result,CV_8UC3);
    result.setTo(cv::Scalar::all(svar.GetInt("Result.BackGroundColor")),pyr_weights[0]==0);
    cv::imwrite(filename,result);
    cout<<"Resolution:["<<result.cols<<" "<<result.rows<<"]";
    if(svar.exist("GPS.Origin"))
          cout<<",_lengthPixel:"<<d->lengthPixel()
       <<",Area:"<<contentCount*d->eleSize()*d->eleSize()<<endl;
    return true;
}
