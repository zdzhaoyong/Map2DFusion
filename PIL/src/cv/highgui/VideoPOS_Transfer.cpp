
#include <string>

#include "base/utils/utils.h"
#include "VideoPOS_Transfer.h"


using namespace std;

namespace pi {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int VideoData::toStream(pi::RDataStream &ds)
{
    // check image is empty
    if( img.empty() ) return -1;

    // clear datastream
    ds.clear();

    // set magic number & version number
    ds.setHeader(0x83F8, 1);

    // get image data
    int w, h, c;
    ru8 *pix;

    w = img.cols;
    h = img.rows;
    c = img.channels();
    pix = img.data;

    // store all data to stream
    ds << w << h << c;
    ds << timestamp;
    ds.write(pix, w*h*c);

    return 0;
}

int VideoData::fromStream(pi::RDataStream &ds)
{
    ru32        d_magic, d_ver;

    int         w, h, c, n;
    ru8         *pix = NULL;

    // rewind to begining
    ds.rewind();

    // get magic & version number
    ds.getHeader(d_magic, d_ver);

    if( d_magic != 0x83F8 ) {
        dbg_pe("Input data magic number error! %x\n", d_magic);
        return -1;
    }

    // read data
    if( 0 != ds.read(w) )           return -2;
    if( 0 != ds.read(h) )           return -2;
    if( 0 != ds.read(c) )           return -2;
    if( 0 != ds.read(timestamp) )   return -2;

    n = w*h*c;
    if ( n == 0 ) return -3;

    if( c == 1 ) {
        img.create(h, w, CV_8UC1);
    } else if( c == 3 ) {
        img.create(h, w, CV_8UC3);
    } else {
        dbg_pe("image channel not support! c:%d\n", c);
        return -4;
    }

    pix = img.data;
    if( 0 != ds.read(pix, n) )      return -2;

    return 0;
}

VideoData& VideoData::operator = (const VideoData& vd)
{
    timestamp = vd.timestamp;
    img = vd.img.clone();

    return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int RGBDVideoData::toStream(pi::RDataStream &ds)
{
    // check image is empty
    if( img.empty() ) return -1;

    // clear datastream
    ds.clear();

    // set magic number & version number
    ds.setHeader(0x83F8, 1);

    // get image data
    int w, h, c;
    ru8 *pix;

    w = img.cols;
    h = img.rows;
    c = img.channels();
    pix = img.data;

    // store all data to stream
    ds << w << h << c;
    ds << timestamp;
    ds .write((ru8*)&asso,sizeof(AssociateData));
    ds .write(pix, w*h*c);

    return 0;
}

int RGBDVideoData::fromStream(pi::RDataStream &ds)
{
    ru32        d_magic, d_ver;

    int         w, h, c, n;
    ru8         *pix = NULL;

    // rewind to begining
    ds.rewind();

    // get magic & version number
    ds.getHeader(d_magic, d_ver);

    if( d_magic != 0x83F8 ) {
        dbg_pe("Input data magic number error! %x\n", d_magic);
        return -1;
    }

    // read data
    if( 0 != ds.read(w) )           return -2;
    if( 0 != ds.read(h) )           return -2;
    if( 0 != ds.read(c) )           return -2;
    if( 0 != ds.read(timestamp) )   return -2;
    if( 0 != ds.read((ru8*)&asso,sizeof(AssociateData))) return -2;

    n = w*h*c;
    if ( n == 0 ) return -3;

    if( c == 1 ) {
        img.create(h, w, CV_8UC1);
    } else if( c == 3 ) {
        img.create(h, w, CV_8UC3);
    } else {
        dbg_pe("image channel not support! c:%d\n", c);
        return -4;
    }

    pix = img.data;
    if( 0 != ds.read(pix, n) )      return -2;

    return 0;
}

RGBDVideoData& RGBDVideoData::operator = (const RGBDVideoData& vd)
{
    timestamp = vd.timestamp;
    img = vd.img.clone();
    img_d=vd.img_d.clone();

    return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

VideoPOS_Transfer::VideoPOS_Transfer()
{
    m_msgID = 0;

    // FIXME: please change node name & node configures here
    m_isMaster   = 0;
    m_nnMaster   = "Master";
    m_nnReceiver = "VideoPOS_RX";

    m_useNetwork = 1;

    // set global obj pointer
    SvarWithType<VideoPOS_Transfer*>::instance()["VideoPOS_Transfer.ptr"] = this;
}

VideoPOS_Transfer::~VideoPOS_Transfer()
{
    SvarWithType<VideoPOS_Transfer*>::instance()["VideoPOS_Transfer.ptr"] = NULL;
}


int VideoPOS_Transfer::setNodeNames(const std::string &nnMaster,
                                    const std::string &nnReceiver)
{
    m_nnMaster = nnMaster;
    m_nnReceiver = nnReceiver;

    return 0;
}


int VideoPOS_Transfer::start(int isMaster, int useNetwork)
{
    m_isMaster = isMaster;
    m_useNetwork = useNetwork;

    if( m_useNetwork ) {
        // start message passing system
        if( isMaster ) {
            return begin(m_nnMaster);
        } else {
            return begin(m_nnReceiver);
        }
    }

    return 0;
}

int VideoPOS_Transfer::stopClient(void)
{
    if( m_isMaster && m_useNetwork ) {
        RMessage msg;

        msg.msgType = 9;                // FIXME: 9 is kill message type
        msg.msgID = m_msgID ++;
        msg.data.write(msg.msgType);
        msg.data.write(msg.msgID);

        // check receiver is alive
        if( NULL == getNode(m_nnReceiver) ) return -3;
        sendMsg(m_nnReceiver, &msg);
    }

    return 0;
}


int VideoPOS_Transfer::recvMessageSlot(RMessage *msg)
{
    if( 1 == msg->msgType ) {
        spPOSData pd(new pi::POS_Data);

        pd->fromStream(msg->data);
        pushPOS_Data(pd);
    } else if ( 2 == msg->msgType ) {
        spVideoData vd(new VideoData);

        vd->fromStream(msg->data);
        pushVideoData(vd);
    } else if( 9 == msg->msgType ) {
        dbg_pt("received stop command!");
        exit(0);
    }

    // free the message
    delete msg;

    return 0;
}



int VideoPOS_Transfer::sendPOS(spPOSData pd)
{
    // work or not
    if( !svar.GetInt("VideoPOS_Transfer.TX.POS", 1) ) return 0;

    // push to local queue
    pushPOS_Data(pd);

    // send to remote client
    if( m_useNetwork ) {
        RMessage msg;

        // check receiver is alive
        if( NULL == getNode(m_nnReceiver) ) return -3;

        // convert POSData to message
        pd->toStream(msg.data);
        msg.msgType = 1;
        msg.msgID = m_msgID ++;

        return sendMsg(m_nnReceiver, &msg);
    }

    return 0;
}

int VideoPOS_Transfer::sendVideo(spVideoData vd)
{
    // send or not
    if( !svar.GetInt("VideoPOS_Transfer.TX.Video", 1) ) return 0;

    // push to local queue
    pushVideoData(vd);

    // use network or internal deliver
    if( m_useNetwork ) {
        RMessage msg;

        // check receiver is alive
        if( NULL == getNode(m_nnReceiver) ) return -3;

        // convert video to message
        vd->toStream(msg.data);
        msg.msgType = 2;
        msg.msgID = m_msgID ++;

        return sendMsg(m_nnReceiver, &msg);
    }

    return 0;
}


int VideoPOS_Transfer::sizePOS(void)
{
    RMutex m(&m_mutex);

    return m_queuePOS_Data.size();
}

int VideoPOS_Transfer::pushPOS_Data(spPOSData pd)
{
    RMutex m(&m_mutex);

    m_queuePOS_Data.push_back(pd);

    // remove unused data
    int qs = svar.GetInt("VideoPOS_Transfer.queueIPC.size", 5);
    if( m_queuePOS_Data.size() >  qs ) {
        int n = m_queuePOS_Data.size() - qs;
        for(int i=0; i<n; i++) m_queuePOS_Data.pop_front();
    }

    return 0;
}

spPOSData VideoPOS_Transfer::popPOS_Data(void)
{
    RMutex m(&m_mutex);
    spPOSData pd;

    if( m_queuePOS_Data.size() > 0 ) {
        pd = m_queuePOS_Data.front();
        m_queuePOS_Data.pop_front();
    }

    return pd;
}

int VideoPOS_Transfer::clearPOS(void)
{
    RMutex m(&m_mutex);

    m_queuePOS_Data.clear();

    return 0;
}


int VideoPOS_Transfer::sizeVideo(void)
{
    RMutex m(&m_mutex);

    return m_queueVideoData.size();
}

int VideoPOS_Transfer::pushVideoData(spVideoData vd)
{
    RMutex m(&m_mutex);

    m_queueVideoData.push_back(vd);

    // remove unused data
    int qs = svar.GetInt("VideoPOS_Transfer.queueIPC.size", 5);
    if( m_queueVideoData.size() >  qs ) {
        int n = m_queueVideoData.size() - qs;
        for(int i=0; i<n; i++) m_queueVideoData.pop_front();
    }

    return 0;
}

spVideoData VideoPOS_Transfer::popVideoData(void)
{
    RMutex m(&m_mutex);
    spVideoData vd;

    if( m_queueVideoData.size() > 0 ) {
        vd = m_queueVideoData.front();
        m_queueVideoData.pop_front();
    }

    return vd;
}

int VideoPOS_Transfer::clearVideo(void)
{
    RMutex m(&m_mutex);

    m_queueVideoData.clear();

    return 0;
}

} // end of namespace pi
