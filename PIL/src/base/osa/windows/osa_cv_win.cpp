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

typedef struct CvHandle {
    list_t      list;
    ru32    	magic;

    HANDLE      handle;
} CvHandle_t;

static CvHandle_t   *g_CvList = NULL;
static HANDLE       g_ListMux;


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
inline OSA_RESULT _osa_cv_checkHandle(OSA_HANDLE hCv, CvHandle_t **handle)
{
    *handle = (CvHandle_t *) hCv;

    if( NULL != *handle ) {
        try {
            if( OSA_CV_MAGIC == (*handle)->magic ) {
                return 0;
            }
        } catch (...) {
            return -1;
        }
    }

    return -1;
}


/******************************************************************************
 *	Initialize condition-variable sub-system
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0						Success
 *****************************************************************************/
OSA_RESULT	osa_cv_init(void)
{
    if( g_CvList != NULL )
        osa_cv_release();

    g_CvList = (CvHandle_t*) malloc(sizeof(CvHandle_t));
	memset(g_CvList, 0, sizeof(g_CvList));

	INIT_LIST_HEAD( &(g_CvList->list) );
	g_CvList->magic = OSA_CV_MAGIC;

	g_ListMux = CreateMutex(NULL, FALSE, NULL);
	
	return 0;
}

/******************************************************************************
 *	De-init condition-variable sub-system
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0						Success
 *****************************************************************************/
OSA_RESULT	osa_cv_deinit(void)
{
    CvHandle_t  *phandle;
    list_t      *plist;

    if( g_CvList != NULL ) {
        WaitForSingleObject(g_ListMux, INFINITE);
        plist = &(g_CvList->list);
        while( plist->next != plist->prev ) {
            phandle = list_entry(plist->next, CvHandle_t, list);

            list_del( &(phandle->list) );

            CloseHandle(phandle->handle);
            memset(phandle, 0, sizeof(CvHandle_t));
            free(phandle);
        }
        ReleaseMutex(g_ListMux);

        CloseHandle(g_ListMux);
        g_ListMux = NULL;

        memset(g_CvList, 0, sizeof(CvHandle_t));
        free(g_CvList);
        g_CvList = NULL;
    }

	return 0;
}

/******************************************************************************
 *	Create cv 
 *
 *	Parameter:
 *		hCV					[out] cv handle
 *		mode				[in] create flag
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_cv_create(OSA_HANDLE *hCV, ru32 mode)
{
	CvHandle_t *phandle;

    phandle = (CvHandle_t*) malloc(sizeof(CvHandle_t));
	INIT_LIST_HEAD( &(phandle->list) );
	phandle->magic = OSA_CV_MAGIC;
	phandle->handle = (HANDLE) CreateEvent(NULL, FALSE, FALSE, NULL);

    if( g_CvList != NULL ) {
        WaitForSingleObject(g_ListMux, INFINITE);
        list_add( &(phandle->list), &(g_CvList->list) );
        ReleaseMutex(g_ListMux);
    }

	return 0;
}

/******************************************************************************
 *	Delete given cv 
 *
 *	Parameter:
 *		hCV					[in] cv handle
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_cv_delete(OSA_HANDLE hCV)
{
	CvHandle_t *phandle;

	if( _osa_cv_checkHandle(hCV, &phandle) ) {
        d1(printf("osa_cv_delete: Input handle error!\n"));
        return E_OSA_CV_HANDLE;
	}

	/* fixme: may be faile when user call osa_cv_wait */
	CloseHandle(phandle->handle);

    if( g_CvList != NULL ) {
        WaitForSingleObject(g_ListMux, INFINITE);
        list_del( &(phandle->list) );
        ReleaseMutex(g_ListMux);
    }

	memset(phandle, 0, sizeof(CvHandle_t));
	free(phandle);

	return 0;
}

/******************************************************************************
 *	Wait for CV signal
 *
 *	Parameter:
 *		hCV					[in] cv's handle
 *		hMu					[in] peer mutex handle
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_cv_wait(OSA_HANDLE hCV, OSA_HANDLE hMu)
{
    CvHandle_t  *phandle;
    OSA_RESULT  res;
    DWORD       res2;


    if( _osa_cv_checkHandle(hCV, &phandle) ) {
        d1(printf("osa_cv_wait: Input handle error!\n"));
        return E_OSA_CV_HANDLE;
	}

	/* fixme: do we put these code to critical-section */
	res = osa_mu_unlock(hMu);
	res2 = WaitForSingleObject(phandle->handle, INFINITE);
	res = osa_mu_lock(hMu);
	
	if( WAIT_OBJECT_0 == res2 )
		return 0;

    return E_OSA_CV_WAIT;
}

/******************************************************************************
 *	Wait cv for a timeout
 *
 *	Parameter:
 *		hCV						[in] cv handle
 *		hMu						[in] peer mutex handle
 *		timeout					[in] time out value
 *
 *	return value:
 *		0						Success
 *****************************************************************************/
OSA_RESULT	osa_cv_wait_timeout(OSA_HANDLE hCV, OSA_HANDLE hMu, ru32 timeout)
{
	CvHandle_t *phandle;
	OSA_RESULT res;
	DWORD res2;

	if( _osa_cv_checkHandle(hCV, &phandle) ) {
        d1(printf("osa_cv_wait_timeout: Input handle error!\n"));
        return E_OSA_CV_HANDLE;
	}

	/* fixme: do we put these code to critical-section */
	res = osa_mu_unlock(hMu);
	res2 = WaitForSingleObject(phandle->handle, timeout);
	res = osa_mu_lock(hMu);
	
	if( WAIT_OBJECT_0 == res2 )
		return 0;

    return E_OSA_CV_WAIT;
}

/******************************************************************************
 *	Set the cv to signal state
 *
 *	Parameter:
 *		hCV						[in] cv handle
 *
 *	Return value:
 *		0						Success
 *****************************************************************************/
OSA_RESULT	osa_cv_signal(OSA_HANDLE hCV)
{
	CvHandle_t *phandle;

	if( _osa_cv_checkHandle(hCV, &phandle) ) {
        d1(printf("osa_cv_signal: Input handle error!\n"));
        return E_OSA_CV_HANDLE;
	}

	if( PulseEvent(phandle->handle) )
		return 0;

    return E_OSA_CV_SIGNAL;
}

/******************************************************************************
 *	broadcast cv signal stat
 *
 *	Parameter:
 *		hCV						[in] cv handle
 *
 *	Return value:
 *		0						Success
 *****************************************************************************/
OSA_RESULT	osa_cv_broadcast(OSA_HANDLE hCV)
{
	CvHandle_t *phandle;

	if( _osa_cv_checkHandle(hCV, &phandle) ) {
        d1(printf("osa_cv_broadcast: Input handle error!\n"));
        return E_OSA_CV_HANDLE;
	}

	if( PulseEvent(phandle->handle) )
		return 0;

    return E_OSA_CV_BROADCAST;
}


} // end of namespace pi

#endif
