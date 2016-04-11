#ifndef _UNDISTORTER_HPP_
#define _UNDISTORTER_HPP_

#include <opencv2/core/core.hpp>

#include "Camera.h"

class Undistorter
{
public:
    Undistorter(std::string config_file);
    Undistorter(Camera* in, Camera* out) : camera_in(in),camera_out(out) {
        prepareReMap();
    }
    ~Undistorter();

    bool undistort(const cv::Mat& image, cv::Mat& result);
    bool undistortFast(const cv::Mat& image, cv::Mat& result);

protected:
    bool prepareReMap();

public:
    Camera* camera_in;
    Camera* camera_out;

    int width_in, height_in;
    int width_out,height_out;

    float*  remapX;
    float*  remapY;
    int*    remapFast;

    int     *remapIdx;
    float   *remapCoef;

    /// Is true if the undistorter object is valid (has been initialized with
    /// a valid configuration)
    bool    valid;
};

#endif
