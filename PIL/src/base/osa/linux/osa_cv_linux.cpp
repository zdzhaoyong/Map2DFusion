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
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>
     
#include "../osa.h"
#include "../_osa_in.h"


namespace pi {


typedef struct CvHandle {
    list_t          list;
    ru32            magic;

    pthread_cond_t  *handle;
} CvHandle_t;


static CvHandle_t   *g_CvList = NULL;
static sem_t        g_ListMux;

/******************************************************************************
 *  Local: Check mutex handle
 *
 *  Parameter:
 *      hMu             [in] input handle
 *      handle          [out] inner used handle
 *
 *  Return value:
 *      0               Success
 *      E_OSA_MU_HANDLE   handle error
 *****************************************************************************/
inline OSA_RESULT _osa_cv_checkHandle(OSA_HANDLE hCv, CvHandle_t **handle)
{
    if( handle == NULL )
        return -1;

    *handle = (CvHandle_t *) hCv;

    try {
        if( OSA_CV_MAGIC == (*handle)->magic )
            return 0;
        else
            return E_OSA_CV_HANDLE;
    } catch(...) {
        return E_OSA_CV_HANDLE;
    }
}


/******************************************************************************
 *  Initialize condition-variable sub-system
 *
 *  Parameter:
 *      None
 *
 *  Return value:
 *      0                       Success
 *****************************************************************************/
OSA_RESULT   osa_cv_init(void)
{
    d3(printf("osa_cv_init:\n"));
        
    g_CvList = (CvHandle_t*) malloc(sizeof(CvHandle_t));
    memset(g_CvList, 0, sizeof(CvHandle_t));

    INIT_LIST_HEAD( &(g_CvList->list) );
    g_CvList->magic = OSA_CV_MAGIC;

    sem_init(&g_ListMux, 0, 1);

    d3(printf("osa_cv_init: end\n"));
    
    return 0;
}

/******************************************************************************
 *  De-init condition-variable sub-system
 *
 *  Parameter:
 *      None
 *
 *  Return value:
 *      0                       Success
 *****************************************************************************/
OSA_RESULT   osa_cv_release(void)
{
    CvHandle_t  *phandle;
    list_t      *plist;

    d3(printf("osa_cv_release:\n"));
    
    plist = &(g_CvList->list);
    while( plist->next != plist->prev ) {
        phandle = list_entry(plist->next, CvHandle_t, list);

        osa_cv_delete( (OSA_HANDLE) phandle );
    }

    sem_destroy(&g_ListMux);
    
    memset(g_CvList, 0, sizeof(CvHandle_t));
    free(g_CvList);
    g_CvList = NULL;

    d3(printf("osa_cv_release: end\n"));

    return 0;
}

/******************************************************************************
 *  Create CV 
 *
 *  Parameter:
 *      hCV                 [out] cv handle
 *      mode                [in] create flag
 *
 *  Return value:
 *      0                   Success
 *****************************************************************************/
OSA_RESULT osa_cv_create(OSA_HANDLE *hCV, ru32 mode)
{
    CvHandle_t  *phandle;
    int         res;

    phandle = (CvHandle_t*) malloc(sizeof(CvHandle_t));
    INIT_LIST_HEAD( &(phandle->list) );
    phandle->magic = OSA_CV_MAGIC;

    res = pthread_cond_init( phandle->handle, NULL );

    // add to list
    if( g_CvList != NULL ) {
        sem_wait( &g_ListMux );
        list_add( &(phandle->list), &(g_CvList->list) );
        sem_post( &g_ListMux );
    }

    *hCV = (OSA_HANDLE) phandle;

    return 0;
}

/******************************************************************************
 *  Delete given cv 
 *
 *  Parameter:
 *      hCV                 [in] cv handle
 *
 *  Return value:
 *      0                   Success
 *****************************************************************************/
OSA_RESULT osa_cv_delete(OSA_HANDLE hCV)
{
    CvHandle_t *phandle = NULL;

    if( _osa_cv_checkHandle(hCV, &phandle) ) {
        d1(printf("osa_cv_delete: Input handle error!\n"));
        return E_OSA_CV_HANDLE;
    }

    /* FIXME: may be failed when user call osa_cv_wait */
    pthread_cond_destroy( phandle->handle );

    if( g_CvList != NULL ) {
        sem_wait( &g_ListMux );
        list_del( &(phandle->list) );
        sem_post( &g_ListMux );
    }

    memset(phandle, 0, sizeof(CvHandle_t));
    free(phandle);

    return 0;
}

/******************************************************************************
 *  Wait for CV signal
 *
 *  Parameter:
 *      hCV                 [in] cv's handle
 *      hMu                 [in] peer mutex handle
 *
 *  Return value:
 *      0                   Success
 *****************************************************************************/
OSA_RESULT osa_cv_wait(OSA_HANDLE hCV, OSA_HANDLE hMu)
{
    CvHandle_t      *phandle = NULL;
    MutexHandle_t   *pMuxHandle;
    OSA_RESULT      res;
    int             cvRes;

    if( _osa_cv_checkHandle(hCV, &phandle) ) {
        d1(printf("osa_cv_wait: Input handle error!\n"));
        return E_OSA_CV_HANDLE;
    }

    if( _osa_mu_checkHandle(hMu, &pMuxHandle) ) {
        d1(printf("osa_cv_wait: Input mutex handle error!\n"));
        return E_OSA_MU_HANDLE;
    }

    cvRes = pthread_cond_wait(phandle->handle, &(pMuxHandle->handle));
    if( cvRes ) {
        d1(printf("osa_cv_wait: Failed at pthread_cond_wait!\n"));
        return E_OSA_CV_WAIT;
    }

    return 0;
}

/******************************************************************************
 *  Wait cv for a timeout
 *
 *  Parameter:
 *      hCV                     [in] cv handle
 *      hMu                     [in] peer mutex handle
 *      timeout                 [in] time out value (in ms)
 *
 *  return value:
 *      0                       Success
 *      -1                      get time error
 *      E_OSA_CV_HANDLE         input cv handle error
 *      E_OSA_MU_HANDLE         input mutex handle error
 *      E_OSA_CV_TIMEOUT        Wait time out
 *****************************************************************************/
OSA_RESULT osa_cv_wait_timeout(OSA_HANDLE hCV, OSA_HANDLE hMu, ru32 timeout)
{
    CvHandle_t      *phandle = NULL;
    MutexHandle_t   *pMuxHandle;
    int             ret;
    
    struct timeval  begin_time; 
    struct timespec end_time;

    /* check cv handle */
    if( _osa_cv_checkHandle(hCV, &phandle) ) {
        d1(printf("osa_cv_wait_timeout: Input handle error!\n"));
        return E_OSA_CV_HANDLE;
    }

    /* check mutex handle */
    if( _osa_mu_checkHandle(hMu, &pMuxHandle) ) {
        d1(printf("osa_cv_wait: Input mutex handle error!\n"));
        return E_OSA_MU_HANDLE;
    }

    ret = gettimeofday(&begin_time, 0);
    if( ret ) {
        d1(printf("osa_cv_wait_timeout: Failed to get time(gettimeofday)!\n"));
        return -1;
    }

    end_time.tv_sec = begin_time.tv_sec + (ru32)(timeout / 1000);
    /* FIXME: calucat us maybe error */
    end_time.tv_nsec = (begin_time.tv_usec + (timeout % 1000) * 1000 ) * 1000;

    if(end_time.tv_nsec >= 1000000000) {
        end_time.tv_sec++;
        end_time.tv_nsec -= 1000000000;
    }

    ret = pthread_cond_timedwait(phandle->handle, &(pMuxHandle->handle), &end_time);
    
    if ( ETIMEDOUT == ret ) 
        return E_OSA_CV_HANDLE;
    
    return 0;
}

/******************************************************************************
 *  Set the cv to signal state
 *
 *  Parameter:
 *      hCV                     [in] cv handle
 *
 *  Return value:
 *      0                       Success
 *      E_OSA_CV_HANDLE       Input cv handle error
 *      E_OSA_CV_SIGNAL       Failed at cv signal
 *****************************************************************************/
OSA_RESULT   osa_cv_signal(OSA_HANDLE hCV)
{
    CvHandle_t  *phandle = NULL;
    int         ret;

    if( _osa_cv_checkHandle(hCV, &phandle) ) {
        d1(printf("osa_cv_signal: Input handle error!\n"));
        return E_OSA_CV_HANDLE;
    }

    ret = pthread_cond_signal(phandle->handle);
    if( ret ) {
        d1(printf("osa_cv_signal: Failed at pthread_cond_signal!\n"));
        return  E_OSA_CV_SIGNAL;
    }

    return 0;
}

/******************************************************************************
 *  broadcast cv signal stat
 *
 *  Parameter:
 *      hCV                     [in] cv handle
 *
 *  Return value:
 *      0                       Success
 *****************************************************************************/
OSA_RESULT   osa_cv_broadcast(OSA_HANDLE hCV)
{
    CvHandle_t  *phandle = NULL;
    int         ret;
    
    if( _osa_cv_checkHandle(hCV, &phandle) ) {
        d1(printf("osa_cv_broadcast: Input handle error!\n"));
        return E_OSA_CV_HANDLE;
    }

    ret = pthread_cond_broadcast( phandle->handle );
    if( ret ) {
        d1(printf("osa_cv_broadcast: Failed at pthread_cond_broadcast!\n"));
        return E_OSA_CV_BROADCAST;
    }

    return 0;   
}

} // end namespace pi

#endif // end of PIL_LINUX
