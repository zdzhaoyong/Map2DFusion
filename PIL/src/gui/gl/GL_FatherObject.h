#ifndef GL_FATHEROBJECT_H
#define GL_FATHEROBJECT_H

#include "GL_Object.h"
#include <base/types/SPtr.h>
#include <base/system/thread/ThreadBase.h>
#include <vector>

namespace pi{
namespace gl{

typedef SPtr<GL_Object>           GL_ObjectPtr;
typedef std::vector<GL_Object>    ObjectVec;
typedef std::vector<GL_ObjectPtr> ObjectPtrVec;

struct Father_Object:public GL_Object
{
    virtual void draw()
    {
        pi::ScopedMutex lock(m_mutex);
        for(ObjectVec::iterator it=children.begin();it!=children.end();it++)
            it->draw();
        for(ObjectPtrVec::iterator it=ptr_children.begin();it!=ptr_children.end();it++)
            (*it)->draw();
    }

    bool insert(const GL_ObjectPtr& obj){pi::ScopedMutex lock(m_mutex);ptr_children.push_back(obj);}
    bool insert(const GL_Object& obj){pi::ScopedMutex lock(m_mutex);children.push_back(obj);}

    void clear(){pi::ScopedMutex lock(m_mutex);children.clear();ptr_children.clear();}

private:
    ObjectVec       children;
    ObjectPtrVec    ptr_children;
    Mutex           m_mutex;
};

}}
#endif // GL_FATHEROBJECT_H
