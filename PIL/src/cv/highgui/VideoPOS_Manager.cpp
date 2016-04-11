
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>

#define HAS_CVD

#ifdef HAS_CVD
#include <cvd/Linux/v4lbuffer.h>
#include <cvd/colourspace_convert.h>
#include <cvd/colourspaces.h>
#endif

#include "VideoReader.h"
#include "VideoPOS_Manager.h"


using namespace std;

namespace pi {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class VideoReader_Base
{
public:
    VideoReader_Base(std::string confName, int doImgUndist = 0) {
        m_confName  = confName;

        videoFile   = svar.GetString(confName + ".File", "");
        videoType   = svar.GetString(confName + ".Type", "FILE");

        FPS         = svar.GetDouble(confName + ".fps", 60);
        ImgSkip     = &svar.GetInt(confName + ".ImgSkip", 10);
        timeDelay   = &svar.GetDouble(confName + ".timeDelay", 0.0);

        camInName   = svar.GetString(confName + ".CameraInName", "GoProIdeaM960");
        camOutName  = svar.GetString(confName + ".CameraOutName", "GoProIdeaM960");

        camIn = SPtr<Camera>(GetCameraFromName(camInName));
        if( camInName == camOutName ) {
            camOut = camIn;
        } else {
            camOut = SPtr<Camera>(GetCameraFromName(camOutName));

            if( doImgUndist )
                camUndis = SPtr<Undistorter>(new Undistorter(GetCopy(camIn.get()), GetCopy(camOut.get())));
        }

        if( 0 ) {
            printf("Video:\n");
            printf("    Video.File      = %s\n", videoFile.c_str());
            printf("    Video.Type      = %s\n", videoType.c_str());
            printf("    Video.fps       = %f\n", FPS);
            printf("    Video.ImgSkip   = %d\n", ImgSkip);
            printf("    Video.timeDelay = %f\n", timeDelay);
            printf("    Video.CameraIn  = %s\n", camInName.c_str());
            printf("    Video.CameraOut = %s\n", camOutName.c_str());
        }
    }

    virtual ~VideoReader_Base() {}

    virtual int open(std::string fn="") = 0;
    virtual int close(void) = 0;

    virtual int grabImage(VideoData &img) = 0;

    virtual int seek(double dt) { return 0; }

public:
    std::string                 m_confName;
    std::string                 videoType;
    std::string                 videoFile;

    double                      FPS;
    int                         *ImgSkip;
    double                      *timeDelay;

    std::string                 camInName, camOutName;
    SPtr<Camera>                camIn, camOut;
    int                         imgUndistort;
    SPtr<Undistorter>           camUndis;
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class VideoReader_FILE : public VideoReader_Base
{
public:
    VideoReader_FILE(std::string confName, int doImgUndist = 0)
        : VideoReader_Base(confName, doImgUndist) {

    }
    virtual ~VideoReader_FILE() {
        close();
    }

    virtual int open(std::string fn="") {
        // close previous video
        if( m_reader.isOpened() ) close();

        if( fn == "" )
            return m_reader.open(videoFile);
        else
            return m_reader.open(fn);
    }

    virtual int close(void) {
        return m_reader.close();
    }

    virtual int grabImage(VideoData &img) {
        int ret = 0;

        // skip some frames
        if( *ImgSkip > 10 ) {
            ret = ff(*ImgSkip-10);

            for(int i=0; ret == 0 && i<10; i++) ret = read(img.img, &videoinfo);
        } else {
            if( *ImgSkip > 0 )
                for(int i=0; ret == 0 && i<*ImgSkip; i++) ret = read(img.img, &videoinfo);
        }
        if( ret < 0 ) return -1;

        // read the image
        ret = read(img.img, &videoinfo);
        if( ret < 0 ) return -1;

        if( img.img.empty() ) {
            //dbg_pe("get videoframe failed\n");
            return -1;
        }

        img.timestamp = videoinfo.timestamp;

        return 0;
    }

    virtual int seek(double dt) {
        cv::Mat         img;

        int ret = m_reader.read(img, &videoinfo, videoinfo.pts + dt);
        if( ret < 0 ) m_reader.close();

        return 0;
    }

    int read(cv::Mat &img, RVideoFrameInfo *vi) {
        int ret = m_reader.read(img, vi);

        if( ret < 0 ) m_reader.close();

        return ret;
    }

    int ff(int n) {
        int ret = m_reader.ff(n);

        if( ret < 0 ) m_reader.close();

        return ret;
    }


protected:
    RVideoReader        m_reader;
    RVideoFrameInfo     videoinfo;
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef HAS_CVD
class VideoReader_V4L : public VideoReader_Base
{
public:
    VideoReader_V4L(std::string confName, int doImgUndist = 0)
        : VideoReader_Base(confName, doImgUndist) {
        pvb = NULL;

        videoFile = svar.GetString(confName + ".Live.devFile", "/dev/video0");
        m_imgW = svar.GetInt(m_confName + ".Live.imgW", 1920);
        m_imgH = svar.GetInt(m_confName + ".Live.imgH", 1080);
    }

    virtual ~VideoReader_V4L() {
        if( pvb != NULL ) {
            delete pvb;
            pvb = NULL;
        }
    }


    virtual int open(std::string fn="") {
        using namespace CVD;

        m_bFirstFrame = 1;

        string  QuickCamFile;

        if( fn == "" )  QuickCamFile = videoFile;
        else            QuickCamFile = fn;

        dbg_pt("Opening V4L video: %s (%d x %d)\n",
               QuickCamFile.c_str(),
               m_imgW, m_imgH);

        ImageRef irSize = ImageRef(m_imgW, m_imgH);
        int nFrameRate =  FPS;

        // create autosave folder
        if( svar.GetInt("FastGCS.Video.AutoSave", 1) || 1 ) {
            string AutoSavePrjName = svar.GetString("FastGCS.AutoSavePrjName", "autosave");
            string dp = svar.GetString("FastGCS.DataPath", "./Data/FastGCS");
            m_autoSaveFN = path_join(dp, "video_data", AutoSavePrjName);
            path_mkdir(m_autoSaveFN.c_str());
        }

        // open device
        try {
            pvb = new V4LBuffer<yuv422>(QuickCamFile, irSize, -1, false, nFrameRate);
        } catch (...) {
            pvb = NULL;

            dbg_pe("Can not open video: %s\n", fn.c_str());
            return -1;
        }

        if( !pvb ) {
            dbg_pe("Can not open video: %s\n", fn.c_str());
            return -1;
        } else {
            return 0;
        }

        return 0;
    }

    virtual int close(void) {

    }

    virtual int grabImage(VideoData &img) {
        using namespace CVD;

        if( !pvb ) return -1;

        ImageRef irSize = pvb->size();

        Image<Rgb<byte> > imRGB;
        imRGB.resize(irSize);
        CVD::VideoFrame<yuv422> *pVidFrame = pvb->get_frame();

        //img.timestamp = pVidFrame->timestamp();
        img.timestamp = tm_getTimeStamp() + *timeDelay;
        //dbg_pt("timeStamp = %f\n", img.timestamp);

        convert_image(*pVidFrame, imRGB);
        pvb->put_frame(pVidFrame);              // release frame buffer

        cv::cvtColor(cv::Mat(irSize.y, irSize.x,
                     CV_8UC3, imRGB.data()),
                     img.img, CV_BGR2RGB);

        // save file name
        if( svar.GetInt("FastGCS.Video.AutoSave", 1) || svar.GetInt("FastGCS.Video.SaveOneShot", 0) ) {
            string imgFN = fmt::sprintf("%12.3f.png", img.timestamp);
            string imgPath = path_join(m_autoSaveFN, imgFN);
            cv::imwrite(imgPath, img.img);

            if( svar.GetInt("FastGCS.Video.SaveOneShot", 0) ) {
                dbg_pt("Save one video frame: %s", imgPath.c_str());
                svar.GetInt("FastGCS.Video.SaveOneShot", 0) = 0;
            }
        }

        return 0;
    }

protected:
    CVD::V4LBuffer<CVD::yuv422>     *pvb;
    int                             m_imgW, m_imgH;

    int                             m_bFirstFrame;      ///< first frame
    double                          m_tsBeg;            ///< timeStamp for begin

    string                          m_autoSaveFN;       ///< auto save file name
};
#else
typedef VideoReader_FILE VideoReader_V4L;
#endif


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class VideoReader_GSLAM : public VideoReader_Base
{
public:
    VideoReader_GSLAM(std::string confName, int doImgUndist = 0)
        : VideoReader_Base(confName, doImgUndist) {
    }

    virtual ~VideoReader_GSLAM() {
        close();
    }

    virtual int open(std::string fn="") {
        close();

        string vf;
        if( fn == "" ) vf = videoFile;
        else           vf = fn;

        StringArray path_name = path_split(vf);
        if(path_name.size()) PathTop=path_name[0];

        int ret = readlines(vf.c_str(), arrLines);
        if( ret != 0 )
            dbg_pe("Can't open Munich video file.\n");
        else
            dbg_pt("Open video file: %s", vf.c_str());

        return 0;
    }

    virtual int close(void) {
        arrLines.clear();
        idxLine = 0;

        return 0;
    }

    virtual int grabImage(VideoData &img) {
        double RGBTime;
        string RGBFile, line;

        if( idxLine >= arrLines.size() ) {
            return -3;
        }

        line = arrLines[idxLine++];
        idxLine += *ImgSkip;

        stringstream ss(line);
        ss>>RGBTime>>RGBFile;

        string filename=RGBFile;
        if(PathTop.size())
            filename=PathTop+"/"+RGBFile;

        if( path_exist(filename.c_str()) ) {
            img.img = cv::imread(filename,1);
            if(img.img.empty()) return -2;

            img.timestamp = RGBTime;

            return 0;
        } else {
            dbg_pe("Can't find video frame: %s", filename.c_str());
            return -3;
        }
    }

public:
    string          PathTop;
    StringArray     arrLines;
    int             idxLine;
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

VideoReader_Base* GetVideoReaderByName(std::string confName)
{
    VideoReader_Base *vr = NULL;

    std::string videoType = svar.GetString(confName + ".Type", "RTK");

    if( videoType == "RTK" ) {
        vr = new VideoReader_FILE(confName);
    } else if ( videoType == "V4L" ) {
        vr = new VideoReader_V4L(confName);
    } else if ( videoType == "GSLAM" ) {
        vr = new VideoReader_GSLAM(confName);
    }

    return vr;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief VideoPOS_Manager::VideoPOS_Manager
/// \param runType      - program running type
///                         AllInOne       - All function intergeted into FGCS
///                         Distributed    - Functions are distributed to process
///                         Standalone     - Run single program
/// \param isMaster     - is master node or client node
///                         0 - client node
///                         1 - master node
///
VideoPOS_Manager::VideoPOS_Manager(int isMaster)
{
    SvarWithType<VideoPOS_Manager*>::instance()["VideoPOS_Manager.ptr"] = this;

    m_vpt = SPtr<VideoPOS_Transfer>(createVPT_slot());
    m_pdm = SPtr<POS_DataManager>(new POS_DataManager);

    // get configures
    string nodeName = "Master";

    if( isMaster) {
        nodeName = svar.GetString("VPM.serverName", "Master");
        m_vpt->setMasterNodeName(nodeName);
        m_vpt->start(1);
    } else {
        nodeName = svar.GetString("VPM.clientName", "VideoPOS_RX");
        m_vpt->setReceiverNodeName(nodeName);
        m_vpt->start(0);
    }
}

VideoPOS_Manager::~VideoPOS_Manager()
{
    close();

    m_vpt->stop();

    SvarWithType<VideoPOS_Manager*>::instance()["VideoPOS_Manager.ptr"] = NULL;
}



int VideoPOS_Manager::videoIncome_slot(VideoData *img, POS_DataManager *pDM)
{
    return 0;
}


VideoPOS_Transfer* VideoPOS_Manager::createVPT_slot(void)
{
    return (new VideoPOS_Transfer());
}


int VideoPOS_Manager::getCameras(Camera **camIn, Camera **camOut, Undistorter **undistort)
{
    if( m_videoReader.get() ) {
        *camIn  = m_videoReader->camIn.get();
        *camOut = m_videoReader->camOut.get();
        *undistort = m_videoReader->camUndis.get();

        return 0;
    }

    return -1;
}


int VideoPOS_Manager::thread_func(void *arg)
{
    int r;

    m_playTS = &svar.GetDouble("IPC.PlayTS", -1.0);
    int *undisType = &svar.GetInt("VideoPOS_Manager.ImageUndistortType", 0);

    Undistorter *undis = NULL;
    if( m_videoReader.get() ) undis = m_videoReader->camUndis.get();

    int &pause=svar.GetInt("Pause", 0);

    while( getAlive() ) {
        if( NULL == m_videoReader.get() || pause) {
            tm_sleep(80);
            continue;
        }

        // get a video frame
        spVideoData vd(new VideoData);
        {
            pi::RMutex m(&m_mutexVR);
            r = m_videoReader->grabImage(*vd);
        }

        if( 0 != r ) {
            tm_sleep(80);
            continue;
        }

        // undistort input image
        if( undis ) {
            cv::Mat img;

            if( undisType == 0 ) undis->undistortFast(vd->img, img);
            else                 undis->undistort(vd->img, img);

            vd->img = img;
        }

        // sync to given time stamp or keep playspeed
        if( svar.GetInt("VideoPOS_Manager.syncTimeStamp", 0) )
            syncTimeStamp(vd);

        // call video income slot and then send current video frame to network
        videoIncome_slot(vd.get(), m_pdm.get());
        m_vpt->sendVideo(vd);

        // push to image queue for FGCS use
        push(vd);
    }

    return 0;
}

void VideoPOS_Manager::syncTimeStamp(SPtr<VideoData> &vd)
{
    if( *m_playTS < 0 ) {
        // sync to actual time or GPS time
        if( m_tm0 == -1 ) {
            m_tm0 = vd->timestamp;
            m_st0 = tm_getTimeStamp();
        } else {
            m_tm1 = vd->timestamp;
            m_st1 = tm_getTimeStamp();

            double tm_dt = m_tm1 - m_tm0;
            if( tm_dt > 1e-3 ) {
                if( tm_dt < 10.0 && tm_dt > 1e-6 ) {
                    ri64 dt = 1e6*(tm_dt - (m_st1 - m_st0));

                    if( dt > 1 ) tm_sleep_us(dt);

                    m_tm0 = m_tm1;
                    m_st0 = m_st1;
                }

                // reset begin time
                if( tm_dt >= 10.0 ) {
                    m_tm0 = m_tm1;
                    m_st0 = m_st1;
                }
            }
        }
    } else {
        double dt = vd->timestamp - *m_playTS;

        if( dt < -10 ) {
            dbg_pt("seek video poistion: %f s", -dt);
            m_videoReader->seek(-dt);
            return;
        }

        if( dt > 0 ) {
            // sync to given PTS
            while ( 1 ) {
                dt = vd->timestamp - *m_playTS;
                if( dt > 0 ) tm_sleep_us(2000);
                else break;
            }
        }
    }
}

int VideoPOS_Manager::open(std::string videoName, int doImgUndist)
{
    int ret = -1;

    m_tm0 = -1;
    m_st0 = -1;

    m_vpt->clearPOS();
    m_vpt->clearVideo();

    VideoReader_Base *vr = NULL;

    // try to open project configure file
    if( path_exist(videoName.c_str()) ) {
        svar.ParseFile(videoName);

        // if project file can open, then the video must be file
        vr = new VideoReader_FILE("Video", doImgUndist);
        m_videoReader = SPtr<VideoReader_Base>(vr);

        // open video
        ret = m_videoReader->open();
    } else {
        string vt = svar.GetString(videoName + ".Type", "NONE");

        if( vt == "V4L" ) {
            vr = new VideoReader_V4L(videoName, doImgUndist);
        } else if( vt == "RTK" ) {
            vr = new VideoReader_FILE(videoName, doImgUndist);
        } else if( vt == "GSLAM" ) {
            vr = new VideoReader_GSLAM(videoName, doImgUndist);
        } else {
            return -1;
        }

        m_videoReader = SPtr<VideoReader_Base>(vr);
        if( m_videoReader.get() == NULL ) return -1;

        ret = m_videoReader->open();
    }

    // start reading thread
    if( ret == 0 ) start();
    else cerr<<"Failed to open video!\n";

    return ret;
}

int VideoPOS_Manager::close(void)
{
    // stop thread
    setAlive(0);
    wait(20);
    kill();

    m_vpt->clearPOS();
    m_vpt->clearVideo();

    return 0;
}

int VideoPOS_Manager::seek(double dt)
{
    pi::RMutex m(&m_mutexVR);

    m_videoReader->seek(dt);

    return 0;
}



int VideoPOS_Manager::size(void)
{
    RMutex m(&m_mutex);

    return m_videoQueue.size();
}

int VideoPOS_Manager::push(SPtr<VideoData> vd)
{
    RMutex m(&m_mutex);

    m_videoQueue.push_back(vd);

    // FIXME: if FGCS do not use the image data then clear it
    int queueSize = svar.GetInt("VideoPOS_Manager.queueSize", 20);
    if( m_videoQueue.size() > queueSize ) {
        for(int i=0; i<m_videoQueue.size() - queueSize; i++) m_videoQueue.pop_front();
    }

    return 0;
}

SPtr<VideoData> VideoPOS_Manager::pop(void)
{
    RMutex m(&m_mutex);

    SPtr<VideoData> d;

    if( m_videoQueue.size() > 0 ) {
        d = m_videoQueue.front();
        m_videoQueue.pop_front();
    }

    return d;
}

int VideoPOS_Manager::clear(void)
{
    RMutex m(&m_mutex);

    m_videoQueue.clear();

    return 0;
}

int VideoPOS_Manager::sendPOS(spPOSData pd)
{
    m_pdm->addData(*pd);
    m_vpt->sendPOS(pd);

    return 0;
}


} // end of namespace pi
