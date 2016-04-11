#ifndef __MINMAXFILTER_H__
#define __MINMAXFILTER_H__


#include <stdio.h>
#include <stdlib.h>

#include <deque>

#include <opencv2/core/core.hpp>


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// maxValues - size of (nd - width + 1)
template<class T>
void simpleLemireMax(T *array, int nd, int width,
                     T *maxValues)
{
    std::deque<int> maxfifo;
    maxfifo.push_back(0);

    for (int i = 1; i < width; ++i) {
        if (array[i] > array[i - 1]) { //overshoot
            maxfifo.pop_back();
            while (!maxfifo.empty()) {
                if (array[i] <= array[maxfifo.back()])
                    break;
                maxfifo.pop_back();
            }
        }
        maxfifo.push_back(i);
    }

    for (int i = width; i < nd; ++i) {
        maxValues[i - width] = array[maxfifo.front()];
        if (array[i] > array[i - 1]) { //overshoot
            maxfifo.pop_back();
            while (!maxfifo.empty()) {
                if (array[i] <= array[maxfifo.back()])
                    break;
                maxfifo.pop_back();
            }
        }

        maxfifo.push_back(i);

        if (i == width + maxfifo.front())
            maxfifo.pop_front();
    }

    maxValues[nd - width] = array[maxfifo.front()];
}

template<class T>
void simpleLemireMin(T *array, int nd, int width,
                     T *minValues)
{
    std::deque<int> minfifo;
    minfifo.push_back(0);

    for (int i = 1; i < width; ++i) {
        if (array[i] <= array[i - 1]) {
            minfifo.pop_back();
            while (!minfifo.empty()) {
                if (array[i] >= array[minfifo.back()])
                    break;
                minfifo.pop_back();
            }
        }
        minfifo.push_back(i);
    }

    for (int i = width; i < nd; ++i) {
        minValues[i - width] = array[minfifo.front()];
        if (array[i] <= array[i - 1]) {
            minfifo.pop_back();
            while (!minfifo.empty()) {
                if (array[i] >= array[minfifo.back()])
                    break;
                minfifo.pop_back();
            }
        }
        minfifo.push_back(i);
        if (i == width + minfifo.front())
            minfifo.pop_front();
    }

    minValues[nd - width] = array[minfifo.front()];
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int minFilter2D(cv::Mat &src, cv::Mat &dst, int kSize);


#endif // end of __MINMAXFILTER_H__
