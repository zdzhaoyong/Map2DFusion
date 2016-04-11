#ifndef __VIDEOPOS_TRANSMITTER_H__
#define __VIDEOPOS_TRANSMITTER_H__

#include <string>
#include <deque>

#include <opencv2/highgui/highgui.hpp>

#include <base/types/SPtr.h>
#include <base/types/SE3.h>
#include <base/osa/osa++.h>
#include <network/MessagePassing.h>
#include <hardware/Gps/POS_reader.h>


namespace pi {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class VideoData
{
public:
    VideoData() { timestamp = 0.0; }
    virtual ~VideoData() { timestamp = 0.0; }

    virtual std::string DataType(){return "VideoData";}

    int width() { return img.cols; }
    int height() { return img.rows; }

    virtual int toStream(pi::RDataStream &ds);
    virtual int fromStream(pi::RDataStream &ds);

    int toMessage(pi::RMessage &msg);
    int fromMessage(pi::RMessage &msg);

    virtual VideoData& operator = (const VideoData& vd);

public:
    cv::Mat         img;                            ///< image data
    double          timestamp;                      ///< time stamp in second
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct AssociateData
{
    double timestamp,timestampRGB,timestampDepth;
    double x,y,z,rx,ry,rz,w;
    std::string depthfile,rgbfile;
    pi::SE3d    se3;

    inline friend std::istream& operator >>(std::istream& is,AssociateData& rhs)
    {
        is>>rhs.timestamp>>rhs.x>>rhs.y>>rhs.z
         >>rhs.rx>>rhs.ry>>rhs.rz>>rhs.w;
        is>>rhs.timestampDepth>>rhs.depthfile;
        is>>rhs.timestampRGB>>rhs.rgbfile;
        rhs.se3=pi::SE3d(rhs.x,rhs.y,rhs.z,rhs.rx,rhs.ry,rhs.rz,rhs.w);
        return is;
    }

    inline friend std::ostream& operator <<(std::ostream& os,const AssociateData& rhs)
    {
        os<<rhs.timestamp<<" "<<rhs.x<<" "<<rhs.y<<" "<<rhs.z<<" ";
        os<<rhs.rx<<" "<<rhs.ry<<" "<<rhs.rz<<" "<<rhs.w<<" ";
        os<<rhs.timestampDepth<<" "<<rhs.depthfile<<" ";
        os<<rhs.timestampRGB<<" "<<rhs.rgbfile<<"\n";
        return os;
    }
};

class RGBDVideoData: public VideoData
{
public:
    RGBDVideoData() { timestamp = 0.0; }
    virtual ~RGBDVideoData() { timestamp = 0.0; }

    virtual std::string DataType(){return "RGBDVideoData";}

    int width() { return img.cols; }
    int height() { return img.rows; }

    virtual int toStream(pi::RDataStream &ds);
    virtual int fromStream(pi::RDataStream &ds);

    int toMessage(pi::RMessage &msg);
    int fromMessage(pi::RMessage &msg);

    virtual RGBDVideoData& operator = (const RGBDVideoData& vd);

public:
    cv::Mat         img_d;                            ///< image data
    AssociateData   asso;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef SPtr<POS_Data>          spPOSData;
typedef SPtr<VideoData>         spVideoData;


class VideoPOS_Transfer : public RMessagePassing
{
public:
    typedef std::deque<spPOSData>       Queue_POSData;
    typedef std::deque<spVideoData>     Queue_VideoData;


public:
    VideoPOS_Transfer();
    virtual ~VideoPOS_Transfer();

    int sendPOS(spPOSData pd);
    int sendVideo(spVideoData vd);


    int sizePOS(void);
    int pushPOS_Data(spPOSData pd);
    spPOSData popPOS_Data(void);
    int clearPOS(void);

    int sizeVideo(void);
    int pushVideoData(spVideoData vd);
    spVideoData popVideoData(void);
    int clearVideo(void);

    ///
    /// \brief set node names
    ///
    /// \param nnMaster     - master node name
    /// \param nnSlive      - slive node name
    ///
    /// \return
    ///
    virtual int setNodeNames(const std::string &nnMaster, const std::string &nnReceiver);

    virtual int setMasterNodeName(const std::string &nnMaster) {
        m_nnMaster = nnMaster;

        return 0;
    }

    virtual int setReceiverNodeName(const std::string &nnReceiver) {
        m_nnReceiver = nnReceiver;

        return 0;
    }

    ///
    /// \brief message passing system startup
    ///             Node information will be load through Svar
    ///
    /// \param isMaster     - node type
    ///                         1 - Master node, nodename = "Master" (default)
    ///                         0 - RX node, nodename = "" (default)
    ///
    /// \param useNetwork   - use network to transfer data or not
    ///
    /// \note Please change node configure through system svar variable
    ///
    virtual int start(int isMaster, int useNetwork=1);

    ///
    /// \brief stop client
    ///
    /// \return
    ///
    virtual int stopClient(void);

    ///
    /// \brief receiving message slot
    ///
    /// \param msg - received message
    ///
    /// \return
    ///
    virtual int recvMessageSlot(RMessage *msg);

protected:
    Queue_POSData       m_queuePOS_Data;
    Queue_VideoData     m_queueVideoData;

    RMutex              m_mutex;

    int                 m_isMaster;                     ///< master node or not
    std::string         m_nnMaster;                     ///< node name of master
    std::string         m_nnReceiver;                   ///< node name of receiver
    int                 m_useNetwork;                   ///< use network to transfer data or not

    int                 m_msgID;                        ///< message index
};


} // end of namespace pi

#endif // end of __VIDEOPOS_TRANSMITTER_H__
