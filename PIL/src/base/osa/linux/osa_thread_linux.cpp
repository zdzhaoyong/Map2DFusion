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

#ifdef PIL_LINUX


#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <errno.h>
#include <sys/timeb.h>
#include <sys/types.h>

#include "../osa.h"
#include "../_osa_in.h"


namespace pi {


/* define thread signals */
#define THREAD_SIG_SUSPEND  SIGUSR2
#define THREAD_SIG_CONTINUE THREAD_SIG_SUSPEND
#define THREAD_SIG_KILL     SIGQUIT


/* Define max & min Thread priority */
#define OSA_T_LNX_PRIORITY_MAX      99
#define OSA_T_LNX_PRIORITY_MIN      0
#define OSA_T_LNX_PRIORITY_DEFAULT  0

/* define Thread Sates */
#define OSA_TS_READY    0
#define OSA_TS_RUN      1
#define OSA_TS_SUSPEND  2
#define OSA_TS_STOP     3

/* define Thread operations */
#define OSA_TO_CREATE   0
#define OSA_TO_SUSPEND  1
#define OSA_TO_CONTINUE 2
#define OSA_TO_KILL     3


/* private used thread handle */
typedef struct ThreadHandle {
    list_t    list;
    ru32      magic;
    ru32      id;
    
    ri32      ret;

    ru32      state;
    ru32      op;
    ru32      priority;

    ru32      suspend_count;
    
    pthread_t   threadid;
    
    sem_t       th_sem;
    sem_t       suspend_sem;
    sem_t       kill_sem;
} ThreadHandle_t;

typedef struct ThreadParam {
    OSA_T_ARGS      arg;
    ThreadHandle_t  *handle;
    OSA_T_FUNC      func;
} ThreadParam_t;


static ru32 g_TaskID = 1;

static ThreadHandle_t *g_ThreadList=NULL;

/* local used functions */
static void *_osa_t_create(void *arg);
static inline OSA_RESULT _osa_t_delete(ThreadHandle_t *handle);

/* signal handle function */
static void _sig_suspend_handler(ri32 signo);
static void _sig_kill_handler(ri32 signo);
static void _sig_cont_handler(ri32 signo);



/******************************************************************************
 *  Local: check if thread handle correct
 *
 *  Parameter:
 *      handle                  [in] thread handle
 *
 *  Return value:
 *      0                       Success
 *      -1                      handle error (MAGIC number error)
 *****************************************************************************/
inline OSA_RESULT _osa_t_checkHandle(struct ThreadHandle *handle)
{
    if( NULL != handle ) {
        try {
            if( handle->magic == OSA_T_MAGIC )
                return 0;
            else
                return -1;
        } catch(...) {
            return -1;
        }
    } else
        return -1;
}

/******************************************************************************
 *  Local: setup signal functions
 *
 *  Parameter:
 *      None
 *
 *  Return value:
 *      0                       Success
 *      -1                      Failed
 *****************************************************************************/
OSA_RESULT _osa_t_setup_signal(void)
{
    ri32                ret;
    struct sigaction    act;

    /* setup SUSPEND signal handle */
    act.sa_flags = SA_RESTART;
    act.sa_handler = _sig_suspend_handler;
    sigemptyset(&act.sa_mask);
    ret = sigaction(THREAD_SIG_SUSPEND, &act, 0);
    if ( 0 != ret ) {
        d1(printf( "osa_t_init: failed at sigaction!\n"));
        return -1;
    }

    /* setup KILL signal handle */
    act.sa_flags = SA_RESTART;
    act.sa_handler = _sig_kill_handler;
    sigemptyset(&act.sa_mask);
    ret = sigaction(THREAD_SIG_KILL, &act, 0);
    if ( 0 != ret ) {
        d1(printf( "osa_t_init: failed to regist KILL signal handler\n"));
        return -1;
    }

    /* setup SIGCONT signal handler */
    act.sa_flags = SA_RESTART;
    act.sa_handler = _sig_cont_handler;
    sigemptyset(&act.sa_mask);
    ret = sigaction(SIGCONT, &act, 0);
    if ( 0 != ret ) {
        d1(printf("osa_t_init: failed to regist SIGCONT signal handler\n"));
        return -1;
    }
}

/******************************************************************************
 *  Initialize Portable layer Thread functions
 *
 *  Parameter:
 *      NONE
 *  Return value:
 *      0                   successful
 *****************************************************************************/
OSA_RESULT  osa_t_init(void)
{
    struct sched_param  sparam;
    pid_t               mypid;

    d3(printf("osa_t_init:\n"));
    
    /* setup thread handle list */
    g_ThreadList = (ThreadHandle_t*) malloc( sizeof(ThreadHandle_t) );
    memset(g_ThreadList, 0, sizeof(ThreadHandle_t));
    INIT_LIST_HEAD( &(g_ThreadList->list) );
    g_ThreadList->id = 0;
    g_ThreadList->magic = OSA_T_MAGIC;

    /* create, set tid tsd */
    if( _osa_tid_key == -1 )
        _osa_tsd_create(&_osa_tid_key);
    _osa_tsd_set( &_osa_tid_key, (size_t) g_ThreadList);

    /* set my schedule policy ?*/
    #if 1
    mypid = getpid();
    sched_getparam(mypid,&sparam);
    sparam.sched_priority = (OSA_T_LNX_PRIORITY_MAX - OSA_T_LNX_PRIORITY_MIN + 1)/2;
    sched_setscheduler(mypid, SCHED_RR, &sparam);
    //sched_setscheduler(mypid,SCHED_OTHER,&sparam);
    #endif

    // setup thread signal actions
    _osa_t_setup_signal();

    d3(printf("osa_t_init: end\n"));
    
    return 0;
}

/******************************************************************************
 *  Release Portable layer Thread enviroment
 *
 *  Parameter:
 *      NONE
 *  Return value:
 *      0                   successful
 *****************************************************************************/
OSA_RESULT  osa_t_release(void)
{
    list_t          *list;
    ThreadHandle_t  *handle;

    d3(printf("osa_t_release:\n"));

    if( g_ThreadList ) {
        /* stop all thread & delete thread handle list */
        list = &(g_ThreadList->list);
        while( list->next != list->prev ) {
            handle = list_entry(list->next, ThreadHandle_t, list);

            /* if it state isn't STOP then stop it */
            if( handle->state != OSA_TS_STOP ) {
                osa_t_kill((OSA_HANDLE) handle);
            }

            /* delete thread resource */
            osa_t_delete( (OSA_HANDLE) handle );
        }
    }

    d3(printf("osa_t_release: end\n"));
    
    return 0;
}

/******************************************************************************
 *  Create a thread
 *
 *  Parameter:
 *      hTask               [out] Thread's handle
 *      func                [in] Thread function
 *      arg                 [in] Thread function's argument
 *      stacksize           [in] Thread function's stack size
 *      priority            [in] Priority level
 *      mode                [in] Schedule policy valiad value:
 *                              OSA_T_RT_PREEMPT: Realtime preempt policy (SCHED_RR)
 *                              OSA_T_RT_FIFO: Realtime nono preempt policy (SCHED_FIFO)
 *                              OSA_T_NR_TSLICE: Normal time share policy
 *
 *  Return value:
 *      0                   success
 *****************************************************************************/
OSA_RESULT  osa_t_create(OSA_HANDLE *hTask, OSA_T_FUNC func, OSA_T_ARGS arg,
                ru32 stackSize, ru32 mode)
{
    ThreadHandle_t      *pHandle;
    ThreadParam_t       *param;
    ri32                ret;
    pthread_attr_t      attr;
    struct sched_param  schedparam;

    d3(printf("osa_t_create:\n"));
    
    // if thread sub-system is not initialized
    if( _osa_tid_key == -1 ) {
        // setup thread signal actions
        _osa_t_setup_signal();

        // create TSD key
        _osa_tsd_create(&_osa_tid_key);
    }

    // init pthread attr struction
    ret = pthread_attr_init(&attr);
    
    /* set stack size */
    pthread_attr_setstacksize(&attr, stackSize); 
    ret |= pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    /* set schedule parameters */
    schedparam.sched_priority = (OSA_T_LNX_PRIORITY_MAX - OSA_T_LNX_PRIORITY_MIN)/2;
    if (mode & OSA_T_RT_PREEMPT)
        ret = pthread_attr_setschedpolicy(&attr,SCHED_RR);
    else if( mode & OSA_T_RT_FIFO )
        ret = pthread_attr_setschedpolicy(&attr,SCHED_FIFO);
    else {
        ret = pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
        schedparam.sched_priority = 0;
    }
    ret = pthread_attr_setschedparam(&attr, &schedparam);

    /* alloc a thread handle */
    pHandle = (ThreadHandle_t*) malloc(sizeof(ThreadHandle_t));
    INIT_LIST_HEAD( &(pHandle->list) );
    if( g_ThreadList )
        list_add( &(pHandle->list), &(g_ThreadList->list) );
    pHandle->priority = schedparam.sched_priority;
    
    param = (ThreadParam_t*) malloc( sizeof(ThreadParam_t) );
    param->arg    = arg;
    param->func   = func;
    param->handle = pHandle;

    pHandle->suspend_count = 0;
    sem_init( &(pHandle->th_sem),      0, 0);
    sem_init( &(pHandle->suspend_sem), 0, 0);
    sem_init( &(pHandle->kill_sem),    0, 0);
    
    pHandle->magic = OSA_T_MAGIC;
    pHandle->id    = g_TaskID ++;
    pHandle->state = OSA_TS_READY;
    pHandle->op    = OSA_TO_CREATE;

    *hTask = (OSA_HANDLE) pHandle;

    // create pthread routine
    ret = pthread_create(&(pHandle->threadid), &attr, 
                        _osa_t_create, param);

    // wait created thread begin run
    sem_wait( &(pHandle->th_sem) );

    d3(printf("osa_t_create: end\n"));

    return 0;
}

/******************************************************************************
 *  local used function: wrap function for create thread
 *
 *  Parameter:
 *      arg                 [in] input data
 *  Return value:
 *      
 *****************************************************************************/
static void *_osa_t_create(void *arg)
{
    ThreadParam_t   param;
    ThreadHandle_t  *pHandle;
    ri32            ret;

    d3(printf("_osa_t_create:\n"));
    
    // copy argument & free old argument
    memcpy( &param, arg, sizeof(ThreadParam_t) );
    free(arg);
    pHandle = param.handle;

    /* save thread handle to TSD */
    _osa_tsd_set(&_osa_tid_key, (size_t) pHandle);

    // resume caller thread
    sem_post(&(pHandle->th_sem));
    sem_post(&(pHandle->th_sem));

    pHandle->state = OSA_TS_RUN;

    /* call Thread function */
    ret = param.func(param.arg);

    sem_wait( &(pHandle->th_sem) );
    pHandle->state = OSA_TS_STOP;
    pHandle->ret   = ret;
    sem_post( &(pHandle->th_sem) );
    
    pthread_exit(0);

    return NULL;
}

/******************************************************************************
 *  Local: delete thread resource
 *
 *  Parameter:
 *      handle                  [in] Thread's descriptor
 *
 *  Return value:
 *      0                       Success
 *****************************************************************************/
static inline OSA_RESULT _osa_t_delete(ThreadHandle_t *handle)
{
    //_osa_tsd_set( &_osa_tid_key, 0 );
    sem_destroy( &(handle->th_sem) );
    sem_destroy( &(handle->suspend_sem) );
    sem_destroy( &(handle->kill_sem) );

    return 0;   
}

/******************************************************************************
 *  Terminate a thread ( stop a thread's running)
 *
 *  Parameter:
 *      hTask               [in] Task's handle
 *
 *  Return value:
 *      0                   Success
 *****************************************************************************/
OSA_RESULT  osa_t_kill(OSA_HANDLE hTask)
{
    ri32            ret;
    ThreadHandle_t  *handle, 
                    *self_handle;

    d3(printf("osa_t_kill:\n"));
    
    // get self thread handle
    self_handle = (ThreadHandle_t *) osa_t_self();

    // if input thread handle is not 0 (other thread)
    if( 0 != hTask ) {
        handle = (ThreadHandle_t *) hTask;
        if( _osa_t_checkHandle(handle) ) {
            d1(printf("osa_t_kill: input handle error!\n"));
            return E_OSA_T_BADHANDLE;
        }

        // check input handle is self handle
        if( self_handle != NULL ) {
            if( self_handle->id == handle->id )
                goto OSA_T_KILL_SELF;
        }

        d3(printf("\tThread ID:%d\n", handle->id));

        sem_wait( &(handle->th_sem) );
        if( handle->state == OSA_TS_STOP ) {
            sem_post( &(handle->th_sem) );           
            return E_OSA_T_STOP;
        }
        
        handle->state = OSA_TS_STOP;
        sem_post( &(handle->th_sem) );

        d3(printf("osa_t_kill: send THREAD_SIG_KILL\n"));
        
        // send kill signal
        ret = pthread_kill(handle->threadid, THREAD_SIG_KILL);
        if( ret ) {
            d1(printf("osa_t_kill: failed to send THREAD_SIG_KILL signal!\n"));
            return -1;
        }

        sem_wait( &(handle->kill_sem) );

        d3(printf("osa_t_kill: end\n"));
        return 0;
    }
    
OSA_T_KILL_SELF:
    handle = self_handle;

    // check self handle is legal
    if( handle == NULL ) {
        goto OSA_T_KILL_SELF_FAIL;
    } else {
        try {
            if( handle->magic != OSA_T_MAGIC ) {
                goto OSA_T_KILL_SELF_FAIL;
            }
        } catch(...) {
            goto OSA_T_KILL_SELF_FAIL;
        }
    }

    goto OSA_T_KILL_SELF_CONTINUE;

OSA_T_KILL_SELF_FAIL:
    d1(printf("osa_t_kill: self handle is not correct!\n"));
    return -1;

OSA_T_KILL_SELF_CONTINUE:

    d3(printf("\tThread ID:%d\n", handle->id));

    sem_wait( &(handle->th_sem) );
    if( handle->state == OSA_TS_STOP ) {
        sem_post( &(handle->th_sem) );
        d2(printf("osa_t_kill: thread have been stoped!\n"));
        
        return E_OSA_T_STOP;
    }
    
    //d1(printf(">> set thread state to OSA_TS_STOP 3\n")); fflush(stdout);
    handle->state = OSA_TS_STOP;
    sem_post( &(handle->th_sem) );

    _osa_tsd_set( &_osa_tid_key, 0 );

    pthread_exit(0);

    return 0;
}

/******************************************************************************
 *  Delete a thread handle 
 *
 *  Parameter:
 *      hTask               [in] Thread's handle
 *
 *  Return value:
 *      0                   Success
 *      E_OSA_T_NOSTOP      Thead didn't stoped yet. (When delete a thread,
 *                              it must have been stoped!)
 *      E_OSA_T_BADHANDLE   Handle error (may be deleted)
 *****************************************************************************/
OSA_RESULT  osa_t_delete(OSA_HANDLE hTask)
{
    ThreadHandle_t  *self_handle, 
                    *handle;

    d3(printf("osa_t_delete: \n"));
    
    self_handle = (ThreadHandle_t *) osa_t_self();

    if( hTask == 0 ) {
        /* delete myself */
        handle = self_handle;
    } else {
        /* delete other handle */
        handle = (ThreadHandle_t *) hTask;
    }

    /* check thread handle */
    if( _osa_t_checkHandle(handle) ) {
        d1(printf("osa_t_delete: input handle error!\n"));
        return E_OSA_T_BADHANDLE;
    }

    /* check thread state */
    if( handle->state != OSA_TS_STOP ) {
        osa_t_kill(hTask);

        //d1(printf("osa_t_delete: thread didn't stop!\n"));
        //return E_OSA_T_NOSTOP;
    }

    /* free thread descriptor resource */
    _osa_t_delete( handle );
    
    /* remove from thread list and free it */
    if( g_ThreadList )
        list_del( &(handle->list) );
    memset(handle, 0, sizeof(ThreadHandle_t));
    free(handle);

    d3(printf("osa_t_delete: end\n"));
    
    return 0;
}

/******************************************************************************
 *  Get myself handle
 *
 *  Parameter:
 *      NONE
 *
 *  Return value:
 *      Thread handle
 *****************************************************************************/
OSA_HANDLE  osa_t_self(void)
{
    OSA_HANDLE      handle;
    ThreadHandle_t  *t;

    // get current thread handle
    _osa_tsd_get(&_osa_tid_key, &handle);

    // check obtained handle is correct
    if( handle != 0 ) {
        try {
            t = (ThreadHandle_t*) handle;
            if( t->magic == OSA_T_MAGIC )
                return handle;
        } catch(...) {
            return 0;
        }
    }

    // return self handle
    return handle;
}

/*****************************************************************************
 *  Wait for given thread finish
 *
 *  Parameter:
 *      hTask                   [in] Thread handle
 *      timeout                 [in] timeout (in ms)
 *
 *  Return value:
 *      0                       Success
 *****************************************************************************/
OSA_RESULT	osa_t_wait(OSA_HANDLE hTask, ru32 timeout)
{
    ThreadHandle_t  *handle;
    ru64            t_beg, t_end, t_now;

    // get handle & check it
    handle = (ThreadHandle_t *) hTask;
    if( _osa_t_checkHandle(handle) ) {
        d1(printf("osa_t_wait: input handle error!\n"));
        return E_OSA_T_BADHANDLE;
    }

    // if do not wait
    if( timeout == OSA_WAIT_NO ) {
        return -2;
    }

    // calculate end time
    osa_tm_getMillis(&t_beg);
    t_end = t_beg + timeout;

    d3(printf("\tThread ID:%d\n", handle->id));

    while(1) {
        // check given thread is stoped
        if( handle->state == OSA_TS_STOP ) {
            return handle->ret;
        }

        // check time-out
        if( timeout != OSA_WAIT_INFINITE ) {
            osa_tm_getMillis(&t_now);
            if( t_now > t_end ) {
                return 0;
            }
        }

        // sleep for some time
        osa_t_sleep(10);
    }
}

/*****************************************************************************
 *  Get current thread ID
 *
 *  Parameter:
 *      hTask                   Input thread handle
 *                                  0     - for self thread
 *                                  other - get speficed thead's ID
 *
 *  Return value:
 *      0                       Failed
 *      other                   Thread ID
 *****************************************************************************/
ru32 osa_t_getid(OSA_HANDLE hTask)
{
    OSA_HANDLE      handle;
    ThreadHandle_t  *handle_in;

    if( hTask == 0 ) {
        handle = osa_t_self();
        if( handle != 0 ) {
            handle_in = (ThreadHandle_t*) handle;
            return handle_in->id;
        }
    } else {
        handle_in = (ThreadHandle_t*) hTask;
        if( _osa_t_checkHandle(handle_in) ) {
            d1(printf("osa_t_getid: input handle error!\n"));
            return 0;
        }

        return handle_in->id;
    }

    return 0;
}


/*****************************************************************************
 *  Sleep for some period ( on ms )
 *
 *  Parameter:
 *      iTime                   [in] Time ( on ms )
 *
 *  Return value:
 *      0                       Success
 *****************************************************************************/
OSA_RESULT  osa_t_sleep(ru32 iTime)
{
    struct timespec tp;
    ri32            ret;
    
    tp.tv_sec = iTime / 1000;
    tp.tv_nsec = ( iTime % 1000 ) * 1000000;
    ret = nanosleep(&tp, &tp);
    
    return ret ;
}

/******************************************************************************
 *  Suspend a thread
 *
 *  Parameter:
 *      hTask               [in] Thread's handle ( 0 - myself )
 *
 *  Return value:
 *      0                   Success
 *      E_OSA_T_BADHANDLE   input handle error
 *****************************************************************************/
OSA_RESULT  osa_t_suspend(OSA_HANDLE hTask)
{
    ThreadHandle_t  *handle_self, *handle_t;
    sigset_t        mask;
    ri32            ret;

    d3(printf( "osa_t_suspend:\n"));
    
    handle_self = (ThreadHandle_t *) osa_t_self();

    /* suspend other thread */
    if ( hTask != 0 ) {
        handle_t = (ThreadHandle_t *) hTask;
        if( _osa_t_checkHandle( handle_t ) ) {
            d1(printf("osa_t_suspend: Input handle error!\n"));
            return E_OSA_T_BADHANDLE;
        }

        d3(printf("\tThreadID:%d\n", handle_t->id));
        
        /* if current thread is self */
        if( handle_self != 0 ) {
            if( handle_self->id == handle_t->id )
                goto OSA_T_SUSPEND_SELF;
        }

        /* suspend other thread */
        sem_wait( &(handle_t->th_sem) );

        /* if it has been suspend or stoped */
        if( handle_t->state == OSA_TS_STOP ) {
            sem_post( &(handle_t->th_sem) );

            d2(printf("osa_t_suspend: thread have been stoped!\n"));
            return E_OSA_T_STOP;
        }

        /* check if it has been suspend */
        handle_t->suspend_count++;
        if( handle_t->suspend_count > 1 ) {
            sem_post( &(handle_t->th_sem) );

            d2(printf("osa_t_suspend: thread have been suspend!(%d)\n",
                            handle_t->suspend_count));
            return E_OSA_T_SUSPEND;
        }

        /* now send suspend signal to target thread */
        handle_t->op = OSA_TO_SUSPEND;
        sem_post( &(handle_t->th_sem) );

        ret = pthread_kill(handle_t->threadid, THREAD_SIG_SUSPEND);
        if( 0 != ret ) {
            d1(printf("osa_t_suspend: send THREAD_SIG_SUSPEND failed!\n"));
            return E_OSA_LL_FUNC_ERR;
        }

        /* wait signale handler run */
        sem_wait( &(handle_t->suspend_sem) );

        return 0;
    }

OSA_T_SUSPEND_SELF:
    d3(printf("\tThreadID:%d\n", handle_self->id));

    /* suspend myself */
    sem_wait( &(handle_self->th_sem) );
    
    /* increase suspend count */
    handle_self->suspend_count ++;
    handle_self->state = OSA_TS_SUSPEND;
    handle_self->op = OSA_TO_SUSPEND;
    
    sigemptyset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, &mask);
    sigdelset(&mask, THREAD_SIG_CONTINUE);

    do {
        sem_post( &(handle_self->th_sem) );

        sigsuspend(&mask);

        sem_wait(&(handle_self->th_sem));
    } while ( OSA_TO_CONTINUE != handle_self->op);

    handle_self->state = OSA_TS_RUN;
    
    sem_post(&(handle_self->th_sem));
    
    return 0 ;
}

/******************************************************************************
 *  Resume a thread to run
 *
 *  Parameter:
 *      hTask                   [in] Thread's handle
 *
 *  Return value:
 *      0                       Success
 *****************************************************************************/
OSA_RESULT  osa_t_resume(OSA_HANDLE hTask)
{
    ThreadHandle_t  *handle;
    ri32            ret;

    handle = (ThreadHandle_t *) hTask;
    ret = _osa_t_checkHandle(handle);
    if( ret != 0 ) {
        d1(printf("osa_t_resume: given thread's handle error!\n"));
        return E_OSA_T_BADHANDLE;
    }

    d3(printf("osa_t_resume: ThreadID: %d\n", handle->id));

    sem_wait( &(handle->th_sem) );
    if( OSA_TS_STOP == handle->state ) {
        sem_post( &(handle->th_sem) );
        return E_OSA_T_NOSUSPEND;
    }

    if( (handle->suspend_count == 0) || (handle->state == OSA_TS_RUN) ) {
        sem_post( &(handle->th_sem) );
        return E_OSA_T_RUNNING;
    }
    
    handle->suspend_count --;
    if( handle->suspend_count > 0 ) {
        d2(printf("osa_t_resume: suspend count large than 2(%d)\n",
                  handle->suspend_count));
        sem_post( &(handle->th_sem) );

        return E_OSA_T_SUSPEND;
    }

    handle->op = OSA_TO_CONTINUE;
    ret = pthread_kill(handle->threadid, THREAD_SIG_CONTINUE);
    if ( 0 != ret ) {
        sem_post( &(handle->th_sem) );
        d1(printf("osa_t_resume: send THREAD_SIG_CONTINUE failed!\n"));
        return -1;
    }
    
    sem_post( &(handle->th_sem) );

    d3(printf( "osa_t_resume: end\n"));
    
    return 0;
}

/******************************************************************************
 *  Cause current thread suspend and select another thread to run
 *
 *  Parameter:
 *      hTask                   [in] Thread handle which will be suspend
 *
 *  Return value:
 *      0                       Success
 *****************************************************************************/
OSA_RESULT  osa_t_reschedule(OSA_HANDLE hTask)
{
    sched_yield();
    
    return 0;
}

/******************************************************************************
 *  Set thread priority level
 *
 *  Parameter:
 *      hTask                   [in] Thread's handle
 *      iPriority               [in] priority level
 *
 *  Return value:
 *      0                       Success
 *      E_OSA_T_BADHANDLE       Handle error
 *      E_OSA_T_BADPRIORITY     Priority out of range
 *****************************************************************************/
OSA_RESULT  osa_t_setPriority(OSA_HANDLE hTask, OSA_PRIORITY iPriority)
{
    ru32                new_pri;
    int                 policy;
    ri32                ret;
    struct sched_param  schedparam;
    struct ThreadHandle *handle;

    if( hTask == 0 )
        handle = (ThreadHandle_t *) osa_t_self();
    else
        handle = (struct ThreadHandle *) hTask;

    if( !_osa_t_checkHandle(handle) ) {
        d1(printf( "osa_t_setPriority: thread handle error!\n"));

        return E_OSA_T_BADHANDLE;
    }

    if( (iPriority > OSA_T_PRIORITY_MAX) || (iPriority < OSA_T_PRIORITY_MIN) ) {
        d1(printf("osa_t_setPriority: priority range out!\n"));
        return E_OSA_T_BADPRIORITY;
    }
    
    new_pri = iPriority * (OSA_T_LNX_PRIORITY_MAX - OSA_T_LNX_PRIORITY_MIN + 1 )
            / OSA_T_PRIORITY_RANGE + OSA_T_LNX_PRIORITY_MIN;
    
    handle->priority = new_pri;
    pthread_getschedparam(handle->threadid, &policy, &schedparam);
    schedparam.sched_priority = new_pri;
    if (new_pri !=0) {
        if (policy == SCHED_OTHER)
            policy = SCHED_RR;
    } else {
        if (policy != SCHED_OTHER)  
            policy = SCHED_OTHER;
    }

    ret = pthread_setschedparam(handle->threadid, policy, &schedparam);
    if( !ret )
        return 0;
    else {
        d1(printf( "osa_t_setPriority: failed at pthread_setschedparam!\n"));
        return E_OSA_LL_FUNC_ERR;
    }
}

/******************************************************************************
 *  get thread priority level
 *
 *  Parameter:
 *      hTask                   [in] Thread's handle
 *      piPriority              [out] priority level
 *
 *  Return value:
 *      0                       Success
 *      E_OSA_T_BADHANDLE       Handle error
 *****************************************************************************/
OSA_RESULT  osa_t_getPriority(OSA_HANDLE hTask, OSA_PRIORITY *piPriority)
{
    ThreadHandle_t  *handle;

    if( hTask == 0 )
        handle = (ThreadHandle_t *) osa_t_self();
    else
        handle = (ThreadHandle_t *) hTask;

    if( !_osa_t_checkHandle(handle) ) {
        d1(printf("osa_t_getPriority: thread handle error!\n"));

        return E_OSA_T_BADHANDLE;
    }

    return handle->priority;
}


/******************************************************************************
 *  Local: set thread specifice data
 *
 *  Parameter:
 *      threadvar               Thread's TSD
 *      value                   TSD
 *
 *  Return value:
 *      0                       Success
 *****************************************************************************/
static void _sig_suspend_handler(ri32 signo)
{
    ThreadHandle_t  *handle;
    ri32            ret;
    sigset_t        mask;

    handle = (ThreadHandle_t *) osa_t_self();
    if( _osa_t_checkHandle(handle) ) {
        d1(printf("_sig_suspend_handler: thread's handle error!\n"));
        return;
    }

    d3(printf("_sig_suspend_handler: id=%x\n", handle->id));

    ret = sem_trywait( &(handle->th_sem) );
    if( 0 != ret ) {
        d1(printf("_sig_suspend_handler: cant tyrwait semaphore!"));
        return ;
    }
        
    if( OSA_TO_SUSPEND == handle->op) {
        /* check current thread state */
        if (OSA_TS_RUN != handle->state) {
            d1(printf("_sig_suspend_handler: error thread state!\n"));
            sem_post( &(handle->th_sem) );
            return ;
        }

        sigemptyset(&mask);
        pthread_sigmask(SIG_BLOCK, &mask, &mask);
        sigdelset(&mask, THREAD_SIG_CONTINUE);

        handle->state = OSA_TS_SUSPEND;

        sem_post( &(handle->th_sem) );
        sem_post( &(handle->suspend_sem) );
        
        do {
            sigsuspend(&mask);
        } while ( OSA_TO_CONTINUE != handle->op );

        /* set state to RUNNING */
        sem_wait( &(handle->th_sem) );
        handle->state = OSA_TS_RUN;
        sem_post( &(handle->th_sem) );

        d3(printf( "_sig_suspend_handler: end\n"));
        
        return;
    }

    d2(printf("_sig_suspend_handler: error operation code.\n"));

    sem_post( &(handle->th_sem));
}


/******************************************************************************
 *  Local: set thread specifice data
 *
 *  Parameter:
 *      threadvar               Thread's TSD
 *      value                   TSD
 *
 *  Return value:
 *      0                       Success
 *****************************************************************************/
static void _sig_kill_handler(ri32 signo)
{
    ThreadHandle_t *handle;

    d3(printf("_sgi_kill_handler:\n"));
    
    handle = (ThreadHandle_t *) osa_t_self();
    if( _osa_t_checkHandle(handle) ) {
        d1(printf("_sig_kill_handler: thread handle error!\n"));
        return;
    }

    sem_post( &(handle->kill_sem) );

    /* set state to STOP */
    sem_wait( &(handle->th_sem) );
    //d1(printf(">> set thread state to OSA_TS_STOP 4 (%5d)\n",
    //          osa_t_getid())); fflush(stdout);
    handle->state = OSA_TS_STOP;
    sem_post( &(handle->th_sem) );

    _osa_tsd_set( &_osa_tid_key, 0 );

    pthread_exit(0);
}

/******************************************************************************
 *  Local: continue signal handler
 *
 *  Parameter:
 *      signo                   [in] Signal no
 *
 *  Return value:
 *      None
 *****************************************************************************/
static void _sig_cont_handler(ri32 signo)
{
    d3(printf("_sig_cont_handler:\n"));
    
    return;
}

} // end of namespace pi

#endif
