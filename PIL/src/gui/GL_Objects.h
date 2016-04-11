#ifndef GL_OBJECTS_H
#define GL_OBJECTS_H

#include "Win3D.h"

namespace pi {
#define DEFAUT_COLOR Color3b(255,255,255)
class CameraSquare:public GL_Object
{
public:
    CameraSquare(double width,double height,
                 double fx,double fy,
                 double cx,double cy,
                 double depth,Color3b color=DEFAUT_COLOR)
    {
        setAlige(Point3f((0-cx)/fx*depth,(0-cy)/fy*depth,depth),
                 Point3f((width-cx)/fx*depth,(0-cy)/fy*depth,depth),
                 Point3f((0-cx)/fx*depth,(width-cy)/fy*depth,depth),
                 Point3f((width-cx)/fx*depth,(height-cy)/fy*depth,depth),
                 color);
    }

    CameraSquare(Point3f left_top   ,Point3f right_top,
                 Point3f left_bottom,Point3f right_bottom,
                 Color3b color=DEFAUT_COLOR)
    {
        setAlige(left_top,right_top,left_bottom,right_bottom,color);
    }

    void setAlige(Point3f left_top   ,Point3f right_top,
                  Point3f left_bottom,Point3f right_bottom,
                  Color3b color=DEFAUT_COLOR)
    {
        InsertLine(ColorfulLine(Point3f(0,0,0),left_top,color));
        InsertLine(ColorfulLine(Point3f(0,0,0),left_bottom,color));
        InsertLine(ColorfulLine(Point3f(0,0,0),right_top,color));
        InsertLine(ColorfulLine(Point3f(0,0,0),right_bottom,color));

        InsertLine(ColorfulLine(left_top,right_top,color));
        InsertLine(ColorfulLine(right_top,right_bottom,color));
        InsertLine(ColorfulLine(right_bottom,left_bottom,color));
        InsertLine(ColorfulLine(left_bottom,left_top,color));
    }

};

class AxisXYZ:public GL_Object
{
public:
    AxisXYZ(const double& length,const SE3<float>& pose=SE3<float>())
    {
        InsertLine(ColorfulLine(Point3f(0,0,0),Point3f(length,0,0),Color3b(255,0,0)));
        InsertLine(ColorfulLine(Point3f(0,0,0),Point3f(0,length,0),Color3b(0,255,0)));
        InsertLine(ColorfulLine(Point3f(0,0,0),Point3f(0,0,length),Color3b(0,0,255)));
    }
};
}
#endif // GL_OBJECTS_H
