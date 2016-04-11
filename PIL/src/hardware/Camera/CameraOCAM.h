#ifndef CAMERAOCAM_H
#define CAMERAOCAM_H
//This is going to remove!
//#include "Camera.h"
#include <iostream>
#include "Undistorter.h"
#define CORRECT_AXIS 1
class CameraOCAM:public Camera
{
public:
    CameraOCAM();
    CameraOCAM(std::string filename){fromFile(filename);}

    virtual bool fromFile(std::string filename);
    virtual std::string CameraType(){return "OCAM";}

    virtual Camera* getCopy()
    {
        CameraOCAM* cp=new CameraOCAM;
        *cp=*this;
        return (Camera*)cp;
    }

    virtual std::string info()
    {
        std::stringstream sst;
        sst<<CameraType()<<" ["<<width<<" "<<height<<" "
#if CORRECT_AXIS
          <<cx<<" "<<cy<<" "<<c<<" "<<d<<" "<<e<<"]\n";
#else
          <<xc<<" "<<yc<<" "<<c<<" "<<d<<" "<<e<<"]\n";
#endif
//        sst<<"pol=";
//        for(int i=0;i<length_pol;i++) sst<<" "<<pol[i];
//        sst<<"\ninv_pol=";
//        for(int i=0;i<length_invpol;i++) sst<<" "<<invpol[i];
        return sst.str();
    }

    virtual inline Point2D Project(const double& X,const double& Y)
    {
//        std::cerr<<"Using virtual...\n";
#if CORRECT_AXIS
        return world2camera(Point3D(X,Y,1.));
#else
        Point2D result=world2camera(Point3D(Y,X,-1));
        return Point2D(result.y,result.x);
#endif
    }

    virtual inline Point2D UnProject(const double& x,const double& y)
    {
        Point3D result=camera2world(Point2D(x,y));
        if(result.z>0.0000001)
        {
            result.z=1./result.z;
            return Point2D(result.x*result.z,result.y*result.z);
        }
        else
            return Point2D(0,0);
    }

    inline Point2D world2camera(const Point3D& point)
    {
        Point2D result;
        if(width==0||height==0)
        {
            std::cerr<<"Invalid camera with width or height equle 0\n";
            return result;
        }
        double norm        = sqrt(point.x*point.x + point.y*point.y);
#if CORRECT_AXIS
        double theta       = atan(-point.z/norm);
#else
        double theta       = atan(point.z/norm);
#endif
        double t, t_i;
        double rho, x, y;
        double invnorm;
        int i;
//        std::cerr<<"Point3D="<<point<<",norm="<<norm<<",theta="<<theta<<std::endl;

        if (norm != 0)
        {
            invnorm = 1/norm;
            t  = theta;
            rho = invpol[0];
            t_i = 1;
//            std::cerr<<"i="<<0<<",invpol="<<invpol[0]<<",rho="<<rho<<std::endl;

            for (i = 1; i < length_invpol; i++)
            {
                t_i *= t;
                rho += t_i*invpol[i];
//                std::cerr<<"i="<<i<<",invpol="<<invpol[i]<<",rho="<<rho<<std::endl;
            }
#if CORRECT_AXIS
            x = point.x*invnorm*rho;
            y = point.y*invnorm*rho;

            result.y = y*c + x*d + cy;
            result.x = y*e + x   + cx;
#else
            x = point.x*invnorm*rho;
            y = point.y*invnorm*rho;

            result.x = x*c + y*d + xc;
            result.y = x*e + y   + yc;
#endif
        }
        else
        {
#if CORRECT_AXIS
            result.x=cx;
            result.y=cy;
#else
            result.x = xc;
            result.y = yc;
#endif
        }
        return result;
    }

    inline Point3D camera2world(const Point2D& point)
    {
        double invdet  = 1/(c-d*e); // 1/det(A), where A = [c,d;e,1] as in the Matlab file
#if CORRECT_AXIS
        double yp = invdet*(    (point.y - cy) - d*(point.x - cx) );
        double xp = invdet*( -e*(point.y - cy) + c*(point.x - cx) );
#else
        double xp = invdet*(    (point.x - xc) - d*(point.y - yc) );
        double yp = invdet*( -e*(point.x - xc) + c*(point.y - yc) );
#endif

        double r   = sqrt(  xp*xp + yp*yp ); //distance [pixels] of  the point from the image center
        double zp  = pol[0];
        double r_i = 1;
        int i;

        for (i = 1; i < length_pol; i++)
        {
          r_i *= r;
          zp  += r_i*pol[i];
        }

        //normalize to unit norm
        double invnorm = 1/sqrt( xp*xp + yp*yp + zp*zp );
#if CORRECT_AXIS
        return Point3D(invnorm*xp,invnorm*yp,-invnorm*zp);
#else
        return Point3D(invnorm*xp,invnorm*yp,invnorm*zp);
#endif
    }

public:
    double pol[64];    // the polynomial coefficients: pol[0] + x"pol[1] + x^2*pol[2] + ... + x^(N-1)*pol[N-1]
    int length_pol;                // length of polynomial
    double invpol[64]; // the coefficients of the inverse polynomial
    int length_invpol;             // length of inverse polynomial
#if CORRECT_AXIS
#else
    double xc;         // row coordinate of the center
    double yc;         // column coordinate of the center
#endif
    double c;          // affine parameter
    double d;          // affine parameter
    double e;          // affine parameter
};

void create_panoramic_undistortion_LUT ( CvMat *mapx, CvMat *mapy, float Rmin, float Rmax, float xc, float yc );
#endif // CAMERAOCAM_H
