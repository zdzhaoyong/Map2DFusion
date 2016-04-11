#ifndef __GUIDEDFILTER_H__
#define __GUIDEDFILTER_H__


#include <opencv2/core/core.hpp>


cv::Mat guidedFilter(cv::Mat &I, cv::Mat &p, int r, double eps);



#endif // end of __GUIDEDFILTER_H__
