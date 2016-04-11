/******************************************************************************

  Robot Toolkit ++ (RTK++)

  Copyright (c) 2007-2013 Shuhui Bu <bushuhui@nwpu.edu.cn>
  http://www.adv-ci.com

  ----------------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <string>
#include <iostream>
#include <set>

#include "base/utils/utils.h"
#include "Socket++.h"


namespace pi {



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class RSocket_PrivateData
{
public:
    RSocket_PrivateData() {
        memset(&m_addr, 0, sizeof(sockaddr_in));
        memset(&m_addrClient, 0, sizeof(sockaddr_in));
    }

    ~RSocket_PrivateData() {
        memset(&m_addr, 0, sizeof(sockaddr_in));
        memset(&m_addrClient, 0, sizeof(sockaddr_in));
    }

public:
    sockaddr_in     m_addr;
    sockaddr_in     m_addrClient;
};



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

RSocket::RSocket() : m_sock ( -1 )
{
    RSocket_PrivateData *pd = new RSocket_PrivateData;
    m_priData = pd;

    m_socketType = SOCKET_TCP;
    m_server = 0;

    m_maxConnections = 500;
    m_maxHostname    = 1024;
}

RSocket::~RSocket()
{
    if ( isOpened() )
        ::close(m_sock);

    RSocket_PrivateData *pd = (RSocket_PrivateData*) m_priData;
    delete pd;
    m_priData = NULL;
}

int RSocket::create(void)
{
    // create socket
    if( m_socketType == SOCKET_TCP )
        m_sock = socket(AF_INET, SOCK_STREAM, 0);
    else
        m_sock = socket(AF_INET, SOCK_DGRAM, 0);

    // check socket created or not
    if ( !isOpened() )
        return -1;

    // set SO_REUSEADDR on
    if( m_socketType == SOCKET_TCP ||
        //(m_socketType == SOCKET_UDP_MULTICAST && !m_server) )  {
        m_socketType == SOCKET_UDP_MULTICAST ) {
        setReuseAddr(1);
    }

    return 0;
}

int RSocket::close(void)
{
    if ( isOpened() )
        ::close(m_sock);

    m_sock = -1;
    m_server = 0;

    return 0;
}

int RSocket::bind(int port)
{
    RSocket_PrivateData *pd = (RSocket_PrivateData*) m_priData;

    if ( !isOpened() ) {
        return -1;
    }

    pd->m_addr.sin_family       = AF_INET;
    pd->m_addr.sin_addr.s_addr  = htonl(INADDR_ANY);
    pd->m_addr.sin_port         = htons(port);

    int ret = ::bind(m_sock, (struct sockaddr*) &(pd->m_addr), sizeof(pd->m_addr));
    if ( ret == -1 ) {
        return -2;
    }

    return 0;
}


int RSocket::listen(void)
{
    if ( !isOpened() ) {
        return -1;
    }

    int listen_return = ::listen(m_sock, m_maxConnections);

    if ( listen_return == -1 ) {
        return -2;
    }

    return 0;
}


int RSocket::accept(RSocket& new_socket)
{
    RSocket_PrivateData *pd = (RSocket_PrivateData*) new_socket.m_priData;

    int addr_length = sizeof(pd->m_addrClient);
    new_socket.m_sock = ::accept(m_sock,
                                 (sockaddr*) &(pd->m_addrClient),
                                 (socklen_t*) &addr_length);

    if ( new_socket.m_sock <= 0 )
        return -1;
    else
        return 0;
}



int RSocket::send(uint8_t *dat, int len)
{
    if( m_socketType == SOCKET_TCP ) {
        return ::send(m_sock, dat, len, MSG_NOSIGNAL);
    } else if( m_socketType == SOCKET_UDP ) {
        RSocket_PrivateData *pd = (RSocket_PrivateData*) m_priData;

        if( m_server ) {
            return ::sendto(m_sock, dat, len, 0,
                          (struct sockaddr *) &(pd->m_addrClient), sizeof(pd->m_addrClient));
        } else {
            return ::sendto(m_sock, dat, len, 0,
                          (struct sockaddr *) &(pd->m_addr), sizeof(pd->m_addr));
        }
    } else if( m_socketType == SOCKET_UDP_MULTICAST ) {
        RSocket_PrivateData *pd = (RSocket_PrivateData*) m_priData;

        return ::sendto(m_sock, dat, len, 0,
                        (struct sockaddr *) &(pd->m_addr), sizeof(pd->m_addr));
    }
}

int RSocket::send (std::string &s)
{
    return send((uint8_t*)s.c_str(), s.size());
}

int RSocket::send(RDataStream &ds)
{
    return send(ds.data(), ds.size());
}


int RSocket::recv(uint8_t *dat, int len)
{
    RSocket_PrivateData *pd = (RSocket_PrivateData*) m_priData;

    if( m_socketType == SOCKET_TCP )
        return ::recv(m_sock, dat, len, 0);
    else if( m_socketType == SOCKET_UDP ) {
        if( m_server ) {
            socklen_t addrLen = sizeof(pd->m_addrClient);
            return ::recvfrom(m_sock, dat, len, 0,
                              (struct sockaddr *) &(pd->m_addrClient), &addrLen);
        } else {
            return ::recvfrom(m_sock, dat, len, 0, NULL, NULL);
        }
    } else if( m_socketType == SOCKET_UDP_MULTICAST ) {
        socklen_t addrLen = sizeof(pd->m_addr);
        return ::recvfrom(m_sock, dat, len, 0,
                          (struct sockaddr *) &(pd->m_addr), &addrLen);
    }
}

int RSocket::recv(std::string& s, int maxLen)
{
    char *buf;
    int status = 0;

    buf = new char[maxLen + 1];
    memset(buf, 0, maxLen + 1);
    s = "";

    status = recv((uint8_t*) buf, maxLen);
    if ( status > 0 ) s = buf;

    delete [] buf;
    
    return status;
}

int RSocket::recv(RDataStream &ds)
{
    ru8     buf[32];
    ru32    header_len, ds_magic, ds_ver, ds_size, ds_size2;
    int     ret;

    // read header
    header_len = 2*sizeof(ru32);
    ret = recv_until(buf, header_len);
    if( ret < header_len ) return -1;

    // get magic, ver, size
    datastream_get_header(buf, ds_magic, ds_ver);
    ds_size = datastream_get_length(buf);

    // resize DataStream
    ds.setHeader(ds_magic, ds_ver);
    ds.resize(ds_size);

    // read contents
    ds_size2 = ds_size - header_len;
    ret = recv_until(ds.data()+header_len, ds_size2);
    if( ret < ds_size2 ) return -1;

    // rewind position index
    ds.rewind();

    return 0;
}

int RSocket::recv_until(uint8_t *dat, int len)
{
    uint8_t     *p;
    int         ret, read, readed = 0;

    p    = dat;
    read = len;

    while(1) {
        ret = recv(p, read);
        if( ret < 0 ) return ret;

        readed += ret;
        p      += ret;

        if( readed >= len ) return readed;

        read = len - readed;
    }

    return -1;
}


int RSocket::connect(std::string host, int port)
{
    uint32_t addr;

    inet_pton(AF_INET, host.c_str(), &addr);
    if ( errno == EAFNOSUPPORT )
        return -1;

    return connect(addr, port);
}


int RSocket::connect(uint32_t addr, int port)
{
    RSocket_PrivateData *pd = (RSocket_PrivateData*) m_priData;

    if ( ! isOpened() ) return -1;

    pd->m_addr.sin_family       = AF_INET;
    pd->m_addr.sin_port         = htons(port);
    pd->m_addr.sin_addr.s_addr  = addr;

    if( m_socketType == SOCKET_TCP ) {
        int status = ::connect(m_sock, (sockaddr *) &(pd->m_addr), sizeof(pd->m_addr) );

        if ( status == 0 )
            return 0;
        else
            return -3;
    } else {
        return 0;
    }
}

int RSocket::startServer(int port, RSocketType t)
{
    m_socketType = t;
    m_server = 1;

    if ( 0 != create() ) {
        return -1;
    }

    if ( 0 != bind (port) ) {
        return -2;
    }

    if( m_socketType == SOCKET_TCP ) {
        if ( 0 != listen() ) return -3;
    }

    return 0;
}

int RSocket::startServer(const std::string& addr, int port, RSocketType t)
{
    m_socketType = t;
    m_server = 1;

    if ( 0 != create() ) {
        return -1;
    }

    if( t != SOCKET_UDP_MULTICAST ) {
        if ( 0 != bind (port) ) {
            return -2;
        }
    } else {
        RSocket_PrivateData *pd = (RSocket_PrivateData*) m_priData;

        memset(&(pd->m_addr), 0, sizeof(pd->m_addr));

        pd->m_addr.sin_family       = AF_INET;
        pd->m_addr.sin_addr.s_addr  = inet_addr(addr.c_str());
        pd->m_addr.sin_port         = htons(port);
    }

    if( m_socketType == SOCKET_TCP ) {
        if ( 0 != listen() ) return -3;
    }

    return 0;
}


int RSocket::startClient(std::string host, int port, RSocketType t)
{
    uint32_t addr;

    inet_pton(AF_INET, host.c_str(), &addr);
    if ( errno == EAFNOSUPPORT )
        return -1;

    addr = inet_addr(host.c_str());

    return startClient(addr, port, t);
}

int RSocket::startClient(uint32_t addr, int port, RSocketType t)
{
    m_socketType = t;
    m_server = 0;

    if ( 0 != create() ) {
        return -1;
    }

    if( t == SOCKET_UDP_MULTICAST ) {
        RSocket_PrivateData *pd = (RSocket_PrivateData*) m_priData;

        memset(&(pd->m_addr), 0, sizeof(pd->m_addr));

        pd->m_addr.sin_family       = AF_INET;
        pd->m_addr.sin_addr.s_addr  = htonl(INADDR_ANY);
        pd->m_addr.sin_port         = htons(port);

        // bind to receive address
        if ( ::bind(m_sock, (struct sockaddr *) &(pd->m_addr), sizeof(pd->m_addr)) < 0 ) {
            dbg_pe("Failed to bind UDP port [%d]", port);
            return -2;
        }

        // use setsockopt() to request that the kernel join a multicast group
        struct ip_mreq mreq;
        mreq.imr_multiaddr.s_addr = addr;
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);
        if (setsockopt(m_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                       &mreq, sizeof(mreq)) < 0) {
            dbg_pe("Failed to setsockopt for setting join a multicast group");
            return -3;
        }
    } else {
        if ( 0 != connect(addr, port) ) {
            return -2;
        }
    }

    return 0;
}


int RSocket::getMyAddress(RSocketAddress &a)
{
    RSocket_PrivateData *pd = (RSocket_PrivateData*) m_priData;

    a.port      = ntohs(pd->m_addr.sin_port);
    a.addr_inet = ntohl(pd->m_addr.sin_addr.s_addr);
    inet_ntop(AF_INET, &(pd->m_addr.sin_addr),
              a.addr_str, INET_ADDRSTRLEN);
    a.type = m_socketType;

    return 0;
}

int RSocket::getClientAddress(RSocketAddress &a)
{
    RSocket_PrivateData *pd = (RSocket_PrivateData*) m_priData;

    a.port      = ntohs(pd->m_addrClient.sin_port);
    a.addr_inet = ntohl(pd->m_addrClient.sin_addr.s_addr);
    inet_ntop(AF_INET, &(pd->m_addrClient.sin_addr),
              a.addr_str, INET_ADDRSTRLEN);
    a.type = m_socketType;

    return 0;
}

int RSocket::setNonBlocking(int nb)
{
    int opts;

    opts = fcntl(m_sock, F_GETFL);

    if ( opts < 0 ) {
        return -1;
    }

    if ( nb )
        opts = opts | O_NONBLOCK;
    else
        opts = opts & ~O_NONBLOCK;

    fcntl(m_sock, F_SETFL, opts);

    return 0;
}

int RSocket::setReuseAddr(uint32_t reuse)
{
    int ret = setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR,
                         (const char*) &reuse, sizeof(reuse));
    if( ret == -1 ) {
        dbg_pe("setsockopt failed");
    }

    return ret;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// how to change kernel UDP receiving buffer size
///   http://lcm.googlecode.com/svn-history/r452/www/reference/lcm/multicast-setup.html#table:kernel-udp-buffer-resizing

#define NETTRANSFER_PACKET_HEADER_SIZE      64
#define NETTRANSFER_PACKET_DATASIZE         32768
#define NETTRANSFER_PACKET_MAXSIZE          (NETTRANSFER_PACKET_HEADER_SIZE*2+NETTRANSFER_PACKET_DATASIZE)

#define NETTRANSFER_PACKET_MAGICNUM         0xA55AA55AA55AA55A


int parsePacketHeader(uint8_t *dat, uint64_t len,
                      uint64_t &magicNum,
                      uint64_t &msgID,
                      uint64_t &msgSize,
                      uint64_t &packN,
                      uint64_t &packIdx,
                      uint64_t &packLen,
                      uint64_t &CRC)
{
    uint64_t *p = (uint64_t*) dat;

    if( len < NETTRANSFER_PACKET_HEADER_SIZE ) return -1;

    magicNum    = convByteOrder_n2h(*(p++));
    msgID       = convByteOrder_n2h(*(p++));
    msgSize     = convByteOrder_n2h(*(p++));
    packN       = convByteOrder_n2h(*(p++));
    packIdx     = convByteOrder_n2h(*(p++));
    packLen     = convByteOrder_n2h(*(p++));
    CRC         = convByteOrder_n2h(*(p++));

    // check length
    if( len != packLen + NETTRANSFER_PACKET_HEADER_SIZE ) return -2;

    // check current packet CRC
    uint8_t *pu = dat + NETTRANSFER_PACKET_HEADER_SIZE;
    uint64_t crcV = 0;
    for(int i=0; i<packLen; i++) crcV += *(pu++);
    if( crcV != CRC ) return -3;

    return 0;
}

int generatePacket(uint8_t *dat, uint64_t len,
                   uint8_t *datOut, uint64_t *lenOut,
                   uint64_t &msgID,
                   uint64_t &packN,
                   uint64_t &packIdx)
{
    uint64_t packLen;

    // determin packLen
    if( len > NETTRANSFER_PACKET_DATASIZE ) {
        uint64_t lenRem = len - packIdx*NETTRANSFER_PACKET_DATASIZE;

        packLen = NETTRANSFER_PACKET_DATASIZE;
        if( lenRem <= NETTRANSFER_PACKET_DATASIZE ) packLen = lenRem;
    } else {
        packLen = len;
    }

    // output buffer size
    *lenOut = NETTRANSFER_PACKET_HEADER_SIZE + packLen;

    // copy and calculate CRC
    uint8_t *pDat    = dat + packIdx*NETTRANSFER_PACKET_DATASIZE;
    uint8_t *pDatDst = datOut + NETTRANSFER_PACKET_HEADER_SIZE;
    uint64_t crcV = 0;
    for(int i=0; i<packLen; i++) {
        crcV += pDat[i];
        pDatDst[i] = pDat[i];
    }

    // generate header
    uint64_t *p = (uint64_t*) datOut;
    *(p++) = convByteOrder_h2n(NETTRANSFER_PACKET_MAGICNUM);
    *(p++) = convByteOrder_h2n(msgID);
    *(p++) = convByteOrder_h2n(len);
    *(p++) = convByteOrder_h2n(packN);
    *(p++) = convByteOrder_h2n(packIdx);
    *(p++) = convByteOrder_h2n(packLen);
    *(p++) = convByteOrder_h2n(crcV);
    *(p++) = convByteOrder_h2n(0);

    return 0;
}


class NetTransfer_Packet
{
public:
    NetTransfer_Packet() {
        dat = NULL;
        msg_correct = 0;
    }

    ~NetTransfer_Packet() {
        if( dat ) delete [] dat;
        dat = NULL;
    }

    int inputData(uint8_t *datBuf, uint32_t len, uint64_t packIdx) {
        bitmap.insert(packIdx);

        uint8_t *p = dat + packIdx*NETTRANSFER_PACKET_DATASIZE;
        memcpy(p, datBuf, len);

        if( bitmap.size() == msg_packN )
            return 1;
        else
            return 0;
    }

    int malloc(void) {
        if( dat ) delete [] dat;

        dat = new uint8_t[NETTRANSFER_PACKET_DATASIZE*msg_packN];
        return 0;
    }

public:
    uint8_t                     *dat;

    uint64_t                    msg_magicNum;
    uint64_t                    msg_id;
    uint64_t                    msg_len;
    uint64_t                    msg_packN;
    uint64_t                    msg_packIdx;

    int                         msg_correct;
    std::set<uint64_t>          bitmap;
};


typedef std::map<uint64_t, NetTransfer_Packet*> PacketPool_Map;

class NetTransfer_PacketPool
{
public:
    NetTransfer_PacketPool() {}
    ~NetTransfer_PacketPool() {}

    int pushBuffer(uint8_t *dat, uint32_t len) {
        uint64_t magicNum   = 0;
        uint64_t msgID      = 0;
        uint64_t msgSize    = 0;
        uint64_t packN      = 0;
        uint64_t packIdx    = 0;
        uint64_t packLen    = 0;
        uint64_t CRC        = 0;

        // parse received data
        parsePacketHeader(dat, len,
                          magicNum, msgID, msgSize, packN, packIdx, packLen, CRC);


        NetTransfer_Packet *pack = NULL;
        PacketPool_Map::iterator it = m_recvBuff.find(msgID);

        if( it == m_recvBuff.end() ) {
            pack = new NetTransfer_Packet;
            pack->msg_magicNum  = magicNum;
            pack->msg_id        = msgID;
            pack->msg_len       = msgSize;
            pack->msg_packN     = packN;
            pack->msg_packIdx   = packIdx;

            pack->malloc();
        } else {
            pack = it->second;
        }

        // insert to packet
        int f = pack->inputData(dat+NETTRANSFER_PACKET_HEADER_SIZE, packLen, packIdx);        
        if( f ) {
            // insert to packet queue
            {
                RMutex m(&m_mutex);
                m_packQueue.push_back(pack);
            }

            if( it != m_recvBuff.end() ) m_recvBuff.erase(it);

            m_sem.signal();
        } else {
            m_recvBuff.insert(std::make_pair(msgID, pack));
        }

        return 0;
    }

    NetTransfer_Packet* getPack(void) {
        m_sem.wait();

        {
            RMutex m(&m_mutex);

            if( m_packQueue.size() > 0 ) {
                NetTransfer_Packet *p = m_packQueue.front();
                m_packQueue.pop_front();

                return p;
            }

            return NULL;
        }
    }


protected:
    PacketPool_Map                      m_recvBuff;
    std::deque<NetTransfer_Packet*>     m_packQueue;

    RSemaphore                          m_sem;
    RMutex                              m_mutex;
};



NetTransfer_UDP::NetTransfer_UDP()
{
    m_isServer      = 0;
    m_isConnected   = 0;
    m_port          = 30000;
    m_msgID         = 0;

    m_priData       = NULL;
}

NetTransfer_UDP::~NetTransfer_UDP()
{
    close();
}

int NetTransfer_UDP::thread_func(void *arg)
{
    int     ret;

    uint8_t *buf;
    int     bufSize;

    NetTransfer_PacketPool *pp = (NetTransfer_PacketPool*) m_priData;

    // create receiving buffer
    bufSize = NETTRANSFER_PACKET_MAXSIZE;
    buf = new uint8_t[bufSize];

    // loop until stop
    while( getAlive() ) {
        ret = m_socket.recv(buf, bufSize);

        if( ret < 0 ) break;
        else if( ret == 0 ) tm_sleep(1);
        else pp->pushBuffer(buf, ret);
    }

    delete [] buf;

    return 0;
}


int NetTransfer_UDP::open(int isServer, int port, const std::string &addr, RSocketType st)
{
    int ret = -1;

    if( m_isConnected ) return ret;

    m_socketType    = st;
    m_isServer      = isServer;
    m_addr          = addr;
    m_port          = port;
    m_msgID         = 0;
    m_isConnected   = 0;


    // create packet pool object
    {
        if( m_priData != NULL ) {
            NetTransfer_PacketPool *pp = (NetTransfer_PacketPool*) m_priData;
            delete pp;
        }

        NetTransfer_PacketPool *pp = new NetTransfer_PacketPool;
        m_priData = pp;
    }

    if( m_socketType == SOCKET_UDP ) {
        if( m_isServer ) {
            ret = m_socket.startClient(m_addr, m_port, SOCKET_UDP);
        } else {
            // start UDP server
            ret = m_socket.startServer(m_port, SOCKET_UDP);

            // start receiving thread
            if( ret == 0 ) start();
        }
    } else if( m_socketType == SOCKET_UDP_MULTICAST ) {
        if( m_isServer ) {
            ret = m_socket.startServer(addr, port, SOCKET_UDP_MULTICAST);
        } else {
            ret = m_socket.startClient(addr, port, SOCKET_UDP_MULTICAST);

            // start receiving thread
            if( ret == 0 ) start();
        }
    }

    if( ret == 0 )
        m_isConnected = 1;

    return ret;
}

int NetTransfer_UDP::close(void)
{
    if( !m_isConnected ) return -1;

    // stop receiving
    if( !m_isServer ) {
        setAlive(0);
        wait(20);
        kill();
    }

    // close socket
    m_socket.close();
    m_isConnected = 0;
    m_isServer = 0;

    // free packet pool
    if( m_priData != NULL ) {
        NetTransfer_PacketPool *pp = (NetTransfer_PacketPool*) m_priData;
        delete pp;
        m_priData = NULL;
    }

    return 0;
}

int NetTransfer_UDP::send(uint8_t *dat, int len)
{
    uint8_t     *datOut;
    uint64_t    lenOut;
    uint64_t    msgID;
    uint64_t    packN;
    uint64_t    packIdx;

    int         ret = -1;
    int         nSend = 0;

    if( !m_isConnected ) return -1;
    if( len == 0 ) return 0;

    {
        RMutex m(&m_mutexSend);
        msgID = m_msgID++;
    }

    datOut = new uint8_t[NETTRANSFER_PACKET_MAXSIZE];
    packN  = len / NETTRANSFER_PACKET_DATASIZE + 1;

    for(packIdx=0; packIdx<packN; packIdx++) {
        generatePacket(dat, len, datOut, &lenOut,
                       msgID, packN, packIdx);

        ret = m_socket.send(datOut, lenOut);        
        if( ret < 0 ) goto SEND_QUIT;

        nSend += ret;
    }

SEND_QUIT:
    delete [] datOut;

    return nSend;
}


int NetTransfer_UDP::send(RDataStream &ds)
{
    return send(ds.data(), ds.size());
}

int NetTransfer_UDP::recv(uint8_t *dat, int len)
{
    NetTransfer_PacketPool *pp = (NetTransfer_PacketPool*) m_priData;

    NetTransfer_Packet *p = pp->getPack();
    if( p ) {
        uint64_t dl = p->msg_len;

        if( len < p->msg_len )
            memcpy(dat, p->dat, len);
        else
            memcpy(dat, p->dat, dl);

        delete p;

        return dl;
    }

    return -1;
}

int NetTransfer_UDP::recv(RDataStream &ds)
{
    ru32    ds_magic, ds_ver, ds_size;
    int     ret = -1;

    NetTransfer_PacketPool *pp = (NetTransfer_PacketPool*) m_priData;

    NetTransfer_Packet *p = pp->getPack();
    if( p ) {
        // read header
        datastream_get_header(p->dat, ds_magic, ds_ver);
        ds_size = datastream_get_length(p->dat);

        if( ds_size == p->msg_len ) ret = p->msg_len;

        ds.fromRawData(p->dat, p->msg_len, 0);

        p->dat = NULL;
        delete p;
    }

    return ret;
}

int NetTransfer_UDP::recvSlot(uint8_t *dat, int len)
{
    return 0;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int inet4_addr_str2i(std::string na, uint32_t &nai)
{
    inet_pton(AF_INET, na.data(), &nai);
    return 0;
}

int inet4_addr_i2str(uint32_t nai, std::string &na)
{
    char    buf[INET_ADDRSTRLEN+1];

    inet_ntop(AF_INET, &nai, buf, INET_ADDRSTRLEN);
    na = buf;

    return 0;
}

int inet4_addr_ni2hi(uint32_t ni, uint32_t &hi)
{
    hi = ntohl(ni);

    return 0;
}

int inet4_addr_hi2ni(uint32_t hi, uint32_t &ni)
{
    ni = htonl(hi);

    return 0;
}

int inet4_port_n2h(uint16_t np, uint16_t &hp)
{
    hp = ntohs(np);

    return 0;
}

int inet4_port_h2n(uint16_t hp, uint16_t &np)
{
    np = htons(hp);

    return 0;
}


void convByteOrder_h2n_16(void *s, void *d)
{
    uint16_t *_s, *_d;

    _s = (uint16_t*) s;
    _d = (uint16_t*) d;

    *_d = htons(*_s);
}

void convByteOrder_n2h_16(void *s, void *d)
{
    uint16_t *_s, *_d;

    _s = (uint16_t*) s;
    _d = (uint16_t*) d;

    *_d = ntohs(*_s);
}

void convByteOrder_h2n_32(void *s, void *d)
{
    uint32_t *_s, *_d;

    _s = (uint32_t*) s;
    _d = (uint32_t*) d;

    *_d = htonl(*_s);
}

void convByteOrder_n2h_32(void *s, void *d)
{
    uint32_t *_s, *_d;

    _s = (uint32_t*) s;
    _d = (uint32_t*) d;

    *_d = ntohl(*_s);
}

void convByteOrder_h2n_64(void *s, void *d)
{
    uint32_t *_s, *_d, tmp;

    _s = (uint32_t*) s;
    _d = (uint32_t*) d;
    tmp = _s[0];

    _d[0] = htonl(_s[1]);
    _d[1] = htonl(tmp);
}

void convByteOrder_n2h_64(void *s, void *d)
{
    uint32_t *_s, *_d, tmp;

    _s = (uint32_t*) s;
    _d = (uint32_t*) d;
    tmp = _s[0];

    _d[0] = ntohl(_s[1]);
    _d[1] = ntohl(tmp);
}

double htond(double x)
{
    double v;

    convByteOrder_h2n_64(&x, &v);
    return v;
}

float  htonf(float x)
{
    float v;

    convByteOrder_h2n_32(&x, &v);
    return v;
}


} // end of namespace pi
