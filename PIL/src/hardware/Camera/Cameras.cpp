#include "Cameras.h"
#include "CameraImpl.h"

#include <base/system/file_path/file_path.h>
#include <base/Svar/Svar.h>
#include <base/Svar/VecParament.h>

using namespace std;

namespace pi {
namespace hardware {


Camera::Camera(const std::string& name):impl(new CameraImpl())
{
    if(name.size())
    {
        *this=createFromName(name);
    }
}

Camera::Camera(SPtr<CameraImpl>& Impl):impl(Impl)
{}

std::string Camera::CameraType()
{
    return impl->CameraType();
}

std::string Camera::info()
{
    return impl->info();
}

int Camera::applyScale(double scale)
{
    return impl->applyScale(scale);
}

bool Camera::isValid()
{
    return impl.get()&&impl->isValid();
}

Point2d Camera::Project(const Point3d& p3d)
{
    return impl->Project(p3d);
}

Point3d Camera::UnProject(const Point2d& p2d)
{
    return impl->UnProject(p2d);
}

int Camera::width()
{
    return impl->w;
}

int Camera::height()
{
    return impl->h;
}

Camera Camera::createFromName(const std::string& name)
{
    SPtr<CameraImpl> impl_result=SPtr<CameraImpl>(new CameraImpl());;
    if(pi::path_exist(name.c_str()))//try to read as file
    {
        Svar config;
        if(config.ParseFile(name)==0)
        {
            config.clear();
            return Camera(impl_result);
        }

        string cameraType=config.GetString("CameraType","NoCamera");
        if(cameraType=="NoCamera") {
            config.clear();return Camera(impl_result);
        }
        else if(cameraType=="PinHole"||cameraType=="Pinhole")
        {
            CameraPinhole* camera=new CameraPinhole();
            camera->fx=config.GetDouble("fx",camera->fx);
            camera->fy=config.GetDouble("fy",camera->fy);
            camera->cx=config.GetDouble("cx",camera->cx);
            camera->cy=config.GetDouble("cy",camera->cy);
            impl_result=SPtr<CameraImpl>(camera);
        }
        else if(cameraType=="PTAM"||cameraType=="ANTA")
        {
            CameraANTA* camera=new CameraANTA();
            camera->fx=config.GetDouble("fx",camera->fx);
            camera->fy=config.GetDouble("fy",camera->fy);
            camera->cx=config.GetDouble("cx",camera->cx);
            camera->cy=config.GetDouble("cy",camera->cy);
            camera->d=config.GetDouble("w",camera->d);
            impl_result=SPtr<CameraImpl>(camera);
        }
        else if(cameraType=="OpenCV")
        {
            CameraOpenCV* camera=new CameraOpenCV();
            camera->fx=config.GetDouble("fx",camera->fx);
            camera->fy=config.GetDouble("fy",camera->fy);
            camera->cx=config.GetDouble("cx",camera->cx);
            camera->cy=config.GetDouble("cy",camera->cy);
            camera->k1=config.GetDouble("k1",camera->k1);
            camera->k2=config.GetDouble("k2",camera->k1);
            camera->p1=config.GetDouble("p1",camera->p1);
            camera->p2=config.GetDouble("p2",camera->p2);
            camera->k3=config.GetDouble("k3",camera->k3);
            impl_result=SPtr<CameraImpl>(camera);
        }
        else if(cameraType=="OCAM")
        {
            impl_result=SPtr<CameraOCAM>(new CameraOCAM(name));
        }
        else if(cameraType=="Ideal"){
            impl_result=SPtr<CameraIdeal>(new CameraIdeal());
        }
        impl_result->w=config.GetDouble("width",impl_result->w);
        impl_result->h=config.GetDouble("height",impl_result->h);
        config.clear();
        if(impl_result->isValid())
        {
            impl_result->refreshParaments();
            return Camera(impl_result);
        }
        else
        {
            return Camera();
        }
    }
    else//try to read from name
    {
        if(!svar.exist(name+".CameraType"))
        {
            cout<<"Can't find paraments for Camera "<<name<<endl;
            return Camera();
        }

        string cameraType=svar.GetString(name+".CameraType","NoCamera");
        VecParament para;
        if(svar.exist(name+".Paraments"))
        {
            para=svar.get_var(name+".Paraments",para);
            if((cameraType=="ANTA"||cameraType=="ATAN"||cameraType=="PTAM")&&para.size()==7)
            {
                impl_result=SPtr<CameraImpl>(new CameraANTA(para[0],para[1],
                        para[2],para[3],para[4],para[5],para[6]));
            }
            else if(cameraType=="OpenCV"&&para.size()==11)
            {
                impl_result=SPtr<CameraImpl>(new CameraOpenCV(para[0],para[1],
                        para[2],para[3],para[4],para[5],
                        para[6],para[7],para[8],para[9],para[10]));
            }
            if((cameraType=="PinHole"||cameraType=="Pinhole")&&para.size()==6)
            {
                impl_result=SPtr<CameraImpl>(new CameraPinhole(para[0],para[1],
                        para[2],para[3],para[4],para[5]));
            }
            else if(cameraType=="OCAM")
            {
                impl_result=SPtr<CameraOCAM>(new CameraOCAM(svar.GetString(name+".File","")));
            }
            else if(cameraType=="Ideal")
            {
                impl_result=SPtr<CameraIdeal>(new CameraIdeal());
            }
        }
        else
        {
            int width=svar.GetInt(name+".width",0);
            int height=svar.GetInt(name+".height",0);
            double fx=svar.GetDouble(name+".fx",0);
            double fy=svar.GetDouble(name+".fy",0);
            double cx=svar.GetDouble(name+".cx",0);
            double cy=svar.GetDouble(name+".cy",0);
            double w=svar.GetDouble(name+".w",0);
            double k1=svar.GetDouble(name+".k1",0);
            double k2=svar.GetDouble(name+".k2",0);
            double p1=svar.GetDouble(name+".p1",0);
            double p2=svar.GetDouble(name+".p2",0);
            double k3=svar.GetDouble(name+".k3",0);

            if((cameraType=="ANTA"||cameraType=="PTAM"))
            {
                impl_result=SPtr<CameraImpl>(new CameraANTA(width,height,fx,fy,cx,cy,w));
            }
            else if(cameraType=="OpenCV")
            {
                impl_result=SPtr<CameraImpl>(new CameraOpenCV(width,height,fx,fy,cx,cy,
                                                              k1,k2,p1,p2,k3));
            }
            if((cameraType=="PinHole"||cameraType=="Pinhole"))
            {
                impl_result=SPtr<CameraImpl>(new CameraPinhole(width,height,fx,fy,cx,cy));
            }
            else if(cameraType=="OCAM")
            {
                impl_result=SPtr<CameraOCAM>(new CameraOCAM(svar.GetString(name+".File","")));
            }
        }

        if(impl_result->isValid())
        {
            impl_result->refreshParaments();
            return Camera(impl_result);
        }
        else
        {
            return Camera();
        }
    }
    return Camera();
}

}}
