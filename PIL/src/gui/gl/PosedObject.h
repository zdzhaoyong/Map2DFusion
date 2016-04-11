#ifndef POSEDOBJECT_H
#define POSEDOBJECT_H

#include <base/types/SE3.h>
#include "GL_FatherObject.h"


namespace pi{
namespace gl{

class PosedObject:public GL_Object
{
public:
    PosedObject(const GL_ObjectPtr& object=GL_ObjectPtr()):obj(object){}

    bool setObject(const GL_ObjectPtr& object){ScopedMutex lock(m_mutex);obj=object;}

    bool setPose(const pi::SE3f& ps){ScopedMutex lock(m_mutex);pose=ps;}
    const pi::SE3f& getPose(){ScopedMutex lock(m_mutex);return pose;}

    virtual void draw();

private:
    GL_ObjectPtr    obj;
    pi::SE3f        pose;
    Mutex           m_mutex;
};

}}
#endif // POSEDOBJECT_H
