#ifndef HIGHGUI_H
#define HIGHGUI_H


#include <opencv2/highgui/highgui.hpp>

#include "gui/Win3D.h"

namespace pi {

void imshow(Win3D* win3d,cv::Mat img2show,float scale=0.5);

}//end of namespace pi

#endif // HIGHGUI_H
