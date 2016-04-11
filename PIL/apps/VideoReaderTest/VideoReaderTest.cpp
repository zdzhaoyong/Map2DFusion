#include <fstream>

#include <cv/highgui/VideoReader.h>
#include <opencv2/imgproc/imgproc.hpp>

#include <base/Svar/Svar_Inc.h>
#include <base/types/types.h>
#include <base/system/file_path/file_path.h>
#include <base/time/Global_Timer.h>
#include <base/time/DateTime.h>
#include <hardware/Camera/Cameras.h>
#include <hardware/Camera/undistorter.h>

using namespace cv;
using namespace std;
using namespace pi;

int UndisVideo()
{
    RVideoReader    vr(svar.GetString("UndisVideo.video","in.avi"));
    if(!vr.isOpened()) return -1;

    string          folder2save=svar.GetString("UndisVideo.folder2save",
                                               pi::path_split(svar.GetString("UndisVideo.video","in.avi"))[0]
                                               +"/Result/GSLAM");
    bool save2file=false;
    if(folder2save.size()) save2file=true;

    ofstream        ofs;
    if(save2file)
    {
        svar.ParseLine("system mkdir -p "+folder2save);
        svar.ParseLine("system mkdir -p "+folder2save+"/rgb");
        ofs.open((folder2save+"/frames.txt").c_str());
        if(!ofs.is_open()) return -2;
    }

    pi::hardware::Camera cameraIn=pi::hardware::Camera::createFromName(svar.GetString("CameraIn","NoCamera"));
    pi::hardware::Camera cameraOut=pi::hardware::Camera::createFromName(svar.GetString("CameraOut","NoCamera"));
    if(cameraIn.isValid()&&cameraOut.isValid())
    {
        cout<<"CameraIn:"<<cameraIn.info()<<endl;
        cout<<"CameraOut:"<<cameraOut.info()<<endl;
    }
    else
    {
        cout<<"CameraIn:"<<cameraIn.info()<<endl;
        cout<<"CameraOut:"<<cameraOut.info()<<endl;
        cerr<<"Camera in or out not valid!\n";
        return -3;
    }
    pi::hardware::Undistorter undis(cameraIn,cameraOut);

    RVideoFrameInfo vfi;
    Mat             img, imgS, imgU;
    while( 0 == vr.read(img, &vfi) ) {

        DateTime       t;
        t.fromTimeStampF(vfi.timestamp);
//        fmt::printf("[%6d] ts = %12lld, time = ", vfi.frameIdx, vfi.timestamp);
        cout << t << ", ";
        fmt::printf("pts = %15f\n", vfi.pts);

        undis.undistort(img,imgU);

        if(save2file)
        {
            string filename=folder2save+"/rgb/"+dtos(vfi.timestamp,6)+".jpg";
            imwrite(filename,imgU);
            ofs<<dtos(vfi.timestamp,6)<<" rgb/"+dtos(vfi.timestamp,6)+".jpg"<<endl;
        }

        {
            char buf[256];
            resize(imgU, imgS, cv::Size(640,imgU.rows*640/imgU.cols));
            // draw frame time into image
            sprintf(buf, "Frame time = %4d-%02d-%02d %02d:%02d:%02d.%09d",
                    t.year, t.month, t.day, t.hour, t.min, t.sec,
                    t.nano_sec);
            putText(imgS, buf, Point(10, 20),
                    FONT_HERSHEY_PLAIN, 1.0,
                    Scalar(0xFF, 0, 0), 1);

            imshow("result", imgS);
        }

        int key= waitKey(10);
        if( key == 1113939 ) {
            if( 0 != vr.ff(10*vr.fps) ) break;
            key = 0;
            continue;
        } else if ( key == 1113940 ) {
            //if( 0 != vr.ff(30*vr.fps) ) break;
            vr.read(img, &vfi,vfi.pts+300);
            key = 0;
            continue;
        } else if ( key == 1048603 ) {
            break;
        }
        if(key==27) break;
    }


    return 0;
}

int main(int argc,char** argv)
{
    svar.ParseMain(argc,argv);
    string act=svar.GetString("Act","UndisVideo");
    if(act=="UndisVideo") return UndisVideo();
}
