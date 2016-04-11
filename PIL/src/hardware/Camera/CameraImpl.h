#ifndef CAMERAIMPL_H
#define CAMERAIMPL_H

#include <iostream>
#include <base/types/types.h>
#if 0//USE_SELF_DEFINED
#else
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define HAS_OPENCV
#endif

namespace pi
{
namespace hardware {

class CameraImpl//No camera
{
public:
    CameraImpl(int _w=-1,int _h=-1):w(_w),h(_h){}
    ~CameraImpl(){}

    virtual std::string CameraType(){return "NoCamera";}

    virtual std::string info(){return "No camera established.\n";}

    virtual int applyScale(double scale=0.5){}

    virtual bool isValid(){return false;}
    virtual int refreshParaments(){return -1;}

    virtual Point2d Project(const Point3d& p3d){return Point2d(-1,-1);}

    virtual Point3d UnProject(const Point2d& p2d){return Point3d(0,0,0);}

    int32_t w,h;
    int32_t state,tmp;
};

/// Identity pinhole camera,fx=fy=1&&cx=cy=0
/// (x y)^T=(fx*X/Z+cx,fy*Y/Z+cy)^T
class CameraIdeal:public CameraImpl
{
public:
    virtual std::string CameraType(){return "Ideal";}

    virtual std::string info(){return "CameraType:Ideal\n";}

    virtual int applyScale(double scale=0.5){return -1;}

    virtual bool isValid(){return true;}

    virtual Point2d Project(const Point3d& p3d){double z_inv=1./p3d.z;return Point2d(p3d.x*z_inv,p3d.y*z_inv);}

    virtual Point3d UnProject(const Point2d& p2d){return Point3d(p2d.x,p2d.y,1.);}
};

/// Pinhole model
/// (x y)^T=(fx*X/Z+cx,fy*Y/Z+cy)^T
class CameraPinhole:public CameraImpl
{
public:
    CameraPinhole():fx(0),fy(0),fx_inv(0),fy_inv(0),cx(0),cy(0){}
    CameraPinhole(int _w,int _h,double _fx,double _fy,double _cx,double _cy)
        :CameraImpl(_w,_h),fx(_fx),fy(_fy),fx_inv(0),fy_inv(0),cx(_cx),cy(_cy)
    {refreshParaments();}
    virtual std::string CameraType(){return "PinHole";}

    virtual std::string info();

    virtual int applyScale(double scale=0.5);

    virtual bool isValid();

    virtual int refreshParaments();

    virtual Point2d Project(const Point3d& p3d);

    virtual Point3d UnProject(const Point2d& p2d);

    double fx,fy,cx,cy,fx_inv,fy_inv;
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
class CameraATAN:public CameraImpl
{
public:
    CameraATAN();

    CameraATAN(int _w,int _h,double _fx,double _fy,double _cx,double _cy,double _d);

    virtual std::string CameraType(){return "ATAN";}

    virtual std::string info();

    virtual bool isValid(){return w>0&&h>0&&fx!=0&&fy!=0;}

    virtual int refreshParaments();

    virtual int applyScale(double scale=0.5);

    virtual Point2d Project(const Point3d& p3d);
    virtual Point3d UnProject(const Point2d& p2d);

    bool& UseDistortion(){return useDistortion;}

    double fx,fy,cx,cy,d,fx_inv,fy_inv;
    double tan2w,d_inv,tan2w_inv;
    bool   useDistortion;
};
typedef CameraATAN CameraPTAM;
typedef CameraATAN CameraANTA;

/// Camera model used by OpenCV
/// Project:
/// r^2= X^2+Y^2;
/// X1= X*(1+k1*r^2 + k2*r^4+k3*r^6) + 2*p1*XY + p2*(r^2 + 2*X^2);
/// Y1= Y*(1+k1*r^2 + k2*r^4+k3*r^6) + 2*p2*XY + p1*(r^2 + 2*Y^2);
/// (x y)^T=(fx*X1+cx,fy*Y1+cy)^T
///
/// UnProject:
///
class CameraOpenCV:public CameraImpl
{
public:
    CameraOpenCV():fx(0),fy(0),fx_inv(0),fy_inv(0),cx(0),cy(0){}
    CameraOpenCV(int Width,int Height,
                 double Fx,double Fy,double Cx,double Cy,
                 double K1,double K2,double P1,double P2,double K3)
        :CameraImpl(Width,Height),fx(Fx),fy(Fy),cx(Cx),cy(Cy),
    k1(K1),k2(K2),p1(P1),p2(P2),k3(K3){refreshParaments();}

    virtual std::string CameraType(){return "OpenCV";}

    virtual std::string info();

    virtual int applyScale(double scale=0.5);

    virtual bool isValid(){return w>0&&h>0&&fx!=0&&fy!=0;}

    virtual int refreshParaments();

    virtual Point2d Project(const Point3d& p3d);

    virtual Point3d UnProject(const Point2d& p2d);

    double fx,fy,cx,cy,fx_inv,fy_inv,
    k1,k2,p1,p2,k3;
#ifdef HAS_OPENCV
    cv::Mat cam_k,cam_d;
#endif
};


class CameraOCAM:public CameraImpl
{
public:
    CameraOCAM(const std::string& file);
    virtual std::string CameraType(){return "OCAM";}

    virtual std::string info();

    virtual int applyScale(double scale=0.5){return -1;}

    virtual bool isValid(){return w>0&&h>0&&length_pol&&length_invpol;}

    virtual int refreshParaments(){if(isValid()){
            return 0;}else return -1;}

    virtual Point2d Project(const Point3d& point);

    virtual Point3d UnProject(const Point2d& point);

public:
    double pol[64];    // the polynomial coefficients: pol[0] + x"pol[1] + x^2*pol[2] + ... + x^(N-1)*pol[N-1]
    int length_pol;                // length of polynomial
    double invpol[64]; // the coefficients of the inverse polynomial
    int length_invpol;             // length of inverse polynomial

    double cx;         // row coordinate of the center
    double cy;         // column coordinate of the center
    double c;          // affine parameter
    double d;          // affine parameter
    double e;          // affine parameter
};

}}
#endif // CAMERAIMPL_H
