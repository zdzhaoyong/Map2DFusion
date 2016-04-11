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

#include "base/types/FastList.h"
#include "../osa.h"
#include "../_osa_in.h"


namespace pi {

#define OSA_SEM_WIN32_MAXVLAUE	0x60000000

/* Semaphore defines & functions */
#define	OSA_SEM_FIFO			1
#define	OSA_SEM_PRIORITY		2
#define	OSA_SEM_TIMEOUT		4


typedef struct SemHandle {
    ru32	magic;
    list_t  list;

	HANDLE	handle;
    ru32	value;

	HANDLE	hmux;
} SemHandle_t;


static SemHandle_t *g_pSems=NULL;
static HANDLE g_ListMux=0;


/******************************************************************************
 *	Local: Check semaphor's handle
 *
 *	Parameter:
 *		hSem				[in] Semaphore handle
 *		handle				[out] inner used handle
 *
 *	Return value:
 *		0					Success
 *		E_PLOSA_MAGIC		Magic number error
 *****************************************************************************/
inline OSA_RESULT _osa_sem_checkHandle(OSA_HANDLE hSem, SemHandle_t **handle)
{
    *handle = (SemHandle_t *) hSem;

    if( *handle != NULL ) {
        try {
            if( OSA_SEM_MAGIC == (*handle)->magic ) {
                return 0;
            }
        } catch (...) {
            return -1;
        }
    }

    return -1;
}

/******************************************************************************
 *	Initialize Semaphore sub-module
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_sem_init(void)
{
    if( g_pSems != NULL ) {
        osa_sem_release();
    }

    g_pSems = (SemHandle_t*) malloc(sizeof(SemHandle_t));
	g_pSems->magic = OSA_SEM_MAGIC;
	INIT_LIST_HEAD( &(g_pSems->list) );

	g_ListMux = CreateMutex(NULL, TRUE, NULL);
	
	return 0;
}

/******************************************************************************
 *	De-init Semaphore sub-module
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_sem_release(void)
{
    list_t      *plist;
	SemHandle_t *phandle;

    if( g_pSems != NULL ) {
        /* delete all open semaphores */
        plist = &(g_pSems->list);
        while( plist->next != plist->prev ) {
            phandle = list_entry(plist, SemHandle_t, list);

            osa_sem_delete( (OSA_HANDLE) phandle);
        }

        CloseHandle(g_ListMux);

        memset(g_pSems, 0, sizeof(SemHandle_t));
        free(g_pSems);
        g_pSems = NULL;
    }

	return 0;
}

/******************************************************************************
 *	Create semaphore
 *
 *	Parameter:
 *		hSem				[out] Semaphore handle
 *		iInitVal			[in] initialize sem value
 *		mode				[in] Create flags
 *
 *	Return value:
 *		0					Success
 *		E_PLOSA_SEM_CREATE	Failed to create semaphore
 *****************************************************************************/
OSA_RESULT	osa_sem_create(OSA_HANDLE *hSem, ri32 iInitVal, ru32 mode)
{
	SemHandle_t *phandle;

	
    phandle = (SemHandle_t*) malloc(sizeof(SemHandle_t));
	INIT_LIST_HEAD( &(phandle->list) );

    if( g_pSems != NULL ) {
        WaitForSingleObject(g_ListMux, INFINITE);
        list_add(&phandle->list, &g_pSems->list);
        ReleaseMutex(g_ListMux);
    }
	
	phandle->magic = OSA_SEM_MAGIC;
	phandle->value = iInitVal;

	phandle->hmux = CreateMutex(NULL, FALSE, NULL);
	
	phandle->handle = CreateSemaphore(NULL, iInitVal, 
						OSA_SEM_WIN32_MAXVLAUE, NULL);
	if( !phandle->handle ) {
        d1(printf("osa_sem_create: Failed at CreateSemaphore!\n"));
        return E_OSA_SEM_CREATE;
	}

	*hSem = (OSA_HANDLE) phandle;

	return 0;
}

/******************************************************************************
 *	Delete given semaphore
 *
 *	Parameter:
 *		hSem				[in] Semaphore's handle
 *
 *	Return value:
 *		0					Success
 *		E_PLOSA_SEM_HANDLE	Input handle error
 *****************************************************************************/
OSA_RESULT	osa_sem_delete(OSA_HANDLE hSem)
{
	SemHandle_t *handle;

	if( _osa_sem_checkHandle(hSem, &handle) ) {
        d1(printf("osa_sem_delete: Semaphore's handle error!\n"));
        return E_OSA_SEM_HANDLE;
	}

    if( g_pSems != NULL ) {
        WaitForSingleObject(g_ListMux, INFINITE);
        list_del( &(handle->list) );
        ReleaseMutex(g_ListMux);
    }

	CloseHandle(handle->handle);
	CloseHandle(handle->hmux);
	
	memset(handle, 0, sizeof(SemHandle_t));
	free(handle);

	return 0;
}

/******************************************************************************
 *	Wait for semapore signal (wait infinite)
 *
 *	Parameter:
 *		hSem				[in] Semaphore's handle
 *
 *	Return vlaue:
 *		0					Success
 *		E_PLOSA_SEM_HANDLE	input handle error
 *****************************************************************************/
OSA_RESULT	osa_sem_wait(OSA_HANDLE hSem)
{
	SemHandle_t *handle;
    DWORD       res;


	if( _osa_sem_checkHandle(hSem, &handle) ) {
        d1(printf("osa_sem_wait: Sem's handle error!\n"));
        return E_OSA_SEM_HANDLE;
	}

	WaitForSingleObject(handle->hmux, INFINITE);
	handle->value --;
	ReleaseMutex(handle->hmux);
	
	res = WaitForSingleObject(handle->handle, INFINITE);

	if( res == WAIT_OBJECT_0 ) {
		return 0;
	} else {
        d1(printf("osa_sem_wait: Failed at WaitForSingleObject\n"));
        return E_OSA_SEM_WAIT;
	}
}

/******************************************************************************
 *	Wait for semaphore signal (wait timeout) 0--immederate return
 *
 *	Parameter:
 *		hSem				[in] Sem's handle
 *		iTimeout			[in] timeout value (ms)
 *
 *	Return value:
 *		0					Success
 *		E_PLOSA_SEM_HANDLE	input handle error
 *		E_PLOSA_SEM_TIMEOUT	wait timeout
 *		E_PLOSA_SEM_WAIT	Wait failed
 *****************************************************************************/
OSA_RESULT	osa_sem_wait_timeout(OSA_HANDLE hSem, ru32 iTimeout)
{
    SemHandle_t *handle;
    DWORD       res, _waittime;


	if( _osa_sem_checkHandle(hSem, &handle) ) {
        d1(printf("osa_sem_wait: Sem's handle error!\n"));
        return E_OSA_SEM_HANDLE;
	}

	WaitForSingleObject(handle->hmux, INFINITE);
	handle->value --;
	ReleaseMutex(handle->hmux);

	if( iTimeout == OSA_WAIT_INFINITE )
		_waittime = INFINITE;
	else if( iTimeout == OSA_WAIT_NO )
		_waittime = 0;
		
	res = WaitForSingleObject(handle->handle, _waittime);

	if( res == WAIT_OBJECT_0 ) {
		return 0;
	} else if( res == WAIT_TIMEOUT ) {
		WaitForSingleObject(handle->hmux, INFINITE);
		handle->value ++;
		ReleaseMutex(handle->hmux);
        return E_OSA_SEM_TIMEOUT;
	}
	
	WaitForSingleObject(handle->hmux, INFINITE);
	handle->value ++;
	ReleaseMutex(handle->hmux);
	
    return E_OSA_SEM_WAIT;
}

/******************************************************************************
 *	Signal semaphore
 *
 *	Parameter:
 *		hSem				[in] Sem's handle
 *
 *	Return value:
 *		0					Success
 *		E_PLOSA_SEM_HANDLE	Input handle error
 *		E_PLOSA_SEM_SIGNAL	Failed to signal
 *****************************************************************************/
OSA_RESULT	osa_sem_signal(OSA_HANDLE hSem)
{
	SemHandle_t *handle;
    BOOL        res;

	if( _osa_sem_checkHandle(hSem, &handle) ) {
        d1(printf("osa_sem_signal: Sem's handle error!\n"));
        return E_OSA_SEM_HANDLE;
	}
	
	WaitForSingleObject(handle->hmux, INFINITE);
	handle->value ++;
	ReleaseMutex(handle->hmux);
	
	res = ReleaseSemaphore(handle->handle, 1, NULL);

	if( res ) {
		return 0;
	}
	
    return E_OSA_SEM_SIGNAL;
}

/******************************************************************************
 *	Get current semaphore value
 *
 *	Parameter:
 *		hSem				[in] Sem's handle
 *		piVal				[out] Sem's cout value
 *
 *	Return value:
 *		0					Success
 *		E_PLOSA_SEM_HANDLE	Input handle error
 *****************************************************************************/
OSA_RESULT	osa_sem_getValue(OSA_HANDLE hSem, ri32 *piVal)
{
	SemHandle_t *handle;

	if( _osa_sem_checkHandle(hSem, &handle) ) {
        d1(printf("osa_sem_getValue: Sem's handle error!\n"));
        return E_OSA_SEM_HANDLE;
	}
	
	WaitForSingleObject(handle->hmux, INFINITE);
	*piVal = handle->value;
	ReleaseMutex(handle->hmux);
	
	return 0;
}


} // end of namespace pi

#endif
