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
#include "Map2DRender.h"
#include <gui/gl/glHelper.h>
#include <GL/gl.h>
#include <base/Svar/Svar.h>
#include <base/time/Global_Timer.h>
#include <gui/gl/SignalHandle.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/stitching/detail/seam_finders.hpp>
#include <opencv2/stitching/detail/blenders.hpp>

#undef HAVE_OPENCV_GPU

using namespace std;

namespace Map2DFusion {
#define WEIGHT_EPS (1e-5)

bool mulWeightMap(const cv::Mat& weight, cv::Mat& src)
{
    if(!(src.type()==CV_16SC3&&weight.type()==CV_32FC1)) return false;
    pi::Point3_<int16_t>* srcP=(pi::Point3_<int16_t>*)src.data;
    float*    weightP=(float*)weight.data;
    for(float* Pend=weightP+weight.cols*weight.rows;weightP!=Pend;weightP++,srcP++)
//        *srcP=(*srcP)*(*weightP);
        if(!(*weightP)) (*srcP)=pi::Point3_<int16_t>(0,0,0);
    return true;
}

class CV_EXPORTS MultiBandBlender : public cv::detail::Blender
{
public:
    MultiBandBlender(int try_gpu = false, int num_bands = 5, int weight_type = CV_32F)
    {
        setNumBands(num_bands);
    #if defined(HAVE_OPENCV_GPU) && !defined(DYNAMIC_CUDA_SUPPORT)
        can_use_gpu_ = try_gpu && gpu::getCudaEnabledDeviceCount();
    #else
        (void)try_gpu;
        can_use_gpu_ = false;
    #endif
        CV_Assert(weight_type == CV_32F || weight_type == CV_16S);
        weight_type_ = weight_type;
    }

    int numBands() const { return actual_num_bands_; }
    void setNumBands(int val) { actual_num_bands_ = val; }

    void prepare(cv::Rect dst_roi)
    {
        using namespace cv;
        dst_roi_final_ = dst_roi;

        // Crop unnecessary bands
        double max_len = static_cast<double>(max(dst_roi.width, dst_roi.height));
        num_bands_ = min(actual_num_bands_, static_cast<int>(ceil(log(max_len) / log(2.0))));

        // Add border to the final image, to ensure sizes are divided by (1 << num_bands_)
        dst_roi.width += ((1 << num_bands_) - dst_roi.width % (1 << num_bands_)) % (1 << num_bands_);
        dst_roi.height += ((1 << num_bands_) - dst_roi.height % (1 << num_bands_)) % (1 << num_bands_);

        Blender::prepare(dst_roi);

        dst_pyr_laplace_.resize(num_bands_ + 1);
        dst_pyr_laplace_[0] = dst_;

        dst_band_weights_.resize(num_bands_ + 1);
        dst_band_weights_[0].create(dst_roi.size(), weight_type_);
        dst_band_weights_[0].setTo(0);

        for (int i = 1; i <= num_bands_; ++i)
        {
            dst_pyr_laplace_[i].create((dst_pyr_laplace_[i - 1].rows + 1) / 2,
                                       (dst_pyr_laplace_[i - 1].cols + 1) / 2, CV_16SC3);
            dst_band_weights_[i].create((dst_band_weights_[i - 1].rows + 1) / 2,
                                        (dst_band_weights_[i - 1].cols + 1) / 2, weight_type_);
            dst_pyr_laplace_[i].setTo(Scalar::all(0));
            dst_band_weights_[i].setTo(0);
        }
    }
    void feed(const cv::Mat &img, const cv::Mat &mask, cv::Point tl)
    {
        using namespace cv;
        using namespace cv::detail;
        CV_Assert(img.type() == CV_16SC3 || img.type() == CV_8UC3);
        CV_Assert(mask.type() == CV_8U);

        // Keep source image in memory with small border
        int gap = 3 * (1 << num_bands_);
        Point tl_new(max(dst_roi_.x, tl.x - gap),
                     max(dst_roi_.y, tl.y - gap));
        Point br_new(min(dst_roi_.br().x, tl.x + img.cols + gap),
                     min(dst_roi_.br().y, tl.y + img.rows + gap));

        // Ensure coordinates of top-left, bottom-right corners are divided by (1 << num_bands_).
        // After that scale between layers is exactly 2.
        //
        // We do it to avoid interpolation problems when keeping sub-images only. There is no such problem when
        // image is bordered to have size equal to the final image size, but this is too memory hungry approach.
        tl_new.x = dst_roi_.x + (((tl_new.x - dst_roi_.x) >> num_bands_) << num_bands_);
        tl_new.y = dst_roi_.y + (((tl_new.y - dst_roi_.y) >> num_bands_) << num_bands_);
        int width = br_new.x - tl_new.x;
        int height = br_new.y - tl_new.y;
        width += ((1 << num_bands_) - width % (1 << num_bands_)) % (1 << num_bands_);
        height += ((1 << num_bands_) - height % (1 << num_bands_)) % (1 << num_bands_);
        br_new.x = tl_new.x + width;
        br_new.y = tl_new.y + height;
        int dy = max(br_new.y - dst_roi_.br().y, 0);
        int dx = max(br_new.x - dst_roi_.br().x, 0);
        tl_new.x -= dx; br_new.x -= dx;
        tl_new.y -= dy; br_new.y -= dy;

        int top = tl.y - tl_new.y;
        int left = tl.x - tl_new.x;
        int bottom = br_new.y - tl.y - img.rows;
        int right = br_new.x - tl.x - img.cols;

        // Create the source image Laplacian pyramid
        Mat img_with_border;
        copyMakeBorder(img, img_with_border, top, bottom, left, right,
                       BORDER_REFLECT);
        vector<Mat> src_pyr_laplace;
        if (can_use_gpu_ && img_with_border.depth() == CV_16S)
            createLaplacePyrGpu(img_with_border, num_bands_, src_pyr_laplace);
        else
            createLaplacePyr(img_with_border, num_bands_, src_pyr_laplace);

        // Create the weight map Gaussian pyramid
        Mat weight_map;
        vector<Mat> weight_pyr_gauss(num_bands_ + 1);

        if(weight_type_ == CV_32F)
        {
            mask.convertTo(weight_map, CV_32F, 1./255.);
        }
        else// weight_type_ == CV_16S
        {
            mask.convertTo(weight_map, CV_16S);
            add(weight_map, 1, weight_map, mask != 0);
        }

        copyMakeBorder(weight_map, weight_pyr_gauss[0], top, bottom, left, right, BORDER_CONSTANT);

        for (int i = 0; i < num_bands_; ++i)
            pyrDown(weight_pyr_gauss[i], weight_pyr_gauss[i + 1]);

        int y_tl = tl_new.y - dst_roi_.y;
        int y_br = br_new.y - dst_roi_.y;
        int x_tl = tl_new.x - dst_roi_.x;
        int x_br = br_new.x - dst_roi_.x;

        if(svar.GetInt("Map2DRender.ShowPyrLaplace",1))
        {
            vector<cv::Mat> pyr_laplaceClone(src_pyr_laplace.size());
            for(int i=0;i<src_pyr_laplace.size();i++)
            {
                pyr_laplaceClone[i]=src_pyr_laplace[i].clone();
                mulWeightMap(weight_pyr_gauss[i],pyr_laplaceClone[i]);
//                normalizeUsingWeightMap(weight_pyr_gauss[i],pyr_laplaceClone[i]);
                {
                    cv::Mat result;pyr_laplaceClone[i].convertTo(result,CV_8U);
                    cv::imshow("pyrImage",result);
                    cv::imshow("pyrWeight",weight_pyr_gauss[i]);
                    cv::waitKey(0);
                }
            }
            restoreImageFromLaplacePyr(pyr_laplaceClone);
            cv::Mat result=pyr_laplaceClone[0];
            result.convertTo(result,CV_8U);
            result.setTo(cv::Scalar::all(0),weight_pyr_gauss[0]==0);
            cv::imshow("imgWithBorder",img_with_border);
            cv::imshow("weight",weight_pyr_gauss[0]);
            cv::imshow("restoreImage",result);
            cv::waitKey(0);
        }
        // Add weighted layer of the source image to the final Laplacian pyramid layer
        if(weight_type_ == CV_32F)
        {
            for (int i = 0; i <= num_bands_; ++i)
            {
                for (int y = y_tl; y < y_br; ++y)
                {
                    int y_ = y - y_tl;
                    const Point3_<short>* src_row = src_pyr_laplace[i].ptr<Point3_<short> >(y_);
                    Point3_<short>* dst_row = dst_pyr_laplace_[i].ptr<Point3_<short> >(y);
                    const float* weight_row = weight_pyr_gauss[i].ptr<float>(y_);
                    float* dst_weight_row = dst_band_weights_[i].ptr<float>(y);

                    for (int x = x_tl; x < x_br; ++x)
                    {
                        int x_ = x - x_tl;
#if 1
                        if(weight_row[x_]>=dst_weight_row[x])
                        {
                            dst_weight_row[x]=weight_row[x_];
                            dst_row[x]=src_row[x_];
//                            dst_row[x]=(dst_row[x]*dst_weight_row[x]+src_row[x_]*weight_row[x_])*(1./(dst_weight_row[x]+weight_row[x_]+1e-5));
                        }
#else
                        dst_row[x].x += static_cast<short>(src_row[x_].x * weight_row[x_]);
                        dst_row[x].y += static_cast<short>(src_row[x_].y * weight_row[x_]);
                        dst_row[x].z += static_cast<short>(src_row[x_].z * weight_row[x_]);
                        dst_weight_row[x] += weight_row[x_];
                        should_normalize=true;
#endif
                    }
                }
                x_tl /= 2; y_tl /= 2;
                x_br /= 2; y_br /= 2;
            }
        }
        else// weight_type_ == CV_16S
        {
            for (int i = 0; i <= num_bands_; ++i)
            {
                for (int y = y_tl; y < y_br; ++y)
                {
                    int y_ = y - y_tl;
                    const Point3_<short>* src_row = src_pyr_laplace[i].ptr<Point3_<short> >(y_);
                    Point3_<short>* dst_row = dst_pyr_laplace_[i].ptr<Point3_<short> >(y);
                    const short* weight_row = weight_pyr_gauss[i].ptr<short>(y_);
                    short* dst_weight_row = dst_band_weights_[i].ptr<short>(y);

                    for (int x = x_tl; x < x_br; ++x)
                    {
                        int x_ = x - x_tl;
                        dst_row[x].x += short((src_row[x_].x * weight_row[x_]) >> 8);
                        dst_row[x].y += short((src_row[x_].y * weight_row[x_]) >> 8);
                        dst_row[x].z += short((src_row[x_].z * weight_row[x_]) >> 8);
                        dst_weight_row[x] += weight_row[x_];
                    }
                }
                x_tl /= 2; y_tl /= 2;
                x_br /= 2; y_br /= 2;
            }
        }
    }
    void blend(cv::Mat &dst, cv::Mat &dst_mask)
    {
        using namespace cv::detail;
        using namespace cv;

        if(should_normalize)
        for (int i = 0; i <= num_bands_; ++i)
            normalizeUsingWeightMap(dst_band_weights_[i], dst_pyr_laplace_[i]);

        if (can_use_gpu_)
            restoreImageFromLaplacePyrGpu(dst_pyr_laplace_);
        else
            restoreImageFromLaplacePyr(dst_pyr_laplace_);


        dst_ = dst_pyr_laplace_[0];
        dst_mask_ = dst_band_weights_[0] > WEIGHT_EPS;

//        {
//            cv::Mat result=dst_;
//            result.convertTo(result,CV_8U);
//            result.setTo(Scalar::all(0), dst_mask_ == 0);
//            cv::imshow("dst_",result);
//            cv::imshow("dst_mask_",dst_mask_);
//            cv::waitKey(0);
//        }

        dst_ = dst_(Range(0, dst_roi_final_.height), Range(0, dst_roi_final_.width));
        dst_mask_ = dst_mask_(Range(0, dst_roi_final_.height), Range(0, dst_roi_final_.width));
        dst_pyr_laplace_.clear();
        dst_band_weights_.clear();

//        {
//            cv::Mat result=dst_;
//            result.convertTo(result,CV_8U);
//            cv::imshow("dst_",result);
//            cv::imshow("dst_mask_",dst_mask_);
//            cv::waitKey(0);
//        }

        Blender::blend(dst, dst_mask);
    }

private:
    int actual_num_bands_, num_bands_;
    std::vector<cv::Mat> dst_pyr_laplace_;
    std::vector<cv::Mat> dst_band_weights_;
    cv::Rect dst_roi_final_;
    bool can_use_gpu_,should_normalize;
    int weight_type_; //CV_32F or CV_16S
};
}

/**

  __________max
  |    |    |
  |____|____|
  |    |    |
  |____|____|
 min
 */

Map2DRender::Map2DRenderEle::~Map2DRenderEle()
{
    //    if(texName) pi::gl::Signal_Handle::instance().delete_texture(texName);
}

bool Map2DRender::Map2DRenderPrepare::prepare(const pi::SE3d& plane,const PinHoleParameters& camera,
                                              const std::deque<std::pair<cv::Mat,pi::SE3d> >& frames)
{
    if(frames.size()==0||camera.w<=0||camera.h<=0||camera.fx==0||camera.fy==0)
    {
        cerr<<"Map2DRender::Map2DRenderPrepare::prepare:Not valid prepare!\n";
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

bool Map2DRender::Map2DRenderData::prepare(SPtr<Map2DRenderPrepare> prepared)
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

Map2DRender::Map2DRender(bool thread)
    :alpha(svar.GetInt("Map2D.Alpha",0)),
      _valid(false),_thread(thread)
{
}

bool Map2DRender::prepare(const pi::SE3d& plane,const PinHoleParameters& camera,
                          const std::deque<std::pair<cv::Mat,pi::SE3d> >& frames)
{
    //insert frames
    SPtr<Map2DRenderPrepare> p(new Map2DRenderPrepare);
    SPtr<Map2DRenderData>    d(new Map2DRenderData);

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

bool Map2DRender::feed(cv::Mat img,const pi::SE3d& pose)
{
    if(!_valid) return false;
    SPtr<Map2DRenderPrepare> p;
    SPtr<Map2DRenderData>    d;
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

bool Map2DRender::getFrame(std::pair<cv::Mat,pi::SE3d>& frame)
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

bool Map2DRender::renderFrame(const std::pair<cv::Mat,pi::SE3d>& frame)
{
    return false;
}

bool Map2DRender::getFrames(std::deque<std::pair<cv::Mat,pi::SE3d> >& frames)
{
    pi::ReadMutex lock(mutex);
    pi::ReadMutex lock1(prepared->mutexFrames);
    if(prepared->_frames.size())
    {
        frames=prepared->_frames;
        prepared->_frames.clear();
        return true;
    }
    else return false;
}

bool Map2DRender::renderFrames(std::deque<std::pair<cv::Mat,pi::SE3d> >& frames)
{
    // 0. Prepare things
    SPtr<Map2DRenderPrepare> p;
    SPtr<Map2DRenderData>    d;
    {
        pi::ReadMutex lock(mutex);
        p=prepared;d=data;
    }

    std::vector<pi::Point2d>  imgPts;
    {
        imgPts.reserve(4);
        imgPts.push_back(pi::Point2d(0,0));
        imgPts.push_back(pi::Point2d(p->_camera.w,0));
        imgPts.push_back(pi::Point2d(0,p->_camera.h));
        imgPts.push_back(pi::Point2d(p->_camera.w,p->_camera.h));
    }

    std::vector<cv::Point2f>          imgPtsCV;
    {
        imgPtsCV.reserve(imgPts.size());
        for(int i=0;i<imgPts.size();i++)
            imgPtsCV.push_back(cv::Point2f(imgPts[i].x,imgPts[i].y));
    }

    {
        pi::WriteMutex lock(mutex);
        if(weightImage.empty())
        {
            int w=p->_camera.w;
            int h=p->_camera.h;
            weightImage=cv::Mat(h,w,CV_8UC1,cv::Scalar(255));
            if(1)
            {
                pi::byte *p=(weightImage.data);
                float x_center=w*0.5;
                float y_center=h*0.5;
                float dis_maxInv=1./sqrt(x_center*x_center+y_center*y_center);
                for(int i=0;i<h;i++)
                    for(int j=0;j<w;j++,p++)
                    {
                        float dis=(i-y_center)*(i-y_center)+(j-x_center)*(j-x_center);
                        dis=1-sqrt(dis)*dis_maxInv;
                        *p=dis*dis*254;
                        if(*p<1) *p=1;
                    }
            }
        }
    }
    // 1. Unproject frames to the plane and warp the images while update the area
    std::vector<cv::Mat>        imgwarped(frames.size());
    std::vector<cv::Mat>        maskwarped(frames.size());
    std::vector<cv::Point2f>    cornersWorld(frames.size());
    std::vector<cv::Size>       sizes(frames.size());

    pi::Point2d min,max;
    int idx=0;
    std::vector<pi::Point2d>  planePts=imgPts;
    pi::Point3d downLook(0,0,-1);
    for(std::deque<std::pair<cv::Mat,pi::SE3d> >::iterator it=frames.begin();
        it<frames.end();it++,idx++)
    {
        cv::Mat& img=it->first;
        pi::SE3d& pose=it->second;
        pi::Point2d curMin(1e6,1e6),curMax(-1e6,-1e6);
        bool bOK=true;
        if(pose.get_translation().z<0) downLook=pi::Point3d(0,0,1);
        else downLook=pi::Point3d(0,0,-1);
        for(int j=0;j<imgPts.size();j++)
        {
            pi::Point3d axis=pose.get_rotation()*p->UnProject(imgPts[j]);
            if(axis.dot(downLook)<0.4)
            {
                bOK=false;break;
            }
            axis=pose.get_translation()-axis*(pose.get_translation().z/axis.z);
            planePts[j]=pi::Point2d(axis.x,axis.y);
        }
        if(!bOK)
        {
            continue;
        }

        for(int i=0;i<planePts.size();i++)
        {
            if(planePts[i].x<curMin.x) curMin.x=planePts[i].x;
            if(planePts[i].y<curMin.y) curMin.y=planePts[i].y;
            if(planePts[i].x>curMax.x) curMax.x=planePts[i].x;
            if(planePts[i].y>curMax.y) curMax.y=planePts[i].y;
        }
        {
            if(curMin.x<min.x) min.x=curMin.x;
            if(curMin.y<min.y) min.y=curMin.y;
            if(curMax.x>max.x) max.x=curMax.x;
            if(curMax.y>max.y) max.y=curMax.y;
        }
        cornersWorld[idx]=cv::Point2f(curMin.x,curMin.y);
        sizes[idx]=cv::Size((curMax.x-curMin.x)*d->lengthPixelInv(),
                            (curMax.y-curMin.y)*d->lengthPixelInv());

        std::vector<cv::Point2f> destPoints;
        destPoints.reserve(imgPtsCV.size());
        for(int i=0;i<imgPtsCV.size();i++)
        {
            destPoints.push_back(cv::Point2f((planePts[i].x-curMin.x)*d->lengthPixelInv(),
                                             (planePts[i].y-curMin.y)*d->lengthPixelInv()));
        }
        cv::Mat transmtx = cv::getPerspectiveTransform(imgPtsCV, destPoints);
        cv::warpPerspective(img, imgwarped[idx], transmtx, sizes[idx],cv::INTER_LINEAR,cv::BORDER_REFLECT);
        cv::warpPerspective(weightImage, maskwarped[idx], transmtx, sizes[idx],cv::INTER_NEAREST);
        if(0)
        {
            cv::imshow("imgwarped",imgwarped[idx]);
            cv::imshow("maskwarped",maskwarped[idx]);
            cv::waitKey(0);
        }
    }

    // 2. spread the map and find seams of warped images
    if(min.x<d->min().x||min.y<d->min().y||max.x>d->max().x||max.y>d->max().y)
    {
        if(!spreadMap(min.x,min.y,max.x,max.y))
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

    int xminInt=floor((min.x-d->min().x)*d->eleSizeInv());
    int yminInt=floor((min.y-d->min().y)*d->eleSizeInv());
    int xmaxInt= ceil((max.x-d->min().x)*d->eleSizeInv());
    int ymaxInt= ceil((max.y-d->min().y)*d->eleSizeInv());

    if(xminInt<0||yminInt<0||xmaxInt>d->w()||ymaxInt>d->h()||xminInt>=xmaxInt||yminInt>=ymaxInt)
    {
        //        cerr<<"Map2DCPU::renderFrame:should never happen!\n";
        return false;
    }
    {
        min.x=d->min().x+d->eleSize()*xminInt;
        min.y=d->min().y+d->eleSize()*yminInt;
        max.x=d->min().x+d->eleSize()*xmaxInt;
        max.y=d->min().y+d->eleSize()*ymaxInt;
    }
    std::vector<cv::Point> cornersImages(frames.size());
    for(int i=0;i<frames.size();i++)
    {
        if(imgwarped[i].empty())
        {
            cornersImages[i]=cv::Point(0,0);
            sizes[i]=cv::Size(0,0);
            continue;
        }
        cornersImages[i]=cv::Point((cornersWorld[i].x-min.x)*d->lengthPixelInv(),
                                   (cornersWorld[i].y-min.y)*d->lengthPixelInv());
    }
    if(svar.GetInt("Map2DRender.EnableSeam",1))//find seam
    {
        std::vector<cv::Mat>  seamwarped(frames.size());
        for(int i=0;i<maskwarped.size();i++)
        {
            seamwarped[i]=maskwarped[i].clone();
        }
        using namespace cv;
        using namespace cv::detail;
        string seam_find_type = "dp_colorgrad";
        cv::Ptr<cv::detail::SeamFinder> seam_finder;
        if (seam_find_type == "no")
            seam_finder = new cv::detail::NoSeamFinder();
        else if (seam_find_type == "voronoi")
            seam_finder = new cv::detail::VoronoiSeamFinder();
        else if (seam_find_type == "gc_color")
        {
#ifdef HAVE_OPENCV_GPU
            if (try_gpu && gpu::getCudaEnabledDeviceCount() > 0)
                seam_finder = new cv::detail::GraphCutSeamFinderGpu(GraphCutSeamFinderBase::COST_COLOR);
            else
#endif
                seam_finder = new cv::detail::GraphCutSeamFinder(cv::detail::GraphCutSeamFinderBase::COST_COLOR);
        }
        else if (seam_find_type == "gc_colorgrad")
        {
#ifdef HAVE_OPENCV_GPU
            if (try_gpu && gpu::getCudaEnabledDeviceCount() > 0)
                seam_finder = new cv::detail::GraphCutSeamFinderGpu(GraphCutSeamFinderBase::COST_COLOR_GRAD);
            else
#endif
                seam_finder = new cv::detail::GraphCutSeamFinder(GraphCutSeamFinderBase::COST_COLOR_GRAD);
        }
        else if (seam_find_type == "dp_color")
            seam_finder = new detail::DpSeamFinder(DpSeamFinder::COLOR);
        else if (seam_find_type == "dp_colorgrad")
            seam_finder = new detail::DpSeamFinder(DpSeamFinder::COLOR_GRAD);
        if (seam_finder.empty())
        {
            cout << "Can't create the following seam finder '" << seam_find_type << "'\n";
            return 1;
        }

        seam_finder->find(imgwarped, cornersImages, seamwarped);
        int eleSize=3;
        Mat element = getStructuringElement( 0,Size( 2*eleSize + 1, 2*eleSize+1 ), Point(eleSize, eleSize ) );
        for(int i=0;i<seamwarped.size();i++)
        {
            if(imgwarped[i].empty()) continue;
            dilate(seamwarped[i], seamwarped[i], element);
            maskwarped[i]=seamwarped[i]&maskwarped[i];
        }
    }

    // 3. blender images
    cv::Mat result, result_mask;
    {
        using namespace cv;
        using namespace cv::detail;
        Ptr<Blender> blender;
        int blend_type = Blender::MULTI_BAND;
        bool try_gpu = false;
        double blend_strength=5;

        if (blender.empty())
        {
            blender = Blender::createDefault(blend_type, try_gpu);
            Size dst_sz = cv::Size((xmaxInt-xminInt)*ELE_PIXELS,(ymaxInt-yminInt)*ELE_PIXELS);
            float blend_width = sqrt(static_cast<float>(dst_sz.area())) * blend_strength / 100.f;
            if (blend_width < 1.f)
                blender = Blender::createDefault(Blender::NO, try_gpu);
            else if (blend_type == Blender::MULTI_BAND)
            {
                Map2DFusion::MultiBandBlender* mb = new Map2DFusion::MultiBandBlender();
                mb->setNumBands(static_cast<int>(ceil(log(blend_width)/log(2.)) - 1.));
                blender=mb;
            }
            else if (blend_type == Blender::FEATHER)
            {
                FeatherBlender* fb = dynamic_cast<FeatherBlender*>(static_cast<Blender*>(blender));
                fb->setSharpness(1.f/blend_width);
            }
            blender->prepare(Rect(0,0,dst_sz.width,dst_sz.height));
        }

        // Blend the current image
        for(int i=0;i<frames.size();i++)
        {
            if(!imgwarped[i].empty())
            {
                if(blend_type == Blender::FEATHER)
                    imgwarped[i].convertTo(imgwarped[i],CV_16SC3);
                blender->feed(imgwarped[i], maskwarped[i], cornersImages[i]);
                if(0)
                {
                    cv::imshow("imgwarped",imgwarped[i]);
                    cv::imshow("maskwarped",maskwarped[i]);
                    cv::waitKey(0);
                }
            }
        }
        blender->blend(result, result_mask);
    }
    // 4.apply to the map
    if(0)
    {

    }
    else
    {
        cv::imwrite("result.jpg",result);
        result.convertTo(result,CV_8U);
        cv::resize(result,result,cv::Size(1000,1000./result.cols*result.rows));
        cv::resize(result_mask,result_mask,cv::Size(1000,1000./result_mask.cols*result_mask.rows));
        cv::imshow("result",result);
        cv::imshow("result_mask",result_mask);
        cv::waitKey(0);
    }
    stop();
    return true;
}


bool Map2DRender::spreadMap(double xmin,double ymin,double xmax,double ymax)
{
    pi::timer.enter("Map2DRender::spreadMap");
    SPtr<Map2DRenderData> d;
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
    std::vector<SPtr<Map2DRenderEle> > dataOld=d->data();
    std::vector<SPtr<Map2DRenderEle> > dataCopy;
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
        data=SPtr<Map2DRenderData>(new Map2DRenderData(d->eleSize(),d->lengthPixel(),
                                                       pi::Point3d(max.x,max.y,d->max().z),
                                                       pi::Point3d(min.x,min.y,d->min().z),
                                                       w,h,dataCopy));
    }
    pi::timer.leave("Map2DRender::spreadMap");
    return true;
}

void Map2DRender::run()
{
    std::deque<std::pair<cv::Mat,pi::SE3d> > frames;
    while(!shouldStop())
    {
        if(_valid)
        {
            if(getFrames(frames))
            {
                pi::timer.enter("Map2DRender::renderFrame");
                renderFrames(frames);
                pi::timer.leave("Map2DRender::renderFrame");
            }
        }
        sleep(10);
    }
    svar.GetInt("ShouldStop")=1;
}

void Map2DRender::draw()
{
    if(!_valid) return;

    SPtr<Map2DRenderPrepare> p;
    SPtr<Map2DRenderData>    d;
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
    std::vector<SPtr<Map2DRenderEle> > dataCopy=d->data();
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
            SPtr<Map2DRenderEle> ele=dataCopy[idxData];
            if(!ele.get())  continue;
            if(ele->img.empty()) continue;
            if(ele->texName==0)
            {
                glGenTextures(1, &ele->texName);
            }
            if(ele->Ischanged&&ticTac.Tac()<0.02)
            {
                pi::timer.enter("glTexImage2D");
                pi::ReadMutex lock1(ele->mutexData);
                glBindTexture(GL_TEXTURE_2D,ele->texName);
                glTexImage2D(GL_TEXTURE_2D, 0,
                             GL_RGBA, ele->img.cols,ele->img.rows, 0,
                             GL_BGRA, GL_UNSIGNED_BYTE,ele->img.data);
                if(svar.GetInt("ShowTex",0))
                    cv::imshow("tex",ele->img);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
                ele->Ischanged=false;
                pi::timer.leave("glTexImage2D");
            }
            glBindTexture(GL_TEXTURE_2D,ele->texName);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(x0,y0,0);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(x0,y1,0);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(x1,y1,0);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(x1,y0,0);
            glEnd();
        }
    glBindTexture(GL_TEXTURE_2D, last_texture_ID);
    glPopMatrix();
}

bool Map2DRender::save(const std::string& filename)
{
    return false;
}
