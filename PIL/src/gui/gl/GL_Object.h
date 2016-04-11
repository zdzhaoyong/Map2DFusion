#ifndef GL_OBJECT_H
#define GL_OBJECT_H

namespace pi{
namespace gl{

struct GL_Object
{
    virtual void draw(){};
    virtual void fastDraw(){draw();}
};

}}

#endif // GL_OBJECT_H
