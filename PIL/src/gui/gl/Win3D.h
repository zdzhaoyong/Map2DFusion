#ifndef PI_GL_WIN3D_H
#define PI_GL_WIN3D_H

#include <vector>
#include <set>
#include <unistd.h>
#include <sstream>

#include <qapplication.h>

#include <QGLViewer/qglviewer.h>
#include <QKeyEvent>
#include <QMouseEvent>

#include <base/types/types.h>
#include <base/types/SE3.h>
#include <base/system/thread/ThreadBase.h>
#include <base/types/SPtr.h>

#include "GL_FatherObject.h"
#include "SignalHandle.h"

namespace pi {
namespace gl {

typedef unsigned char byte;
typedef pi::Point3_<byte>  Color3b;
typedef pi::Point3_<float> Point3f;

class Win3D: public QGLViewer
{
    Q_OBJECT

public:
    Win3D(QWidget *parent=NULL);

    void insert(const GL_Object& obj)   {pi::ScopedMutex lock(m_mutex);scence.insert(obj);}
    void insert(const GL_ObjectPtr& obj){pi::ScopedMutex lock(m_mutex);scence.insert(obj);}
    void insert(GL_Object* obj){pi::ScopedMutex lock(m_mutex);scence.insert(GL_ObjectPtr(obj));}

    template <class T>
    void InsertInfo(T msg){pi::ScopedMutex lock(m_mutex);info<<msg;}
    void ClearInfo(){pi::ScopedMutex lock(m_mutex);info.str("");}
    void ShowStream(std::stringstream* str){pi::ScopedMutex lock(m_mutex);infos.push_back(str);}
    void ShowStream(std::stringstream& str){pi::ScopedMutex lock(m_mutex);infos.push_back(&str);}

    void SetEventHandle(EventHandle *handle) {
        pi::ScopedMutex lock(m_mutex);
        event_handle = handle;
    }

    void SetDraw_Opengl(Draw_Opengl *drawer) {
        pi::ScopedMutex lock(m_mutex);
        draw_opengl.push_back(drawer);
    }

    void clear(){scence.clear();}
    void update(){emit update_signal();}
    void Show(){emit show_signal();}

    void setPose(pi::SE3f pose);
    void setCamera(int w,int h,double fx,double fy,double cx,double cy);
    void loadProjectionMatrix();
    virtual void preDraw();

    bool drawingWithNames();

signals:
    void update_signal(void);
    void show_signal(void);

protected :
    virtual void initializeGL();
    virtual void draw();
    virtual void keyPressEvent(QKeyEvent * event);
    virtual void mousePressEvent(QMouseEvent* e);

    virtual void drawWithNames();
    virtual void endSelection(const QPoint& point);

    virtual QString helpString() const;

protected slots:
    void update_slot(void)
    {
        this->updateGL();
    }
    void show_slot()
    {
        this->show();
    }

public:
    std::stringstream               info;
    std::vector<std::stringstream*> infos;

protected:
    Father_Object scence;

    EventHandle *event_handle;
    std::vector<Draw_Opengl*> draw_opengl;

    pi::Mutex   m_mutex;
    double fx,fy,cx,cy;
    bool            bDrawWithNames;
};


}}
#endif // WIN3D_H
