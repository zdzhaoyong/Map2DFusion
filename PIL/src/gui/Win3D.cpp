#include <iostream>
#include <string.h>

#include "base/Svar/Svar.h"
#include "base/time/Global_Timer.h"
#include "Win3D.h"

namespace pi {

#define A360byPI 114.59155902616439

Signal_Handle signal_handle;
int GL_Image::win3d_h,GL_Image::win3d_w;

void glMultMatrix(SE3<float> pose)
{
    Point3f trans=pose.get_translation();
    glTranslatef(trans.x,trans.y,trans.z);
    float x,y,z,w;
    pose.get_rotation().getValue(x,y,z,w);
    glRotatef(A360byPI*acos(w),x,y,z);
}

Signal_Handle& Signal_Handle::instance()
{
    return signal_handle;
}

GL_Object::GL_Object():pointSize(2.5),lineWidth(5),scale(1),isChanged(true)
{

}

GL_Object::~GL_Object()
{
//    cout<<"list "<<listName<<" need to be deleted.\n";
    signal_handle.delete_list(listName);
}

bool GL_Object::LoadFromFile(string off_file)
{
    cout<<"Loading object mesh from file "<<off_file<<"...\n";
    int num=m_mesh.load(off_file.c_str());
    m_mesh.mesh_normalize();
//    cout<<"Loaded "<<num<<" elements.\n";
    isChanged=true;
    return num;
}

void GL_Object::Draw()
{
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
    glMultMatrix(pose);
    if(!listName) listName=glGenLists(1);
    if(isChanged&&listName)
    {
//        timer.enter("DrawWithoutList");
        glNewList(listName,GL_COMPILE_AND_EXECUTE);
        if(points.size())
        {
            //Draw colorful points
            glPointSize(pointSize);

            glBegin(GL_POINTS);
            glNormal3d(0, 0, 1);
            for(int i=0;i<points.size();i++)
            {
                ColorfulPoint &point=points[i];
                glColor3ub(point.color[0],point.color[1],point.color[2]);
                glVertex3d(point.point[0],point.point[1],point.point[2]);
            }
            glEnd();
        }

        if(lines.size())
        {
            //Draw colorful lines
            glLineWidth(lineWidth);

            glBegin(GL_LINES);
            for(int i=0;i<lines.size();i++)
            {
                ColorfulLine &line=lines[i];
                glColor3ub(line.color[0],line.color[1],line.color[2]);
                glVertex3d(line.point1[0],line.point1[1],line.point1[2]);
                glVertex3d(line.point2[0],line.point2[1],line.point2[2]);
            }
            glEnd();
        }


        //Draw the triangles &&off model
        glBegin(GL_TRIANGLES);
        int     v1, v2, v3;
        double  *v, *n;
        v = m_mesh.vex_arr;
        n = m_mesh.ele_norm;
        //    cout<<"element_num="<<m_mesh.ele_num<<endl;
        if(v!=NULL&&n!=NULL)
            for(int i=0; i<m_mesh.ele_num; i++) {
                v1 = m_mesh.ele_arr[i*3+0];
                v2 = m_mesh.ele_arr[i*3+1];
                v3 = m_mesh.ele_arr[i*3+2];

                glColor3f(1, 1, 1);
                glNormal3d(n[i*3+0],  n[i*3+1],  n[i*3+2]);

                glVertex3d(v[v1*3+0], v[v1*3+1], v[v1*3+2]);
                glVertex3d(v[v2*3+0], v[v2*3+1], v[v2*3+2]);
                glVertex3d(v[v3*3+0], v[v3*3+1], v[v3*3+2]);
            }
        //    else cout<<"Object got nothing to draw!\n";
        glEnd();
        glEndList();
        isChanged=false;
//        timer.leave("DrawWithoutList");
    }
    else
    {
//        timer.enter("DrawWithList");
        glCallList(listName);
//        timer.leave("DrawWithList");
    }
    glPopMatrix();
}

void GL_Object::ApplyScale(double Scale)
{
    cout<<"Applying scale "<<Scale<<".\n";
    scale=Scale;
    for(int i=0;i<m_mesh.vex_num*3;i++)
        m_mesh.vex_arr[i]=m_mesh.vex_arr[i]*scale;
    isChanged=true;
}

GL_Image::~GL_Image()
{
    signal_handle.delete_texture(texture);
}

void GL_Image::draw()
{
    ScopedMutex lc(lock);

    if(!data) return;

    if(!texture)
    {
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &(texture));
    }

    if(ischanged)
    {
        GLint last_texture_ID;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
        glBindTexture(GL_TEXTURE_2D,texture);
//        timer.enter("TextureGenarate");
        // Create the OpenGL texture map
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_w, img_h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//        timer.leave("TextureGenarate");
        glBindTexture(GL_TEXTURE_2D,last_texture_ID);
        ischanged=false;
    }

    {
        glColor3ub(255,255,255);
        GLint last_texture_ID;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
        glBindTexture(GL_TEXTURE_2D,texture);
        // Display the OpenGL texture map
        glBegin(GL_QUADS);
        // upper left
        glTexCoord2f(0, 0);
        glVertex2f(x_min, win3d_h-y_max);
        // upper right
        glTexCoord2f(1,0);
        glVertex2f(x_max, win3d_h-y_max);
        // bottom right
        glTexCoord2f(1,1);
        glVertex2f(x_max,win3d_h-y_min);
        // bottom left
        glTexCoord2f(0, 1);
        glVertex2f(x_min, win3d_h-y_min);

        glEnd();
        glBindTexture(GL_TEXTURE_2D,last_texture_ID);
    }
}

Win3D::Win3D(QWidget *parent)
    : QGLViewer(parent)
{
    connect(this, SIGNAL(update_signal(void)), this, SLOT(update_slot(void)));
    restoreStateFromFile();

    //help();
    this->setBackgroundColor(QColor(0, 0, 0));
    locked=false;isDrawing=false;drawNow=false;
//    cout<<"MousePressHandle="<<MousePressHandle<<endl<<"DoNothing="<<DoNothing<<endl;

    event_handle = NULL;
    setSceneRadius(10);

    infos.push_back(&info);
//   this->startTimer(20);
}

void Win3D::timerEvent(QTimerEvent *event)
{
    if((!isDrawing)&&drawNow)
    {
        this->updateGL();
        drawNow=false;
        //temp.clear();
    }
}

void Win3D::keyPressEvent(QKeyEvent *e)
{
    if( event_handle != NULL ) {
        if( event_handle->KeyPressHandle(e) == false )
            QGLViewer::keyPressEvent(e);
    }
    else
        QGLViewer::keyPressEvent(e);
}

void Win3D::mousePressEvent(QMouseEvent* e)
{
    if( event_handle != NULL ) {
        if( event_handle->MousePressHandle(e) == false )
            QGLViewer::mousePressEvent(e);
    }
    else
        QGLViewer::mousePressEvent(e);
}

QString Win3D::helpString() const
{
    QString text("<h2>I n t e r f a c e</h2>");
    text += "A GUI can be added to a QGLViewer widget using Qt's <i>Designer</i>. Signals and slots ";
    text += "can then be connected to and from the viewer.<br><br>";
    text += "You can install the QGLViewer designer plugin to make the QGLViewer appear as a ";
    text += "standard Qt widget in the Designer's widget tabs. See installation pages for details.";
    return text;
}

void Win3D::InsertAxis(SE3<float> pose,float width,float length,bool forver)
{
    float R[9];
    pose.get_rotation().getMatrix(R);
    Point3f   T=pose.get_translation();
    ColorfulLine axis;
    axis.point1=T;
    axis.point2=T+length*Point3f(R[0],R[3],R[6]);
    axis.color=Color3b(255,0,0);
    InsertLine(axis,forver);
    axis.point2=T+length*Point3f(R[1],R[4],R[7]);
    axis.color=Color3b(0,255,0);
    InsertLine(axis,forver);
    axis.point2=T+length*Point3f(R[2],R[5],R[8]);
    axis.color=Color3b(0,0,255);
    InsertLine(axis,forver);
}

void Win3D::draw()
{
    //Draw text
    if( svar.GetInt("Win3D.DrawInfoText", 1) ) {
        int j=1;
        for(int i=0;i<infos.size();i++)
        {
            stringstream& strs=*(infos[i]);
            if(strs.str().size()<2) continue;
            string str;
            stringstream tmp(strs.str());

            glColor3ub(0x00, 0xFF, 0xFF);
            for(;getline(tmp,str);j++)
                this->drawText(10, 25*j, QString::fromStdString(str),QFont("",12,2,false));
        }
    }

    lock();
    temp.Draw();
    forv.Draw();
    for(int i=0;i<scence.size();i++)
    {
        if(scence[i]!=NULL)
        {
            GL_Object &obj=*scence[i];
            obj.Draw();
        }
//        unlock();
//        tm_sleep(2);
//        lock();
    }
    unlock();

    if(images.size())
    {
        GL_Image::win3d_h=height();
        GL_Image::win3d_w=width();

        startScreenCoordinatesSystem();
        for(int i=0;i<images.size();i++)
        {
//            timer.enter("Win3d.DrawImg");
            GL_Image& img=*images[i];
            img.draw();
//            timer.leave("Win3d.DrawImg");

        }
        stopScreenCoordinatesSystem();
    }

    {
        pi::ScopedMutex m(m_mutexDrawOpenGL);

        for(int i=0;i<draw_opengl.size();i++)
            draw_opengl[i]->Draw_Something();
    }
}

void glVertex(const Point3d& pt) {glVertex3d(pt.x,pt.y,pt.z);}
void glVertex(const Point3f& pt) {glVertex3f(pt.x,pt.y,pt.z);}

}//end of namespace
