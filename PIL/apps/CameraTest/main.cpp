#include <base/Svar/Svar_Inc.h>
#include <base/time/Global_Timer.h>
#include <hardware/Camera/Camera.h>
#include <hardware/Camera/Undistorter.h>
#include <base/types/Random.h>

#include <hardware/Camera/Cameras.h>
#include <hardware/Camera/undistorter.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#ifdef HAS_VIKIT
#include <vikit/atan_camera.h>
#include <vikit/pinhole_camera.h>
#endif

using namespace std;
using namespace pi;

vector<pi::Point2d> vec_p;

#ifdef HAS_VIKIT
bool VIKITCamera(vk::AbstractCamera* camera,string name)
{
    if(!camera) return  false;

    int times=svar.GetInt("Times",10000);

    vector<Eigen::Vector2d> vec_re;
    vector<Eigen::Vector3d> vec_world;
    vec_re.resize(times);
    vec_world.resize(times);
    Eigen::Vector2d testP=*((Eigen::Vector2d*)&vec_p[0]);
    Eigen::Vector3d cam_P=camera->cam2world(testP);
    Eigen::Vector2d rep_P=camera->world2cam(cam_P);
    cout<<"VIKIT:Image:"<<testP<<",World:"<<cam_P<<",ReProject:"<<rep_P<<endl;

    //UnProject
    string time_name=string(name+"::UnProject");
    timer.enter(time_name.c_str());
    for(int i=0;i<times;i++)
        vec_world[i]=camera->cam2world(*((Eigen::Vector2d*)&vec_p[i]));
    timer.leave(time_name.c_str());

    //Project
    time_name=string(name+"::Project");
    timer.enter(time_name.c_str());
    for(int i=0;i<times;i++)
        vec_re[i]=camera->world2cam(vec_world[i]);
    timer.leave(time_name.c_str());

    double sum=0;
    for(int i=0;i<times;i++)
        sum+=(*(Eigen::Vector2d*)&vec_p[i]-vec_re[i]).norm();
    cout<<"Everage Error:"<<sum/times<<endl;
}
#endif

bool CameraTest(Camera* camera)
{
    if(!camera) return false;

    int times=svar.GetInt("Times",10000);
    cout<<"Testing "<<camera->info()<<",Times="<<times<<endl;

    vector<pi::Point2d> vec_cam,vec_re;
    vec_cam.resize(times);
    vec_p.resize(times);
    vec_re.resize(times);
    for(int i=0;i<times;i++)
    {
        double rdx=pi::Random::RandomValue<double>()*camera->Width();
        double rdy=pi::Random::RandomValue<double>()*camera->Height();
        vec_p[i]=(pi::Point2d(rdx,rdy));
    }

    pi::Point2d testP=vec_p[0];
    pi::Point2d cam_P=camera->UnProject(testP);
    pi::Point2d rep_P=camera->Project(cam_P);
    cout<<"Image:"<<testP<<",Plane:"<<cam_P<<",ReProject:"<<rep_P<<endl;

    //UnProject
    string time_name=string(camera->CameraType()+"::UnProject");
    timer.enter(time_name.c_str());
    for(int i=0;i<times;i++)
        vec_cam[i]=camera->UnProject(vec_p[i]);
    timer.leave(time_name.c_str());

    //Project
    time_name=string(camera->CameraType()+"::Project");
    timer.enter(time_name.c_str());
    for(int i=0;i<times;i++)
        vec_re[i]=camera->Project(vec_cam[i]);
    timer.leave(time_name.c_str());

    double sum=0;
    for(int i=0;i<times;i++)
        sum+=(vec_p[i]-vec_re[i]).norm();
    cout<<"Everage Error:"<<sum/times<<endl;

}

bool CameraTest(pi::hardware::Camera camera)
{
    if(!camera.isValid()) return false;

    int times=svar.GetInt("Times",10000);
    cout<<"Testing hardware "<<camera.info()<<",Times="<<times<<endl;

    vector<pi::Point2d> vec_re;
    vector<pi::Point3d> vec_cam;
    vec_cam.resize(times);
    vec_re.resize(times);
//    vec_p.resize(times);
//    for(int i=0;i<times;i++)
//    {
//        double rdx=pi::Random::RandomValue<double>()*camera.width();
//        double rdy=pi::Random::RandomValue<double>()*camera.height();
//        vec_p[i]=(pi::Point2d(rdx,rdy));
//    }

    pi::Point2d testP=vec_p[0];
    pi::Point3d cam_P=camera.UnProject(testP);
    pi::Point2d rep_P=camera.Project(cam_P);
    cout<<"Image:"<<testP<<",Plane:"<<cam_P<<",ReProject:"<<rep_P<<endl;

    //UnProject
    string time_name=string(camera.CameraType()+"::H::UnProject");
    timer.enter(time_name.c_str());
    for(int i=0;i<times;i++)
        vec_cam[i]=camera.UnProject(vec_p[i]);
    timer.leave(time_name.c_str());

    //Project
    time_name=string(camera.CameraType()+"::H::Project");
    timer.enter(time_name.c_str());
    for(int i=0;i<times;i++)
        vec_re[i]=camera.Project(vec_cam[i]);
    timer.leave(time_name.c_str());

    double sum=0;
    for(int i=0;i<times;i++)
        sum+=(vec_p[i]-vec_re[i]).norm();
    cout<<"Everage Error:"<<sum/times<<endl;

}

int main(int argc,char** argv)
{
    svar.ParseMain(argc,argv);

    int times=svar.GetInt("Times",10000);
    cout<<"Times="<<times<<endl;

    timer.enter("DoNothing");
    for(int i=0;i<times;i++)
        ;
    timer.leave("DoNothing");

    Camera pinhole=*GetCameraFromName("GoProIdeaM1080");
    CameraTest(&pinhole);
    CameraTest(pi::hardware::Camera::createFromName("GoProIdeaM1080"));

    CameraANTA anta=*(CameraANTA*)GetCameraFromName("GoProMiddle1080");
    CameraTest(&anta);
    CameraTest(pi::hardware::Camera::createFromName("GoProMiddle1080"));

    CameraOpenCV cv_cam=*(CameraOpenCV*)GetCameraFromName("GoProMiddle1080CV");
    CameraTest(&cv_cam);
    CameraTest(pi::hardware::Camera::createFromName("GoProMiddle1080CV"));

    {
        pi::hardware::Camera camera=pi::hardware::Camera::createFromName("GoProFISH");
        cout<<camera.info()<<endl;
        CameraTest(camera);
    }
#ifdef HAS_VIKIT
    vk::PinholeCamera vk_pinhole(pinhole.Width(),pinhole.Height(),
                                 pinhole.Fx(),pinhole.Fy(),pinhole.Cx(),pinhole.Cy());
    VIKITCamera(&vk_pinhole,pinhole.CameraType()+"::VK");

    vk::ATANCamera vk_anta(anta.Width(),anta.Height(),anta.Fx()/anta.Width(),anta.Fy()/anta.Height(),
                           anta.Cx()/anta.Width(),anta.Cy()/anta.Height(),anta.W());
    VIKITCamera(&vk_anta,anta.CameraType()+"::VK");


    double k1,k2,p1,p2,k3;
    cv_cam.getDistortion(k1,k2,p1,p2,k3);
    vk::PinholeCamera vk_cv(cv_cam.Width(),cv_cam.Height(),cv_cam.Fx(),cv_cam.Fy(),cv_cam.Cx(),cv_cam.Cy(),
                            k1,k2,p1,p2,k3);
    VIKITCamera(&vk_cv,cv_cam.CameraType()+"::VK");
#endif

    cout<<pinhole.info()<<endl;
    pinhole.applyScale();
    cout<<pinhole.info()<<endl;

    Undistorter undis(GetCopy(&anta),GetCopy(&pinhole));
    if(!undis.valid){cout<<"Undis not valid";return -1;}
//    pi::hardware::Undistorter undis(pi::hardware::Camera(svar.GetString("Undis.CameraIn","GoProFISH")),
//                                    pi::hardware::Camera(svar.GetString("Undis.CameraOut","GoProIdeaM1080")));

//    if(!undis.valid()) {cout<<"Undis not valid";return -1;}

//    return 0;
    cv::Mat img=cv::imread(svar.GetString("img_in","img.jpg"));

    if(!img.empty())
    {
        cv::imshow("img",img);
        cv::Mat result,result_fast;

        timer.enter("UndistortionFast");
        undis.undistortFast(img,result_fast);
        timer.leave("UndistortionFast");
        cv::imshow("result_fast",result_fast);

        timer.enter("Undistortion");
        undis.undistort(img,result);
        timer.leave("Undistortion");
        cv::imshow("result",result);

        cv::Mat gray_img,gray_result,grayfast;
        cv::cvtColor(img,gray_img,CV_BGR2GRAY);
        timer.enter("UndistorionFastGray");
        undis.undistortFast(gray_img,grayfast);
        timer.leave("UndistorionFastGray");
        cv::imshow("grayfast",grayfast);

        timer.enter("UndistorionGray");
        undis.undistort(gray_img,gray_result);
        timer.leave("UndistorionGray");
        cv::imshow("gray_result",gray_result);

        cv::waitKey();
    }
    else
    {
        cout<<"No such file "<<svar.GetString("img_in","img.jpg");
    }

    return 0;
}
