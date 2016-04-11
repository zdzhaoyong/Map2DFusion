#include "PosedObject.h"
#include "glHelper.h"

#include <GL/gl.h>


namespace pi{
namespace gl{

void PosedObject::draw()
{
    if(obj.get())
    {
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);

        glMultMatrix(pose);
        obj->draw();

        glPopMatrix();
    }
}

}}
