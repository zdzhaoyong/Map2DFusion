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

#include <stdio.h>
#include <windows.h>
	 
#include "base/types/FastList.h"
#include "../osa.h"
#include "../_osa_in.h"


namespace pi {


#define	_OSA_TIMER_RESOLUTION       10

#define	_OSA_TIMER_STATE_READY      0
#define	_OSA_TIMER_STATE_RUN        1
#define	_OSA_TIMER_STATE_STOP       2


/******************************************************************************
 *	Local struct
 *****************************************************************************/
typedef struct TimerHandle {
    list_t          list;
    ru32            magic;
	
    ru32            res;
    ru32            count;
    ru32            mode;
    void            *arg;
    OSA_TM_FUNCPTR  func;

    ru32            state;
} TimerHandle_t;


/******************************************************************************
 *	Local variables
 *****************************************************************************/
static TimerHandle_t    	*g_TimerList = NULL;
static HANDLE       		g_ListMux;
static TIMECAPS 			g_TimeCaps;
static UINT 				g_TimerHandler;
static ru32					g_TimerCount;


OSA_RESULT	_osa_tm_beginTimer(void);
OSA_RESULT	_osa_tm_endTimer(void);

void CALLBACK _osa_tm_TimeProc(UINT uID, UINT uMsg, DWORD dwUser,
                               DWORD dw1, DWORD dw2);


/******************************************************************************
 *	Local: Check timer handle
 *
 *	Parameter:
 *		hMu				[in] input handle
 *		handle			[out] inner used handle
 *
 *	Return value:
 *		0				Success
 *		E_PLOSA_TM_HANDLE	handle error
 *****************************************************************************/
inline OSA_RESULT _osa_tm_checkHandle(OSA_HANDLE hTM, TimerHandle_t **handle)
{
    *handle = (TimerHandle_t *) hTM;

    if( NULL != *handle ) {
        try {
            if( OSA_TM_MAGIC == (*handle)->magic ) {
                return 0;
            }
        } catch (...) {
            return -1;
        }
    }

    return -1;
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
    if( g_TimerList != NULL )
        osa_tm_release();

    g_TimerList = (TimerHandle_t*) malloc(sizeof(TimerHandle_t));
	memset(g_TimerList, 0, sizeof(TimerHandle_t));
	INIT_LIST_HEAD( &(g_TimerList->list) );

	g_ListMux = CreateMutex(NULL, FALSE, NULL);

	timeGetDevCaps(&g_TimeCaps, sizeof(TIMECAPS));
	g_TimerCount = 0;
	
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

    if( g_TimerList != NULL ) {
        WaitForSingleObject(g_ListMux, INFINITE);
        plist = &(g_TimerList->list);
        while( plist->next != plist->prev ) {
            phandle = list_entry(plist->next, TimerHandle_t, list);

            list_del( &(phandle->list) );
            memset(phandle, 0, sizeof(TimerHandle_t));
            free(phandle);
        }

        if( g_TimerCount > 0 )
            _osa_tm_endTimer();

        ReleaseMutex(g_ListMux);

        CloseHandle(g_ListMux);

        memset(g_TimerList, 0, sizeof(TimerHandle_t));
        free(g_TimerList);
    }

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
	SYSTEMTIME	systime;


	GetSystemTime(&systime);

    datetime->year      = systime.wYear;
    datetime->month     = systime.wMonth;
    datetime->day       = systime.wDay;
    datetime->hour      = systime.wHour;
    datetime->minute    = systime.wMinute;
    datetime->second    = systime.wSecond;
    datetime->millis    = systime.wMilliseconds;

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
 *		E_PLOSA_TM_SET		Failed to set time
 *****************************************************************************/
OSA_RESULT	osa_tm_set(OSA_DATETIME_T *datetime)
{
	SYSTEMTIME systime;


    systime.wYear           = datetime->year;
    systime.wMonth          = datetime->month;
    systime.wDay            = datetime->day;
    systime.wHour           = datetime->hour;
    systime.wMinute         = datetime->minute;
    systime.wSecond         = datetime->second;
    systime.wMilliseconds   = datetime->millis;

	if( SetSystemTime(&systime) )
		return 0;
	else
        return E_OSA_TM_SET;
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
	*time = GetTickCount();
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
	TimerHandle_t *phandle;
	
    d3(printf("osa_tm_create: \n"));
	
	/* check input parameter */
	if( (uRes > g_TimeCaps.wPeriodMax) || (uRes < _OSA_TIMER_RESOLUTION) ) {
        d1(printf("osa_tm_create: input time resolution out of range!\n"));
        return E_OSA_TM_RESOLUTION;
	}
		
    phandle = (TimerHandle_t*) malloc(sizeof(TimerHandle_t));
    memset(phandle, 0, sizeof(TimerHandle_t));
    INIT_LIST_HEAD( &(phandle->list) );

	phandle->magic = OSA_TM_MAGIC;
    phandle->res   = uRes;
    phandle->func  = func;
    phandle->arg   = arg;
    phandle->mode  = mode;
	phandle->count = uRes / _OSA_TIMER_RESOLUTION;
	phandle->state = _OSA_TIMER_STATE_RUN;

	WaitForSingleObject(g_ListMux, INFINITE);
	if( g_TimerCount++ == 0 ) {
		if( _osa_tm_beginTimer() ) {
			memset(phandle, 0, sizeof(TimerHandle_t));
			free(phandle);
			ReleaseMutex(g_ListMux);

            d1(printf("osa_tm_create: failed at _osa_tm_beginTimer()\n"));
            return E_OSA_TM_CREATE;
		}
	}
		
	list_add( &(phandle->list), &(g_TimerList->list) );
	phandle->state = _OSA_TIMER_STATE_RUN;
	ReleaseMutex(g_ListMux);

	*hTimer = (OSA_HANDLE) phandle;

    d3(printf("osa_tm_create: end\n"));

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
 *		E_PLOSA_TM_HANDLE	Input handle error
 *****************************************************************************/
OSA_RESULT	osa_tm_delete(OSA_HANDLE hTimer)
{
	TimerHandle_t *phandle;

    d3(printf("osa_tm_delete: begin\n"));
	
	if( _osa_tm_checkHandle(hTimer, &phandle) ) {
        d1(printf("osa_tm_delete: input handle error\n"));
        return E_OSA_TM_HANDLE;
	}

	/* set timer state */
	phandle->state = _OSA_TIMER_STATE_STOP;

	WaitForSingleObject(g_ListMux, INFINITE);
	if( --g_TimerCount == 0 )
		_osa_tm_endTimer();
	ReleaseMutex(g_ListMux);

    d3(printf("osa_tm_delete: end\n"));
	
	return 0;
}

/******************************************************************************
 *	reset a given timer
 *
 *	Parameter:
 *		hTimer				[in] Timer handle
 *
 *	Return vlaue:
 *		0					Success
 *		E_PLOSA_TM_HANDLE	Input handle error
 *****************************************************************************/
OSA_RESULT	osa_tm_reset(OSA_HANDLE hTimer)
{
	TimerHandle_t *phandle;

    d3(printf("osa_tm_reset: begin\n"));
	
	if( _osa_tm_checkHandle(hTimer, &phandle) ) {
        d1(printf("osa_tm_reset: input handle error\n"));
        return E_OSA_TM_HANDLE;
	}

	/* reset it to full count value */
	phandle->count = phandle->res / _OSA_TIMER_RESOLUTION;

    d3(printf("osa_tm_reset: end\n"));
	
	return 0;
}

/******************************************************************************
 *	Local: clean unused slot
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	_osa_tm_cleanList(void)
{
	TimerHandle_t	*phandle;
	list_t			*plist;

	plist = &(g_TimerList->list);
	list_for_each(plist, &(g_TimerList->list)) {
		phandle = list_entry(plist, TimerHandle_t, list);

		if( phandle->state == _OSA_TIMER_STATE_STOP ) {
			list_del( &(phandle->list) );

			memset(phandle, 0, sizeof(TimerHandle_t));
			free(phandle);
		}
	}

	return 0;
}

/******************************************************************************
 *	Local: Timer call-back function
 *
 *	Parameter:
 *		uID				[in] Timer id
 *		uMsg			[in] Win32 message
 *		dwUser			[in] User specific data
 *		dw1				[in] reserved 1
 *		dw2				[in] reserved 2
 *
 *	Return value:
 *		0				Success
 *****************************************************************************/
void CALLBACK _osa_tm_TimeProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
    TimerHandle_t   *phandle;
    list_t          *plist;

    /* d3(print("_osa_tm_TimeProc: \n")); */
	
	plist = &(g_TimerList->list);
	list_for_each(plist, &(g_TimerList->list)) {
		phandle = list_entry(plist, TimerHandle_t, list);

		if( phandle->count == 0 ||
			phandle->state != _OSA_TIMER_STATE_RUN )
			continue;
		
		if( --(phandle->count) == 0 ) {
			phandle->func(phandle->arg);

			if( phandle->mode & OSA_TM_ONCE ) {				
				WaitForSingleObject(g_ListMux, INFINITE);
				if( --g_TimerCount == 0 )
					_osa_tm_endTimer();
				ReleaseMutex(g_ListMux);

				phandle->state = _OSA_TIMER_STATE_STOP;
			} 

			if( phandle->mode & OSA_TM_PERIODIC )
				phandle->count = phandle->res / _OSA_TIMER_RESOLUTION;
		}
	}

	/* clear unused timer slot */
	_osa_tm_cleanList();
}

/******************************************************************************
 *	Local: Begin inner timer source
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0					Success
 *		E_PLOSA_TM_CREATE	Failed to create timer source
 *****************************************************************************/
OSA_RESULT	_osa_tm_beginTimer(void)
{
    d3(printf("_osa_tm_beginTimer: begin\n"));
	
	timeBeginPeriod(_OSA_TIMER_RESOLUTION);
	g_TimerHandler = timeSetEvent(_OSA_TIMER_RESOLUTION, 1, _osa_tm_TimeProc, 
							0, TIME_PERIODIC);

	if( 0 == g_TimerHandler ) {
        d1(printf("_osa_tm_beginTimer: Faild at timeSetEvent\n"));
        return E_OSA_TM_CREATE;
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
 *		0					Success
 *		E_PLOSA_TM_CREATE	Failed to create timer source
 *****************************************************************************/
OSA_RESULT	_osa_tm_endTimer(void)
{
    d3(printf("_osa_tm_endTimer: begin\n"));
	
	timeKillEvent(g_TimerHandler);
	timeEndPeriod(_OSA_TIMER_RESOLUTION);

	return 0;
}


} // end of namespace pi

#endif
