#ifndef __VIDEOPOS_MANAGER_H__
#define __VIDEOPOS_MANAGER_H__


#include "base/types/SPtr.h"
#include "base/osa/osa++.h"
#include "hardware/Camera/Undistorter.h"
#include "hardware/Gps/POS_reader.h"
#include "VideoPOS_Transfer.h"


namespace pi {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class VideoData;
class VideoReader_Base;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class VideoPOS_Manager : public RThread
{
public:
    typedef std::deque< SPtr<VideoData> > VideoQueue;

public:
    VideoPOS_Manager(int isMaster = 1);
    virtual ~VideoPOS_Manager();

    ///
    /// \brief user derived function, called when received a video
    ///
    /// \param img          - video data
    /// \param pDM          - POS data manager
    ///
    /// \return
    ///         0           - success
    ///         other       - failed
    ///
    virtual int videoIncome_slot(VideoData *img, POS_DataManager *pDM);

    ///
    /// \brief uder derived function, when create VideoPOS_Transfer
    ///
    /// \return
    ///         vpt         - VideoPOS_Transfer object
    ///
    virtual VideoPOS_Transfer* createVPT_slot(void);


    ///
    /// \brief get Cameras
    ///
    /// \param camIn        - input camera model
    /// \param camOut       - output camera model
    /// \param undistort    - image undistort
    ///
    /// \return
    ///
    virtual int getCameras(Camera **camIn, Camera **camOut, Undistorter **undistort);

    ///
    /// \brief get VideoPOS_Transer
    ///
    /// \return VideoPOS_Transfer obj
    ///
    VideoPOS_Transfer* getVideoPOS_Transer(void) {
        return m_vpt.get();
    }

    ///
    /// \brief open video file
    ///
    /// \param videoName    - video file name or configure name
    /// \param doImgUndist  - do image undistoration
    ///
    /// \return
    ///         0           - success
    ///         other       - failed
    ///
    virtual int open(std::string videoName="", int doImgUndist=0);


    ///
    /// \brief close the video file
    ///
    /// \return
    ///         0           - success
    ///         other       - failed
    ///
    virtual int close(void);

    ///
    /// \brief seek to given offset timestamp (in second)
    ///
    /// \param dt           - time offset (second)
    ///
    /// \return
    ///         0           - success
    ///         other       - failed
    ///
    virtual int seek(double dt);


    ///
    /// \brief Thread function
    ///
    /// \param arg          - thread argument
    ///
    /// \return
    ///
    virtual int thread_func(void *arg);


    virtual int size(void);
    virtual int push(SPtr<VideoData> vd);
    virtual SPtr<VideoData> pop(void);
    virtual int clear(void);


    virtual int sendPOS(spPOSData pd);

protected:
    void syncTimeStamp(SPtr<VideoData> &vd);

protected:
    SPtr<VideoReader_Base>      m_videoReader;          ///< video reader

    SPtr<POS_DataManager>       m_pdm;                  ///< POS data manager
    SPtr<VideoPOS_Transfer>     m_vpt;                  ///< VideoPOS transfer

    VideoQueue                  m_videoQueue;           ///< video data queue
    pi::RMutex                  m_mutex;                ///< mutex
    pi::RMutex                  m_mutexVR;              ///< mutex for video reading

    double                      m_tm0, m_tm1;
    double                      m_st0, m_st1;

    double                      *m_playTS;              ///< global configure - current play timestamp (in second)
};


} // end of namespace pi

#endif // end of __VIDEOPOS_MANAGER_H__
