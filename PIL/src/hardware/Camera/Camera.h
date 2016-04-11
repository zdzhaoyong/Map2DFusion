#ifndef CAMERA_H
#define CAMERA_H

#include <stdio.h>
#include <math.h>

#include <base/types/types.h>

#if 0//USE_SELF_DEFINED
#else
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define HAS_OPENCV

typedef pi::Point2_<double> Point2D;
typedef pi::Point3_<double> Point3D;

#endif


/// Camera base class, pinhole model
/// (x y)^T=(fx*X/Z+cx,fy*Y/Z+cy)^T
class Camera
{
public:
    Camera();

    Camera(std::string file_name);

    Camera(int Width,int Height,double Fx,double Fy,double Cx,double Cy);

    virtual std::string CameraType(){return "PinHole";}

    virtual std::string info();

    int setCamera(int Width,int Height,double Fx,double Fy,double Cx,double Cy);

    int setCamera(){return setCamera(width,height,fx,fy,cx,cy);}

    virtual bool fromFile(std::string file_name);

    virtual void applyScale(double scale=0.5);

    virtual bool isValid(){return (fx!=0&&fy!=0&&width>0&&height>0);}

    virtual Camera* getCopy();

    /// project from the z=1 plane
    virtual inline Point2D Project(const double& x,const double& y)
    {
        return Point2D(fx*x+cx,fy*y+cy);
    }

    inline Point2D Project(const Point2D& p)
    {
        return Project(p[0],p[1]);
    }

    /// project from the camera coordinate
    inline Point2D Project(const double& x,const double& y,const double& z)
    {
        return Project(x/z,y/z);
    }

    inline Point2D Project(const Point3D& p)
    {
        return Project(p[0]/p[2],p[1]/p[2]);
    }

    /// unproject pixels to z=1 plane
    virtual inline Point2D UnProject(const double& x,const double& y)
    {
        return Point2D((x-cx)*fx_inv,(y-cy)*fy_inv);
    }

    inline Point2D UnProject(const Point2D& p)
    {
        return UnProject(p[0],p[1]);
    }

    inline Point3D UnProject3D(const double& x,const double& y)
    {
        Point3D result(0,0,1);
        *(Point2D*)&result=UnProject(x,y);
        return result;
    }

public:
    double& Fx(){return fx;}
    double& Fy(){return fy;}
    double& Cx(){return cx;}
    double& Cy(){return cy;}
    double& Fx_inv() {return fx_inv;}
    double& Fy_inv() {return fy_inv;}
    int&    Width()  {return width; }
    int&    Height() {return height;}

protected:
    double fx,fy,cx,cy;//unit pixel
    double fx_inv,fy_inv;
    int    width,height;
};

/// Camera described in PTAM(alse called ANTA camera), measurement: (x,y)^T, z=1 plane: (X,Y)^T
/// Project:
/// |x|=|cx| + |fx 0| |X| *r'/r
/// |y| |cy|   |0 fy| |Y|
/// r =sqrt(X*X+Y*Y)
/// r'=atan(r * tan2w) / w
/// tan2w=2.0 * tan(w / 2.0)
///
/// UnProject:
/// |X|=(|x|-|cx|) * |1/fx 0|*r/r'
/// |Y|  |y| |cy|    |0 1/fy|
/// r' =sqrt(X*X+Y*Y)
/// r  =(tan(r' * w) / tan2w)
class CameraANTA : public Camera
{
public:
    CameraANTA();

    CameraANTA(std::string file_name);

    CameraANTA(int Width,int Height,double Fx,double Fy,double Cx,double Cy,double W);

    virtual std::string CameraType(){return "ANTA";}

    virtual std::string info();

    int setCameraANTA(int Width,int Height,double Fx,double Fy,double Cx,double Cy,double W=0);

    virtual bool fromFile(std::string file_name);

    void refreshParaments();

    virtual inline Point2D Project(const double& X,const double& Y)
    {
        if(!useDistortion) return Point2D(fx*X+cx,fy*Y+cy);
        //return Camera::Project(X,Y);

        double r= sqrt(X*X+Y*Y);
        if(r < 0.001 || w == 0.0)
            r= 1.0;
        else
            r=(w_inv* atan(r * tan2w) / r);

        return Point2D(cx + fx * r * X,cy + fy * r * Y);
    }

    virtual inline Point2D UnProject(const double &x, const double &y)
    {
        if(useDistortion)
        {
            Point2D result((x-cx)*fx_inv,(y-cy)*fy_inv);
            double r = sqrt(result.x*result.x+result.y*result.y);

            if(w != 0.0&& r >0.01)
            {
                r = ( tan(r * w)*tan2w_inv )/r;//;
                result.x=result.x*r;
                result.y=result.y*r;
            }
            return result;
        }
        else return Point2D((x-cx)*fx_inv,(y-cy)*fy_inv);

    }

    bool& UseDistortion(){return useDistortion;}

public:
    double& W(){return w;}

protected:
    double w;
    double tan2w,w_inv,tan2w_inv;
    bool   useDistortion;
};
typedef CameraANTA CameraPTAM;

/// Camera model used by OpenCV
/// Project:
/// r^2= X^2+Y^2;
/// X1= X*(1+k1*r^2 + k2*r^4+k3*r^6) + 2*p1*XY + p2*(r^2 + 2*X^2);
/// Y1= Y*(1+k1*r^2 + k2*r^4+k3*r^6) + 2*p2*XY + p1*(r^2 + 2*Y^2);
/// (x y)^T=(fx*X1+cx,fy*Y1+cy)^T
///
/// UnProject:
///
class CameraOpenCV : public Camera
{
public:
    CameraOpenCV();

    CameraOpenCV(std::string file_name);

    CameraOpenCV(int Width,int Height,
                 double Fx,double Fy,double Cx,double Cy,
                 double K1,double K2,double P1,double P2,double K3);

    virtual std::string CameraType(){return "OpenCV";}

    virtual std::string info();

    void setCameraOpenCV(int Width,int Height,
                         double Fx,double Fy,double Cx,double Cy,
                         double K1,double K2,double P1,double P2,double K3);

    void setDistortion(double K1,double K2,double P1,double P2,double K3)
    {
        p1=P1;p2=P2;k1=K1;k2=K2;k3=K3;
    }

    void getDistortion(double& K1,double& K2,double& P1,double& P2,double& K3)
    {
        P1=p1;P2=p2;K1=k1;K2=k2;K3=k3;
    }

    virtual bool fromFile(std::string file_name);

    virtual inline Point2D Project(const double &X, const double &Y)
    {
        double r2,r4,r6,X1,Y1,X2,Y2,xy2;
        X2=X*X;
        Y2=Y*Y;
        r2= X2+Y2;
        r4=r2*r2;
        r6=r2*r4;
        xy2=X*Y*2.0;
        X1= X*(1+k1*r2 + k2*r4+k3*r6) + xy2*p1 + p2*(r2 + 2.0*X2);
        Y1= Y*(1+k1*r2 + k2*r4+k3*r6) + xy2*p2 + p1*(r2 + 2.0*Y2);
        return Camera::Project(X1,Y1);
    }

    virtual inline Point2D UnProject(const double &x, const double &y);

protected:
    double k1,k2,p1,p2,k3;
#ifdef HAS_OPENCV
    cv::Mat cam_k,cam_d;
#endif
};


Camera* GetCameraFromFile(std::string camera_file);
Camera* GetCameraFromName(std::string camera_name);
Camera* GetCopy(Camera*);

#endif // CAMERA_H
