#ifndef COLORFULPOINT_H
#define COLORFULPOINT_H

#include "GL_Object.h"
#include <base/types/types.h>
#include <GL/gl.h>

namespace pi{
namespace gl{
typedef pi::Point3ub Color3b;

struct ColorfulPoint:public GL_Object
{
    ColorfulPoint(const pi::Point3f& Point=pi::Point3f(0.,0.,0.),
                  const Color3b&     Color=Color3b(255,255,255))
        :point(Point),color(Color){}

    virtual void draw()
    {
        glBegin(GL_POINT);
        glColor3b(color.x,color.y,color.z);
        glVertex3f(point.x,point.y,point.z);
        glEnd();
    }

    pi::Point3f  point;
    Color3b      color;
};

}}
#endif // COLORFULPOINT_H
