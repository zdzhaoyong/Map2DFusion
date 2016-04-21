#ifndef UNDISTORTER_H
#define UNDISTORTER_H

#include <opencv2/core/core.hpp>

#include "Cameras.h"

namespace pi{
namespace hardware{

class UndistorterImpl;
class Undistorter
{
public:
    Undistorter(Camera in, Camera out);

    bool undistort(const cv::Mat& image, cv::Mat& result);
    //Undistorting fast, no interpolate (bilinear) is used
    bool undistortFast(const cv::Mat& image, cv::Mat& result);

    Camera cameraIn();
    Camera cameraOut();

    bool prepareReMap();
    bool valid();
private:
    SPtr<UndistorterImpl> impl;
};

}
}
#endif // UNDISTORTER_H
