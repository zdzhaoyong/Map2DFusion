#ifndef __VIDEOREADER_H__
#define __VIDEOREADER_H__

#include <stdio.h>
#include <string.h>
#include <string>

#include <opencv2/highgui/highgui.hpp>

#include "base/base.h"

namespace pi {


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The VideoFrameInfo class
///
class RVideoFrameInfo
{
public:
    int         frameIdx;           ///< frame index
    double      timestamp;          ///< second since 1970-01-01 00:00:00
    double      pts;                ///< pts (unit: second)

public:
    RVideoFrameInfo() {
        frameIdx = 0;
        timestamp = 0.0;
        pts = 0.0;
    }
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The VideoReader class
///
class RVideoReader
{
public:
    RVideoReader() {
        init();
    }

    RVideoReader(const std::string& vf) {
        init();
        open(vf);
    }

    virtual ~RVideoReader() {
        release();
    }

    ///
    /// \brief open a single video file
    /// \param vf - video file name
    /// \return  0 - success
    ///         -1 - failed to decode given file
    ///         -2 - Couldn't find stream information
    ///         -3 - Didn't find a video stream
    ///         -4 - Codec not found
    ///         -5 - Could not open codec
    ///
    int open(const std::string& vf);

    ///
    /// \brief open multi video file
    /// \param vfBase   - video file base name
    /// \param vfExt    - video file ext name
    /// \param vfBegIdx - video begin index
    /// \return
    ///
    int open(const std::string& vfBase, const std::string& vfExt, int vfIdx=0);

    ///
    /// \brief Video file opened or not
    /// \return 1 - opened
    ///         0 - not opened
    ///
    int isOpened(void) {
        return m_isOpened;
    }

    ///
    /// \brief close video file
    /// \return  0 - success
    ///         -1 - failed to close video
    ///
    int close(void);

    ///
    /// \brief read an image from video stream
    ///
    /// \param image     - readed image
    /// \param fi        - frame information
    /// \param timestamp - want to jump to the given timestamp (micro-second)
    ///
    /// \return  0 - success read a frame
    ///         -1 - video file not opened
    ///         -2 - end of stream
    ///
    virtual int read(cv::Mat& image, RVideoFrameInfo* fi=NULL, int64_t timestamp=0);

    ///
    /// \brief read an image from video stream
    ///
    /// \param image     - readed image
    /// \param fi        - frame information
    /// \param timestamp - want to jump to the given timestamp (second)
    ///
    /// \return  0 - success read a frame
    ///         -1 - video file not opened
    ///         -2 - end of stream
    ///
    virtual int read(cv::Mat& image, RVideoFrameInfo* fi, double timestamp);

    ///
    /// \brief retrieve an image from video stream
    /// \param image - readed image
    /// \return 0 - success
    ///         other \see read
    ///
    /// \see read
    ///
    virtual int retrieve(cv::Mat& image) {
        return read(image);
    }

    ///
    /// \brief operator >> read an image from video stream
    /// \param image - readed image
    /// \return RVideoReader object
    ///
    /// \see read
    ///
    virtual RVideoReader& operator >> (cv::Mat& image) {
        read(image);
        return *this;
    }

    ///
    /// \brief video fast forward by given frames
    /// \param frames - skip frames
    /// \return  0 - success
    ///         -1 - end of stream
    ///
    virtual int ff(int frames);


    ///
    /// \brief set extra time fix coefficient for video file begin
    /// \param tfc - time fix coefficient
    ///
    void setExtraFixCoeff(ri64 tfc) {
        m_extraFixCoeff = tfc;
    }

    ///
    /// \brief get extra time fix coefficient for video file begin
    /// \return time fix coefficient
    ///
    double getExtraFixCoeff(void) {
        return m_extraFixCoeff;
    }


    ///
    /// \brief set time drift coefficient
    /// \param tdc - time drift coefficient (drift value / second)
    ///
    void setTimeDriftCoeff(double tdc=1);

    ///
    /// \brief get time drift coefficient
    /// \param tdc - time drift coefficient (drift value / second)
    ///
    double getTimeDriftCoeff(void) {
        return m_timeDriftCoeff;
    }

protected:
    void init(void);
    void release(void);


public:
    double              fps;                            ///< frame-per-second
    int                 fps_num, fps_den;               ///< fps num/den
    double              start_time;                     ///< start time
    double              duration;                       ///< video length

    double              playedTime;                     ///< played time (in second)
    double              tsBeg;                          ///< begin time in timestamp (in second)
    double              tsNow;                          ///< current timestamp (in second)
    double              tsPartBeg;                      ///< part's beginning timestamp (in second)

    int                 imgW, imgH;                     ///< image width & height
    int                 frameIdx;                       ///< current frame index

protected:
    void                *m_priData;                     ///< private data storing codec etc.

    std::string         m_vfBase, m_vfExt;              ///< video file base & ext name
    int                 m_vfIdx;                        ///< video file index (0 ~ vfN-1)
    int                 m_vfType;                       ///< video file type
                                                        ///<        0 - single video file
                                                        ///<        1 - multi video files

    int                 m_videoFileBeg;                 ///< video file begin
    ri64                m_PTS_initVal;                  ///< PTS initial value

    double              m_extraFixCoeff;                ///< extra time fix coefficient
                                                        ///<    (for each beginning video file)
    double              m_timeDriftCoeff;               ///< Time drift coefficient
                                                        ///<    (drift value per second)
    double              m_timeToGPS;                    ///< Time difference between GPS time (second)
    double              m_timeToUTC;                    ///< video time to UTC (second)


    int                 m_isOpened;                     ///< flags indicating video is open or not
};

} // end of namespace pi

#endif // end of __VIDEOREADER_H__

