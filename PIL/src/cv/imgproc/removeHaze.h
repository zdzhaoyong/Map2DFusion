#ifndef __REMOVEHAZE_H__
#define __REMOVEHAZE_H__


#include <opencv2/core/core.hpp>

///
/// \brief remove haze of the image
///
/// \param src              - source image
/// \param dst              - resulting image
/// \param mfSize           - min filter size
/// \param gfSize           - guided filter size
/// \param minAtomsLight    - min atomsfer light value (220 default)
/// \return
///
int removeHaze(cv::Mat &src, cv::Mat &dst,
               int mfSize, int gfSize=0, int minAtomsLight=220);


#endif // end of __REMOVEHAZE_H__

