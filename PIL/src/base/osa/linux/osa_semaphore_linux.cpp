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


/* Semaphore defines & functions */
#define	OSA_SEM_FIFO			1
#define	OSA_SEM_PRIORITY		2
#define	OSA_SEM_TIMEOUT         4


typedef struct SemHandle {
	ru32		magic;
    list_t      list;

    sem_t       handle;
	ru32		value;

	pthread_mutex_t	hmux;
} SemHandle_t;


static SemHandle_t  *g_pSems = NULL;
static sem_t        g_ListMux;


/******************************************************************************
 *	Local: Check semaphor's handle
 *
 *	Parameter:
 *		hSem				[in] Semaphore handle
 *		handle				[out] inner used handle
 *
 *	Return value:
 *		0					Success
 *		E_OSA_MAGIC		Magic number error
 *****************************************************************************/
inline OSA_RESULT _osa_sem_checkHandle(OSA_HANDLE hSem, SemHandle_t **handle)
{
    if( NULL == handle )
        return -1;

    *handle = (SemHandle_t *) hSem;

    try {
        if( OSA_SEM_MAGIC == (*handle)->magic )
            return 0;
        else
            return E_OSA_MAGIC;
    } catch( ... ) {
        return E_OSA_MAGIC;
    }

    return E_OSA_MAGIC;
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
    d3(printf("osa_sem_init:\n"));
	
    g_pSems = (SemHandle_t*) malloc(sizeof(SemHandle_t));
	g_pSems->magic = OSA_SEM_MAGIC;
	INIT_LIST_HEAD( &(g_pSems->list) );

	sem_init( &g_ListMux, 0, 1);

    d3(printf("osa_sem_init: end\n"));
	
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
	list_t *plist;
	SemHandle_t *phandle;

    d3(printf("osa_sem_release:\n"));
	
    if( g_pSems ) {
        /* delete all open semaphores */
        plist = &(g_pSems->list);
        while( plist->next != plist->prev ) {
            phandle = list_entry(plist, SemHandle_t, list);

            osa_sem_delete( (OSA_HANDLE) phandle );
        }

        sem_destroy( &g_ListMux );

        memset(g_pSems, 0, sizeof(SemHandle_t));
        free(g_pSems);
        g_pSems = NULL;
    }

    d3(printf("osa_sem_release: end\n"));

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
 *		E_OSA_SEM_CREATE	Failed to create semaphore
 *****************************************************************************/
OSA_RESULT	osa_sem_create(OSA_HANDLE *hSem, ri32 iInitVal, ru32 mode)
{
	SemHandle_t *phandle;
	int			res;

    // create new sem object
    phandle = (SemHandle_t*) malloc(sizeof(SemHandle_t));
	INIT_LIST_HEAD( &(phandle->list) );

    // add to list
    if( g_pSems ) {
        sem_wait( &g_ListMux );
        list_add(&phandle->list, &g_pSems->list);
        sem_post( &g_ListMux );
    }

	phandle->magic = OSA_SEM_MAGIC;
	phandle->value = iInitVal;

	pthread_mutex_init( &(phandle->hmux), NULL);

	res = sem_init( &(phandle->handle), 0, iInitVal);
	if( res ) {
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
 *		E_OSA_SEM_HANDLE	Input handle error
 *****************************************************************************/
OSA_RESULT	osa_sem_delete(OSA_HANDLE hSem)
{
    SemHandle_t *handle = NULL;

	if( _osa_sem_checkHandle(hSem, &handle) ) {
        d1(printf("osa_sem_delete: Semaphore's handle error!\n"));
		return E_OSA_SEM_HANDLE;
	}

    if( g_pSems ) {
        sem_wait( &g_ListMux );
        list_del( &(handle->list) );
        sem_post( &g_ListMux );
    }

	sem_destroy( &(handle->handle) );
	pthread_mutex_destroy( &(handle->hmux) );
	
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
 *		E_OSA_SEM_HANDLE	input handle error
 *****************************************************************************/
OSA_RESULT	osa_sem_wait(OSA_HANDLE hSem)
{
    SemHandle_t *handle = NULL;

	if( _osa_sem_checkHandle(hSem, &handle) ) {
        d1(printf("osa_sem_wait: Sem's handle error!\n"));
		return E_OSA_SEM_HANDLE;
	}

	pthread_mutex_lock( &(handle->hmux) );
	handle->value --;
	pthread_mutex_unlock( &(handle->hmux) );

	sem_wait( &(handle->handle) );

	return 0;
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
 *		E_OSA_SEM_HANDLE	input handle error
 *		E_OSA_SEM_TIMEOUT	wait timeout
 *		E_OSA_SEM_WAIT	Wait failed
 *****************************************************************************/
OSA_RESULT	osa_sem_wait_timeout(OSA_HANDLE hSem, ru32 iTimeout)
{
    SemHandle_t     *handle = NULL;
    struct timespec tp;
    int             res;
    OSA_RESULT      ret;

	if( _osa_sem_checkHandle(hSem, &handle) ) {
        d1(printf("osa_sem_wait: Sem's handle error!\n"));
		return E_OSA_SEM_HANDLE;
	}

	pthread_mutex_lock( &(handle->hmux) );
	handle->value --;
	pthread_mutex_unlock( &(handle->hmux) );
	
	res = sem_trywait( &(handle->handle) );
	if( 0 == res ) {
		return 0;
	}

	if( iTimeout == OSA_WAIT_INFINITE ) {
		res = sem_wait( &(handle->handle) );
		return 0;
	} else if( iTimeout == OSA_WAIT_NO ) {
		ret = E_OSA_SEM_TIMEOUT;

		goto _OSA_SEM_WAIT_TIMEOUT_ERR0;
	} else {
		tp.tv_sec = iTimeout / 1000;
		tp.tv_nsec = ( iTimeout % 1000 ) * 1000000;

		/* wait specific time */
		do {
			res = nanosleep(&tp, &tp);

			if( res == 0 ) {
				break;
			}
			res = sem_trywait( &(handle->handle) );
		} while( res != 0 );

		if( res != 0 )
			return 0;
		else {
			ret = E_OSA_SEM_TIMEOUT;
			goto _OSA_SEM_WAIT_TIMEOUT_ERR0;
		}
	}

_OSA_SEM_WAIT_TIMEOUT_ERR0:
	pthread_mutex_lock( &(handle->hmux) );
	handle->value ++;
	pthread_mutex_unlock( &(handle->hmux) );

	return ret;
}

/******************************************************************************
 *	Signal semaphore
 *
 *	Parameter:
 *		hSem				[in] Sem's handle
 *
 *	Return value:
 *		0					Success
 *		E_OSA_SEM_HANDLE	Input handle error
 *		E_OSA_SEM_SIGNAL	Failed to signal
 *****************************************************************************/
OSA_RESULT	osa_sem_signal(OSA_HANDLE hSem)
{
    SemHandle_t *handle = NULL;
	int			res;

	if( _osa_sem_checkHandle(hSem, &handle) ) {
        d1(printf("osa_sem_signal: Sem's handle error!\n"));
		return E_OSA_SEM_HANDLE;
	}

	pthread_mutex_lock( &(handle->hmux) );
	handle->value ++;
	pthread_mutex_unlock( &(handle->hmux) );

	res = sem_post( &(handle->handle) );
	if( res ) {
		return E_OSA_SEM_SIGNAL;
	} else {
		return 0;
	}
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
 *		E_OSA_SEM_HANDLE	Input handle error
 *****************************************************************************/
OSA_RESULT	osa_sem_getValue(OSA_HANDLE hSem, ri32 *piVal)
{
    SemHandle_t *handle = NULL;

	if( _osa_sem_checkHandle(hSem, &handle) ) {
        d1(printf("osa_sem_getValue: Sem's handle error!\n"));
		return E_OSA_SEM_HANDLE;
	}

	pthread_mutex_lock( &(handle->hmux) );
	*piVal = handle->value;
	pthread_mutex_unlock( &(handle->hmux) );
	
	return 0;
}


} // end of namespace pi

#endif
