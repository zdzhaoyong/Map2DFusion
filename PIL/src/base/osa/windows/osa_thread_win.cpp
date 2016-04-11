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

#ifdef PIL_WINDOWS

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base/types/FastList.h"
#include "../osa.h"
#include "../_osa_in.h"

namespace pi {

/* Define max & min Thread priority */
#define OSA_T_WIN_PRIORITY_MAX		31
#define OSA_T_WIN_PRIORITY_MIN		0
#define OSA_T_WIN_PRIORITY_DEFAULT	0

/* Local struct defines */
typedef struct ThreadHandle {
    ru32        magic;
    list_t      list;

    ru32        state;
    HANDLE      thandle;
    ru32        tid;

    HANDLE      hmutex;
    ru32        suspend_count;
} ThreadHandle_t;

typedef struct ThreadParam {
    OSA_T_ARGS          arg;
    ThreadHandle_t      *handle;
    OSA_T_FUNC          func;
} ThreadParam_t;

DWORD WINAPI _osa_t_create(LPVOID lpParameter);

ThreadHandle_t *g_ThreadList = NULL;
DWORD g_TlsTid = 0;
static HANDLE g_ListMux = 0;



/******************************************************************************
 *	local: Check thread's handle correct
 *
 *	Parameter:
 *		hTask				[in] Thread's handle
 *
 *	Return value:
 *		0					Success
 *		E_PLOSA_T_BADHANDLE	Input handle error
 *****************************************************************************/
inline OSA_RESULT _osa_t_checkHandle(ThreadHandle_t *handle)
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
 *	Initialize thread functions
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_t_init(void)
{
    d3(printf("osa_t_init: begin\n"));
	
    // free previous setup
    if( g_ThreadList != NULL ) {
        osa_t_release();
    }

    // create thread list
    g_ThreadList = (ThreadHandle_t*) malloc(sizeof(ThreadHandle_t));
    memset(g_ThreadList, 0, sizeof(ThreadHandle_t));
    g_ThreadList->magic = OSA_T_MAGIC;
    INIT_LIST_HEAD(&g_ThreadList->list);

    g_ThreadList->thandle = GetCurrentThread();
    g_ThreadList->tid = GetCurrentThreadId();
    g_ThreadList->state = OSA_TS_RUN;

	g_TlsTid = TlsAlloc();
    TlsSetValue(g_TlsTid, g_ThreadList);

	g_ListMux = CreateMutex(NULL, FALSE, NULL);
	
    d3(printf("osa_t_init: end\n"));
	
	return 0;
}

/******************************************************************************
 *	De-initialize thread functions
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0						Success
 *		-1						Failed
 *****************************************************************************/
OSA_RESULT	osa_t_release(void)
{
    list_t          *plist;
    ThreadHandle_t  *pHandle;

    d3(printf("osa_t_deinit: begin\n"));

    if( g_ThreadList ) {
        WaitForSingleObject(g_ListMux, INFINITE);
        plist = &g_ThreadList->list;
        while( plist->next != plist->prev ) {
            pHandle = list_entry(plist->next, ThreadHandle_t, list);

            /* if it state isn't STOP then stop it */
            if( pHandle->state != OSA_TS_STOP ) {
                osa_t_kill((OSA_HANDLE) pHandle);
            }

            /* delete thread resource */
            osa_t_delete( (OSA_HANDLE) pHandle );
        }
        ReleaseMutex(g_ListMux);

        memset(g_ThreadList, 0, sizeof(ThreadHandle_t));
        free(g_ThreadList);
        g_ThreadList = NULL;

        TlsFree(g_TlsTid);
        CloseHandle(g_ListMux);
    }

    d3(print("osa_t_deinit: end\n"));
	
	return 0;
}

/******************************************************************************
 *	Create a new thread
 *
 *	Parameter:
 *		hTask					[out] Thread's handle
 *		func					[in] thread's function
 *		arg						[in] thread argument
 *		stacksize				[in] thread's stack size
 *		mode					[in] availble:
 *									OSA_T_CREATSUSPEND create suspend
 *
 *****************************************************************************/
OSA_RESULT	osa_t_create(OSA_HANDLE *hTask,
                         OSA_T_FUNC func, OSA_T_ARGS arg,
                         ru32 stackSize, ru32 mode)
{
    ThreadHandle_t  *phandle;
    DWORD           flags, tid;
    ThreadParam_t   *param;

	
    phandle = (ThreadHandle_t*) malloc(sizeof(ThreadHandle_t));
	memset(phandle, 0, sizeof(ThreadHandle_t));
    phandle->magic = OSA_T_MAGIC;
	phandle->suspend_count = 0;
	INIT_LIST_HEAD(&phandle->list);

    param = (ThreadParam_t*) malloc(sizeof(ThreadParam_t));
    param->arg    = arg;
    param->func   = func;
	param->handle = phandle;

	/* sync mutex */
	phandle->hmutex = CreateMutex(NULL, FALSE, NULL);
	
	/* add thread descriptor into list */
    if( g_ThreadList != NULL ) {
        WaitForSingleObject(g_ListMux, INFINITE);
        list_add( &(phandle->list), &(g_ThreadList->list));
        ReleaseMutex(g_ListMux);
    }

	flags = 0;
	if( mode & OSA_T_CREATESUSPEND )
        flags = CREATE_SUSPENDED;

	phandle->state = OSA_TS_READY;
	phandle->thandle = CreateThread(NULL, stackSize, _osa_t_create, param, flags, &tid);
	phandle->tid = tid;
	
	*hTask = (OSA_HANDLE) phandle;

	return 0;
}

/******************************************************************************
 *	Local: Thread create function
 *
 *	Parameter:
 *		lpParameter				[in] input parameter
 *
 *	Return value:
 *		0						Success
 *****************************************************************************/
DWORD WINAPI _osa_t_create(LPVOID lpParameter)
{
    ThreadParam_t   *param;
    OSA_T_ARGS      arg;
    ThreadHandle_t  *handle;
    OSA_T_FUNC      func;

    ru32 ret;

    param  = (ThreadParam_t *) lpParameter;
    arg    = param->arg;
	handle = param->handle;
    func   = param->func;
	free(param);

	TlsSetValue(g_TlsTid, handle);

	handle->state = OSA_TS_RUN;
	ret = func(arg);
	handle->state = OSA_TS_STOP;

	return 0;
}

/******************************************************************************
 *	Kill a thread (cause a thread stop)
 *
 *	Parameter:
 *		hTask					[in] thread's handle
 *
 *	Return value:
 *		0						Success
 *		E_PLOSA_T_BADHANDLE		Input thread handle error
 *		E_PLOSA_T_KILL			Failed to kill thread
 *****************************************************************************/
OSA_RESULT	osa_t_kill(OSA_HANDLE hTask)
{
    ThreadHandle_t *handle;

    // kill my self
    if( 0 == hTask ) {
        ExitThread(0);
        return 0;
    }

    // get handle
    handle = (ThreadHandle_t*) hTask;
    if( _osa_t_checkHandle(handle) ) {
        d1(printf("osa_t_kill: Failed to get thread handle\n"));
        return E_OSA_T_BADHANDLE;
    }

    if( TerminateThread(handle->thandle, 0) ) {
        handle->state = OSA_TS_STOP;
        return 0;
    } else
        return E_OSA_T_KILL;
}

/******************************************************************************
 *	Delete thread handle
 *
 *	Parameter:
 *		hTask				[in] Thread id
 *
 *	Return value:
 *		0					Success
 *		E_PLOSA_T_BADHANDLE	Input thread handle error
 *		E_PLOSA_T_DELETE		failed to delete handle
 *****************************************************************************/
OSA_RESULT osa_t_delete(OSA_HANDLE hTask)
{
	ThreadHandle_t *handle;
	BOOL res;
	
	if( 0 == hTask )
        return E_OSA_T_DELETE;

    handle = (ThreadHandle_t*) hTask;
    if( _osa_t_checkHandle(handle) ) {
        d1(printf("osa_t_delete: Failed to delete handle!\n"));
        return E_OSA_T_DELETE;
    }

	/* only thread stopped */
	if( OSA_TS_STOP != handle->state )
        return E_OSA_T_DELETE;

	/* close handle and delete it */
	res = CloseHandle(handle->thandle);

	/* close sync mutex */
	CloseHandle(handle->hmutex);

	/* delete thread descriptor from list */
    if( g_ThreadList != NULL ) {
        WaitForSingleObject(g_ListMux, INFINITE);
        list_del(&handle->list);
        ReleaseMutex(g_ListMux);
    }

	memset(handle, 0, sizeof(ThreadHandle_t));
	free(handle);
	
	return 0;
}

/******************************************************************************
 *	Get current thread's handle
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		Thread handle
 *****************************************************************************/
OSA_HANDLE	osa_t_self(void)
{
    OSA_HANDLE      handle;
    ThreadHandle_t  *t;

    handle = (OSA_HANDLE) TlsGetValue(g_TlsTid);

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
}

/*****************************************************************************
 *	Wait thread finished
 *
 *	Parameter:
 *		hTask			[in] Thread handle
 *
 *	Return value:
 *		0				Success
 ****************************************************************************/
OSA_RESULT	osa_t_wait(OSA_HANDLE hTask)
{
	ThreadHandle_t *handle;

	if( 0 == hTask ) {
        d1(printf("osa_t_wait: cann't wait myself!\n"));
        return E_OSA_T_WAIT;
	}
	
    handle = (ThreadHandle_t *) hTask;
    if( _osa_t_checkHandle(handle) ) {
        d1(printf("osa_t_delete: Failed to delete handle!\n"));
        return E_OSA_T_BADHANDLE;
	}

 	WaitForSingleObject(handle->thandle, INFINITE);

	return 0;
}

/******************************************************************************
 *	Cause current thread to sleep
 *
 *	Parameter:
 *		iTime			[in] time in ms
 *
 *	Return value:
 *		0				Success
 *****************************************************************************/
OSA_RESULT	osa_t_sleep(ru32 iTime)
{
	Sleep(iTime);
	return 0;
}

/******************************************************************************
 *	Suspend given thread
 *
 *	Parameter:
 *		hTask			[in] Thread handle
 *
 *	Return value:
 *		0						Success
 *		E_PLOSA_T_BADHANDLE		Input handle error
 *		E_PLOSA_T_SUSPEND		Thread have been suspend
 *****************************************************************************/
OSA_RESULT	osa_t_suspend(OSA_HANDLE hTask)
{
	ThreadHandle_t *handle;
	
    d3(print("osa_t_suspend: handle(%d)\n", hTask));

    // get private thread handle
	if( 0 == hTask )
		hTask = osa_t_self();

    handle = (ThreadHandle_t*) hTask;
    if( _osa_t_checkHandle(handle) ) {
        d1(printf("osa_t_suspend: Failed to get handle!\n"));
        return E_OSA_T_BADHANDLE;
    }


	WaitForSingleObject(handle->hmutex, INFINITE);

	handle->suspend_count ++;
	if( handle->suspend_count > 1 ) {
		ReleaseMutex(handle->hmutex);
        return E_OSA_T_SUSPEND;
	}
    handle->state = OSA_TS_SUSPEND;

	ReleaseMutex(handle->hmutex);
	
	SuspendThread(handle->thandle);
	
	return 0;
}

/******************************************************************************
 *	Resume given thread
 *
 *	Parameter:
 *		hTask			[in] Thread's handle
 *
 *	Return value:
 *		0					Success
 *		E_PLOSA_T_RESUME	Faild to resume a given thread
 *		E_PLOSA_T_BADHANDLE	Bad thead handle
 *		E_PLOSA_T_RUNNING	Thread is running
 *****************************************************************************/
OSA_RESULT osa_t_resume(OSA_HANDLE hTask)
{
	ThreadHandle_t *handle;

    // get private thread handle
    if( 0 == hTask )
        hTask = osa_t_self();

    handle = (ThreadHandle_t*) hTask;
    if( _osa_t_checkHandle(handle) ) {
        d1(printf("osa_t_resume: Failed to get handle!\n"));
        return E_OSA_T_BADHANDLE;
    }


	WaitForSingleObject(handle->hmutex, INFINITE);

	if( handle->suspend_count == 0 ) {
		ReleaseMutex(handle->hmutex);
        return E_OSA_T_RUNNING;
	} else {
		handle->suspend_count --;
		
		if( handle->suspend_count > 0 ) {
			ReleaseMutex(handle->hmutex);
            return E_OSA_T_SUSPEND;     /* thread is suspended */
		}
	}
	
	handle->state = OSA_TS_RUN;
	ResumeThread(handle->thandle);

	ReleaseMutex(handle->hmutex);
	
	return 0;
}

/******************************************************************************
 *	Cause calling thread to yield execution to another thread
 *
 *	Parameter:
 *		hTask					[in] Input thread handle
 *
 *	Return value:
 *		0						Success
 *****************************************************************************/
OSA_RESULT	osa_t_reschedule(OSA_HANDLE hTask)
{
	//SwitchToThread();
	
	return 0;
}

/******************************************************************************
 *	Set given thread's priority
 *
 *	Parameter:
 *		hTask				[in] Thread's handle
 *		iPriority			[in] Thread's new priority
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_t_setPriority(OSA_HANDLE hTask, OSA_PRIORITY iPriority)
{
	return 0;
}

/******************************************************************************
 *	Get given thread's priority
 *
 *	Parameter:
 *		hTask				[in] Thread's handle
 *		iPriority			[out] Thread's priority
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_t_getPriority(OSA_HANDLE hTask, OSA_PRIORITY *piPriority)
{
	return 0;
}


} // end of namespace pi

#endif
