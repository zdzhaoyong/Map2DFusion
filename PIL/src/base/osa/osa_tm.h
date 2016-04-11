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

#ifndef __RTK_OSA_TM_H__
#define __RTK_OSA_TM_H__

#include "osa.h"

namespace pi {

/* Time & Timer defines & functions */
typedef struct OSA_DATETIME {
	ru32	year;
	ru32	month;
	ru32	day;
	ru32	hour;
	ru32	minute;
	ru32	second;
	ru32	millis;
} OSA_DATETIME_T;

typedef void (*OSA_TM_FUNCPTR)(void *data);

#define OSA_TM_ONCE			1
#define	OSA_TM_PERIODIC		2

// The property of 
typedef enum
{
	OSA_TM_STAT_RUNNING,
	OSA_TM_STAT_SUSPEND,
    OSA_TM_STAT_STOPPED,
	OSA_TM_STAT_INVALID
} OSA_TM_STAT;

typedef enum
{
	OSA_TM_PRO_RUNONCE	= OSA_TM_ONCE,
	OSA_TM_PRO_PERIODIC	= OSA_TM_PERIODIC,
	OSA_TM_PRO_INVALID
} OSA_TM_PRO;

#ifdef USE_C
    #ifdef __cplusplus
    extern "C" {
    #endif
#endif

OSA_RESULT	osa_tm_init(void);
OSA_RESULT	osa_tm_release(void);

OSA_RESULT	osa_tm_get(OSA_DATETIME_T *datetime);
OSA_RESULT	osa_tm_set(OSA_DATETIME_T *datetime);

OSA_RESULT	osa_tm_getMillis(ru64 *time);
OSA_RESULT	osa_tm_setMillis(ru64 time);


OSA_RESULT	osa_tm_create(OSA_HANDLE *hTimer, 
                ru32 uRes, 
                OSA_TM_FUNCPTR func, OSA_DATA arg,
                ru32 mode = OSA_TM_PRO_PERIODIC);
OSA_RESULT  osa_tm_pause(OSA_HANDLE hTimer);
OSA_RESULT  osa_tm_resume(OSA_HANDLE hTimer);

OSA_RESULT	osa_tm_delete(OSA_HANDLE hTimer);
OSA_RESULT	osa_tm_reset(OSA_HANDLE hTimer);

#ifdef USE_C
    #ifdef __cplusplus
    }
    #endif
#endif

} // end namespace pi

#endif /* end of __RTK_OSA_TM_H__ */

