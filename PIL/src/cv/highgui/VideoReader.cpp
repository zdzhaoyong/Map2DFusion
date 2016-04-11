#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <string>
#include <fstream>
#include <sstream>

#include <opencv2/highgui/highgui.hpp>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

#include "base/time/DateTime.h"
#include "VideoReader.h"

using namespace std;
/**

  |                                       |  |
  |                                       |  |
 **/
namespace pi {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int year_is_leap(int y) {
    y += 1900;
    return (y % 4) == 0 && ((y % 100) != 0 || (y % 400) == 0);
}

void tm_init(struct tm *t)
{
    t->tm_year = 70;
    t->tm_mon  = 0;
    t->tm_mday = 1;
    t->tm_hour = 0;
    t->tm_min  = 0;
    t->tm_sec  = 0;
}

ri64 tm2time_t(struct tm *tm)
{
    static const unsigned ndays[2][12] = {
        {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
    };

    ri64 res = 0;
    int i;

    for (i = 70; i < tm->tm_year; ++i)
        res += year_is_leap(i) ? 366 : 365;

    for (i = 0; i < tm->tm_mon; ++i)
        res += ndays[year_is_leap(tm->tm_year)][i];

    res += tm->tm_mday - 1;
    res *= 24;

    res += tm->tm_hour;
    res *= 60;

    res += tm->tm_min;
    res *= 60;

    res += tm->tm_sec;
    return res;
}


int str2tm(const char *s, struct tm *tm)
{
    int     l;
    int     i1, i2, i3, i4;

    l = strlen(s);
    if( l < 19 ) return -1;

    if( tm == NULL ) return -2;

    // year
    i1 = s[0] - '0';
    i2 = s[1] - '0';
    i3 = s[2] - '0';
    i4 = s[3] - '0';
    tm->tm_year = i1*1000 + i2*100 + i3*10 + i4 - 1900;

    // month
    i1 = s[5] - '0';
    i2 = s[6] - '0';
    tm->tm_mon = i1*10 + i2 - 1;

    // day
    i1 = s[8] - '0';
    i2 = s[9] - '0';
    tm->tm_mday = i1*10 + i2;

    // hour
    i1 = s[11] - '0';
    i2 = s[12] - '0';
    tm->tm_hour = i1*10 + i2;

    // min
    i1 = s[14] - '0';
    i2 = s[15] - '0';
    tm->tm_min = i1*10 + i2;

    // sec
    i1 = s[17] - '0';
    i2 = s[18] - '0';
    tm->tm_sec = i1*10 + i2;

    return 0;
}

int str2timecode(const char *s, struct tm *tm, int &f)
{
    int     l;
    int     i1, i2;

    l = strlen(s);
    if( l < 11 ) return -1;

    if( tm == NULL ) return -2;

    // hour
    i1 = s[0] - '0';
    i2 = s[1] - '0';
    tm->tm_hour = i1*10 + i2;

    // min
    i1 = s[3] - '0';
    i2 = s[4] - '0';
    tm->tm_min = i1*10 + i2;

    // sec
    i1 = s[6] - '0';
    i2 = s[7] - '0';
    tm->tm_sec = i1*10 + i2;

    // frame
    i1 = s[9] - '0';
    i2 = s[10] - '0';
    f = i1*10 + i2;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class VideoReader_InnerData
{
public:
    VideoReader_InnerData() {
        init();
    }

    void init(void) {
        pFormatCtx = NULL;
        pCodecCtx  = NULL;
        pCodec     = NULL;

        vStreamIdx = -1;
        vStream    = NULL;

        pFrame     = NULL;
        pFrameRGB  = NULL;

        videoBuf   = NULL;

        optionsDict = NULL;
        sws_ctx     = NULL;

        vs_timebase = 0;
    }

    void closeVideo(void) {
        if( pFormatCtx == NULL ) return;

        // Free the RGB image
        av_free(videoBuf);
        av_free(pFrameRGB);

        // Free the YUV frame
        av_free(pFrame);

        // Close the codec
        avcodec_close(pCodecCtx);

        // Close the video file
        avformat_close_input(&pFormatCtx);

        init();
    }

public:
    AVFormatContext *pFormatCtx;
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;

    int             vStreamIdx;
    AVStream        *vStream;

    AVFrame         *pFrame;
    AVFrame         *pFrameRGB;
    AVPacket        packet;

    ru8             *videoBuf;

    AVDictionary    *optionsDict;
    SwsContext      *sws_ctx;

    double          vs_timebase;
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void RVideoReader::init(void)
{
    // initialize variables
    m_isOpened = 0;

    fps = 0; fps_num = 0; fps_den = 1;
    start_time = 0;
    duration = 0;

    imgW = 0;
    imgH = 0;

    tsBeg      = 0.0;
    tsPartBeg  = 0.0;
    tsNow      = 0.0;
    playedTime = 0.0;

    m_vfBase = "";
    m_vfExt  = "MP4";
    m_vfType = 0;
    m_vfIdx  = 0;

    m_timeDriftCoeff = 0.0;
    m_extraFixCoeff = 0.0;
    m_timeToGPS = 0.0;                                              // time difference between GPS time
    m_timeToUTC = -pi::get_timeZone()*3600;                         // set time to UTC using current local time setting

    // create private data
    VideoReader_InnerData *pd = new VideoReader_InnerData;
    m_priData = pd;

    // Register all formats and codecs
    av_register_all();
}

void RVideoReader::release(void)
{
    VideoReader_InnerData *pd = (VideoReader_InnerData*) m_priData;

    // close video
    if( m_isOpened ) pd->closeVideo();

    pd->init();

    delete pd;
}

int RVideoReader::open(const std::string& vf)
{
    // load video information
    {
        std::string infofile = vf + "_info";
        CParamArray pa;

        pa.load(infofile);

        pa.d("Video.TimeToUTC", m_timeToUTC);
        pa.d("Video.TimeDiffGps", m_timeToGPS);
        pa.d("Video.TimeDriftCoeff", m_timeDriftCoeff);
        pa.d("Video.TimeBetween", m_extraFixCoeff);

        pa.i("Video.Type", m_vfType);
        pa.i("Video.Index",m_vfIdx);

        pa.s("Video.Ext", m_vfExt);
        pa.s("Video.BaseName", m_vfBase);

        // load begin video file
        if( m_vfType == 1 ) {
            m_vfBase = vf;
            return open(m_vfBase, m_vfExt, m_vfIdx);
        }
    }

    VideoReader_InnerData *pd = (VideoReader_InnerData*) m_priData;

    m_isOpened = 0;
    m_vfType = 0;

    // Open video file
    if(avformat_open_input(&pd->pFormatCtx, vf.c_str(), NULL, NULL) !=0 )
        return -1; // Couldn't open file

    // Retrieve stream information
    if(avformat_find_stream_info(pd->pFormatCtx, NULL)<0)
        return -2; // Couldn't find stream information

    // Dump information about file onto standard error
    //  utils.c, line: 4001
    av_dump_format(pd->pFormatCtx, 0, vf.c_str(), 0);

    // Find the first video stream
    pd->vStreamIdx=-1;
    for(int i=0; i<pd->pFormatCtx->nb_streams; i++)
        if(pd->pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            pd->vStreamIdx = i;
            break;
        }
    if(pd->vStreamIdx == -1)
        return -3; // Didn't find a video stream

    pd->vStream = pd->pFormatCtx->streams[pd->vStreamIdx];

    // Get a pointer to the codec context for the video stream
    pd->pCodecCtx = pd->pFormatCtx->streams[pd->vStreamIdx]->codec;

    // Find the decoder for the video stream
    pd->pCodec = avcodec_find_decoder(pd->pCodecCtx->codec_id);
    if(pd->pCodec == NULL) {
        printf("ERR: Unsupported codec (%d)\n", pd->pCodecCtx->codec_id);
        return -4; // Codec not found
    }

    // Open codec
    if(avcodec_open2(pd->pCodecCtx, pd->pCodec, &pd->optionsDict)<0)
        return -5; // Could not open codec

    // Allocate video frame
    pd->pFrame = avcodec_alloc_frame();

    // Allocate an AVFrame structure
    pd->pFrameRGB = avcodec_alloc_frame();
    if(pd->pFrameRGB == NULL) return -1;

    // Determine required buffer size and allocate buffer
    int numBytes = avpicture_get_size(PIX_FMT_BGR24, pd->pCodecCtx->width,
                                  pd->pCodecCtx->height);
    pd->videoBuf = (ru8 *) av_malloc(numBytes*sizeof(ru8));

    pd->sws_ctx = sws_getContext(
                pd->pCodecCtx->width,
                pd->pCodecCtx->height,
                pd->pCodecCtx->pix_fmt,
                pd->pCodecCtx->width,
                pd->pCodecCtx->height,
                PIX_FMT_BGR24,
                SWS_BILINEAR,
                NULL, NULL, NULL);

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    avpicture_fill((AVPicture *)pd->pFrameRGB, pd->videoBuf, PIX_FMT_BGR24,
                   pd->pCodecCtx->width, pd->pCodecCtx->height);


    // get stream info
    fps_num = pd->vStream->r_frame_rate.num;
    fps_den = pd->vStream->r_frame_rate.den;
    fps = 1.0*fps_num / fps_den;

    start_time = 1.0 * pd->pFormatCtx->start_time / AV_TIME_BASE;
    duration   = 1.0 * pd->pFormatCtx->duration   / AV_TIME_BASE;

    pd->vs_timebase = av_q2d(pd->vStream->time_base);

    imgW = pd->pCodecCtx->width;
    imgH = pd->pCodecCtx->height;

    // get video begining date/time
    AVDictionaryEntry* tag = NULL;
    struct tm tb;
    char *creation_time = NULL;

    tm_init(&tb);
    tsBeg = 0.0;

    tag = av_dict_get(pd->vStream->metadata, "creation_time", NULL, 0);
    if( tag != NULL ) {
        creation_time = tag->value;

        if( 0 == str2tm(tag->value, &tb) ) {
            tsBeg = 1.0*tm2time_t(&tb);
        } else {
            tsBeg = 0.0;
        }
    }

    if( 1 ) {
        printf("\nVideo File    = %s\n", vf.c_str());
        printf("    vs_timebase   = %f (%d/%d)\n", pd->vs_timebase,
                                pd->vStream->time_base.num,
                                pd->vStream->time_base.den);
        printf("    fps           = %f (%d/%d)\n", fps, fps_num, fps_den);
        printf("    start_time    = %f\n", start_time);
        printf("    duration      = %f\n", duration);
        printf("    img size      = %d x %d\n", imgW, imgH);

        if( creation_time != NULL )
            printf("    creation_time = %s\n", creation_time);

        printf("    tsBeg         = %f\n", tsBeg);
        printf("\n");
    }

    // set flags & index
    m_isOpened = 1;
    frameIdx = 0;

    tsPartBeg = tsBeg;
    tsNow     = tsPartBeg;

    m_videoFileBeg = 1;

    return 0;
}


int RVideoReader::open(const std::string& vfBase, const std::string& vfExt, int vfIdx)
{
    VideoReader_InnerData *pd = (VideoReader_InnerData*) m_priData;

    //m_isOpened = 0;
    m_vfType   = 1;

    if( m_isOpened == 0 ) {
        m_vfBase = vfBase;
        m_vfExt  = vfExt;
    } else {
        // close previous video
        pd->closeVideo();
    }
    m_vfIdx = vfIdx;

    // Open video file
    char vfFN[512];
    sprintf(vfFN, "%s_%02d.%s", vfBase.c_str(), vfIdx, vfExt.c_str());

    cout<<"Opening "<<vfFN<<endl;

    // load video information
    {
        std::string infofile = vfFN;
        infofile = infofile + "_info";
        CParamArray pa;

        pa.load(infofile);

        pa.d("Video.TimeToUTC", m_timeToUTC);
        pa.d("Video.TimeDiffGps", m_timeToGPS);
        pa.d("Video.TimeDriftCoeff", m_timeDriftCoeff);
        pa.d("Video.TimeBetween", m_extraFixCoeff);
    }

    // open input file
    if(avformat_open_input(&pd->pFormatCtx, vfFN, NULL, NULL) !=0 )
    {
        return -1; // Couldn't open file
    }

    // Retrieve stream information
    if(avformat_find_stream_info(pd->pFormatCtx, NULL) < 0)
        return -2; // Couldn't find stream information

    // Dump information about file onto standard error
    //  utils.c, line: 4001
    av_dump_format(pd->pFormatCtx, 0, vfFN, 0);

    // Find the first video stream
    pd->vStreamIdx=-1;
    for(int i=0; i<pd->pFormatCtx->nb_streams; i++)
        if(pd->pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            pd->vStreamIdx = i;
            break;
        }
    if(pd->vStreamIdx == -1)
        return -3; // Didn't find a video stream

    pd->vStream = pd->pFormatCtx->streams[pd->vStreamIdx];

    // Get a pointer to the codec context for the video stream
    pd->pCodecCtx = pd->pFormatCtx->streams[pd->vStreamIdx]->codec;

    // Find the decoder for the video stream
    pd->pCodec = avcodec_find_decoder(pd->pCodecCtx->codec_id);
    if(pd->pCodec == NULL) {
        printf("ERR: Unsupported codec (%d)\n", pd->pCodecCtx->codec_id);
        return -4; // Codec not found
    }

    // Open codec
    if(avcodec_open2(pd->pCodecCtx, pd->pCodec, &pd->optionsDict)<0)
        return -5; // Could not open codec

    // Allocate video frame
    pd->pFrame = avcodec_alloc_frame();

    // Allocate an AVFrame structure
    pd->pFrameRGB = avcodec_alloc_frame();
    if(pd->pFrameRGB == NULL) return -1;

    // Determine required buffer size and allocate buffer
    int numBytes = avpicture_get_size(PIX_FMT_BGR24, pd->pCodecCtx->width,
                                  pd->pCodecCtx->height);
    pd->videoBuf = (ru8 *) av_malloc(numBytes*sizeof(ru8));

    pd->sws_ctx = sws_getContext(
                pd->pCodecCtx->width,
                pd->pCodecCtx->height,
                pd->pCodecCtx->pix_fmt,
                pd->pCodecCtx->width,
                pd->pCodecCtx->height,
                PIX_FMT_BGR24,
                SWS_BILINEAR,
                NULL, NULL, NULL);

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    avpicture_fill((AVPicture *)pd->pFrameRGB, pd->videoBuf, PIX_FMT_BGR24,
                   pd->pCodecCtx->width, pd->pCodecCtx->height);


    // get stream info
    fps_num = pd->vStream->r_frame_rate.num;
    fps_den = pd->vStream->r_frame_rate.den;
    fps = 1.0*fps_num / fps_den;

    if( vfIdx == 0 ) {
        start_time = 1.0 * pd->pFormatCtx->start_time / AV_TIME_BASE;
        duration   = 1.0 * pd->pFormatCtx->duration   / AV_TIME_BASE;
    } else {
        duration  += 1.0 * pd->pFormatCtx->duration   / AV_TIME_BASE;
    }

    pd->vs_timebase = av_q2d(pd->vStream->time_base);

    imgW = pd->pCodecCtx->width;
    imgH = pd->pCodecCtx->height;

    // get video begining date/time
    char *creation_time = NULL;
    AVDictionaryEntry* tag = NULL;
    tag = av_dict_get(pd->vStream->metadata, "creation_time", NULL, 0);
    if( tag != NULL ) {
        creation_time = tag->value;
    }

    if( m_isOpened == 0 ) {
        struct tm tb;
        tm_init(&tb);

        if( creation_time != NULL ) {
            if( 0 == str2tm(tag->value, &tb) )
                tsBeg = 1.0*tm2time_t(&tb);
            else
                tsBeg = 0.0;
        }

        tsPartBeg = tsBeg;
        frameIdx = 0;
    } else {
        //tsPartBeg = tsNow + 1000000.0/fps;
        tsPartBeg = tsNow + m_extraFixCoeff;
        //m_PTS_initVal=
    }


    if( 1 ) {
        printf("\nVideo File    = %s\n", vfFN);
        printf("    vs_timebase   = %f (%d/%d)\n", pd->vs_timebase,
                                pd->vStream->time_base.num,
                                pd->vStream->time_base.den);
        printf("    fps           = %f (%d/%d)\n", fps, fps_num, fps_den);
        printf("    start_time    = %f\n", start_time);
        printf("    duration      = %f\n", duration);
        printf("    img size      = %d x %d\n", imgW, imgH);

        if( creation_time != NULL )
            printf("    creation_time = %s\n", creation_time);

        printf("    tsBeg         = %f\n", tsBeg);
        printf("    tsPartBeg     = %f\n", tsPartBeg);
        printf("\n");
    }

    // set flags & index
    m_isOpened = 1;
    tsNow      = tsPartBeg;

    m_videoFileBeg = 1;

    return 0;
}

int RVideoReader::close(void)
{
    // close video
    if( m_isOpened ) {
        VideoReader_InnerData *pd = (VideoReader_InnerData*) m_priData;

        pd->closeVideo();
        pd->init();

        m_isOpened = 0;
    }

    return 0;
}

int RVideoReader::read(cv::Mat& image, RVideoFrameInfo* fi, double playtime)
{
    if( playtime < 0 )
        return read(image, fi);
    else {
        int64_t timestamp = (playtime + tsPartBeg) * 1e6;

        return read(image, fi, timestamp);
    }
}

int RVideoReader::read(cv::Mat& image, RVideoFrameInfo* fi, int64_t timestamp)
{
    AVPacket    packet;
    int         frameFinished = 0;
    int         retCode = -2;

    double      pts;
    double      t_drift = 0;

    static int64_t last_timestamp = 0;

    VideoReader_InnerData *pd = (VideoReader_InnerData*) m_priData;

    // clear old contents
    image.release();

    // check video is opened
    if( !m_isOpened ) return -1;


READ_AGAIN:

    // seek to given position
    if( timestamp ) {
        int64_t threshold2jump=AV_TIME_BASE;
        timestamp = (1.0*timestamp/1e6-tsPartBeg)/pd->vs_timebase;
        int64_t jump=timestamp-last_timestamp;

        if(jump<0 || jump>threshold2jump) {
            //cout<<"jump:"<<jump<<","<<pd->vs_timebase<<endl;
            av_seek_frame(pd->pFormatCtx, pd->vStreamIdx, timestamp, AVSEEK_FLAG_FRAME);
        }
    }

    while( av_read_frame(pd->pFormatCtx, &packet) >= 0 ) {

        // Is this a packet from the video stream?
        if(packet.stream_index == pd->vStreamIdx) {
            last_timestamp = packet.pts;
            if( timestamp ) {
                if( packet.pts < timestamp ) {
                    av_free_packet(&packet);
                    continue;
                }
            }

            // Decode video frame
            avcodec_decode_video2(pd->pCodecCtx, pd->pFrame,
                                  &frameFinished, &packet);

            // Did we get a video frame?
            if( frameFinished ) {
                // Convert the image from its native format to RGB
                sws_scale(  pd->sws_ctx,
                            (uint8_t const * const *)pd->pFrame->data,
                            pd->pFrame->linesize,
                            0,
                            pd->pCodecCtx->height,
                            pd->pFrameRGB->data,
                            pd->pFrameRGB->linesize );

                //printf("[%6d] dts = %lld\n", frameIdx, packet.dts);

                if( m_videoFileBeg ) {
                    m_PTS_initVal  = 0;
                    m_videoFileBeg = 0;
                }

                pts   = pd->vs_timebase*(packet.dts - m_PTS_initVal);
                tsNow = tsPartBeg + pts;
                playedTime = tsNow - tsBeg;

                // return frame info
                if( fi != NULL ) {
                    /*
                    t_drift =playedTime/1000000.0 ;
                    printf("[%6d] t_drift = %f\n", frameIdx, t_drift);
                    */

                    fi->frameIdx  = frameIdx;
                    fi->pts       = pts;
                    fi->timestamp = tsNow + m_timeToGPS + m_timeToUTC;//(ri64)(t_drift*1000000.0);//packet.pts;//
                }

                // copy image
                {
                    image.create(imgH, imgW, CV_8UC3);
                    uint8_t *p = image.data;
                    uint8_t *pS = pd->pFrameRGB->data[0];
                    int ls = pd->pFrameRGB->linesize[0];

                    for(int i=0; i<imgH; i++) {
                        memcpy((p+i*imgW*3), (pS+i*ls), imgW*3);
                    }
                }

                frameIdx ++;
                retCode = 0;
                goto READ_RET;
            }
        }
    }

    // if no more frame can be readed then open next file
    if( 1 == m_vfType ) {
        if( 0 == open(m_vfBase, m_vfExt, ++m_vfIdx) ) {
            if(timestamp) timestamp=0;
            goto READ_AGAIN;
        }
        else m_isOpened=false;
    }


READ_RET:
    // Free the packet that was allocated by av_read_frame
    av_free_packet(&packet);

    return retCode;
}


int RVideoReader::ff(int frames)
{
    AVPacket    packet;
    int         retCode = -2;
    int         fn = 0;

    double      pts;

    VideoReader_InnerData *pd = (VideoReader_InnerData*) m_priData;

    if( !m_isOpened ) return -1;

FF_AGAIN:
    while( av_read_frame(pd->pFormatCtx, &packet) >= 0 ) {
        // Is this a packet from the video stream?
        if(packet.stream_index == pd->vStreamIdx) {
            fn ++;

            if( m_videoFileBeg ) {
                m_PTS_initVal  = 0;
                m_videoFileBeg = 0;
            }

            pts   = pd->vs_timebase*(packet.dts - m_PTS_initVal);
            tsNow = tsPartBeg + pts;
            playedTime = tsNow - tsBeg;

            frameIdx ++;

            if( fn >= frames ) {
                retCode = 0;
                goto FF_RET;
            }
        }

        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
    }

    // if no more frame can be readed then open next file
    if( 1 == m_vfType ) {
        if( 0 == open(m_vfBase, m_vfExt, ++m_vfIdx) ) {
            goto FF_AGAIN;
        }
        else m_isOpened=false;
    }

FF_RET:
    return retCode;
}

void RVideoReader::setTimeDriftCoeff(double tdc)
{
    m_timeDriftCoeff = tdc;

    VideoReader_InnerData *pd= (VideoReader_InnerData*) m_priData;
    pd->vs_timebase*=(1+m_timeDriftCoeff);
}

} // end of namespace pi

