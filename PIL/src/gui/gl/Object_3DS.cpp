#include "Object_3DS.h"
#include "glHelper.h"

//#define HAS_LIB3DS
#ifdef  HAS_LIB3DS
#include <lib3ds/file.h>
#include <lib3ds/node.h>
#include <lib3ds/camera.h>
#include <lib3ds/mesh.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>
#endif

#include <GL/gl.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

namespace pi{
namespace gl{

Object_3DS::Object_3DS(const char* filename):file(NULL)
{
#ifdef HAS_LIB3DS
    file=lib3ds_file_load(filename);
    if(!file)
    {
        MSG_ERROR("Can't load 3ds file %s",filename);
    }
    else
        lib3ds_file_eval(file,0);
#else
    MSG_ERROR("Object_3DS can't be used. Please ensure lib3ds is linked correctly!");
#endif
}

void Object_3DS::init()
{
#ifdef HAS_LIB3DS
    if(!file) return ;
    lib3ds_file_eval(file,0);

    // Lights
    GLfloat amb[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat dif[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat spe[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat pos[] = {0.0, 0.0, 0.0, 1.0};
    int li=GL_LIGHT0;
    for (Lib3dsLight* l=file->lights; l; l=l->next)
      {
        glEnable(li);

        glLightfv(li, GL_AMBIENT,  amb);
        glLightfv(li, GL_DIFFUSE,  dif);
        glLightfv(li, GL_SPECULAR, spe);

        pos[0] = l->position[0];
        pos[1] = l->position[1];
        pos[2] = l->position[2];
        glLightfv(li, GL_POSITION, pos);

        if (!l->spot_light)
        continue;

        pos[0] = l->spot[0] - l->position[0];
        pos[1] = l->spot[1] - l->position[1];
        pos[2] = l->spot[2] - l->position[2];
        glLightfv(li, GL_SPOT_DIRECTION, pos);
        ++li;
      }
#endif
}

void Object_3DS::renderNode(Lib3dsNode *node)
{
#ifdef HAS_LIB3DS
    for (Lib3dsNode* p=node->childs; p!=0; p=p->next)
      renderNode(p);

    if (node->type == LIB3DS_OBJECT_NODE)
      {
        if (strcmp(node->name,"$$$DUMMY")==0)
      return;

        if (!node->user.d)
      {
        Lib3dsMesh *mesh=lib3ds_file_mesh_by_name(file, node->name);
        if (!mesh)
          return;
        MSG_INFO("Rendering node %s",node->name);
        MSG_INFO("Face number: %d",mesh->faces);
//        return ;
        node->user.d = glGenLists(1);
        glNewList(node->user.d, GL_COMPILE);

        Lib3dsVector *normalL = new Lib3dsVector[3*mesh->faces];

        Lib3dsMatrix M;
        lib3ds_matrix_copy(M, mesh->matrix);
        lib3ds_matrix_inv(M);
        glMultMatrixf(&M[0][0]);

        lib3ds_mesh_calculate_normals(mesh, normalL);

        for (unsigned int p=0; p<mesh->faces; ++p)
          {
            Lib3dsFace *f=&mesh->faceL[p];
            Lib3dsMaterial *mat=0;
            if (f->material[0])
          mat=lib3ds_file_material_by_name(file, f->material);

            if (mat)
          {
            static GLfloat a[4]={0,0,0,1};
            float s;
            glMaterialfv(GL_FRONT, GL_AMBIENT, a);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);
            s = pow(2, 10.0*mat->shininess);
            if (s>128.0)
              s=128.0;
            glMaterialf(GL_FRONT, GL_SHININESS, s);
          }
            else
          {
            Lib3dsRgba a={0.2, 0.2, 0.2, 1.0};
            Lib3dsRgba d={0.8, 0.8, 0.8, 1.0};
            Lib3dsRgba s={0.0, 0.0, 0.0, 1.0};
            glMaterialfv(GL_FRONT, GL_AMBIENT, a);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
            glMaterialfv(GL_FRONT, GL_SPECULAR, s);
          }

            glBegin(GL_TRIANGLES);
            glNormal3fv(f->normal);
            for (int i=0; i<3; ++i)
          {
            glNormal3fv(normalL[3*p+i]);
            glVertex3fv(mesh->pointL[f->points[i]].pos);
          }
            glEnd();
          }

        delete[] normalL;

        glEndList();
      }

        if (node->user.d)
      {
        glPushMatrix();
        Lib3dsObjectData* d = &node->data.object;
        glMultMatrixf(&node->matrix[0][0]);
        glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
        glCallList(node->user.d);
        glPopMatrix();
      }
      }
#endif
}


void Object_3DS::draw()
{
#ifdef HAS_LIB3DS

if(!file) return ;

glPushMatrix();
glMatrixMode(GL_MODELVIEW);

glMultMatrix(pose);

for (Lib3dsNode* p=file->nodes; p!=0; p=p->next)
  renderNode(p);

glPopMatrix();
#endif
}


}}
