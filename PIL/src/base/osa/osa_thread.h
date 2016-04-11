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

#ifndef __RTK_OSA_THREAD_H__
#define __RTK_OSA_THREAD_H__

#include "osa.h"

namespace pi {

/* Thread defines & functions */
typedef ru32        OSA_T_ID;
typedef	void*       OSA_T_ARGS;
typedef ru32		OSA_PRIORITY;

typedef OSA_RESULT  (*OSA_T_FUNC)(OSA_T_ARGS);

/* define thread schedule police */
#define	OSA_T_RT_PREEMPT		4
#define	OSA_T_RT_FIFO		    2
#define	OSA_T_NR_TSLICE		    1

/* define thread priority range */
#define	OSA_T_PRIORITY_RANGE	100
#define	OSA_T_PRIORITY_MAX		99
#define	OSA_T_PRIORITY_MIN		0

/* default thread stack size (8M for desktop PC) */
#define OSA_T_STACK_SIZE        8388608

/* define Thread Sates */
#define OSA_TS_READY	0
#define OSA_TS_RUN		1
#define OSA_TS_SUSPEND	2
#define OSA_TS_STOP		3

/* define Thread operations */
#define OSA_TO_CREATE	0
#define OSA_TO_SUSPEND	1
#define OSA_TO_CONTINUE	2
#define OSA_TO_KILL		3

/* define Thread Create flags */
#define OSA_T_CREATESUSPEND		1

#ifdef USE_C
    #ifdef __cplusplus
    extern "C" {
    #endif
#endif

OSA_RESULT	osa_t_init(void);
OSA_RESULT	osa_t_release(void);

OSA_RESULT	osa_t_create(OSA_HANDLE *hTask, OSA_T_FUNC func, OSA_T_ARGS arg,
                ru32 stackSize=OSA_T_STACK_SIZE, ru32 mode=0);

OSA_RESULT	osa_t_kill(OSA_HANDLE hTask);
OSA_RESULT	osa_t_delete(OSA_HANDLE hTask);
OSA_HANDLE	osa_t_self(void);
OSA_RESULT	osa_t_wait(OSA_HANDLE hTask, ru32 timeout=OSA_WAIT_INFINITE);
ru32      osa_t_getid(OSA_HANDLE hTask=0);

OSA_RESULT	osa_t_sleep(ru32 iTime);
OSA_RESULT	osa_t_suspend(OSA_HANDLE hTask);
OSA_RESULT	osa_t_resume(OSA_HANDLE hTask);
OSA_RESULT	osa_t_reschedule(OSA_HANDLE hTask);

OSA_RESULT	osa_t_setPriority(OSA_HANDLE hTask, OSA_PRIORITY iPriority);
OSA_RESULT	osa_t_getPriority(OSA_HANDLE hTask, OSA_PRIORITY *piPriority);

#ifdef USE_C
    #ifdef __cplusplus
    }
    #endif
#endif

} // end namespace pi

#endif /* end of __RTK_OSA_THREAD_H__ */

