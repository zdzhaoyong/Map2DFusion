
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <deque>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "minMaxFilter.h"

using namespace std;
using namespace cv;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int minFilter2D(cv::Mat &src, cv::Mat &dst, int kSize)
{
    int         i, j;
    int         w, h, nw, nh;
    cv::Mat     imgT;

    uint8_t     *buf, *bufT, *p;

    w = src.cols;
    h = src.rows;
    nw = w - kSize + 1;
    nh = h - kSize + 1;

    if( src.channels() != 1 ) return -1;

    buf  = new uint8_t[nw*h];
    bufT = new uint8_t[h*nw];

    for(i=0; i<h; i++) {
        simpleLemireMin(src.data + i*w, w, kSize, buf+i*nw);
    }

    for(i=0; i<h; i++) {
        for(j=0; j<nw; j++) {
            bufT[j*h + i] = buf[i*nw + j];
        }
    }
    for(i=0; i<nw; i++) {
        simpleLemireMin(bufT + i*h, h, kSize, buf+i*nh);
    }

    imgT.create(nh, nw, CV_8UC1);
    p = imgT.data;
    for(i=0; i<nh; i++) {
        for(j=0; j<nw; j++) {
            *(p++) = buf[j*nh + i];
        }
    }

    delete [] buf;
    delete [] bufT;

    // resize to original image size
    cv::resize(imgT, dst, Size(w, h));

    return 0;
}

