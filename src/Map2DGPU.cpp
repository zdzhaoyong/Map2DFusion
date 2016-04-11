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
#ifdef HAS_CUDA
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <GL/glew.h>
#include <GL/gl.h>
#include <cuda_gl_interop.h>

#include <base/Svar/Svar.h>
#include <base/time/Global_Timer.h>
#include <gui/gl/glHelper.h>
#include <gui/gl/SignalHandle.h>

#include "Map2DGPU.h"
#include "UtilGPU.cuh"

using namespace std;

cudaGraphicsResource* cuda_pbo_resource=NULL;
uint                  pbo=0;
int                   blenderBandNum=1;
uint                  eleImageDataSize=0;

/**

  __________max
  |    |    |
  |____|____|
  |    |    |
  |____|____|
 min
 */

Map2DGPU::Map2DGPUEle::~Map2DGPUEle()
{
    if(texName) pi::gl::Signal_Handle::instance().delete_texture(texName);
    if(cuda_pbo_resource)
    {
        checkCudaErrors(cudaGraphicsUnregisterResource(cuda_pbo_resource));
        cuda_pbo_resource=NULL;
    }
    if(pbo)
    {
        glDeleteBuffersARB(1, &pbo);
        pbo=0;
    }
    if(img)
    {
        pi::WriteMutex lock(mutexData);
        checkCudaErrors(cudaFree(img));
    }
}

// this is a bad idea, just for test
bool Map2DGPU::Map2DGPUEle::updateTextureCPU()
{
    size_t num_bytes=ELE_PIXELS*ELE_PIXELS*sizeof(float4);
    cv::Mat tmp(ELE_PIXELS,ELE_PIXELS,CV_32FC4);
    checkCudaErrors(cudaMemcpy(tmp.data,img,num_bytes,cudaMemcpyDeviceToHost));

    if(texName==0)// create texture
    {
        glGenTextures(1, &texName);
        glBindTexture(GL_TEXTURE_2D,texName);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GL_RGBA,tmp.cols,tmp.rows, 0,
                     GL_BGRA, GL_FLOAT,tmp.data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D,texName);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GL_RGBA,tmp.cols,tmp.rows, 0,
                     GL_BGRA, GL_FLOAT,tmp.data);
    }
    Ischanged=false;
}

bool Map2DGPU::Map2DGPUEle::updateTextureGPU()
{
    size_t num_bytes=ELE_PIXELS*ELE_PIXELS*sizeof(float4);
    if(pbo==0)// bind pbo with cuda_pbo_resource
    {
        glGenBuffersARB(1, &pbo);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, num_bytes, 0, GL_DYNAMIC_COPY);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

        // register this buffer object with CUDA
        checkCudaErrors(cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, pbo, cudaGraphicsMapFlagsWriteDiscard));
    }
    //flush data from ele->img to ele->cuda_pbo_resource
    {
        checkCudaErrors(cudaGraphicsMapResources(1, &cuda_pbo_resource, 0));
        size_t bytesNum;
        float4* pboData=NULL;
        checkCudaErrors(cudaGraphicsResourceGetMappedPointer((void **)&pboData, &bytesNum, cuda_pbo_resource));
        if(num_bytes==bytesNum)
        {
            if(blenderBandNum==1)
                checkCudaErrors(cudaMemcpy(pboData,img,bytesNum,cudaMemcpyDeviceToDevice));
        }
        checkCudaErrors(cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0));
    }

    if(texName==0)// create texture
    {
        glGenTextures(1, &texName);
        glBindTexture(GL_TEXTURE_2D,texName);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ELE_PIXELS, ELE_PIXELS,
                     0, GL_BGRA, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    }
    else//flush buffer to texture
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB,pbo);
        glBindTexture(GL_TEXTURE_2D,texName);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ELE_PIXELS, ELE_PIXELS,/*window_width, window_height,*/
                        GL_BGRA, GL_FLOAT, NULL);
    }
    Ischanged=false;
}

bool Map2DGPU::Map2DGPUData::prepare(SPtr<Map2DGPUPrepare> prepared)
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
        double minh;
        if(_min.z>0) minh=_min.z;
        else minh=-_max.z;
        pi::Point3d line=prepared->UnProject(pi::Point2d(prepared->_camera.w,prepared->_camera.h))
                -prepared->UnProject(pi::Point2d(0,0));
        double radius=0.5*minh*sqrt((line.x*line.x+line.y*line.y));
        _lengthPixel=2*radius/sqrt(prepared->_camera.w*prepared->_camera.w
                                   +prepared->_camera.h*prepared->_camera.h);
        _lengthPixel/=svar.GetDouble("Map2D.Scale",1);
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
    return true;
}

Map2DGPU::Map2DGPU(bool thread)
    :alpha(svar.GetInt("Map2D.Alpha",0)),
     _valid(false),_thread(thread)
{
    {
        eleImageDataSize=0;
        int levSize=sizeof(float4)*ELE_PIXELS*ELE_PIXELS;
        for(int i=0;i<blenderBandNum;i++)
        {
            eleImageDataSize+=levSize;
            levSize=levSize>>2;
        }
    }
    // Otherwise pick the device with highest Gflops/s
//    int devID = gpuGetMaxGflopsDeviceId();
}

bool Map2DGPU::prepare(const pi::SE3d& plane,const PinHoleParameters& camera,
                const std::deque<std::pair<cv::Mat,pi::SE3d> >& frames)
{
    //insert frames
    SPtr<Map2DGPUPrepare> p(new Map2DGPUPrepare);
    SPtr<Map2DGPUData>    d(new Map2DGPUData);

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

bool Map2DGPU::feed(cv::Mat img,const pi::SE3d& pose)
{
    if(!_valid) return false;
    SPtr<Map2DGPUPrepare> p;
    SPtr<Map2DGPUData>    d;
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

bool Map2DGPU::renderFrame(const std::pair<cv::Mat,pi::SE3d>& frame)
{
    SPtr<Map2DGPUPrepare> p;
    SPtr<Map2DGPUData>    d;
    {
        pi::ReadMutex lock(mutex);
        p=prepared;d=data;
    }
    if(frame.first.cols!=p->_camera.w||frame.first.rows!=p->_camera.h
            ||frame.first.type()!=CV_8UC3)
    {
        cerr<<"Map2DGPU::renderFrame: frame.first.cols!=p->_camera.w||frame.first.rows!=p->_camera.h||frame.first.type()!=CV_8UC3\n";
        return false;
    }
    // pose->pts
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
    // dest location?
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
    if(xminInt<0||yminInt<0||xmaxInt>d->w()||
            ymaxInt>d->h()||xminInt>=xmaxInt||yminInt>=ymaxInt)
    {
//        cerr<<"Map2DGPU::renderFrame:should never happen!\n";
        return false;
    }
    {
        xmin=d->min().x+d->eleSize()*xminInt;
        ymin=d->min().y+d->eleSize()*yminInt;
        xmax=d->min().x+d->eleSize()*xmaxInt;
        ymax=d->min().y+d->eleSize()*ymaxInt;
    }
    // prepare dst image
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

    cv::Mat inv = cv::getPerspectiveTransform( destPoints,imgPtsCV);
    inv.convertTo(inv,CV_32FC1);
    //warp and render with CUDA
    pi::timer.enter("Map2DGPU::UploadImage");
    CudaImage<uchar3> cudaFrame(frame.first.rows,frame.first.cols);
    checkCudaErrors(cudaMemcpy(cudaFrame.data,frame.first.data,
               cudaFrame.cols*cudaFrame.rows*sizeof(uchar3),cudaMemcpyHostToDevice));
    pi::timer.leave("Map2DGPU::UploadImage");

    // apply dst to eles
    pi::timer.enter("Map2DGPU::Apply");
    std::vector<SPtr<Map2DGPUEle> > dataCopy=d->data();
    pi::Point3d translation=frame.second.get_translation();
    int cenX=(translation.x-d->min().x)*d->lengthPixelInv();
    int cenY=(translation.y-d->min().y)*d->lengthPixelInv();
    {
        int w=xmaxInt-xminInt;
        int h=ymaxInt-yminInt;
        int wh=w*h;

        float4** out_datas=new float4*[wh];
        bool* freshs=new bool[wh];
        float* invs=new float[wh*9];
        int*  centers=new int[wh*2];

        for(int x=xminInt,i=0;x<xmaxInt;x++,i++)
            for(int y=yminInt,j=0;y<ymaxInt;y++,j++)
            {
                int idx=(i+j*w);
                cv::Mat trans=cv::Mat::eye(3,3,CV_32FC1);
                trans.at<float>(2)=i*ELE_PIXELS;
                trans.at<float>(5)=j*ELE_PIXELS;
                trans=inv*trans;
                trans.convertTo(trans,CV_32FC1);
                memcpy(invs+idx*9,trans.data,sizeof(float)*9);

                centers[idx*2]=cenX-x*ELE_PIXELS;
                centers[idx*2+1]=cenY-y*ELE_PIXELS;

                SPtr<Map2DGPUEle> ele=dataCopy[y*d->w()+x];
                bool fresh=false;
                if(!ele.get())
                {
                    ele=d->ele(y*d->w()+x);
                }
                {
                    ele->mutexData.lock();
                    if(!ele->img)
                    {
                        checkCudaErrors(cudaMalloc((void**)&ele->img,sizeof(float4)*ELE_PIXELS*ELE_PIXELS));
                        fresh=true;
                    }
                    out_datas[idx]=ele->img;
                    freshs[idx]=fresh;

                }
            }

        pi::timer.enter("RenderKernal");
        bool success=renderFramesCaller(cudaFrame,ELE_PIXELS,ELE_PIXELS,
                                out_datas,freshs,
                                invs,centers,wh);

        pi::timer.leave("RenderKernal");
        for(int x=xminInt,i=0;x<xmaxInt;x++,i++)
            for(int y=yminInt,j=0;y<ymaxInt;y++,j++)
            {
                SPtr<Map2DGPUEle> ele=dataCopy[y*d->w()+x];
                if(!ele.get())
                {
                    ele=d->ele(y*d->w()+x);
                }
                ele->mutexData.unlock();
                if(success)
                    ele->Ischanged=true;
            }
        delete[] out_datas;
        delete[] freshs;
        delete[] invs;
        delete[] centers;
    }


    pi::timer.leave("Map2DGPU::Apply");

    if(!svar.GetInt("Win3D.Enable"))//show result
    {
        cv::Mat result(ELE_PIXELS*d->h(),ELE_PIXELS*d->w(),CV_32FC4);
        cv::Mat tmp(ELE_PIXELS,ELE_PIXELS,CV_32FC4);
        for(int x=0;x<d->w();x++)
            for(int y=0;y<d->h();y++)
        {
            SPtr<Map2DGPUEle> ele=dataCopy[y*d->w()+x];
            if(!ele.get()) continue;
            pi::ReadMutex lock(ele->mutexData);
            cudaMemcpy(tmp.data,ele->img,ELE_PIXELS*ELE_PIXELS*sizeof(float4),cudaMemcpyDeviceToHost);
            tmp.copyTo(result(cv::Rect(ELE_PIXELS*x,ELE_PIXELS*y,ELE_PIXELS,ELE_PIXELS)));
        }
        cv::resize(result,result,cv::Size(1000,result.rows*1000/result.cols));
        cv::imshow("img",result);
        cv::waitKey(0);
    }
    return true;
}


bool Map2DGPU::spreadMap(double xmin,double ymin,double xmax,double ymax)
{
    pi::timer.enter("Map2DGPU::spreadMap");
    SPtr<Map2DGPUData> d;
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
    std::vector<SPtr<Map2DGPUEle> > dataOld=d->data();
    std::vector<SPtr<Map2DGPUEle> > dataCopy;
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
        data=SPtr<Map2DGPUData>(new Map2DGPUData(d->eleSize(),d->lengthPixel(),
                                                 pi::Point3d(max.x,max.y,d->max().z),
                                                 pi::Point3d(min.x,min.y,d->min().z),
                                                 w,h,dataCopy));
    }
    pi::timer.leave("Map2DGPU::spreadMap");
    return true;
}

bool Map2DGPU::getFrame(std::pair<cv::Mat,pi::SE3d>& frame)
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

void Map2DGPU::run()
{
    std::pair<cv::Mat,pi::SE3d> frame;
    while(!shouldStop())
    {
        if(_valid)
        {
            if(getFrame(frame))
            {
                pi::timer.enter("Map2DGPU::renderFrame");
                renderFrame(frame);
                pi::timer.leave("Map2DGPU::renderFrame");
            }
        }
        sleep(10);
    }
}

void Map2DGPU::draw()
{
    if(!_valid) return;
    static bool inited=false;
    if(!inited)
    {
//        cudaGLSetGLDevice(0);
        glewInit();
    }

    SPtr<Map2DGPUPrepare> p;
    SPtr<Map2DGPUData>    d;
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
    if(1)
    {
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
        std::vector<SPtr<Map2DGPUEle> > dataCopy=d->data();
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
                SPtr<Map2DGPUEle> ele=dataCopy[idxData];
                if(!ele.get())  continue;
                if(!ele->img) continue;
                if(ele->Ischanged&&ticTac.Tac()<0.02)
                {
                    pi::timer.enter("glTexImage2D");
                    pi::ReadMutex lock1(ele->mutexData);

                    ele->updateTextureGPU();
                    pi::timer.leave("glTexImage2D");
                }

                // draw things
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
        glBindTexture(GL_TEXTURE_2D, last_texture_ID);
    }
    glPopMatrix();
}

bool Map2DGPU::save(const std::string& filename)
{
    // determin minmax
    SPtr<Map2DGPUPrepare> p;
    SPtr<Map2DGPUData>    d;
    {
        pi::ReadMutex lock(mutex);
        p=prepared;d=data;
    }
    if(d->w()==0||d->h()==0) return false;

    pi::Point2i minInt(1e6,1e6),maxInt(-1e6,-1e6);
    for(int x=0;x<d->w();x++)
        for(int y=0;y<d->h();y++)
        {
            SPtr<Map2DGPUEle> ele=d->data()[x+y*d->w()];
            if(!ele.get()) continue;
            {
                pi::ReadMutex lock(ele->mutexData);
                if(!ele->img) continue;
            }
            minInt.x=min(minInt.x,x); minInt.y=min(minInt.y,y);
            maxInt.x=max(maxInt.x,x); maxInt.y=max(maxInt.y,y);
        }

    maxInt=maxInt+pi::Point2i(1,1);
    pi::Point2i wh=maxInt-minInt;
    cv::Mat result(wh.y*ELE_PIXELS,wh.x*ELE_PIXELS,CV_32FC4);

    cv::Mat tmp(ELE_PIXELS,ELE_PIXELS,CV_32FC4);
    for(int x=minInt.x;x<d->w();x++)
        for(int y=minInt.y;y<d->h();y++)
    {
        SPtr<Map2DGPUEle> ele=d->data()[y*d->w()+x];
        if(!ele.get()) continue;
        pi::ReadMutex lock(ele->mutexData);
        cudaMemcpy(tmp.data,ele->img,ELE_PIXELS*ELE_PIXELS*sizeof(float4),cudaMemcpyDeviceToHost);
        tmp.copyTo(result(cv::Rect(ELE_PIXELS*(x-minInt.x),ELE_PIXELS*(y-minInt.y),ELE_PIXELS,ELE_PIXELS)));
    }
    result.convertTo(result,CV_8UC3,255.);
    cv::imwrite(filename,result);
    return true;
}
#endif
