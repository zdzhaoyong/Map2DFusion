 
#include <stdio.h>
#include <stdlib.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "minMaxFilter.h"
#include "guidedFilter.h"

template<class T>
inline T minVal3(T *va)
{
    T   m;
    if( va[0] < va[1] ) m = va[0];
    else                m = va[1];

    if( m < va[2] )     return m;
    else                return va[2];
}

inline uint8_t constrintU8(double v)
{
    if( v < 0 ) return 0;
    if( v > 255 ) return 255;

    return (uint8_t)(v);
}


int removeHaze(cv::Mat &src, cv::Mat &dst,
               int mfSize, int gfSize, int minAtomsLight)
{
    int         i, j;
    int         w, h;

    cv::Mat     dc, dc2;
    uint8_t     *p1, *p2;
    double      *p3;
    double      *t_d;

    w = src.cols;
    h = src.rows;

    if( mfSize < 3 ) mfSize = 3;

    // create DC array
    dc.create(h, w, CV_8UC1);

    if( src.channels() == 3 ) {
        p1 = src.data;
        p2 = dc.data;

        for(j=0; j<h; j++) {
            for(i=0; i<w; i++) {
                *(p2++) = minVal3(p1);
                p1 += 3;
            }
        }
    } else {
        cv::cvtColor(src, dc, CV_BGR2GRAY);
    }

    // perform minFilter2D
    minFilter2D(dc, dc2, mfSize);

    int A = 0;
    t_d = new double[w*h];
    p1 = dc2.data;

    for(j=0; j<h; j++) {
        for(i=0; i<w; i++) {
            if( A < *p1 ) A = *p1;
            t_d[j*w + i] = 1.0 * (255-*p1) / 255.0;

            p1 ++;
        }
    }

    if( minAtomsLight < A ) A = minAtomsLight;

    // perform guided filter
    if( gfSize > 0 ) {
        cv::Mat gi(h, w, CV_64FC1, t_d);

        cv::Mat gray;

        if( src.channels() == 3 )
            cv::cvtColor(src, gray, CV_BGR2GRAY);
        else
            gray = src;

        cv::Mat res = guidedFilter(gray, gi, gfSize, 1e-6);
        memcpy(t_d, res.data, sizeof(double)*w*h);
    }

    // perform hazeRemove
    if( src.channels() == 3 ) {
        dst.create(h, w, CV_8UC3);

        p1 = src.data;
        p2 = dst.data;
        p3 = t_d;

        for(j=0; j<h; j++) {
            for(i=0; i<w; i++) {
                p2[0] = constrintU8((p1[0] - (1.0-*p3)*A) / *p3);
                p2[1] = constrintU8((p1[1] - (1.0-*p3)*A) / *p3);
                p2[2] = constrintU8((p1[2] - (1.0-*p3)*A) / *p3);

                p1 += 3;
                p2 += 3;
                p3 ++;
            }
        }
    } else {
        dst.create(h, w, CV_8UC1);

        p1 = src.data;
        p2 = dst.data;
        p3 = t_d;

        for(j=0; j<h; j++) {
            for(i=0; i<w; i++) {
                *p2 = constrintU8((*p1 - (1.0-*p3)*A) / *p3);

                p1 ++;
                p2 ++;
                p3 ++;
            }
        }
    }

    delete [] t_d;

    return 0;
}
