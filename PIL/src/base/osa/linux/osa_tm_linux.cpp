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


#define	_OSA_TIMER_RESOLUTION       10

#define	_OSA_TIMER_STATE_READY      0
#define	_OSA_TIMER_STATE_RUN        1
#define	_OSA_TIMER_STATE_STOP       2
#define _OSA_TIMER_STATE_PAUSE      3

typedef struct TimerHandle {
	list_t			list;
    ru32			magic;
	
    ru32			res;
    ru64			count;

    ru32			mode;
	void 			*arg;
    OSA_TM_FUNCPTR	func;

    ru32			state;

    OSA_HANDLE      tm_thread;
} TimerHandle_t;

static TimerHandle_t    *g_TimerList = NULL;
static ru32             g_TimerCount = 1;
static OSA_HANDLE       g_ListMux;


static OSA_RESULT _tm_func(OSA_T_ARGS arg);

OSA_RESULT	_osa_tm_beginTimer(TimerHandle_t *tm_handle);
OSA_RESULT	_osa_tm_endTimer(TimerHandle_t *tm_handle);


/******************************************************************************
 *	Local: Check timer handle
 *
 *	Parameter:
 *		hMu				[in] input handle
 *		handle			[out] inner used handle
 *
 *	Return value:
 *		0				Success
 *		E_OSA_TM_HANDLE	handle error
 *****************************************************************************/
inline OSA_RESULT _osa_tm_checkHandle(OSA_HANDLE hTM, TimerHandle_t **handle)
{
    if( NULL == handle )
        return E_OSA_TM_HANDLE;

    *handle = (TimerHandle_t *) hTM;

    try {
        if( OSA_TM_MAGIC == (*handle)->magic )
            return 0;
        else
            return E_OSA_TM_HANDLE;
    } catch( ... ) {
        return E_OSA_TM_HANDLE;
    }

    return E_OSA_TM_HANDLE;
}

/******************************************************************************
 *	Init time/timer sub-system
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0				Success
 *****************************************************************************/
OSA_RESULT	osa_tm_init(void)
{
    g_TimerList = (TimerHandle_t*) malloc(sizeof(TimerHandle_t));
	memset(g_TimerList, 0, sizeof(TimerHandle_t));
	INIT_LIST_HEAD( &(g_TimerList->list) );

    osa_mu_create(&g_ListMux);

    //g_TimerCount = 0;
	
	return 0;
}

/******************************************************************************
 *	Init time/timer sub-system
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0				Success
 *****************************************************************************/
OSA_RESULT	osa_tm_release(void)
{
    TimerHandle_t   *phandle;
    list_t          *plist;

    osa_mu_lock(g_ListMux);
	plist = &(g_TimerList->list);
	while( plist->next != plist->prev ) {
		phandle = list_entry(plist->next, TimerHandle_t, list);

        // end timer thread
        _osa_tm_endTimer(phandle);

		list_del( &(phandle->list) );
		memset(phandle, 0, sizeof(TimerHandle_t));
		free(phandle);
	}
    osa_mu_unlock(g_ListMux);
	
    osa_mu_delete(g_ListMux);

	memset(g_TimerList, 0, sizeof(TimerHandle_t));
	free(g_TimerList);

	return 0;
}

/******************************************************************************
 *	Get system time
 *
 *	Parameter:
 *		datetime			[in] date & time
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_tm_get(OSA_DATETIME_T *datetime)
{	
	struct timeval tval;
   	struct tm *ptime;

  	gettimeofday(&tval, NULL);
    	
   	ptime = gmtime( &(tval.tv_sec) );

	datetime->year 		= ptime->tm_year + 1900;
	datetime->month 	= ptime->tm_mon + 1;
	datetime->day 		= ptime->tm_mday;
	datetime->hour 		= ptime->tm_hour;
	datetime->minute 	= ptime->tm_min;
	datetime->second 	= ptime->tm_sec;
	datetime->millis 	= tval.tv_usec / 1000;

	return 0;
}

/******************************************************************************
 *	Set system time
 *
 *	Parameter:
 *		datetime			[in] date & time
 *
 *	Return value:
 *		0					Success
 *		E_OSA_TM_SET		Failed to set time
 *****************************************************************************/
OSA_RESULT	osa_tm_set(OSA_DATETIME_T *datetime)
{
	struct tm time;
	struct timeval tval;
	int		ret;
	
	time.tm_year 	= datetime->year - 1900;
	time.tm_mon 	= datetime->month - 1;
	time.tm_mday 	= datetime->day;
	time.tm_hour 	= datetime->hour;
	time.tm_min 	= datetime->minute;
	time.tm_sec 	= datetime->second;
	
	tval.tv_sec 	= mktime(&time);
	tval.tv_usec 	= 0 ;
	
	ret = settimeofday(&tval, NULL);
	
	return ret;
}

/******************************************************************************
 *	Get tick cout in ms
 *
 *	Parameter:
 *		time			[out] time in ms
 *
 *	Return value:
 *		0				Success
 *****************************************************************************/
OSA_RESULT	osa_tm_getMillis(ru64 *time)
{
	struct timeval  tm_val;
    ru64			isec;
	int				ret;
	
    ret = gettimeofday(&tm_val, NULL);

	isec = tm_val.tv_sec;
	*time = isec * 1000 + tm_val.tv_usec / 1000;
	
	if ( 0 != ret ) 
		return -1;
	else
		return 0;
}

/******************************************************************************
 *	Set time in milli-second
 *
 *	Parameter:
 *		time			[in] time in ms
 *
 *	Return value:
 *		0				Success
 *****************************************************************************/
OSA_RESULT	osa_tm_setMillis(ru64 time)
{
	struct timeval 	tm_val;
	int				ret;

	tm_val.tv_sec = time /1000;
	tm_val.tv_usec = ( time % 1000 ) * 1000;

	ret = settimeofday(&tm_val,NULL);
	
	if ( 0 != ret )
		return -1;
	else
		return 0;
}

/******************************************************************************
 *	Create timer
 *
 *	Parameter:
 *		hTimer			[out] Timer handle
 *		uRes			[in] Timer resolution (in ms)
 *		func			[in] call-back function
 *		mode			[in] create flags
 *							OSA_TM_ONCE		Event occures once
 *							OSA_TM_PERIODIC	Event occurs every time
 *
 *	Return value:
 *		0				Success
 *****************************************************************************/
OSA_RESULT	osa_tm_create(OSA_HANDLE *hTimer, ru32 uRes,
                          OSA_TM_FUNCPTR func, OSA_DATA arg, ru32 mode)
{
    OSA_RESULT      ret = -1;
    TimerHandle_t   *phandle;


    phandle = (TimerHandle_t*) malloc(sizeof(TimerHandle_t));
	memset(phandle, 0, sizeof(TimerHandle_t));
	INIT_LIST_HEAD( &(phandle->list) );

    phandle->magic = OSA_TM_MAGIC;
    phandle->res   = uRes;
    phandle->func  = func;
    phandle->arg   = arg;
    phandle->mode  = mode;
    phandle->count = 0;
    phandle->state = _OSA_TIMER_STATE_READY;

    ret = _osa_tm_beginTimer(phandle);
    if( ret != E_OSA_OK ) {
        free(phandle);
        return E_OSA_TM_CREATE;
    }

    if( g_TimerList != NULL ) {
        osa_mu_lock(g_ListMux);
        list_add( &(phandle->list), &(g_TimerList->list) );
        osa_mu_unlock(g_ListMux);
    }
    g_TimerCount++;

    *hTimer = (OSA_HANDLE) phandle;

	return 0;
}

/******************************************************************************
 *	Timer pause
 *
 *	Parameter:
 *		hTimer				[in] Timer handle
 *
 *	Return vlaue:
 *		0					Success
 *		E_OSA_TM_HANDLE     Input handle error
 *****************************************************************************/
OSA_RESULT  osa_tm_pause(OSA_HANDLE hTimer)
{
    TimerHandle_t *phandle = NULL;

    if( 0 != _osa_tm_checkHandle(hTimer, &phandle) ) {
        d1(printf("osa_tm_pause: input handle error\n"));
        return E_OSA_TM_HANDLE;
    }

    phandle->state = _OSA_TIMER_STATE_PAUSE;

    return 0;
}

/******************************************************************************
 *	Timer resume
 *
 *	Parameter:
 *		hTimer				[in] Timer handle
 *
 *	Return vlaue:
 *		0					Success
 *		E_OSA_TM_HANDLE     Input handle error
 *****************************************************************************/
OSA_RESULT  osa_tm_resume(OSA_HANDLE hTimer)
{
    TimerHandle_t *phandle = NULL;

    if( 0 != _osa_tm_checkHandle(hTimer, &phandle) ) {
        d1(printf("osa_tm_resume: input handle error\n"));
        return E_OSA_TM_HANDLE;
    }

    phandle->state = _OSA_TIMER_STATE_RUN;

    return 0;
}

/******************************************************************************
 *	Delete a given timer
 *
 *	Parameter:
 *		hTimer				[in] Timer handle
 *
 *	Return vlaue:
 *		0					Success
 *		E_OSA_TM_HANDLE	Input handle error
 *****************************************************************************/
OSA_RESULT	osa_tm_delete(OSA_HANDLE hTimer)
{
    TimerHandle_t *phandle = NULL;

    if( 0 != _osa_tm_checkHandle(hTimer, &phandle) ) {
        d1(printf("osa_tm_delete: input handle error\n"));
        return E_OSA_TM_HANDLE;
	}

    // end timer
    _osa_tm_endTimer(phandle);

    if( g_TimerList != NULL ) {
        osa_mu_lock(g_ListMux);
		list_del( &(phandle->list) );
        osa_mu_unlock(g_ListMux);
    }
    g_TimerCount --;

	memset(phandle, 0, sizeof(TimerHandle_t));
	free(phandle);
	
	return 0;
}


OSA_RESULT _tm_func(OSA_T_ARGS arg)
{
    TimerHandle_t   *tm;
    ru64          t_beg, t_cur, t_sleep, t_det;
    ru64          i;

    tm = (TimerHandle_t *) arg;

    if( tm->mode == OSA_TM_PRO_INVALID ) {
        tm->state = OSA_TM_STAT_INVALID;
        return -1;
    } else if( tm->mode == OSA_TM_PRO_RUNONCE ) {
        tm->state = OSA_TM_STAT_RUNNING;
        osa_t_sleep(tm->res);
        tm->func(tm->arg);
        tm->count ++;
        tm->state = OSA_TM_STAT_STOPPED;
    } else {
        tm->state = OSA_TM_STAT_RUNNING;

        // sleep for fist time
        osa_t_sleep(tm->res);

        osa_tm_getMillis(&t_beg);
        i = 1;

        while(1) {
            // run timer function
            if( tm->state == OSA_TM_STAT_RUNNING ) {
                tm->func(tm->arg);
                tm->count ++;
            }

            // get finished time
            osa_tm_getMillis(&t_cur);

            // wait for some time
            t_det = t_beg + (i++)*tm->res;
            if( t_cur < t_det ) {
                t_sleep = t_det - t_cur;
                osa_t_sleep(t_sleep);
            }
        }

        tm->state = OSA_TM_STAT_STOPPED;
    }

    return 0;
}


/******************************************************************************
 *	Local: Begin inner timer source
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0					Success
 *		E_OSA_TM_CREATE	Failed to create timer source
 *****************************************************************************/
OSA_RESULT	_osa_tm_beginTimer(TimerHandle_t *tm)
{
    OSA_RESULT      ret;

    ret = osa_t_create(&(tm->tm_thread), _tm_func, tm);
    if( ret != E_OSA_OK ) {
        d1(printf("_osa_tm_beginTimer: Failed to create thread!\n"));
        return -1;
    }

	return 0;
}

/******************************************************************************
 *	Local: End inner timer source
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0                   Success
 *		E_OSA_TM_CREATE     Failed to create timer source
 *****************************************************************************/
OSA_RESULT	_osa_tm_endTimer(TimerHandle_t *tm)
{
    OSA_RESULT      ret;

    ret = osa_t_kill(tm->tm_thread);
    ret = osa_t_delete(tm->tm_thread);

    tm->state = OSA_TM_STAT_STOPPED;

    return 0;
}


} // end of namespace pi


#endif
