#ifndef COLORFULLINE_H
#define COLORFULLINE_H

#include "ColorfulPoint.h"

namespace pi{
namespace gl{

class ColorfulLine: public GL_Object
{
public:
    ColorfulLine(const std::vector<ColorfulPoint>& pts=std::vector<ColorfulPoint>())
        :points(pts){}

    virtual void draw()
    {
        if(points.size()>=2)
        {
            glBegin(GL_LINE);
            for(size_t i=0,iend=points.size();i<iend;i++)
            {
                glColor3b(points[i].color.x,points[i].color.y,points[i].color.z);
                glVertex3f(points[i].point.x,points[i].point.y,points[i].point.z);
            }
            glEnd();
        }
    }

private:
    std::vector<ColorfulPoint> points;
};

}
}
#endif // COLORFULLINE_H
