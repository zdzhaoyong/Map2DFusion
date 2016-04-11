#include "Camera.h"

#include <base/debug/debug_config.h>
#include <base/Svar/Svar.h>
#include <base/Svar/VecParament.h>
#include "CameraOCAM.h"

using namespace std;

Camera::Camera():
    fx(0),fy(0),cx(0),cy(0),fx_inv(0),fy_inv(0),height(0),width(0)
{

}

Camera::Camera(std::string file_name)
{
    fromFile(file_name);
}

Camera::Camera(int Width,int Height,double Fx,double Fy,double Cx,double Cy)
{
    setCamera(Width,Height,Fx,Fy,Cx,Cy);
}

int Camera::setCamera(int Width,int Height,double Fx,double Fy,double Cx,double Cy)
{
    width=Width;
    height=Height;
    fx=Fx;
    fy=Fy;
    cx=Cx;
    cy=Cy;
    if(isValid())
    {
        fx_inv=1.0/fx;
        fy_inv=1.0/fy;
        return 0;
    }
    else
    {
//        MSG_ERROR("Wrong focal length detected. Please check the camera parament.");
        return -1;
    }
}

bool Camera::fromFile(std::string file_name)
{
    Svar cameraFile;
    cameraFile.ParseFile(file_name);
    if(cameraFile.exist("Paraments"))
    {
        VecParament para;
        cameraFile.get_var("Paraments",para);
        if(para.size()!=6)
        {
            MSG_ERROR("Paraments number should be exactly 6.");
            return false;
        }
        width=para[0];height=para[1];
        fx=para[2];fy=para[3];
        cx=para[4];cy=para[5];
    }
    else if(cameraFile.exist("fx"))
    {
        fx=cameraFile.GetDouble("fx",fx);
        fy=cameraFile.GetDouble("fy",fy);
        cx=cameraFile.GetDouble("cx",cx);
        cy=cameraFile.GetDouble("cy",cy);
        width=cameraFile.GetInt("width",width);
        height=cameraFile.GetInt("height",height);
    }
    cameraFile.clear();
    setCamera(width,height,fx,fy,cx,cy);
    return true;
}

void Camera::applyScale(double scale)
{
    width*=scale;
    height*=scale;
    fx*=scale;
    fy*=scale;
    cx*=scale;
    cy*=scale;

    setCamera(width,height,fx,fy,cx,cy);
}

Camera* Camera::getCopy()
{
    Camera* p=new Camera;
    *p=*this;
    return p;
}

std::string Camera::info()
{
    stringstream sst;
    sst << CameraType() << " [" << Width() << " " << Height() << " "
        << Fx() << " " << Fy() << " "
        << Cx() << " " << Cy() << "]";
    return sst.str();
}

CameraANTA::CameraANTA():w(0),useDistortion(false)
{

}

CameraANTA::CameraANTA(std::string file_name)
{
    fromFile(file_name);
}

std::string CameraANTA::info()
{
    stringstream sst;
    sst << CameraType()
        << " [" << Width() << " " << Height() << " "
        << Fx() << " " << Fy() << " " << Cx() << " " << Cy() << " "
        << W() << "]";

    return sst.str();
}

CameraANTA::CameraANTA(int Width,int Height,double Fx,double Fy,double Cx,double Cy,double W)
{
    setCameraANTA(Width,Height,Fx,Fy,Cx,Cy,W);
}

int CameraANTA::setCameraANTA(int Width,int Height,double Fx,double Fy,double Cx,double Cy,double W)
{
    setCamera(Width,Height,Fx,Fy,Cx,Cy);
    w=W;
    refreshParaments();
}

void CameraANTA::refreshParaments()
{
    setCamera(width,height,fx,fy,cx,cy);
    if(w!= 0.0)
    {
        tan2w = 2.0 * tan(w / 2.0);
        tan2w_inv=1.0/tan2w;
        w_inv = 1.0 / w;
        useDistortion = 1.0;
    }
    else
    {
        w_inv = 0.0;
        tan2w = 0.0;
        useDistortion = false;
    }
}

bool CameraANTA::fromFile(std::string file_name)
{
    Svar cameraFile;
    cameraFile.ParseFile(file_name);

    string Type=cameraFile.GetString("CameraType","PTAM");
    if(Type!="PTAM"&&Type!="ANTA")
    {
        printf("Not the right CameraType!");
        return false;
    }
    if(cameraFile.exist("Paraments"))
    {
        VecParament para;
        cameraFile.get_var("Paraments",para);
        if(para.size()!=7)
        {
            MSG_ERROR("Paraments number should be exactly 6.");
            return false;
        }
        width=para[0];height=para[1];
        fx=para[2];fy=para[3];
        cx=para[4];cy=para[5];
        w=para[6];
        if(cx<1)
        {
            fx*=width;cx*=width;
            fy*=height;cy*=height;
        }
    }
    else if(cameraFile.exist("fx"))
    {
        width=cameraFile.GetInt("width",width);
        height=cameraFile.GetInt("height",height);
        fx=cameraFile.GetDouble("fx",fx);
        fy=cameraFile.GetDouble("fy",fy);
        cx=cameraFile.GetDouble("cx",cx);
        cy=cameraFile.GetDouble("cy",cy);
        w=cameraFile.GetDouble("w",0);
    }

    cameraFile.clear();
    refreshParaments();
}


CameraOpenCV::CameraOpenCV()
{

}

CameraOpenCV::CameraOpenCV(std::string file_name)
{
    fromFile(file_name);
}

CameraOpenCV::CameraOpenCV(int Width,int Height,
                           double Fx,double Fy,double Cx,double Cy,
                           double K1,double K2,double P1,double P2,double K3)
{
    setCameraOpenCV(Width,Height,Fx,Fy,Cx,Cy,K1,K2,P1,P2,K3);
}

std::string CameraOpenCV::info()
{
    stringstream sst;

    sst << CameraType() <<" [" << Width() << " " << Height() << " "
        << Fx() << " " << Fy() << " "
        << Cx() << " " << Cy() << " "
        << k1 << " " << k2 << " "
        << p1 << " " << p2 << " " << k3 << "]";
    return sst.str();
}

void CameraOpenCV::setCameraOpenCV(int Width,int Height,
                     double Fx,double Fy,double Cx,double Cy,
                     double K1,double K2,double P1,double P2,double K3)
{
    setCamera(Width,Height,Fx,Fy,Cx,Cy);
    setDistortion(K1,K2,P1,P2,K3);
}

bool CameraOpenCV::fromFile(std::string file_name)
{
    Svar cameraFile;
    cameraFile.ParseFile(file_name);
    if(cameraFile.GetString("CameraType","OpenCV")!="OpenCV")
    {
        printf("Not the right CameraType!");
        return false;
    }
    if(cameraFile.exist("Paraments"))
    {
        VecParament para;
        cameraFile.get_var("Paraments",para);
        if(para.size()!=11)
        {
            MSG_ERROR("Paraments number should be exactly 6.");
            return false;
        }
        width=para[0];height=para[1];
        fx=para[2];fy=para[3];
        cx=para[4];cy=para[5];
        k1=para[6];k2=para[7];p1=para[8];p2=para[9];k3=para[10];
    }
    else if(cameraFile.exist("fx"))
    {
        width=cameraFile.GetInt("width",width);
        height=cameraFile.GetInt("height",height);
        fx=cameraFile.GetDouble("fx",0);
        fy=cameraFile.GetDouble("fy",0);
        cx=cameraFile.GetDouble("cx",0);
        cy=cameraFile.GetDouble("cy",0);
        k1=cameraFile.GetDouble("k1",0);
        k2=cameraFile.GetDouble("k2",0);
        p1=cameraFile.GetDouble("p1",0);
        p2=cameraFile.GetDouble("p2",0);
        k3=cameraFile.GetDouble("k3",0);
        setCamera();
    }
    else
    {
        return false;
    }
    cameraFile.clear();
}

Point2D CameraOpenCV::UnProject(const double &x, const double &y)
{
#ifdef HAS_OPENCV
    if(!cam_k.data)
    {
        cam_k=(cv::Mat_<float>(3, 3)
               << fx, 0.0, cx,
                  0.0, fy, cy,
                  0.0,0.0, 1.0);
        cam_d=(cv::Mat_<float>(1, 5) << k1, k2, p1, p2, k3);
    }
    cv::Point2d uv(x,y),px;
    const cv::Mat src_pt(1, 1, CV_64FC2, &uv);
    cv::Mat dst_pt(1, 1, CV_64FC2, &px);
    cv::undistortPoints(src_pt, dst_pt, cam_k, cam_d);
    return *(Point2D*)&px;
#else
    Point2D result=Camera::UnProject(x,y);

    double r2,r4,r6;
    r2=result.x*result.x+result.y*result.y;
    r4=r2*r2;
    r6=r4*r2;
    double& X=result.x;
    double& Y=result.y;

    double a,b0,b,c;
    b0=1+k1*r2+k2*r4+k3*r6;
    a=2.0*p2;
    b=b0+2.0*p1*Y;
    c=p2*r2-X;
    if(a*a>0.0000001) result.x= (sqrt(b*b-4.0*a*c)-b)/(a*2.0);

    a=2.0*p1;
    b=b0+2.0*p2*X;
    c=p1*r2-Y;
    if(a*a>0.0000001) result.y= (sqrt(b*b-4.0*a*c)-b)/(a*2.0);

    return result;
#endif
}

Camera* GetCameraFromFile(string camera_file)
{
    Svar config;
    config.ParseFile(camera_file);

    string cameraType=config.GetString("CameraType","PinHole");
    config.clear();
    cout<<"new "<<cameraType<<" camera.\n";
    if(cameraType=="PTAM"||cameraType=="ANTA")
    {
        return new CameraANTA(camera_file);
    }
    else if(cameraType=="OpenCV")
    {
        return new CameraOpenCV(camera_file);
    }
    else return new Camera(camera_file);
}

Camera* GetCameraFromName(std::string name)
{
    if(!svar.exist(name+".CameraType"))
    {
        cout<<"Can't find paraments for Camera "<<name<<endl;
        return NULL;
    }

    string cameraType=svar.GetString(name+".CameraType","PinHole");
    bool   hasParaments=0;
    VecParament para;
    if(svar.exist(name+".Paraments"))
    {
        hasParaments=true;
        para=svar.get_var(name+".Paraments",para);
    }
    int width=svar.GetInt(name+".width",0);
    int height=svar.GetInt(name+".height",0);
    double fx=svar.GetDouble(name+".fx",0);
    double fy=svar.GetDouble(name+".fy",0);
    double cx=svar.GetDouble(name+".cx",0);
    double cy=svar.GetDouble(name+".cy",0);

    if(cameraType=="PTAM"||cameraType=="ANTA")
    {
        double w=svar.GetDouble(name+".w",0);
        if(hasParaments&&para.size()==7)
        {
            double scalex=1,scaley=1;
            width=para[0];height=para[1];w=para[6];
            if(para[4]>0&&para[4]<1)
            {
                scalex=width;
                scaley=height;
            }
            fx=para[2]*scalex;
            fy=para[3]*scaley;
            cx=para[4]*scalex;
            cy=para[5]*scaley;
        }
        return new CameraANTA(width,height,fx,fy,cx,cy,w);
    }
    else if(cameraType=="OpenCV")
    {
        if(hasParaments&&para.size()==11)
            return new CameraOpenCV(para[0],para[1],para[2],para[3],para[4],para[5],
                    para[6],para[7],para[8],para[9],para[10]);
        else
            return new CameraOpenCV(width,height,fx,fy,cx,cy,
                              svar.GetDouble(name+".k1",0),
                              svar.GetDouble(name+".k2",0),
                              svar.GetDouble(name+".p1",0),
                              svar.GetDouble(name+".p2",0),
                              svar.GetDouble(name+".k3",0));
    }
    else if(cameraType=="OCAM")
    {
        return new CameraOCAM(svar.GetString(name+".File","calib_results.txt"));
    }
    else
    {
        if(hasParaments&&para.size()==6)
            return new Camera(para[0],para[1],para[2],para[3],para[4],para[5]);
        else
            return new Camera(width,height,fx,fy,cx,cy);
    }
}

Camera* GetCopy(Camera* camera)
{
    if( camera == NULL )
        return NULL;

    string cameraType=camera->CameraType();
    if(cameraType=="PTAM"||cameraType=="ANTA")
    {
        CameraANTA* p=new CameraANTA;
        *p=*(CameraANTA*)camera;
        return p;
    }
    else if(cameraType=="OpenCV")
    {
        CameraOpenCV* p=new CameraOpenCV;
        *p=*(CameraOpenCV*)camera;
        return p;
    }
    else if(cameraType=="OCAM")
    {
        CameraOCAM* p=new CameraOCAM;
        *p=*(CameraOCAM*)camera;
        return p;
    }
    else
    {
        Camera* p=new Camera;
        *p=*camera;
        return p;
    }
}
