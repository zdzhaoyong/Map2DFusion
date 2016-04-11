/*******************************************************************************

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

#ifndef __RTK_OSA_PP__
#define __RTK_OSA_PP__

#include <stdio.h>
#include <stdint.h>

#include "base/types/types.h"
#include "base/debug/debug_config.h"
#include "osa.h"

namespace pi {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The RThread class
///
class RThread
{
public:
    RThread() {
        m_handle  = 0;
        m_arg     = NULL;
        m_res     = 0;
        m_isAlive = 0;
    }

    virtual ~RThread() {
        if( m_handle != 0 )
            osa_t_delete(m_handle);

        m_handle = 0;
        m_arg    = NULL;
        m_res    = 0;
    }

    ///
    /// \brief user implemented thread function
    /// \param arg - input argument
    /// \return 0 - success
    ///
    virtual int thread_func(void *arg=NULL) = 0;

    ///
    /// \brief start the thread
    /// \param arg        - argument passing to thread function
    /// \param stackSize  - stack size (default: 8M bytes)
    /// \param mode       - running mode (default is none of below)
    ///                         OSA_T_RT_PREEMPT: Realtime preempt policy (SCHED_RR)
    ///                         OSA_T_RT_FIFO:    Realtime nono preempt policy (SCHED_FIFO)
    ///                         OSA_T_NR_TSLICE:  Normal time share policy
    /// \return  0 - success
    ///
    int start(void *arg=NULL, ru32 stackSize=OSA_T_STACK_SIZE, ru32 mode=0);

    ///
    /// \brief kill a thread & release all resources
    ///
    /// \return
    ///     0  - success
    ///     -1 - thread is not exist
    ///
    int kill(void) {
        OSA_RESULT res=-1;

        if( m_handle == 0 ) return res;

        // kill thread
        if( m_isAlive )
            res = osa_t_kill(m_handle);

        // delete thread's resources
        res = osa_t_delete(m_handle);

        m_handle = 0;
        m_isAlive = 0;

        return res;
    }

    ///
    /// \brief free thread's resources (not used)
    /// \return
    ///     0  - success
    ///     -1 - thread is not exist
    ///
    int free(void) {
        return kill();
    }


    ///
    /// \brief suspend the thread
    ///
    /// \return
    ///     0                 - success
    ///     -1                - thread is not exist
    ///     E_OSA_T_BADHANDLE - thread handle is wrong
    ///     E_OSA_T_STOP      - thread has been stoped
    ///     E_OSA_T_SUSPEND   - thread has been suspended
    ///     E_OSA_LL_FUNC_ERR - plateform function error
    ///
    int suspend(void) {
        OSA_RESULT res=-1;

        if( m_handle != 0 ) {
            res = osa_t_suspend(m_handle);
        }

        return res;
    }

    ///
    /// \brief resume the thread
    ///
    /// \return
    ///     0                 - success
    ///     -1                - thread is not exist
    ///     E_OSA_T_BADHANDLE - thread handle is wrong
    ///     E_OSA_T_NOSUSPEND - thread has not been suspended
    ///     E_OSA_T_RUNNING   - thread is running
    ///     E_OSA_T_SUSPEND   - thread is still suspended
    ///     E_OSA_LL_FUNC_ERR - plateform function error
    ///
    int resume(void) {
        OSA_RESULT res=-1;

        if( m_handle != 0 ) {
            res = osa_t_resume(m_handle);
        }

        return res;
    }

    ///
    /// \brief sleep for some time (unit: ms)
    /// \param iTime - sleep time (in mill-second)
    ///
    /// \return
    ///     0   - success
    ///     -1  - interrupted by signal
    ///
    int sleep(ru32 iTime) {
        return osa_t_sleep(iTime);
    }

    ///
    /// \brief wait for the thread finished
    /// \param timeout - wait timeout value (in mill-second)
    ///                     default value is OSA_WAIT_INFINITE (wait forever)
    ///                     or can set to OSA_WAIT_NO (do not wait)
    ///
    /// \return
    ///     0                   - success
    ///     -1                  - thread is not exist
    ///     -2                  - do not wait, return immeditatly
    ///     E_OSA_T_BADHANDLE   - thread handle is wrong
    ///
    int wait(ru32 timeout=OSA_WAIT_INFINITE) {
        OSA_RESULT res=-1;

        if( m_handle != 0 ) {
            res = osa_t_wait(m_handle, timeout);
        }

        return res;
    }

    ///
    /// \brief reschedule the thread
    ///
    /// \return
    ///     0   - success
    ///     -1  - thread is not exist
    ///
    int reschedule() {
        OSA_RESULT res=-1;

        if( m_handle != 0 ) {
            res = osa_t_reschedule(m_handle);
        }

        return res;
    }

    ///
    /// \brief set thread specificed data (TSD)
    /// \param dat - TSD data
    ///
    /// \return
    ///     0           - success
    ///     E_OSA_T_TSD - failed to set TSD
    ///
    int setData(void *dat) {
        return osa_tsd_set(dat);
    }

    ///
    /// \brief get thread specificed data (TSD)
    /// \param dat - TSD data pointer
    ///
    /// \return
    ///     0           - success
    ///     -1          - data does not exist
    ///
    int getData(void **dat) {
        void*   _d;
        int     ret = 0;

        ret = osa_tsd_get(&_d);
        if( ret == 0 )
            *dat = _d;
        else
            return ret;

        return ret;
    }

    ///
    /// \brief get thread specificed data (TSD)
    /// \return TSD data pointer
    ///
    void* getData(void) {
        void*   _d;
        int     ret = 0;

        ret = osa_tsd_get(&_d);
        if( ret == 0 )
            return _d;
        else
            return NULL;
    }

    ///
    /// \brief get thread function return value
    /// \return thread return value
    ///
    int getResult(void) {
        return m_res;
    }

    ///
    /// \brief get thread is alive or not
    ///
    /// \return
    ///     0 - not alive
    ///     1 - alive
    ///
    int getAlive(void) {
        return m_isAlive;
    }

    ///
    /// \brief set thread is alive or not
    /// \param alive    0 - not alive
    ///                 1 - alive
    ///
    void setAlive(int alive) {
        m_isAlive = alive;
    }

    ///
    /// \brief get thread ID
    /// \return thread ID number
    ///
    ru32 get_id(void) {
        ru32 id = 0;

        if( m_handle != 0 ) {
            id = osa_t_getid(m_handle);
        }

        return id;
    }

    ///
    /// \brief set thread priority
    /// \param iPriority - thread priority
    ///
    /// \return
    ///     0                   - success
    ///     -1                  - thread is not exist
    ///     E_OSA_T_BADHANDLE   - thread handle is wrong
    ///     E_OSA_T_BADPRIORITY - priority value is wrong
    ///     E_OSA_LL_FUNC_ERR   - plateform function error
    ///
    int setPriority(int iPriority) {
        OSA_RESULT      res=-1;
        OSA_PRIORITY    pri;

        pri = iPriority;

        if( m_handle != 0 ) {
            res = osa_t_setPriority(m_handle, pri);
        }

        return res;
    }

    ///
    /// \brief get thread's priority
    /// \param piPriority - thread priority
    ///
    /// \return
    ///     0                  - success
    ///     -1                 - thread is not exist
    ///     E_OSA_T_BADHANDLE   - thread handle is wrong
    ///
    int getPriority(int *piPriority) {
        OSA_RESULT      res=-1;
        OSA_PRIORITY    pri;

        if( m_handle != 0 ) {
            res = osa_t_getPriority(m_handle, &pri);
            *piPriority = pri;
        }

        return res;
    }

protected:
    OSA_HANDLE  m_handle;           ///< thread hanle
    void        *m_arg;             ///< thread function's argument
    int         m_res;              ///< return code
    int         m_isAlive;          ///< is alive or not

    /** internal used thread function */
    friend OSA_RESULT RThread_thread_func(OSA_T_ARGS arg);
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Mutex class
///
class RMutex
{
public:
    RMutex(int mode=0) {
        m_scopeMutex = NULL;

        if( 0 != osa_mu_create(&m_handle, mode) ) {
            m_handle = 0;
        }
    }

    virtual ~RMutex() {
        if( m_scopeMutex != NULL ) m_scopeMutex->unlock();

        if( m_handle != 0 ) {
            osa_mu_delete(m_handle);
            m_handle = 0;
        }
    }

    RMutex(RMutex *m) {
        m_handle = 0;

        m_scopeMutex = m;
        if( m_scopeMutex != NULL ) m_scopeMutex->lock();
    }


    ///
    /// \brief lock the mutex
    ///
    /// \return
    ///         0					Success
    ///         OSA_OSA_MU_HANDLE	Input handle error
    ///         OSA_OSA_MU_LOCK		Lock failed
    ///
    int lock(void) {
        return osa_mu_lock(m_handle);
    }

    ///
    /// \brief lock with given timeout value
    /// \param iTime - timeout value (default is infinite)
    ///
    /// \return
    ///     0					Success
    ///     E_PLOSA_MU_HANDLE	Input handle error
    /// 	E_PLOSA_MU_TIMEOUT	Wait timeout
    ///     E_PLOSA_MU_LOCK		Failed wait
    ///
    int	lockTimeout(ru32 iTime=OSA_WAIT_INFINITE) {
        return osa_mu_lock_timeout(m_handle, iTime);
    }

    ///
    /// \brief unlock the mutex
    ///
    /// \return
    ///     0					Success
    ///     E_PLOSA_MU_HANDLE	input handle error
    ///     E_PLOSA_MU_UNLOCK	Failed unlock
    ///
    int unlock(void) {
        return osa_mu_unlock(m_handle);
    }

    ///
    /// \brief determin mutex is lock or not
    ///
    /// \return
    ///     0					Success ( unlock )
    ///     E_PLOSA_MU_HANDLE	input handle error
    /// 	E_PLOSA_MU_LOCK		has locked
    ///
    int isLock(void) {
        OSA_RESULT  res;

        res = osa_mu_isLock(m_handle);

        if( res == 0 )
            return 0;
        else if ( res == E_OSA_MU_LOCK )
            return res;
        else
            return -1;
    }


protected:
    OSA_HANDLE  m_handle;               ///< mutex handle
    RMutex      *m_scopeMutex;          ///< scope mutex
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Semaphore class
///
class RSemaphore
{
public:
    ///
    /// \brief RSemaphore construction function
    /// \param iInitVal - initial semaphore value
    /// \param mode - do not used
    ///
    RSemaphore(int iInitVal=0, ru32 mode=0) {
        if( 0 != osa_sem_create(&m_handle, iInitVal, mode) ) {
            m_handle = 0;
        }
    }

    virtual ~RSemaphore() {
        if( m_handle != 0 ) {
            osa_sem_delete(m_handle);
            m_handle = 0;
        }
    }

    ///
    /// \brief wait for the semaphore
    /// \return
    ///     0                   - success
    ///     E_OSA_SEM_HANDLE    - wrong handle
    ///
    int wait(void) {
        return osa_sem_wait(m_handle);
    }

    ///
    /// \brief wait for the semaphore with a given timeout value
    /// \param iTimeout - timeout value (default is infinite)
    ///
    /// \return
    ///     0					Success
    ///     E_OSA_SEM_HANDLE	input handle error
    ///     E_OSA_SEM_TIMEOUT	wait timeout
    ///     E_OSA_SEM_WAIT      Wait failed
    ///
    int waitTimeout(ru32 iTimeout=OSA_WAIT_INFINITE) {
        return osa_sem_wait_timeout(m_handle, iTimeout);
    }

    ///
    /// \brief signal the semaphore
    ///
    /// \return
    ///     0					Success
    ///     E_OSA_SEM_HANDLE	Input handle error
    ///     E_OSA_SEM_SIGNAL	Failed to signal
    ///
    int signal(void) {
        return osa_sem_signal(m_handle);
    }

    ///
    /// \brief get semaphore value
    ///
    /// \param piVal - semaphore value
    ///
    /// \return
    ///     0					Success
    ///     E_OSA_SEM_HANDLE	Input handle error
    ///
    int getValue(int *piVal) {
        OSA_RESULT  res;
        ri32       val;

        res = osa_sem_getValue(m_handle, &val);
        *piVal = val;

        return res;
    }

protected:
    OSA_HANDLE  m_handle;               ///< semaphore handle
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Timer class
///
class RTimer
{
public:
    RTimer() {
        m_handle  = 0;
        m_iPeriod = 50;
        m_mode    = OSA_TM_PRO_PERIODIC;
        m_state   = 0;
        m_arg     = NULL;
        m_count   = 0;
    }

    RTimer(ru32 iPeriod, ru32 mode = OSA_TM_PRO_PERIODIC) {
        m_handle  = 0;
        m_iPeriod = iPeriod;
        m_mode    = mode;
        m_state   = 0;
        m_arg     = NULL;
        m_count   = 0;
    }

    virtual ~RTimer() {
        if( m_handle != 0 )
            osa_tm_delete(m_handle);

        m_handle = 0;
    }

    ///
    /// \brief user implemented timer function
    /// \param arg - argument for timer function
    ///
    virtual void timer_func(void *arg) = 0;

    ///
    /// \brief start the timer
    ///
    /// \param iPeriod      - running period
    /// \param arg          - argument for timer function
    /// \param mode         - running mode
    ///                         OSA_TM_PRO_RUNONCE  - just run once
    ///                         OSA_TM_PRO_PERIODIC - periodic running
    ///
    /// \return
    ///     0                   - success
    ///     E_OSA_TM_CREATE     - failed to create the timer
    ///
    int start(ru32 iPeriod, void *arg = NULL, ru32 mode = OSA_TM_PRO_PERIODIC);

    ///
    /// \brief start the timer by the default settings
    ///
    /// \param arg          - argument for timer function (defult is NULL)
    /// \param mode         - running mode with OSA_TM_PRO_PERIODIC
    ///
    /// \return
    ///     0                   - success
    ///     E_OSA_TM_CREATE     - failed to create the timer
    ///
    int start(void *arg = NULL);

    ///
    /// \brief pause the timer
    /// \return
    ///     0               - success
    ///
    int pause(void) {
        m_state = 1;
        return 0;
    }

    ///
    /// \brief resume the timer
    ///
    /// \return
    ///     0               - success
    int resume(void) {
        m_state = 0;
        return 0;
    }

    ///
    /// \brief stop the timer
    ///
    /// \return
    ///     0               - success
    ///     E_OSA_TM_HANDLE - timer handle is wrong
    ///
    int stop(void) {
        OSA_RESULT  res;

        m_state = 1;
        res = osa_tm_delete(m_handle);

        m_handle = 0;
        m_count  = 0;

        return res;
    }

protected:
    OSA_HANDLE  m_handle;                   ///< timer handle
    ru32        m_count;                    ///< count number

    ru32        m_iPeriod;                  ///< running period
    ru32        m_mode;                     ///< running mode
                                            ///     OSA_TM_PRO_PERIODIC
                                            ///     OSA_TM_PRO_RUNONCE

    void        *m_arg;                     ///< arguments for timer function

    int         m_state;                    ///< current state

    /** timer inner function */
    friend void RTimer_timerFunc(void *arg);
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The MessageQueue class
///
class RMessageQueue
{
public:
    RMessageQueue() {
        m_handle = 0;
    }

    ///
    /// \brief RMessageQueue construction function
    ///
    /// \param maxNum       - maximum queue size (default is 64)
    /// \param maxLeng      - maximum message size (default is 4k)
    /// \param mode         - message queue mode
    ///                         OSA_MQ_FIFO     - FIFO (default)
    ///                         OSA_MQ_PRIORITY - with priority
    ///                         OSA_MQ_TIMEOUT  - with timeout
    ///
    RMessageQueue(ru32 maxNum = 64, ru32 maxLeng = 4096, ru32 mode = OSA_MQ_FIFO) {
        int ret = osa_mq_create(&m_handle, maxNum, maxLeng, mode);
        if( ret != 0 )
            m_handle = 0;
    }

    virtual ~RMessageQueue() {
        if( m_handle != 0 ) {
            osa_mq_delete(m_handle);
            m_handle = 0;
        }
    }

    ///
    /// \brief send a message to queue
    ///
    /// \param dat          - message
    /// \param len          - message length
    /// \param priority     - priority
    ///
    /// \return
    ///     0               - success
    ///     E_OSA_MQ_HANDLE - handle is wrong
    ///     E_OSA_MQ_MAXNUM - reach queue's maximum size
    ///
    int send(void *dat, ru32 len,
                    ru32 priority=OSA_MQ_PRIORITY_NORM) {
        return osa_mq_send(m_handle, dat, len, priority);
    }

    ///
    /// \brief send a message to queue with a given timeout (return if timeout)
    ///
    /// FIXME: not implemented yet!
    ///
    /// \param dat          - message
    /// \param len          - message length
    /// \param priority     - priority number
    /// \param timeout      - timeout value
    ///
    /// \return
    ///     0               - success
    ///
    int sendTimeout(void *dat, ru32 len,
                    ru32 priority=OSA_MQ_PRIORITY_NORM,
                    ru32 timeout=OSA_WAIT_NO) {
        return osa_mq_send_timeout(m_handle, dat, len, priority, timeout);
    }


    ///
    /// \brief receive a message from queue
    ///
    /// \param dat          - message
    /// \param len          - message length
    /// \param priority     - priority value
    ///
    /// \return
    ///     0               - success
    ///     E_OSA_MQ_HANDLE - handle is wrong
    int recv(void *dat, ru32 *len,
                    ru32 *priority=NULL) {
        ru32 _pri;

        if( priority == NULL )
            return osa_mq_receive(m_handle, dat, len, &_pri);
        else
            return osa_mq_receive(m_handle, dat, len, priority);
    }

    ///
    /// \brief receive a message from queue with timeout (return if timeout)
    ///
    /// FIXME: not implemented yet!
    ///
    /// \param dat          - message
    /// \param len          - message length
    /// \param priority     - priority value
    /// \param timeout      - timeout
    ///
    /// \return
    ///     0               - success
    ///
    int recvTimeout(void *dat, ru32 *len,
                    ru32 *priority=NULL,
                    ru32 timeout=OSA_WAIT_NO) {
        ru32 _pri;

        if( priority == NULL )
            return osa_mq_receive_timeout(m_handle, dat, len, &_pri, timeout);
        else
            return osa_mq_receive_timeout(m_handle, dat, len, priority, timeout);
    }

protected:
    OSA_HANDLE  m_handle;               ///< message queue handle
};



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief return system status
///
/// \return
///     1           - running
///     0           - stopped
///
int isRunning(void);

///
/// \brief set system status
///
/// \param s        - status of system
///                     1 - run
///                     0 - stop
///
/// \return
///
int setStop(int s=0);



} // end of namespace RTK

#endif // end of __RTK_OSA_PP__
