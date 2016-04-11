#ifndef OBJECT_3DS_H
#define OBJECT_3DS_H

#include "PosedObject.h"

class Lib3dsFile;
class Lib3dsNode;
namespace pi{
namespace gl{


class Object_3DS:public GL_Object
{
public:
    Object_3DS(const char* filename);

    void init();

    virtual void draw();

    bool isOpened(){return file;}

    void setPose(const pi::SE3f& ps){ScopedMutex lock(m_mutex);pose=ps;}
    const pi::SE3f& getPose(){ScopedMutex lock(m_mutex);return pose;}

private:
    void renderNode(Lib3dsNode *node);
    Lib3dsFile *file;
    pi::Mutex  m_mutex;
    pi::SE3f   pose;
};


}
}

#endif // OBJECT_3DS_H
