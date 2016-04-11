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

/*
 * Bugs | Improvements:
 *  (1) If client lost connection to master, then client need
 *      send connection request for every 1 second (or configurable).
 *
 */


#ifndef __MESSAGE_PASSING_H__
#define __MESSAGE_PASSING_H__

#include <vector>
#include <map>
#include <deque>

#include "base/utils/utils.h"
#include "base/osa/osa++.h"
#include "base/types/MagicNumber.h"

#include "Socket++.h"

namespace pi {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class RMP_Node;                 ///< node class
class RMP_NodeMap;              ///< node map class
class RMP_SocketThread;         ///< receiving thread class
class RMessage;                 ///< message class
class RMessagePassing;          ///< message passing system class

typedef std::map<std::string, RMP_Node*>    RMP_NodeMap_t;      ///< node map class (inner used)
typedef std::deque<RMessage*>               RMessageQueue_t;    ///< message queue (inner used)



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The RMessagePassing Node class
///
class RMP_Node
{
public:
    RMP_Node() {
        init();
    }

    ~RMP_Node() {
        release();
    }

    void setNodeInfo(void) {
        osa_get_pid(&pid);
        osa_get_tid(&tid);
    }

    int isValidate(void) {
        if( nodeName.size() < 1 ) return 0;
        if( pid < 0 || tid < 0 )  return 0;

        return 1;
    }

    int sameProcess(RMP_Node &mpn) {
        if( mpn.hostName == hostName && mpn.pid == pid )
            return 1;
        else
            return 0;
    }

    int toStream(RDataStream &ds, int isSingleDS = 0) {
        if( isSingleDS ) {
            ds.resize(0);
            ds.setHeader(RTK_MAGIC_MP_NODE, 1);
        }
        
        ru64 p = (ru64) mpObj;
        ds << nodeName << hostName << pid << tid << isMaster
           << inet4_addr << inet4_port << p;

        return 0;
    }

    int fromStream(RDataStream &ds, int isSingleDS = 0) {
        if( isSingleDS ) {
            ru32    magic, ver;

            ds.seek(0);
            ds.getHeader(magic, ver);
            if( magic != RTK_MAGIC_MP_NODE ) return -1;
        }

        ru64 p;
                
        if( 0 != ds.read(nodeName) )    return -2;
        if( 0 != ds.read(hostName) )    return -2;
        if( 0 != ds.read(pid) )         return -2;
        if( 0 != ds.read(tid) )         return -2;
        if( 0 != ds.read(isMaster) )    return -2;
        if( 0 != ds.read(inet4_addr) )  return -2;
        if( 0 != ds.read(inet4_port) )  return -2;
        if( 0 != ds.read(p) )           return -2;
        
        mpObj = (RMessagePassing*) p;

        return 0;
    }

    RMP_Node& operator = (const RMP_Node &n) {
        nodeName        = n.nodeName;
        hostName        = n.hostName;
        pid             = n.pid;
        tid             = n.tid;
        isMaster        = n.isMaster;
        inet4_addr      = n.inet4_addr;
        inet4_port      = n.inet4_port;
        mpObj           = n.mpObj;

        return *this;
    }

    bool operator == (const RMP_Node &n) const {
        if( n.nodeName == nodeName ) return true;
        else                         return false;
    }

    bool operator != (const RMP_Node &n) const {
        if( n.nodeName == nodeName ) return false;
        else                         return true;
    }


    ///
    /// \brief set node information
    ///
    /// \param nn           - node name
    /// \param nodeAddr     - node address (for example "127.0.0.1")
    /// \param port         - port number
    ///
    void set(std::string nn, std::string nodeAddr, int port) {
        nodeName = nn;
        set_inet4_addr(nodeAddr);
        set_inet4_port(port);
    }


    ///
    /// \brief set inet4 address by string
    /// 
    /// \param na           - network address (string format)
    ///
    /// \return
    ///     0               - success
    ///
    int set_inet4_addr(std::string na) {
        hostName = na;
        inet4_addr_str2i(na, inet4_addr);

        return 0;
    }

    
    ///
    /// \brief set inet4 address by uint32_t
    ///
    /// \param na           - network address (uint32_t), 
    ///                         !!! must be network byte order (MSB) !!!
    ///
    /// \return
    ///     0               - success
    ///
    int set_inet4_addr(ru32 na) {
        inet4_addr = na;
        
        return 0;
    }

    ///
    /// \brief get inet4 address by uint32_t
    ///
    /// \param na           - network address (uint32_t)
    ///                         !!! must be network byte order (MSB) !!!
    ///
    /// \return
    ///     0               - sucess
    ///
    int get_inet4_addr(ru32 &na) {
        na = inet4_addr;

        return 0;
    }

    ///
    /// \brief set IP v4 port
    ///
    /// \param p            - port number (host byte-order)
    ///
    /// \return
    ///     0               - success
    ///
    int set_inet4_port(ru16 p) {
        inet4_port = p;

        return 0;
    }

    ///
    /// \brief get IP v4 port
    ///
    /// \param p            - port number (host byte-order)
    ///
    /// \return
    ///     0               - success
    ///
    int get_inet4_port(ru16 &p) {
        p = inet4_port;

        return 0;
    }

    void print(void) {
        printf("Node [%12s] hn = %10s, pid = %6lld, tid = %6lld, isMaster = %d, addr = %10d, port = %8d, mpObj = 0x%X, isSameProcess = %d\n",
               nodeName.c_str(), hostName.c_str(),
               pid, tid,
               isMaster,
               inet4_addr, inet4_port,
               mpObj, isSameProcess);
    }


    int sendMsg(RMessage *m);

    void init(void);
    void release(void);

public:
    std::string         nodeName;               ///< node name
    std::string         hostName;               ///< host name (IP address)
    ri64                pid;                    ///< process ID
    ri64                tid;                    ///< thread ID

    ri32                isMaster;               ///< mast node or not

    ru32                inet4_addr;             ///< IP v4 address
    ru16                inet4_port;             ///< port number (!!!host byte-order!!!)

    RMP_SocketThread*   socketThread;           ///< receiving thread
    RSocket*            socketSend;             ///< socket send
    
    int                 isSameProcess;          ///< same process or not
    RMessagePassing*    mpObj;                  ///< pair-wise RMP obj
    RMessagePassing*    mpMe;                   ///< this RMP obj
};



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class RMP_NodeMap
{
public:
    RMP_NodeMap() {
        init();
    }

    ~RMP_NodeMap() {
        init();
    }

    ///
    /// \brief insert a node to node map
    ///
    /// \param n        - node obj
    ///
    /// \return
    ///     0           - success
    ///
    int insert(RMP_Node *n, int broadcastTable=1);


    ///
    /// \brief erase a node from map
    ///
    /// \param nn       - node name
    ///
    /// \return
    ///     0           - success
    ///     -1          - failed
    ///
    int erase(std::string &nn);

    ///
    /// \brief get a node from node map
    ///
    /// \param nn       - node name
    ///
    /// \return
    ///     NULL        - failed
    ///     other       - RMP_Node pointer
    ///
    RMP_Node* get(std::string &nn);

    ///
    /// \brief get full node list
    ///
    /// \param nl       - a string list store all node names
    ///
    /// \return
    ///     0           - success
    ///     -1          - failed
    ///
    int getNodeList(StringArray &nl);

    int size() {
        RMutex lock(&m_mutex);
        return m_nodeMap.size();
    }

    void setMP(RMessagePassing *mp) {
        m_mp = mp;
    }

    void print(void);

    void init(void);
    void release(void);

protected:
    RMP_NodeMap_t       m_nodeMap;          ///< node map
    RMessagePassing*    m_mp;               ///< message passing obj

    RMutex              m_mutex;            ///< mutex
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class RMP_SocketThread : public RThread
{
public:
    RMP_SocketThread() {
        init();
    }

    RMP_SocketThread(RSocket *socket, RMessagePassing *mp) {
        init();

        m_socketRecv    = socket;
        m_mp            = mp;
    }

    virtual ~RMP_SocketThread() {
        release();
    }

    void setNodeMe(RMP_Node *n) {
        m_nodeMe = n;
    }

    void setNodeOther(RMP_Node *n) {
        m_nodeOther = n;
    }

    virtual int thread_func(void *arg);

    void init(void);
    void release(void);


protected:
    RMP_Node*               m_nodeMe;                   ///< my node
    RMP_Node*               m_nodeOther;                ///< pair-wise node

    RSocket*                m_socketRecv;               ///< socket to receive message
    RMessagePassing*        m_mp;                       ///< message-passing main obj
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class RMessage
{
public:
    RMessage() {
        init();
    }

    ~RMessage() {
        init();
    }


    ///
    /// \brief message to stream
    ///
    /// \param ds   - data stream obj
    ///
    /// \return
    ///     0       - success
    ///
    int toStream(RDataStream &ds, int isSingleDS=0) {
        if( isSingleDS ) {
            ds.resize(0);
            ds.setHeader(RTK_MAGIC_MESSAGE, 1);
        }

        // write message type & ID
        ds << msgType << msgID;

        // write message data length & data
        ru32 dl = data.size();
        ds.write(dl);
        ds.write(data.data(), dl);

        return 0;
    }

    ///
    /// \brief message from stream
    ///
    /// \param  ds      - data stream obj
    ///
    /// \return
    ///     0           - success
    ///     -1          - magic number is not match
    ///     -2          - failed to read
    ///
    int fromStream(RDataStream &ds, int isSingleDS=0) {
        if( isSingleDS ) {
            ru32        magic, ver;

            ds.seek(0);
            ds.getHeader(magic, ver);
            if( magic != RTK_MAGIC_MESSAGE ) return -1;
        }

        // read message type & ID
        if( 0 != ds.read(msgType) ) return -2;
        if( 0 != ds.read(msgID) )   return -2;

        // read data length & content
        ru32 dl;
        if( 0 != ds.read(dl) ) return -2;
        data.resize(dl);
        if( 0 != ds.read(data.data(), dl) ) return -2;

        data.updateHeader();
        data.seek(0);

        return 0;
    }

    void print(void) {
        printf("MSG [type = %4d, ID=%6d, %10s->%10s] data.size = %8d\n",
               msgType, msgID,
               nodeSrc.nodeName.c_str(),
               nodeDst.nodeName.c_str(),
               data.size());
    }

    void init(void) {
        msgType     = 0;
        msgID       = 0;
    }

    void release(void) {
        msgType     = 0;
        msgID       = 0;

        nodeSrc.init();
        nodeDst.init();

        data.clear();
    }

public:
    ru32        msgType;                ///< message type
    ru32        msgID;                  ///< message ID

    RMP_Node    nodeSrc;                ///< source node
    RMP_Node    nodeDst;                ///< destination node

    RDataStream data;                   ///< message data
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class RMessagePassing : public RThread
{
public:
    RMessagePassing() {
        init();
    }

    virtual ~RMessagePassing() {
        release();
    }



    ///
    /// \brief Simple message passing system startup
    ///             Node information will be load through Svar
    ///
    /// \param name         - node name
    ///                         "Master" - master node (default)
    ///
    virtual int begin(std::string name="Master");

    ///
    /// \brief begin message passing system (master mode)
    ///
    /// \param n            - my node configuration
    ///
    /// \return
    ///     0               - success
    ///     -1              - failed
    ///
    virtual int begin(RMP_Node &n);

    ///
    /// \brief begin message passing system (normal mode)
    ///
    /// \param n            - my node configuration
    /// \param nm           - master node
    ///
    /// \return
    ///     0               - success
    ///     -1              - failed
    ///
    virtual int begin(RMP_Node &n, RMP_Node &nm);

    ///
    /// \brief stop message passsing system
    ///
    /// \return
    ///
    virtual int stop(void);


    ///
    /// \brief return size of the message queue
    ///
    /// \return current message number in queue
    ///
    int size();

    ///
    /// \brief clear all messages in the message queue
    ///
    /// \return
    ///
    int clearMsg(void);

    ///
    /// \brief send message to node with its name
    ///
    /// \param nn           - node name
    /// \param msg          - message
    ///
    /// \return
    ///     0               - success
    ///
    int sendMsg(std::string nn, RMessage *msg);

    ///
    /// \brief receive a message
    ///
    /// \return
    ///     NULL            - failed
    ///     other           - message pointer
    ///
    RMessage* recvMsg(void);

    ///
    /// \brief send string to remote node
    ///
    /// \param nn           - node name
    /// \param msg          - message
    /// \param isCommand    - is command or not
    ///
    /// \return
    ///
    int sendString(std::string nn, std::string msg, bool isCommand=false);

    ///
    /// \brief recv string from remote node
    ///
    /// \param str          - received string
    ///
    /// \return
    ///
    RMP_Node* recvString(std::string& str);


    ///
    /// \brief process received message
    ///
    /// \param msg
    ///
    /// \return
    ///
    virtual int recvMessageSlot(RMessage *msg);


    ///
    /// \brief get node map object
    ///
    /// \return
    ///     node map object
    ///
    RMP_NodeMap* getNodeMap(void) {
        return &m_nodeMap;
    }

    ///
    /// \brief get my node object
    ///
    /// \return
    ///     node object
    ///
    RMP_Node* getNodeMe(void) {
        return m_nodeMe;
    }

    ///
    /// \brief get master node object
    ///
    /// \return
    ///     node object
    ///
    RMP_Node* getNodeMaster(void) {
        return m_nodeMaster;
    }

    ///
    /// \brief get node by its name
    ///
    /// \param nn       - node name
    ///
    /// \return
    ///     NULL        - do not exist
    ///     other       - node pointer
    ///
    RMP_Node* getNode(std::string nn) {
        return m_nodeMap.get(nn);
    }

    ///
    /// \brief return flag of isMaster
    /// \return
    ///     0               - not a master system
    ///     1               - is a master system
    int isMaster(void) {
        return m_isMaster;
    }


    ///
    /// \brief thread_func for accepting income connections
    ///
    /// \param arg          - thread argument
    ///
    /// \return
    ///     0               - success
    ///
    virtual int thread_func(void *arg);


    ///
    /// \brief Initialize message passing system
    ///
    void init(void);

    ///
    /// \brief release message passing system
    ///
    void release(void);

protected:
    RMP_NodeMap                 m_nodeMap;                  ///< node map
    RMP_Node*                   m_nodeMe;                   ///< my node
    RMP_Node*                   m_nodeMaster;               ///< master node

    RSocket                     m_socket;                   ///< socket obj

    RMessageQueue_t             m_msgQueue;                 ///< message queue

    int                         m_isMaster;                 ///< is master or not

    RMutex                      m_mutex;                    ///< mutex
    RMutex                      m_mutexSend;                ///< mutex for send
};


} // end of namespace of pi

#endif // end of __MESSAGE_PASSING_H__
