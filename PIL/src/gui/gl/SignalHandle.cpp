
#include <GL/glew.h>

#include "SignalHandle.h"

namespace pi{
namespace gl{

void Signal_Handle::delete_buffer_slot(GLuint buffer)
{
    glDeleteBuffers(1,&buffer);
}

Signal_Handle& Signal_Handle::instance()
{
    static Signal_Handle* sig=NULL;
    if(sig)
    {
        return *sig;
    }
    else
    {
        sig=new Signal_Handle();
        return *sig;
    }
}

}
}
