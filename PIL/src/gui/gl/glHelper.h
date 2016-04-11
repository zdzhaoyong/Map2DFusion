#ifndef GLHELPER_H
#define GLHELPER_H

#include <base/types/SE3.h>

void glVertex(const pi::Point3d& pt);
void glVertex(const pi::Point3f& pt);
void glColor(const pi::Point3ub& color);

void glMultMatrix(const pi::SE3f& pose);
#endif // GLHELPER_H
