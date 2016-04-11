#ifndef SIGNALHANDLE_H
#define SIGNALHANDLE_H
#include <QObject>
#include <GL/gl.h>

namespace pi{
namespace gl {

class EventHandle
{
public:
    virtual bool KeyPressHandle(void *){return false;}
    virtual bool MousePressHandle(void *){return false;}

};

class Draw_Opengl
{
public:
    virtual void Draw_Something(){}
};

class Signal_Handle:public QObject
{
    Q_OBJECT

public:
    Signal_Handle(QWidget *parent=NULL)
    {
        connect(this, SIGNAL(delete_list_signal(GLuint) ), this, SLOT(delete_list_slot(GLuint)) );
        connect(this, SIGNAL(delete_texture_signal(GLuint) ), this, SLOT(delete_texture_slot(GLuint)) );
        connect(this,SIGNAL(delete_buffer_signal(GLuint) ),this,SLOT(delete_buffer_slot(GLuint)) );
    }
    static Signal_Handle& instance();
    void delete_list(GLuint list){emit delete_list_signal(list);}
    void delete_texture(GLuint list){emit delete_texture_signal(list);}
    void delete_buffer(GLuint buffer){emit delete_buffer_signal(buffer);}

signals:
    void delete_list_signal(GLuint list);
    void delete_texture_signal(GLuint list);
    void delete_buffer_signal(GLuint buffer);

protected slots:
    void delete_list_slot(GLuint list)
    {
        glDeleteLists(list,1);
    }

    void delete_texture_slot(GLuint texture)
    {
        glDeleteTextures(1,&texture);
    }

    void delete_buffer_slot(GLuint buffer);
};

}
}
#endif // SIGNALHANDLE_H
