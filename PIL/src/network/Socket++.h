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


#ifndef __SOCKET_PP_H__
#define __SOCKET_PP_H__

#include <string.h>
#include <stdint.h>
#include <string>

#include "base/osa/osa++.h"
#include "base/Svar/DataStream.h"

namespace pi {


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum RSocketType
{
    SOCKET_TCP,                         ///< TCP
    SOCKET_UDP,                         ///< UDP
    SOCKET_UDP_MULTICAST,               ///< multicast UDP
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The RSocketAddress class
///
/// Multicast address:
///     https://en.wikipedia.org/wiki/Multicast_address
///
class RSocketAddress
{
public:
    RSocketAddress() {
        port = -1;
        addr_inet = 0;
        type = SOCKET_TCP;
        memset(addr_str, 0, sizeof(addr_str));
    }

    ~RSocketAddress() {
        port = -1;
        addr_inet = 0;
        type = SOCKET_TCP;
        memset(addr_str, 0, sizeof(addr_str));
    }

public:
    int         port;                   ///> port number
    uint32_t    addr_inet;              ///> uint32_t address
    char        addr_str[32];           ///> address string
    RSocketType type;                   ///> socket type
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The RSocket class support TCP/UDP
///
class RSocket
{
public:
    RSocket();
    virtual ~RSocket();

    ////////////////////////////////////////////////////
    /// start server or client
    ////////////////////////////////////////////////////

    ///
    /// \brief startServer
    ///
    /// \param port             - port number
    /// \param t                - socket type
    ///
    /// \return
    ///        0                - success
    ///        other            - failed
    ///
    int startServer(int port, RSocketType t=SOCKET_TCP);

    ///
    /// \brief startServer
    ///
    /// \param addr             - host or group address
    /// \param port             - port number
    /// \param t                - socket type
    ///
    /// \return
    ///        0                - success
    ///        other            - failed
    ///
    int startServer(const std::string& addr, int port, RSocketType t=SOCKET_UDP_MULTICAST);


    ///
    /// \brief startClient
    ///
    /// \param host             - pair host address or group address
    /// \param port             - port number
    /// \param t                - socket type
    ///
    /// \return
    ///        0                - success
    ///        other            - failed
    ///
    int startClient(std::string host, int port, RSocketType t=SOCKET_TCP);

    ///
    /// \brief startClient
    ///
    /// \param addr             - host or group address
    /// \param port             - port number
    /// \param t                - socket type
    ///
    /// \return
    ///        0                - success
    ///        other            - failed
    ///
    int startClient(uint32_t addr, int port, RSocketType t=SOCKET_TCP);


    ////////////////////////////////////////////////////
    /// Data Transmission
    ////////////////////////////////////////////////////

    ///
    /// \brief send data to remote host
    ///
    /// \param dat              - data buffer
    /// \param len              - data length
    ///
    /// \return
    ///        > 0              - actual sended bytes
    ///        <= 0             - failed
    ///
    int send(uint8_t *dat, int len);

    ///
    /// \brief send string to remote host
    ///
    /// \param msg              - std::string
    ///
    /// \return
    ///        > 0              - actual sended bytes
    ///        <= 0             - failed
    ///
    int send(std::string& msg);

    ///
    /// \brief send DataStream to remote host
    ///
    /// \param ds               - data stream
    ///
    /// \return
    ///        > 0              - actual sended bytes
    ///        <= 0             - failed
    ///
    int send(RDataStream &ds);


    ///
    /// \brief receive data from remote host
    ///
    /// \param dat              - data buffer
    /// \param len              - data length
    ///
    /// \return
    ///        > 0              - actual received bytes
    ///        <= 0             - failed
    ///
    int recv(uint8_t *dat, int len);

    ///
    /// \brief receive string from remote host
    ///
    /// \param msg              - std::string
    /// \param maxLen           - maximum string length
    ///
    /// \return
    ///        > 0              - actual received bytes
    ///        <= 0             - failed
    ///
    int recv(std::string& msg, int maxLen = 4096);

    ///
    /// \brief receive data stream from remote host
    ///
    /// \param ds               - data stream
    ///
    /// \return
    ///        > 0              - actual received bytes
    ///        <= 0             - failed
    ///
    int recv(RDataStream &ds);

    //
    ///
    /// \brief receive data until given data length reached
    ///
    /// \param dat              - data buffer
    /// \param len              - data length
    ///
    /// \return
    ///        > 0              - actual received bytes
    ///        <= 0             - failed
    ///
    int recv_until(uint8_t *dat, int len);


    ////////////////////////////////////////////////////
    /// socket creation & close
    ////////////////////////////////////////////////////

    int create(void);
    int close(void);

    // server functions
    int bind(int port);
    int listen(void);
    int accept(RSocket& s);

    // client functions
    int connect(std::string host, int port);
    int connect(uint32_t addr, int port);

    // get address
    int getMyAddress(RSocketAddress &a);
    int getClientAddress(RSocketAddress &a);

    // socket options
    int setNonBlocking(int nb=1);
    int setReuseAddr(uint32_t reuse=1);

    // status
    int isOpened(void) { return m_sock != -1; }
    bool isSever(void) { return m_server; }


protected:
    int             m_sock;                         ///< socket file descriptor
    int             m_server;                       ///< server or client
    RSocketType     m_socketType;                   ///< socket type
    void*           m_priData;                      ///< socket private data

    int             m_maxConnections;               ///< maximum connections (default: 100)
    int             m_maxHostname;                  ///< maximum length of hostname (default: 1024)
};



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


///
/// \brief The simple data transfer class which uses UDP.
///     server - send data
///     client - recv data
///
///     This class provide a way to transfer data with reliable manner. So you don't
///         worry about the disorder of the received data. But it do not support
///         data missing (resend data).
///
/// \ref
///     UDP buffer size:
///         Linux           131071
///         Windows         No known limit
///         Solaris         262144
///         FreeBSD, Darwin	262144
///         AIX             1048576
///
///     https://access.redhat.com/documentation/en-US/JBoss_Enterprise_Web_Platform/5/html/Administration_And_Configuration_Guide/jgroups-perf-udpbuffer.html
///
class NetTransfer_UDP : public RThread
{
public:
    NetTransfer_UDP();
    virtual ~NetTransfer_UDP();

    ///
    /// \brief thread_func - receiving thread
    ///
    /// \param arg          - thread argument
    ///
    /// \return
    ///
    virtual int thread_func(void *arg);


    ///
    /// \brief open the UDP transfer - for simple transmitting message
    ///
    /// \param isServer     - 0: client, receive message
    ///                       1: server, send message
    /// \param port         - port number (default 30000)
    /// \param addr         - remote address (for client) or multicast address
    /// \param st           - socket type
    ///                         SOCKET_UDP              - normal UDP
    ///                         SOCKET_UPD_MULTICAST    - multicast UDP
    ///
    /// \return
    ///         0           - success
    ///         other       - failed
    ///
    virtual int open(int isServer=1,
                     int port=30000, const std::string &addr="225.0.0.10",
                     RSocketType st=SOCKET_UDP_MULTICAST);

    ///
    /// \brief close the UDP transfer
    ///
    /// \return
    ///
    virtual int close(void);

    ///
    /// \brief send data to remote terminal
    ///
    /// \param dat          - message buffer
    /// \param len          - message length
    ///
    /// \return
    ///     if success return sended message length
    ///     -1              - failed (maybe UDP not opened)
    ///
    virtual int send(uint8_t *dat, int len);

    ///
    /// \brief send data to remote terminal
    ///
    /// \param ds           - datastream
    ///
    /// \return
    ///     if success return sended message length
    ///     -1              - failed (maybe UDP not opened)
    ///
    virtual int send(RDataStream &ds);


    ///
    /// \brief recved message processing function
    ///
    /// \param dat          - message buffer
    /// \param len          - message length
    ///
    /// \return
    ///     if success return sended message length
    ///     -1              - failed (maybe UDP not opened)
    ///
    virtual int recv(uint8_t *dat, int len);

    ///
    /// \brief recved message processing function
    ///
    /// \param ds           - datastream
    ///
    /// \return
    ///     if success return sended message length
    ///     -1              - failed (maybe UDP not opened)
    ///
    virtual int recv(RDataStream &ds);

    ///
    /// \brief recved message slot function
    ///
    /// \param dat          - message buffer
    /// \param len          - message length
    ///
    /// \return
    ///     0               - success
    ///
    virtual int recvSlot(uint8_t *dat, int len);


    ///
    /// \brief return connection is established or not
    ///
    /// \return
    ///     1               - connected
    ///     0               - not connected
    ///
    int isOpened(void) { return m_isConnected; }


protected:
    RSocketType     m_socketType;                   ///< socket type;
    RSocket         m_socket;                       ///< socket obj
    int             m_isServer;                     ///< 1-server(RX), 0-client(TX)
    int             m_isConnected;                  ///< connected or not

    std::string     m_addr;                         ///< remote terminal address
    int             m_port;                         ///< UDP port

    uint64_t        m_msgID;                        ///< message ID
    void            *m_priData;                     ///< private data

    RMutex          m_mutexSend;
};



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int inet4_addr_str2i(std::string na, ru32 &nai);
int inet4_addr_i2str(ru32 nai, std::string &na);

int inet4_addr_ni2hi(ru32 ni, ru32 &hi);
int inet4_addr_hi2ni(ru32 hi, ru32 &ni);

int inet4_port_n2h(ru16 np, ru16 &hp);
int inet4_port_h2n(ru16 hp, ru16 &np);



void convByteOrder_h2n_16(void *s, void *d);
void convByteOrder_n2h_16(void *s, void *d);
void convByteOrder_h2n_32(void *s, void *d);
void convByteOrder_n2h_32(void *s, void *d);
void convByteOrder_h2n_64(void *s, void *d);
void convByteOrder_n2h_64(void *s, void *d);


template<class T>
T convByteOrder_h2n(T s)
{
    if( sizeof(T) == 2 ) {
        uint16_t *_s, *_d;
        T        v;

        _s = (uint16_t*) &s;
        _d = (uint16_t*) &v;

        convByteOrder_h2n_16(_s, _d);
        return v;
    } else if( sizeof(T) == 4 ) {
        uint32_t *_s, *_d;
        T        v;

        _s = (uint32_t*) &s;
        _d = (uint32_t*) &v;

        convByteOrder_h2n_32(_s, _d);
        return v;
    } else if( sizeof(T) == 8 ) {
        uint64_t *_s, *_d;
        T        v;

        _s = (uint64_t*) &s;
        _d = (uint64_t*) &v;

        convByteOrder_h2n_64(_s, _d);
        return v;
    }
}

template<class T>
T convByteOrder_n2h(T s)
{
    if( sizeof(T) == 2 ) {
        uint16_t *_s, *_d;
        T        v;

        _s = (uint16_t*) &s;
        _d = (uint16_t*) &v;

        convByteOrder_n2h_16(_s, _d);
        return v;
    } else if( sizeof(T) == 4 ) {
        uint32_t *_s, *_d;
        T        v;

        _s = (uint32_t*) &s;
        _d = (uint32_t*) &v;

        convByteOrder_n2h_32(_s, _d);
        return v;
    } else if( sizeof(T) == 8 ) {
        uint64_t *_s, *_d;
        T        v;

        _s = (uint64_t*) &s;
        _d = (uint64_t*) &v;

        convByteOrder_n2h_64(_s, _d);
        return v;
    }
}

} // end of namespace pi

#endif // end of __SOCKET_PP_H__
