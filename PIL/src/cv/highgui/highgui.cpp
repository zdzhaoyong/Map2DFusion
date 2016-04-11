#include <opencv2/imgproc/imgproc.hpp>

#include "highgui.h"

namespace pi {

void imshow(Win3D* win3d,cv::Mat img2show,float scale)
{
//    if(img2show.empty()) return;
//    static GL_Image glimage;
//    static cv::Mat  img;
//    if(scale<1.0)
//    {
//        cv::resize(img2show,img,cv::Size(img2show.cols*scale,img2show.rows*scale));
//        scale=1.;
//    }
//    else
//    {
//        img=img2show.clone();
//    }
//    cv::cvtColor(img,img,CV_RGB2BGR);
//    {
//        ScopedMutex lc(glimage.lock);
//        if(!glimage.data) win3d->InsertImage(glimage);
//        glimage.data=img.data;
//        glimage.setImgSize(img.cols,img.rows);
//        glimage.setDisplaySize(img.cols*scale,img.rows*scale);
//        glimage.ischanged=true;
//    }

}


} // end of namespace pi
