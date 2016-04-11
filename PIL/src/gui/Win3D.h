#ifndef WIN3D_H
#define WIN3D_H

#include <qapplication.h>

#include <QGLViewer/qglviewer.h>
#include <QKeyEvent>
#include <QMouseEvent>

#include <vector>
#include <unistd.h>
#include <sstream>

//#include <tr1/memory>

//#include <base/utils.h>
#include <base/types/types.h>
#include <base/types/SE3.h>
#include <base/system/thread/ThreadBase.h>
#include <base/types/SPtr.h>

#include "mesh_off.h"

///All use float here


namespace pi {

using namespace std;
//using namespace TooN;

typedef unsigned char byte;
typedef pi::Point3_<byte>  Color3b;
typedef pi::Point3_<float> Point3f;

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

struct ColorfulPoint
{
    ColorfulPoint(const Point3f& pose,const Color3b& c)
        :color(c),point(pose){}
    ColorfulPoint(const Point3f& pose,byte r=0,byte g=0,byte b=0)
        :color(r,g,b),point(pose){}
    ColorfulPoint(float x=0,float y=0,float z=0,byte r=0,byte g=0,byte b=0)
        :color(r,g,b),point(x,y,z){}
    Color3b   color;
    Point3f   point;
};

struct ColorfulLine
{
    ColorfulLine(){}

    ColorfulLine(const Point3f& p1,const Point3f& p2,const Color3b& rgb)
        :point1(p1),point2(p2),color(rgb){}

    ColorfulLine(const Point3f& p1,const Point3f& p2,byte r=0,byte g=0,byte b=0)
        :color(r,g,b){point1=p1;point2=p2;}

    Color3b color;
    Point3f point1,point2;
};

class Signal_Handle:public QObject
{
    Q_OBJECT

public:
    Signal_Handle(QWidget *parent=NULL)
    {
        connect(this, SIGNAL(delete_list_signal(GLuint) ), this, SLOT(delete_list_slot(GLuint)) );
        connect(this, SIGNAL(delete_texture_signal(GLuint) ), this, SLOT(delete_texture_slot(GLuint)) );
    }
    static Signal_Handle& instance();
    void delete_list(GLuint list){emit delete_list_signal(list);}
    void delete_texture(GLuint list){emit delete_texture_signal(list);}

signals:
    void delete_list_signal(GLuint list);
    void delete_texture_signal(GLuint list);

protected slots:
    void delete_list_slot(GLuint list)
    {
        glDeleteLists(list,1);
//        cout<<"list "<<list<<" deleted.\n";
    }

    void delete_texture_slot(GLuint texture)
    {
        glDeleteTextures(1,&texture);
//        cout<<"list "<<list<<" deleted.\n";
    }
};

class ListObject
{

protected:
    GLuint listName;
};

class GL_Object
{
public:
    GL_Object();
    ~GL_Object();
    void SetPose(SE3<float> p){pose=p;}
    bool LoadFromFile(string off_file);
    void InsertPoint(ColorfulPoint point){points.push_back(point);isChanged=true;}
    void InsertLine(ColorfulLine line){lines.push_back(line);isChanged=true;}
    bool setPoints(vector<ColorfulPoint>& Points){points=Points;}
    void Draw();
    void clear()
    {
        m_mesh.free();
        points.clear();
        lines.clear();
    }
    void ApplyScale(double Scale);
public:
    SE3<float> pose;
    float pointSize,lineWidth;
protected:
    MeshData m_mesh;
    vector<ColorfulPoint> points;
    vector<ColorfulLine>  lines;
    double scale;
    bool isChanged;
    GLuint listName;
};

struct GL_Image
{
    GL_Image():data(NULL),x_min(0),y_min(0),texture(0),ischanged(true){}
    GL_Image(unsigned char* Data,const int &ImgW,const int &ImgH,const int &Xmin=0,const int &Ymin=0)
        :texture(0),img_w(ImgW),img_h(ImgH),x_min(Xmin),y_min(Ymin),x_max(ImgW+Xmin),y_max(ImgH+Ymin),
          data(Data),ischanged(true){}
    GL_Image(unsigned char* Data,const int &ImgW,const int &ImgH,const int &Xmin,const int &Ymin,const int &Xmax,const int &Ymax)
        :texture(0),img_w(ImgW),img_h(ImgH),x_min(Xmin),y_min(Ymin),x_max(Xmax),y_max(Ymax),
          data(Data),ischanged(true){}
    ~GL_Image();

    void setImgSize(int w,int h){img_w=w;img_h=h;}
    void setPose(int x,int y){x_min=x;y_min=y;}
    void setDisplaySize(int w,int h){x_max=x_min+w;y_max=y_min+h;}
    void setDisplaySize(const float& scale){x_max=x_min+(x_max-x_min)*scale;y_max=y_min+(y_max-y_min)*scale;}
    void draw();

    static int win3d_w,win3d_h;
    int img_w,img_h;
    int x_min,y_min,x_max,y_max;
    GLuint texture;
    unsigned char* data;
    bool ischanged;
    Mutex lock;
};

class Win3D: public QGLViewer
{
    Q_OBJECT

public:
    Win3D(QWidget *parent=NULL);

    void insertObject(GL_Object &obj){lock();scence.push_back(&obj);unlock();}
    void insertObject(GL_Object *obj){lock();scence.push_back(obj);unlock();}
    void lock(){while(locked) usleep(10);locked=true;}
    void unlock(){locked=false;}

    void InsertLine(const ColorfulLine &line,bool forver=false)
    {
        if(forver) forv.InsertLine(line);
        else       temp.InsertLine(line);
    }
    void InsertPoint(const ColorfulPoint &point,bool forver=false)
    {
        if(forver) forv.InsertPoint(point);
        else       temp.InsertPoint(point);
    }
    void InsertImage(GL_Image &img){images.push_back(&img);}
    void InsertImage(GL_Image* img){images.push_back(img);}
    GL_Image& GetImage(int i=0){return *images[i];}
    void InsertAxis(SE3<float> pose,float width=1,float length=0.1,bool forver=true);
    template <class T>
    void InsertInfo(T msg){info<<msg;}

    void ClearInfo(){info.str("");}
    void ShowStream(stringstream* str) {
        pi::ScopedMutex m(m_mutexDrawOpenGL);
        infos.push_back(str);
    }
    void ShowStream(stringstream& str) {
        pi::ScopedMutex m(m_mutexDrawOpenGL);
        infos.push_back(&str);
    }

    void SetEventHandle(EventHandle *handle) {
        event_handle = handle;
    }

    void SetDraw_Opengl(Draw_Opengl *drawer) {
        pi::ScopedMutex m(m_mutexDrawOpenGL);
        draw_opengl.push_back(drawer);
    }

//    QImage SnapShotImg(){return frameBufferSnapshot();}

    void ClearTemp(){temp.clear();}
    void ClearAll(){temp.clear();forv.clear();scence.clear();}
    void update(){emit update_signal();}

signals:
    void update_signal(void);

protected :
    virtual void draw();
    void timerEvent(QTimerEvent *event);
    virtual void keyPressEvent(QKeyEvent * event);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual QString helpString() const;
//    virtual void startScreenCoordinatesSystem();
//    virtual void stopScreenCoordinatesSystem();

protected slots:
    void update_slot(void)
    {
        this->updateGL();
    }

public:
    stringstream info;
    vector<stringstream*> infos;


protected:
    vector<GL_Object*> scence;
    GL_Object temp,forv;
    vector<GL_Image*>  images;
    bool locked,isDrawing,drawNow;

    EventHandle *event_handle;
    vector<Draw_Opengl*> draw_opengl;

    pi::Mutex   m_mutexDrawOpenGL;
};

void glVertex(const Point3d& pt);
void glVertex(const Point3f& pt);

}//end of namespace pi

#endif // WIN3D_H
