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

static MutexHandle_t    *g_pMutexs = NULL;
static sem_t            g_ListMux;

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

	sem_init( &g_ListMux, 0, 1);

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

	d3(printf( "osa_mu_release:\n"));

    if( g_pMutexs ) {
        plist = &(g_pMutexs->list);
        while( plist->next != plist->prev ) {
            phandle = list_entry( &(plist->next), MutexHandle_t, list);

            osa_mu_delete( (OSA_HANDLE) phandle);
        }

        sem_destroy( &g_ListMux );

        memset(g_pMutexs, 0, sizeof(MutexHandle_t));
        free(g_pMutexs);
        g_pMutexs = NULL;
    }

    d3(printf("osa_mu_release: end\n"));

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
OSA_RESULT	osa_mu_create(OSA_HANDLE *hMu, ru32 mode)
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
	pthread_mutex_init( &(phandle->handle), NULL );

	/* now insert into list */
    if( g_pMutexs ) {
        sem_wait( &(g_ListMux) );
        list_add( &(phandle->list), &(g_pMutexs->list) );
        sem_post( &(g_ListMux) );
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

	pthread_mutex_destroy( &(handle->handle) );

    if( g_pMutexs ) {
        sem_wait( &g_ListMux );
        list_del( &(handle->list) );
        sem_post( &g_ListMux );
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
    MutexHandle_t	*handle = NULL;
	int				res;

    // FIXME: use %08lx for 64-bit OS
    d3(printf("osa_mu_lock: handle:0x%08lx\n", hMu));
	
	if( _osa_mu_checkHandle(hMu, &handle) ) {
        d1(printf("osa_mu_lock: Input handle error!\n"));
		return E_OSA_MU_HANDLE;
	}

	res = pthread_mutex_lock( &(handle->handle) );
	if( res ) {
		return E_OSA_MU_LOCK;
	}

	return 0;
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
 *		E_OSA_MU_HANDLE	Input handle error
 *		E_OSA_MU_TIMEOUT	Wait timeout
 *		E_OSA_MU_LOCK		Failed wait
 *****************************************************************************/
OSA_RESULT	osa_mu_lock_timeout(OSA_HANDLE hMu, ru32 iTime)
{
    MutexHandle_t 	*handle = NULL;
	int				res;
	struct timespec	tp;

	if( _osa_mu_checkHandle(hMu, &handle) ) {
        d1(printf("osa_mu_lock_timeout: input handle error!\n"));
		return E_OSA_MU_HANDLE;
	}

	res = pthread_mutex_trylock( &(handle->handle) );
	if( 0 == res ) {
		return 0;
	}

	if( iTime == OSA_WAIT_INFINITE ) {
		res = pthread_mutex_lock( &(handle->handle) );
		if( EDEADLOCK == res )  {
			return E_OSA_DEADLOCK;
		}

		return 0;
	} else if( iTime == OSA_WAIT_NO ) {
		return E_OSA_MU_LOCK;
	}

	/* now wait specific time */
	tp.tv_sec = iTime / 1000;
	tp.tv_nsec = ( iTime % 1000 ) * 1000000;
	
	/* wait specific time */
	do {
		res = nanosleep(&tp, &tp);
	
		if( res == 0 ) {
			break;
		}
		res = pthread_mutex_trylock( &(handle->handle) );
	} while( res != 0 );
	
	if( res != 0 )
		return 0;
	else
		return E_OSA_MU_TIMEOUT;
}

/******************************************************************************
 *	unlock mutex
 *
 *	Parameter:
 *		hMu					[in] mutex handle
 *
 *	return vlaue:
 *		0					Success
 *		E_OSA_MU_HANDLE	input handle error
 *		E_OSA_MU_UNLOCK	Failed unlock
 *****************************************************************************/
OSA_RESULT	osa_mu_unlock(OSA_HANDLE hMu)
{
    MutexHandle_t 	*handle = NULL;
	int				res;

    // FIXME: use %08lx for 64bit OS
    d3(printf("osa_mu_unlock: handle:0x%08lx\n", hMu));
	
	if( _osa_mu_checkHandle(hMu, &handle) ) {
        d1(printf("osa_mu_unlock: input handle error!\n"));
		return E_OSA_MU_HANDLE;
	}

	res = pthread_mutex_unlock( &(handle->handle) );
	if( res )
		return E_OSA_MU_UNLOCK;
	else
		return 0;
}

/******************************************************************************
 *	Get mutex current state
 *
 *	Parameter:
 *		hMu					[in] mutex's handle
 *
 *	Return vlaue:
 *		0					Success ( unlock )
 *		E_OSA_MU_HANDLE	input handle error
 *		E_OSA_MU_LOCK		Have lock
 *****************************************************************************/
OSA_RESULT	osa_mu_isLock(OSA_HANDLE hMu)
{
    MutexHandle_t 	*handle = NULL;
	int				res;
	
	if( _osa_mu_checkHandle(hMu, &handle) ) {
        d1(printf("osa_mu_isLock: input handle error!\n"));
		return E_OSA_MU_HANDLE;
	}

	res = pthread_mutex_trylock( &(handle->handle) );

	if( res == 0 )
		return 0;
	else
		return E_OSA_MU_LOCK;
}

} // end of namespace pi

#endif
