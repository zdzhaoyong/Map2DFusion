#include <hardware/Camera/CameraOCAM.h>
#include <hardware/Camera/Undistorter.h>
#include <base/Svar/Svar.h>
#include <base/time/Global_Timer.h>

#include <opencv2/highgui/highgui.hpp>
using namespace std;

int main(int argc,char** argv)
{
    svar.ParseMain(argc,argv);
    string cameraIn=svar.GetString("CameraInFile","calib_results.txt");
    string cameraOut=svar.GetString("CameraOutName","CameraOut");
    string ImageIn=svar.GetString("ImageInFile","fish.jpg");

    CameraOCAM ocam;
    if(ocam.fromFile(cameraIn)!=0) return -1;

    Camera* camIn=&ocam;
    cerr<<"CameraIN:"<<camIn->info()<<endl;

    Point3D testPoint=svar.get_var("ProjectPoint",Point3D(0,0,1));
    Point2D proj=ocam.world2camera(testPoint);
    cerr<<"Point:"<<testPoint<<endl;
    cerr<<"Project:"<<proj<<endl;
#if CORRECT_AXIS
    cerr<<"Project:"<<camIn->Project(testPoint.x/testPoint.z,testPoint.y/testPoint.z)<<endl;
    cerr<<"Project:"<<camIn->Project(Point2D(testPoint.x/testPoint.z,testPoint.y/testPoint.z))<<endl;
#else
    cerr<<"Project:"<<camIn->Project(-testPoint.x/testPoint.z,-testPoint.y/testPoint.z)<<endl;
    cerr<<"Project:"<<camIn->Project(Point2D(-testPoint.x/testPoint.z,-testPoint.y/testPoint.z))<<endl;
#endif
    Point2D unPoint=svar.get_var("UnProjectPoint",proj);

    cerr<<"UnProject of ["<<unPoint<<"]:"<<ocam.camera2world(unPoint)<<endl;

    string act=svar.GetString("Act","Default");
    if(act=="TestUndis")
    {
        if(svar.GetInt("TestUndis",1)==1)
        {
            Camera* camOut=GetCameraFromName(cameraOut);
            cerr<<"CameraOut:"<<camOut->info()<<endl;

            Undistorter undis(ocam.getCopy(),camOut);
            cv::Mat img_in=cv::imread(ImageIn),img_show;
            cv::resize(img_in,img_show,cv::Size(img_in.cols/2.,img_in.rows/2.));
            cv::imshow("imageIn",img_show);

            cv::Mat img_out;
            undis.undistort(img_in,img_out);
            cv::imshow("imgOut",img_out);

            cv::waitKey(0);
        }
        else if(svar.GetInt("TestUndis",1)==2)
        {
            cv::Mat img_in=cv::imread(ImageIn),img_show;
            cv::resize(img_in,img_show,cv::Size(img_in.cols/2.,img_in.rows/2.));
            cv::imshow("imageIn",img_show);

            cv::Mat img_out(600,600,CV_8UC3,cv::Scalar(0,0,0));
#if CORRECT_AXIS
            Point3D pt3d(0,0,200);
#else
            Point3D pt3d(0,0,-200);
#endif
            Point2D pt2d;
            for(int x=0;x<img_out.cols;x++)
                for(int y=0;y<img_out.rows;y++)
                {
                    pt3d.x=x-300;
                    pt3d.y=y-300;
                    pt2d=ocam.world2camera(pt3d);
                    if(pt2d.x>=0&&pt2d.x<img_in.cols&&pt2d.y>=0&&pt2d.y<img_in.rows)
                        img_out.at<pi::Byte<3> >(x,y)=img_in.at<pi::Byte<3> >(pt2d.x,pt2d.y);
                }

            cv::imshow("imgOut",img_out);
            cv::waitKey(0);
        }
    }
    else if(act=="TestUndisVideo")
    {
        Camera* camOut=GetCameraFromName(cameraOut);
        cerr<<"CameraOut:"<<camOut->info()<<endl;

        Undistorter undis(ocam.getCopy(),camOut);


        cv::VideoCapture video(svar.GetString("VideoFile",""));
        uchar key=0;
        while(key!=27)
        {
            cv::Mat img_in,img_show;
            video>>img_in;
            if(img_in.empty())
            {
                cerr<<"Video teminaled !\n";
                break;
            }
            cv::resize(img_in,img_show,cv::Size(img_in.cols/2.,img_in.rows/2.));
            cv::imshow("imageIn",img_show);

            cv::Mat img_out;
            pi::timer.enter("Undistortion");
            undis.undistortFast(img_in,img_out);
            pi::timer.leave("Undistortion");
            cv::imshow("imgOut",img_out);

            key=cv::waitKey(20);
        }
    }
    return 0;
}



