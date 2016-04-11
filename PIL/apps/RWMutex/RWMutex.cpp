//#########测试多线程,读写锁，递归锁
#include <boost/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/progress.hpp>
#include <boost/interprocess/detail/atomic.hpp>
#include <list>
#include <string>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <stdint.h>
#include <base/time/Global_Timer.h>
#include <base/Svar/Svar.h>

using namespace std;

int THREAD_COUNT= 4;
typedef boost::mutex                   Uni_Mutex;
typedef boost::shared_mutex            WR_Mutex;
typedef boost::unique_lock<WR_Mutex>   writeLock;
typedef boost::shared_lock<WR_Mutex>   readLock;

typedef boost::mutex         Rcs_Mutex;
typedef boost::unique_lock<Rcs_Mutex>  recuLock;

class Queue
{
public:
    Queue(){};
    ~Queue(){};

    void write(int value)
    {
        writeLock   lockWrite(m_rwMutex);
        m_list.push_back(value);
//        std::cout<<"write! now size is "<<size()<<" Thread id is "<<boost::this_thread::get_id()<<std::endl;
    }

    int  read(int pos)
    {
        readLock  lockRead(m_rwMutex);

        if(size()==0 || pos>=size())
            return -1;

        std::list<int>::iterator itr = m_list.begin();
        std::advance(itr , pos);
        int value = *itr;

//        std::cout<<"read! now size is "<<size()<<" Thread id is "<<boost::this_thread::get_id()<<std::endl;
        return value;
    }

    void remove(int pos)
    {
        writeLock  lockWrite(m_rwMutex);
        if(size()==0 || pos>=size())
            return;

        std::list<int>::iterator itr = m_list.begin();
        std::advance(itr , pos);
        m_list.erase(itr);

//        std::cout<<"remove! size is "<<size()<<" Thread id is "<<boost::this_thread::get_id()<<std::endl;
    }

    inline void sleep(int mode,int usec)
    {
        if(mode==0)
        {
            pi::ReadMutex lock(m_piMutex);
            usleep(usec);
        }
        else if(mode==1)
        {
            pi::WriteMutex lock(m_piMutex);
            usleep(usec);
        }
        else if(mode==2)
        {
            readLock lock(m_rwMutex);
            usleep(usec);
        }
        else if(mode==3)
        {
            writeLock lock(m_rwMutex);
            usleep(usec);
        }
        else if(mode==4)
        {
            pi::ScopedMutex lock(m_Mutex);
            usleep(usec);
        }
        else if(mode==5)
        {
//            recuLock lock(m_uniMutexBoost);
            m_uniMutexBoost.lock();
            usleep(usec);
            m_uniMutexBoost.unlock();
        }
        else if(mode==6)
        {
            read(5);
        }
        else if(mode==7)
        {
            write(5);
        }
        else if(mode==8)
        {
            remove(1);
        }
    }

    int  size()
    {
        recuLock lock(m_sizeMutex);
        return m_list.size();
    }

private:
    std::list<int>  m_list;
    Rcs_Mutex       m_sizeMutex;
    Uni_Mutex       m_uniMutexBoost;
    WR_Mutex        m_rwMutex;

    pi::MutexRW     m_piMutex;
    pi::Mutex       m_Mutex;
};

Queue queue;
volatile uint32_t   count1 = 0;

typedef boost::shared_ptr<boost::thread> Thread;

string modes[9]={"ReadPI","WritePI","ReadBoost","WriteBoost","UniMutexPI","UniMutexBoost","Read","Write","Remove"};
int mode=0;
string modeStr;
int circulate=10000;
int usec=10;

void increase_count()
{
//    boost::interprocess::detail::atomic_inc32(&count);
    count1++;
}

void  testdata()
{
    boost::this_thread::at_thread_exit(increase_count);
    stringstream sst;
    sst<<boost::this_thread::get_id()<<":Mode"<<mode<<":";
    for(int i=0;i<circulate;i++)
    {
        pi::timer.enter((sst.str()+modeStr).c_str());
        queue.sleep(mode-3,usec);
        pi::timer.leave((sst.str()+modeStr).c_str());

        pi::timer.enter((sst.str()+"Donothing").c_str());
        pi::timer.leave((sst.str()+"Donothing").c_str());

        pi::timer.enter((sst.str()+"Sleep0").c_str());
        usleep(0);
        pi::timer.leave((sst.str()+"Sleep0").c_str());

    }

    std::cout<<"Thread "<<boost::this_thread::get_id()<<" exit"<<std::endl;
}


int main(int argc , char* argv[])
{
    svar.ParseMain(argc,argv);
    mode=svar.GetInt("Mode",0);
    if(mode<0||mode>=9) return -1;
    circulate=svar.GetInt("Circle",circulate);
    THREAD_COUNT=svar.GetInt("Count",THREAD_COUNT);
    usec=svar.GetInt("Delay",usec);
    modeStr=modes[mode];

    std::vector<Thread>  thread_array(THREAD_COUNT);
    boost::progress_timer t;
    for(int i=0;i<10;i++)
        queue.write(i);

    std::cout<<"-----------thread create-----------"<<std::endl;

    for(int i=0;i<THREAD_COUNT;++i)
    {
        thread_array[i].reset(new boost::thread(testdata));
    }

    while(1)
    {
        if(count1>=THREAD_COUNT)
            return 0;
    }
}
