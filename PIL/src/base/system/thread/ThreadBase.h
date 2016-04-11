#ifndef THREAD_H_
#define THREAD_H_

// FIXME: only support POSIX system

//POSIX threads
#include <pthread.h>

namespace pi {
/** The Thread class encapsulates a thread of execution.  It is implemented with POSIX threads.
    Code that uses this class should link with libpthread and librt (for nanosleep).
*/
class Runnable
{
 public:
  //! Perform the function of this object.
   virtual void run()=0;
   virtual ~Runnable(){}
};

class Mutex
{
public:
    Mutex()
    {
//        m_mutex= PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_init(&m_mutex,NULL);
    }

    ~Mutex()
    {
         pthread_mutex_destroy(&m_mutex);
    }

    int lock()
    {
        return pthread_mutex_lock(&m_mutex);
    }

    int lock(unsigned int milli)
    {
        struct timespec ts = { milli/1000, (milli%1000)*1000000 };
        pthread_mutex_timedlock (&m_mutex,&ts);
    }

    int unlock()
    {
        return pthread_mutex_unlock(&m_mutex);
    }

    int trylock()
    {
        return pthread_mutex_trylock(&m_mutex);
    }

public:
    pthread_mutex_t m_mutex;
};

class ScopedMutex
{
public:
    ScopedMutex(Mutex& m_):m(&m_){m->lock();}
    ~ScopedMutex(){m->unlock();}
private:
    Mutex* m;
};

class MutexRW
{
public:
    MutexRW(){pthread_rwlock_init (&m_mutex,NULL);}
    ~MutexRW(){pthread_rwlock_destroy(&m_mutex);}

    inline int readLock(){return pthread_rwlock_rdlock(&m_mutex);}

    inline int tryReadLock(){return pthread_rwlock_tryrdlock(&m_mutex);}

    inline int writeLock(){return pthread_rwlock_wrlock(&m_mutex);}

    inline int tryWriteLock(){return pthread_rwlock_trywrlock(&m_mutex);}

    inline int lock(){return pthread_rwlock_wrlock(&m_mutex);}

    inline int unlock(){return pthread_rwlock_unlock(&m_mutex);}
public:
    pthread_rwlock_t m_mutex;
};

class ReadMutex
{
public:
    ReadMutex(MutexRW& m_):m(&m_){m->readLock();}
    ~ReadMutex(){m->unlock();}
private:
    MutexRW* m;
};

class WriteMutex
{
public:
    WriteMutex(MutexRW& m_):m(&m_){m->writeLock();}
    ~WriteMutex(){m->unlock();}
private:
    MutexRW* m;
};

class Thread : public Runnable
{
 public:
   //! Construct a thread.  If runnable != 0, use that runnable, else use our own "run" method.
   Thread();

   //! This does not destroy the object until the thread has been terminated.
   virtual ~Thread();

   //! Start execution of "run" method in separate thread.
   void start(Runnable* runnable=0);

   //! Tell the thread to stop.
   /** This doesn't make the thread actually stop, it just causes shouldStop() to return true. */
   virtual void stop();

   //! Returns true if the stop() method been called, false otherwise.
   bool shouldStop() const;

   //! Returns true if the thread is still running.
   bool isRunning() const;

   //! This blocks until the thread has actually terminated.
   /** If the thread is infinite looping, this will block forever! */
   void join();

   //! Get the ID of this thread.
   pthread_t getID();

   //! Override this method to do whatever it is the thread should do.
   virtual void run(){};

   //Static methods:

   //! Returns how many threads are actually running, not including the main thread.
   static unsigned int count();

   //! Returns a pointer to the currently running thread.
   static Thread* getCurrent();

   //! Tell the current thread to sleep for milli milliseconds
   static void sleep(unsigned int milli);

   //! Tell the current thread to yield the processor.
   static void yield();

 private:
   static bool init();
   static bool ourInitializedFlag;
   static void* threadproc(void* param);
   static pthread_key_t ourKey;
   static unsigned int ourCount;
   Runnable* myRunnable;
   pthread_t myID;
   bool myRunningFlag;
   bool myStopFlag;
};

}

#endif
