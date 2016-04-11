#ifndef INTERNETTRANSFER_H
#define INTERNETTRANSFER_H

#include <deque>
#include <base/types/types.h>
#include <base/types/SPtr.h>
#include <base/debug/debug_config.h>
#include "MessagePassing.h"

template <typename DataType>
class SocketTransfer: public pi::RThread
{
public:
    typedef typename SPtr<DataType> DataElement;
    typedef typename std::deque<DataElement> DataDeque;
public:
    SocketTransfer(const std::string& nodeName=""):m_bOpened(false){
        if(nodeName.size())
            if(0!=begin(nodeName)) exit(0);
    }

    ~SocketTransfer()
    {
        if( m_bOpened ) {
            m_socket.close();
            kill();
        }

        m_bOpened = 0;
    }

    bool isMaster(){return m_socket.isSever();}

    int size(){
        pi::RMutex lock(&m_mutex);
        return m_dataQueue.size();
    }

    void push(const DataElement& ele){
        pi::RMutex lock(&m_mutex);
        if(m_dataQueue.size()>10) m_dataQueue.pop_front();
        m_dataQueue.push_back(ele);
    }

    DataElement pop(){
        pi::RMutex lock(&m_mutex);
        if(m_dataQueue.size())
        {
            DataElement ele=m_dataQueue.front();
            m_dataQueue.pop_front();
            return ele;
        }
        else return DataElement();
    }


    int begin(std::string nodeName)
    {
        if( m_bOpened ) {
            dbg_pe("socket is open. Please close first!");
            return -1;
        }

        if(nodeName=="Master")
        {
            if(0!=m_socket.startServer(svar.GetInt("Master.port",30000)))
            {
                MSG_ERROR("Failed to start server");
                return -1;
            }
        }
        else
        {
            if(0!=m_socket.startClient(svar.GetString("Master.ip","127.0.0.1"),
                                 svar.GetInt(nodeName+".port",30000)))
            {
                MSG_ERROR("Failed to connect %s",nodeName.c_str());
                return -1;
            }
        }

        start(NULL);
        m_bOpened=true;

        return 0;
    }


    int stop(void)
    {
        if( !m_bOpened ) {
            dbg_pe("Socket is not open!");
            return -1;
        }

        kill();
        m_socket.close();

        m_bOpened = false;
    }

    int send(DataElement ele)
    {
        push(ele);
        return 0;
    }

    int loopServer()
    {
        int    ret;
        while(1) {
            pi::RSocket new_socket;

            if( 0 != m_socket.accept(new_socket) ) {
                dbg_pe("server.accept failed!");
                continue;
            }

            while(1) {
                // send data to remote client
                m_mutex.lock();

                if( m_dataQueue.size() <= 0 ) {
                    m_mutex.unlock();

                    sleep(5);
                    continue;
                }

                DataElement d = m_dataQueue.front();
                m_dataQueue.pop_front();
                m_mutex.unlock();

                pi::RDataStream ds;
                d->toStream(ds);
                ret = new_socket.send(ds);

                if( ret < 0 ) {
                    dbg_pe("Connection lost!");
                    break;
                } else if( ret < ds.size() ) {
                    dbg_pw("Send data not correct!");
                    continue;
                }
            }

            return ret;

    FLIGHT_DATA_NEW_CONNECTION:
            {
                // clear data queue
                pi::RMutex  m(&m_mutex);
                m_dataQueue.clear();
            }
        }
    }

    int loopClient()
    {
        int ret;
        while( 1 ) {
            pi::RDataStream ds;
            ret = m_socket.recv(ds);
            if( ret < 0 ) {
                dbg_pe("Connection lost!");
                break;
            }
            DataElement ele(new DataType);
            ele->fromStream(ds);
            push(ele);
        }

        return ret;
    }

    virtual int thread_func(void *arg=NULL)
    {

        if(m_socket.isSever()) return loopServer();
        else return loopClient();
    }


private:
    pi::RSocket     m_socket;
    bool            m_bOpened;

    DataDeque       m_dataQueue;
    pi::RMutex      m_mutex;
};

template <class DataType>
class InternetTransfer: public pi::RMessagePassing
{
public:
    typedef typename SPtr<DataType> DataElement;
    typedef typename std::deque<DataElement> DataDeque;

public:
    InternetTransfer(const std::string& nodeName=""){
        if(nodeName.size())
            if(0!=begin(nodeName)) exit(0);
    }

    int size(){
        pi::RMutex lock(&m_mutex);
        return data.size();
    }

    void push(const DataElement& ele){
        pi::RMutex lock(&m_mutex);
        if(data.size()>10) data.pop_front();
        data.push_back(ele);
    }

    DataElement pop(){
        pi::RMutex lock(&m_mutex);
        if(data.size())
        {
            DataElement ele=data.front();
            data.pop_front();
            return ele;
        }
        else return DataElement();
    }

    int send(const DataElement& ele)
    {
        if(getNodeMap()->size()<2) return -1;
        pi::StringArray nl;

        // convert POSData to message
        pi::RMessage msg;
        ele->toStream(msg.data);
        msg.msgType = 1;
//        msg.msgID = m_msgID ++;

        getNodeMap()->getNodeList(nl);

        for(int i=0;i<nl.size();i++)
            if(nl[i]!="Master")
                if(sendMsg(nl[i],&msg)!=0)
                {
                    MSG_ERROR("Failed to send message.");
                    return -1;
                }

        return 0;
    }

    virtual int recvMessageSlot(pi::RMessage *msg)
    {
        if( 1 == msg->msgType ) {
            DataElement ele(new DataType);
            ele->fromStream(msg->data);
            push(ele);
        } else if ( 2 == msg->msgType ) {
            dbg_pt("received stop command!");
            exit(0);
        }

        // free the message
        delete msg;

        return 0;
    }

private:
    DataDeque               data;
    pi::RMutex              m_mutex;

};


#endif // TRACKEDIMAGE_H
