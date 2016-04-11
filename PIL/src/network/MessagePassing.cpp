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


#include "base/Svar/Scommand.h"

#include "MessagePassing.h"

namespace pi {

using namespace std;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void RMP_Node::init(void)
{
    nodeName        = "";
    hostName        = "";
    pid             = -1;
    tid             = -1;

    isMaster        = 0;

    inet4_addr      = 0;
    inet4_port      = 0;

    socketThread    = NULL;
    socketSend      = NULL;
    mpObj           = NULL;
    mpMe            = NULL;

    isSameProcess   = 0;
}

void RMP_Node::release(void)
{
    // kill receiving thread
    if( socketThread != NULL ) {
        socketThread->setAlive(0);
        socketThread->wait(10);
        socketThread->kill();

        delete socketThread;
    }

    // delete socket for sending
    if( socketSend != NULL )
        delete socketSend;

    init();
}

int RMP_Node::sendMsg(RMessage *m)
{
     int         ret;
     ru8         *buf = NULL;

     // same process
     if( isSameProcess && mpObj != NULL ) {
         mpObj->recvMessageSlot(m);
         return 0;
     }

     // connect to node
     if( socketSend == NULL ) {
         socketSend = new RSocket;

         // create connections
         ret = socketSend->startClient(inet4_addr, inet4_port);
         if( ret != 0 ) {
             dbg_pe("Can not connect to node: %s (%s), addr = %d, port = %d\n",
                    nodeName.c_str(), hostName.c_str(),
                    inet4_addr, inet4_port);
             ret = -1;
             goto SEND_MSG_ERR;
         }

         // send my node name to pairwise node
         RDataStream ds;
         ds.write(mpMe->getNodeMe()->nodeName);
         ret = socketSend->send(ds);
         //dbg_pt("send my node to other: %s\n", nodeName.c_str());
         if( ret < ds.size() ) {
             dbg_pe("Can not connect to node: %s (%s), addr = %d, port =%d\n",
                    nodeName.c_str(), hostName.c_str(),
                    inet4_addr, inet4_port);
             ret = -1;
             goto SEND_MSG_ERR;
         }
     }

     // send message to node
     ru32    header_len;
     ru32    msg_magic, msg_ver, msg_header;
     ru32    data_len;

     header_len = 4*sizeof(ru32);
     buf = new ru8[header_len+4];

     msg_magic  = RTK_MAGIC_MESSAGE;
     msg_ver    = 1;
     msg_header = (msg_magic << 16) | (msg_ver & 0xFFFF);
     data_len   = m->data.size();

     memcpy(buf + 0*sizeof(ru32), &msg_header,   sizeof(ru32));
     memcpy(buf + 1*sizeof(ru32), &(m->msgType), sizeof(ru32));
     memcpy(buf + 2*sizeof(ru32), &(m->msgID),   sizeof(ru32));
     memcpy(buf + 3*sizeof(ru32), &data_len,     sizeof(ru32));

     // send header
     ret = socketSend->send(buf, header_len);
     if( ret < 0 ) goto SEND_MSG_ERR;

     // send data
     ret = socketSend->send(m->data.data(), m->data.size());
     if( ret < 0 ) goto SEND_MSG_ERR;

     goto SEND_MSG_RETURN;

SEND_MSG_ERR:
     ret = -2;
     dbg_pw("Send message error to node: %s (%s), errcode = %d\n",
            nodeName.c_str(), hostName.c_str(), ret);
     socketSend->close();
     socketSend = NULL;

SEND_MSG_RETURN:
     if( buf != NULL ) delete [] buf;
     return ret;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void RMP_NodeMap::init(void)
{
    m_nodeMap.clear();
    m_mp = NULL;
}

void RMP_NodeMap::release(void)
{
    RMP_NodeMap_t::iterator it;

    for(it = m_nodeMap.begin(); it != m_nodeMap.end(); it++) {
        RMP_Node *n = it->second;
        delete n;
        it->second = NULL;
    }

    m_nodeMap.clear();

    m_mp = NULL;
}

int RMP_NodeMap::insert(RMP_Node *n, int broadcastTable)
{
    // insert to node map
    m_mutex.lock();
    m_nodeMap.insert(std::make_pair(n->nodeName, n));
    m_mutex.unlock();

    // broadcast new node to all other nodes
    if( m_mp->isMaster() && broadcastTable ) {
        RMP_NodeMap_t::iterator it;
        RMP_Node                *pn;

        RMessage msg;
        msg.msgType = 0xFFFFFFF1;

        // generate datastream of node map
        for(it=m_nodeMap.begin(); it!=m_nodeMap.end(); it++) {
            pn = it->second;
            pn->toStream(msg.data);
        }

        // broadcast to all other nodes
        for(it=m_nodeMap.begin(); it!=m_nodeMap.end(); it++) {
            pn = it->second;

            if( !pn->isMaster ) {
                pn->sendMsg(&msg);
            }
        }
    }


    return 0;
}

int RMP_NodeMap::erase(std::string &nn)
{
    RMP_Node    *n = NULL;

    m_mutex.lock();
    RMP_NodeMap_t::const_iterator it = m_nodeMap.find(nn);

    if( it != m_nodeMap.end() ) {
        n = it->second;
        m_nodeMap.erase(nn);
    }
    m_mutex.unlock();

    if( n != NULL ) {
        delete n;
        return 0;
    } else
        return -1;
}


RMP_Node* RMP_NodeMap::get(std::string &nn)
{
    RMP_Node    *n = NULL;

    m_mutex.lock();

    RMP_NodeMap_t::const_iterator it = m_nodeMap.find(nn);
    if( it != m_nodeMap.end() ) {
        n = it->second;
    }

    m_mutex.unlock();

    return n;
}

int RMP_NodeMap::getNodeList(StringArray &nl)
{
    RMP_NodeMap_t::iterator it;

    nl.clear();

    for(it=m_nodeMap.begin(); it!=m_nodeMap.end(); it++) {
        nl.push_back(it->first);
    }

    return 0;
}

void RMP_NodeMap::print(void)
{
    RMP_NodeMap_t::iterator it;
    int                     i;

    printf("NodeMap [%s] map.size() = %d:\n",
           m_mp->getNodeMe()->nodeName.c_str(), m_nodeMap.size());

    for(i=0, it=m_nodeMap.begin(); it!=m_nodeMap.end(); it++) {
        printf("    [%3d] %s\n", i++, it->first.c_str());
    }
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void RMP_SocketThread::init(void)
{
    m_nodeMe        = NULL;
    m_nodeOther     = NULL;
    m_socketRecv    = NULL;
    m_mp            = NULL;
}

void RMP_SocketThread::release(void)
{
    if( m_socketRecv != NULL ) delete m_socketRecv;

    m_nodeMe        = NULL;
    m_nodeOther     = NULL;
    m_socketRecv    = NULL;
    m_mp            = NULL;
}


int RMP_SocketThread::thread_func(void *arg)
{
    int             ret;

    std::string     nodeName;
    RDataStream     ds;
//    Scommand&       scommand = Scommand::instance();

    ru8             *buf;
    ru32            buf_len, msg_header_len;
    ru32            msg_header, msg_magic, msg_ver, msgType, msgID, 
                    data_len;


    // allocate buffer
    buf_len = 4096;
    buf = new ru8[buf_len];

    msg_header_len = 4*sizeof(ru32);

    // receive node name
    ret = m_socketRecv->recv(ds);
    if( ret == 0 ) {
        if( ds.read(nodeName) != 0 ) setAlive(0);
    } else {
        setAlive(0);
    }

    // receiving message forever
    while( getAlive() ) {
        // receive message header
        ret = m_socketRecv->recv_until(buf, msg_header_len);
        if( ret < 0 ) break;

        if( ret < msg_header_len ) {
            dbg_pw("Read message header error! only read %d/%d bytes\n",
                   ret, msg_header_len);
            break;
            //continue;
        }

        // get message header
        memcpy(&msg_header, buf + 0*sizeof(ru32), sizeof(ru32));
        memcpy(&msgType,    buf + 1*sizeof(ru32), sizeof(ru32));
        memcpy(&msgID,      buf + 2*sizeof(ru32), sizeof(ru32));
        memcpy(&data_len,   buf + 3*sizeof(ru32), sizeof(ru32));
        
        msg_magic = msg_header >> 16;
        msg_ver   = msg_header & 0xFFFF;

    
        if( msg_magic != RTK_MAGIC_MESSAGE ) {
            dbg_pw("Magic number is wrong: 0x%X (correct is: 0x%X)\n",
                   msg_magic, RTK_MAGIC_MESSAGE);
            continue;
        }

        // get message length & alloc buffer
        if( buf_len < data_len ) {
            delete [] buf;

            buf_len = data_len + 256;
            buf = new ru8[buf_len];
        }

        // read message data
        ret = m_socketRecv->recv_until(buf, data_len);
        if( ret < 0 ) break;
        if( ret < data_len ) {
            dbg_pw("Read message content error! Only read %d/%d bytes\n",
                   ret, data_len);
            break;
            continue;
        }

        // parse the datastream & process it
        RMessage *msg = new RMessage;
        msg->msgType = msgType;
        msg->msgID   = msgID;
        msg->data.fromRawData(buf, data_len);
        msg->data.seek(0);


        // process special messages
        if( msg->msgType == 0xFFFFFFF0 ) {
            // node add message
            RMP_Node *n  = new RMP_Node,
                     *n2 = NULL;

            ret = n->fromStream(msg->data);
            n->mpMe = m_mp;

            n2 = m_mp->getNodeMap()->get(n->nodeName);
            if( n2 == NULL ) {
                m_nodeOther = n;

                n->socketThread = this;
                if( m_nodeMe->sameProcess(*n) ) n->isSameProcess = 1;

                m_mp->getNodeMap()->insert(n);
            } else {
                m_nodeOther = n2;

                *n2 = *n;
                n2->socketThread = this;
                n2->mpMe = m_mp;
                if( m_nodeMe->sameProcess(*n2) ) n2->isSameProcess = 1;

                delete n;
            }

            delete msg;
            continue;
        } else if (msg->msgType == 0xFFFFFFF1 ) {
            // update node map message
            RMP_Node    *pn;
            int         inode = 0;

            while(1) {
                RMP_Node *n = new RMP_Node;
                if( 0 != n->fromStream(msg->data) ) break;

                /*
                printf("update node [%3d] ", inode++);
                n->print();
                */

                pn = m_mp->getNodeMap()->get(n->nodeName);
                if( pn == NULL ) {
                    if( m_nodeMe->sameProcess(*n) ) n->isSameProcess = 1;
                    n->mpMe = m_mp;

                    m_mp->getNodeMap()->insert(n, 0);
                } else {
                    *pn = *n;

                    if( m_nodeMe->sameProcess(*pn) ) pn->isSameProcess = 1;
                    pn->mpMe = m_mp;

                    delete n;
                }
            }

            // update pair-wise node
            pn = m_mp->getNodeMap()->get(nodeName);
            m_nodeOther = pn;

            // delete temp obj
            delete msg;
            continue;
        } else if( msg->msgType == 0xFFFFFFF2 ) {
            string command;

            msg->data.rewind();
            msg->data.read(command);
            scommand.Call(command);

            // delete temp obj
            delete msg;
            continue;
        }

        // process other message
        if( m_nodeOther != NULL ) msg->nodeSrc = *m_nodeOther;
        if( m_nodeMe != NULL)     msg->nodeDst = *m_nodeMe;
        m_mp->recvMessageSlot(msg);
    }

    // release all resources
    delete [] buf;
    buf = NULL;

    // remove node from node map
    dbg_pt("remove node: m_nodeOther->nodeName = %s\n", m_nodeOther->nodeName.c_str());
    m_mp->getNodeMap()->erase(m_nodeOther->nodeName);

    release();

    return 0;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void RMessagePassing::init(void)
{
    m_isMaster   = 0;
    m_nodeMe     = NULL;
    m_nodeMaster = NULL;
}


void RMessagePassing::release(void)
{
    // stop connecting thread
    setAlive(0);
    wait(10);
    kill();

    // release node map
    m_nodeMap.release();

    // close socket
    m_socket.close();

    // clear message queue
    clearMsg();

    // clear nodes
    m_nodeMe     = NULL;
    m_nodeMaster = NULL;
}


int RMessagePassing::begin(string name)
{
    string  node_ip = "127.0.0.1";
    int     node_port;
    string  nodeMaster_ip   = "127.0.0.1";
    int     nodeMaster_port = 30000;

    int     ret;

    if( "Master" == name )  {
        m_isMaster   = 1;
        node_port = 30000;
    } else {
        m_isMaster  = 0;
        node_port = 30001;
    }

    // load self information (IP address, port)
    node_ip   = svar.GetString(name+".ip", node_ip);
    node_port = svar.GetInt(name+".port", node_port);


    // start message passing system
    if( m_isMaster ) {
        RMP_Node *pn = new RMP_Node;

        pn->set("Master", node_ip, node_port);
        pn->isMaster = 1;
        pn->setNodeInfo();
        pn->mpObj = this;
        pn->mpMe  = this;

        m_nodeMe     = pn;
        m_nodeMaster = pn;

        pn->print();

        // insert to node map
        m_nodeMap.setMP(this);
        m_nodeMap.insert(pn, 0);

        // start server
        ret = m_socket.startServer(pn->inet4_port);
        if( ret != 0 ) {
            dbg_pe("Can not start socket_server: %s (%s)\n",
                   pn->nodeName.c_str(), pn->hostName.c_str());
            return -1;
        }

        // start receiving thread
        start();
    } else {
        // insert to node map
        RMP_Node *pn  = new RMP_Node,
                 *pnm = new RMP_Node;

        nodeMaster_ip   = svar.GetString("Master.ip", nodeMaster_ip);
        nodeMaster_port = svar.GetInt("Master.port", 30000);

        // set self/master node information
        pn->set(name, node_ip, node_port);
        pnm->set("Master", nodeMaster_ip, nodeMaster_port);

        pn->setNodeInfo();
        pn->mpMe  = this;
        pn->mpObj = this;

        pnm->mpMe = this;
        pnm->isMaster = 1;


        m_nodeMe     = pn;
        m_nodeMaster = pnm;

        m_nodeMe->print();
        m_nodeMaster->print();

        m_nodeMap.setMP(this);
        m_nodeMap.insert(pn, 0);
        m_nodeMap.insert(pnm, 0);

        // start server
        ret = m_socket.startServer(pn->inet4_port);
        if( ret != 0 ) {
            dbg_pe("Can not start socket_server: %s (%s)\n",
                    pn->nodeName.c_str(), pn->hostName.c_str());
            return -1;
        }

        // start receiving thread
        start();

        // send my node information to master node
        RMessage msg;
        msg.msgType = 0xFFFFFFF0;
        pn->toStream(msg.data);

        if( pnm->sendMsg(&msg) < 0 ) {
            dbg_pe("Please start master node first!\n");
            return -2;
        }
    }

    return 0;
}


int RMessagePassing::begin(RMP_Node &n)
{
    int ret;

    // set self node to master node
    m_isMaster = 1;

    RMP_Node *pn = new RMP_Node;
    *pn = n;

    pn->isMaster = 1;
    pn->setNodeInfo();
    pn->mpObj = this;
    pn->mpMe  = this;

    m_nodeMe     = pn;
    m_nodeMaster = pn;

    pn->print();

    // insert to node map
    m_nodeMap.setMP(this);
    m_nodeMap.insert(pn, 0);

    // start server
    ret = m_socket.startServer(pn->inet4_port);
    if( ret != 0 ) {
        dbg_pe("Can not start socket_server: %s (%s)\n",
                n.nodeName.c_str(), n.hostName.c_str());
        return -1;
    }

    // start receiving thread
    start();

    return 0;
}

int RMessagePassing::begin(RMP_Node &n, RMP_Node &nm)
{
    int ret;

    // set node to normal node
    m_isMaster = 0;

    // insert to node map
    RMP_Node *pn  = new RMP_Node,
             *pnm = new RMP_Node;

    *pn  = n;
    *pnm = nm;

    pn->setNodeInfo();
    pn->mpMe  = this;
    pn->mpObj = this;

    pnm->mpMe = this;
    pnm->isMaster = 1;


    m_nodeMe     = pn;
    m_nodeMaster = pnm;

    m_nodeMe->print();
    m_nodeMaster->print();

    m_nodeMap.setMP(this);
    m_nodeMap.insert(pn, 0);
    m_nodeMap.insert(pnm, 0);

    // start server
    ret = m_socket.startServer(pn->inet4_port);
    if( ret != 0 ) {
        dbg_pe("Can not start socket_server: %s (%s)\n",
                n.nodeName.c_str(), n.hostName.c_str());
        return -1;
    }

    // start receiving thread
    start();

    // send my node information to master node
    RMessage msg;
    msg.msgType = 0xFFFFFFF0;
    pn->toStream(msg.data);

    if( pnm->sendMsg(&msg) < 0 ) {
        dbg_pe("Please start master node first!\n");
        return -2;
    }

    return 0;
}

int RMessagePassing::stop(void)
{
    // release all resources
    release();

    return 0;
}


int RMessagePassing::size()
{
    RMutex m(&m_mutex);

    return m_msgQueue.size();
}


int RMessagePassing::clearMsg(void)
{
    RMutex m(&m_mutex);

    for(RMessageQueue_t::iterator it=m_msgQueue.begin(); it!=m_msgQueue.end(); it++) {
        RMessage *msg = *it;

        delete msg;
    }

    m_msgQueue.clear();

    return 0;
}

int RMessagePassing::sendMsg(std::string nn, RMessage *msg)
{
    RMutex m(&m_mutexSend);
    RMP_Node *n;

    n = m_nodeMap.get(nn);
    if( NULL == n ) {
        //dbg_pe("Can not find node: %s\n", nn.c_str());
        return -1;
    }

    if( n->sendMsg(msg) < 0 ) {
        m_nodeMap.erase(nn);
        return -2;
    } else {
        return 0;
    }

#if 0
    // send or try to send message
    while( n->sendMsg(msg) < 0 ) {
        if(  nTry-- <= 1 ) break;

        dbg_pw("attempting rebuild connect...\n");
        m_socket.startServer(m_nodeMe->inet4_port);
        tm_sleep(5000);
    }

    if( nTry ) {
        return 0;
    } else {
        m_nodeMap.erase(nn);
        return -2;
    }
#endif
}




int RMessagePassing::sendString(std::string nn, std::string msg, bool isCommand)
{
    RMP_Node *n;

    n = m_nodeMap.get(nn);
    if( NULL == n ) {
        dbg_pe("Can not find node: %s\n", nn.c_str());
        return -1;
    }

    RMessage rmsg;
    if( isCommand ) rmsg.msgType = 0xFFFFFFF2;
    else            rmsg.msgType = 1;
    rmsg.msgID = 1;
    rmsg.data.write(msg);

    return n->sendMsg(&rmsg);
}

RMP_Node* RMessagePassing::recvString(std::string& str)
{
    RMessage *msg = NULL;

    m_mutex.lock();
    if( m_msgQueue.size() > 0 ) {
        msg = m_msgQueue.front();
        m_msgQueue.pop_front();
    }
    m_mutex.unlock();

    if( msg ) {
        msg->data.read(str);
        return &(msg->nodeSrc);
    } else
        return NULL;
}


RMessage* RMessagePassing::recvMsg(void)
{
    RMessage *msg = NULL;

    m_mutex.lock();
    if( m_msgQueue.size() > 0 ) {
        msg = m_msgQueue.front();
        m_msgQueue.pop_front();
    }
    m_mutex.unlock();

    return msg;
}

int RMessagePassing::recvMessageSlot(RMessage *msg)
{
    RMutex m(&m_mutex);

    m_msgQueue.push_back(msg);

    return 0;
}


int RMessagePassing::thread_func(void *arg)
{
    // accept connection forever
    while( getAlive() ) {
        RSocket *new_socket = new RSocket;

        // accept new connection
        if( 0 != m_socket.accept(*new_socket) ) {
            dbg_pe("Accept new connection failed!");
            delete new_socket;
            break;
        }

        // create new receiving thread
        RMP_SocketThread *new_thread;
        new_thread = new RMP_SocketThread(new_socket, this);
        new_thread->setNodeMe(m_nodeMe);

        // start receiving thread
        new_thread->start();
    }

    return 0;
}


} // end of namespace pi
