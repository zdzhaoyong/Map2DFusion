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

typedef struct MutexHandle {
    ru32	magic;
	list_t	list;

	HANDLE	handle;
} MutexHandle_t;

static MutexHandle_t *g_pMutexs = NULL;
static HANDLE g_ListMux = 0;



/******************************************************************************
 *	Local: Check mutex handle
 *
 *	Parameter:
 *		hMu				[in] input handle
 *		handle			[out] inner used handle
 *
 *	Return value:
 *		0				Success
 *		E_PLOSA_MU_HANDLE	handle error
 *****************************************************************************/
inline OSA_RESULT _osa_mu_checkHandle(OSA_HANDLE hMu, MutexHandle_t **handle)
{
    *handle = (MutexHandle_t *) hMu;

    if( NULL != *handle ) {
        try {
            if( OSA_MU_MAGIC == (*handle)->magic ) {
                return 0;
            } else
                return E_OSA_MU_HANDLE;
        } catch(...) {
            return E_OSA_MU_HANDLE;
        }
    }

    return E_OSA_MU_HANDLE;
}

/******************************************************************************
 *	Init mutex sub-module
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_mu_init(void)
{
    g_pMutexs = (MutexHandle_t*) malloc(sizeof(MutexHandle_t));
	INIT_LIST_HEAD( &(g_pMutexs->list) );
	g_pMutexs->magic = OSA_MU_MAGIC;

	g_ListMux = CreateMutex(NULL, FALSE, NULL);


	return 0;
}

/******************************************************************************
 *	Deinit mutex sub-module
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_mu_release(void)
{
	MutexHandle_t	*phandle;
    list_t          *plist;

    if( g_pMutexs != NULL ) {
        WaitForSingleObject(g_ListMux, INFINITE);
        plist = &(g_pMutexs->list);
        while( plist->next != plist->prev ) {
            phandle = list_entry( &(plist->next), MutexHandle_t, list);

            list_del( plist->next );
            CloseHandle(phandle->handle);
            memset(phandle, 0, sizeof(MutexHandle_t));
            free(phandle);
        }
        ReleaseMutex(g_ListMux);

        CloseHandle(g_ListMux);
        g_ListMux = NULL;

        memset(g_pMutexs, 0, sizeof(MutexHandle_t));
        free(g_pMutexs);
        g_pMutexs = NULL;
    }

	return 0;
}

/******************************************************************************
 *	Create mutex
 *
 *	Parameter:
 *		hMu					[out] Mutex's handle
 *		mode				[in] create flags
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT osa_mu_create(OSA_HANDLE *hMu, ru32 mode)
{
	MutexHandle_t *phandle;
	

    d3(printf("osa_mu_create: mode:%d\n", mode));
	
    phandle = (MutexHandle_t*) malloc(sizeof(MutexHandle_t));
	if( phandle == NULL ) {
        d1(printf("osa_mu_create: malloc fail\n"));
        return E_OSA_MEM;
	}
	memset(phandle, 0, sizeof(MutexHandle_t));

	INIT_LIST_HEAD( &(phandle->list) );
	phandle->magic = OSA_MU_MAGIC;
	phandle->handle = CreateMutex(NULL, FALSE, NULL);

	/* now insert into list */
    if( g_pMutexs != NULL ) {
        WaitForSingleObject(g_ListMux, INFINITE);
        list_add( &(phandle->list), &(g_pMutexs->list) );
        ReleaseMutex(g_ListMux);
    }

	*hMu = (OSA_HANDLE) phandle;

    d3(printf("osa_mu_create: end\n"));

	return 0;
}

/******************************************************************************
 *	Delete given mutex
 *
 *	Parameter:
 *		hMu					[in] mutex's handle
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_mu_delete(OSA_HANDLE hMu)
{
	MutexHandle_t *handle;
	
	if( _osa_mu_checkHandle(hMu, &handle) ) {
        d1(printf("osa_mu_delete: Input handle error!\n"));
        return E_OSA_MU_HANDLE;
	}

	CloseHandle(handle->handle);

    if( g_pMutexs != NULL ) {
        WaitForSingleObject(g_ListMux, INFINITE);
        list_del( &(handle->list) );
        ReleaseMutex(g_ListMux);
    }

	memset(handle, 0, sizeof(MutexHandle_t));
	free(handle);

	return 0;
}

/******************************************************************************
 *	Lock given mutex
 *
 *	Parameter:
 *		hMu					[in] mutex handle
 *
 *	Return value:
 *		0					Success
 *		OSA_OSA_MU_HANDLE	Input handle error
 *		OSA_OSA_MU_LOCK		Lock failed
 *****************************************************************************/
OSA_RESULT	osa_mu_lock(OSA_HANDLE hMu)
{
	MutexHandle_t *handle;
	DWORD res;

	
    d3(printf("osa_mu_lock: handle:0x%08x\n", hMu));
	
	if( _osa_mu_checkHandle(hMu, &handle) ) {
        d1(printf("osa_mu_lock: Input handle error!\n"));
        return E_OSA_MU_HANDLE;
	}

	res = WaitForSingleObject(handle->handle, INFINITE);
	if( WAIT_OBJECT_0 == res ) {
		return 0;
	}

    return E_OSA_MU_LOCK;
}

/******************************************************************************
 *	Wait mutex timeout
 *
 *	Parameter:
 *		hMu					[in] mutex handle
 *		iTime				[in] timeout value
 *
 *	Return value:
 *		0					Success
 *		E_PLOSA_MU_HANDLE	Input handle error
 *		E_PLOSA_MU_TIMEOUT	Wait timeout
 *		E_PLOSA_MU_LOCK		Failed wait
 *****************************************************************************/
OSA_RESULT	osa_mu_lock_timeout(OSA_HANDLE hMu, ru32 iTime)
{
	MutexHandle_t *handle;
	DWORD res;


	if( _osa_mu_checkHandle(hMu, &handle) ) {
        d1(printf("osa_mu_lock_timeout: input handle error!\n"));
        return E_OSA_MU_HANDLE;
	}

	res = WaitForSingleObject(handle->handle, INFINITE);
	if( WAIT_OBJECT_0 == res ) {
		return 0;
	} else if( WAIT_TIMEOUT == res ) {
        return E_OSA_MU_TIMEOUT;
	}

    return E_OSA_MU_LOCK;
}

/******************************************************************************
 *	unlock mutex
 *
 *	Parameter:
 *		hMu					[in] mutex handle
 *
 *	return vlaue:
 *		0					Success
 *		E_PLOSA_MU_HANDLE	input handle error
 *		E_PLOSA_MU_UNLOCK	Failed unlock
 *****************************************************************************/
OSA_RESULT	osa_mu_unlock(OSA_HANDLE hMu)
{
	MutexHandle_t *handle;

    d3(printf("osa_mu_unlock: handle:0x%08x\n", hMu));
	
	if( _osa_mu_checkHandle(hMu, &handle) ) {
        d1(printf("osa_mu_unlock: input handle error!\n"));
        return E_OSA_MU_HANDLE;
	}

	if( ReleaseMutex(handle->handle) )
		return 0;
	else
        return E_OSA_MU_UNLOCK;
}

/******************************************************************************
 *	Get mutex current state
 *
 *	Parameter:
 *		hMu					[in] mutex's handle
 *
 *	Return vlaue:
 *		0					Success ( unlock )
 *		E_PLOSA_MU_HANDLE	input handle error
 *		E_PLOSA_MU_LOCK		Have lock
 *****************************************************************************/
OSA_RESULT	osa_mu_isLock(OSA_HANDLE hMu)
{
	MutexHandle_t *handle;
	DWORD res;


	if( _osa_mu_checkHandle(hMu, &handle) ) {
        d1(printf("osa_mu_isLock: input handle error!\n"));
        return E_OSA_MU_HANDLE;
	}

	res = WaitForSingleObject(handle->handle, 0);

	if( WAIT_OBJECT_0 == res )
		return 0;
	else
        return E_OSA_MU_LOCK;
}

} // end of namespace pi

#endif
