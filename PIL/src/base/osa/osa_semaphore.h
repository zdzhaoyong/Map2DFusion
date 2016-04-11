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

#ifndef __RTK_OSA_SEMAPHORE_H__
#define __RTK_OSA_SEMAPHORE_H__

#include "osa.h"

namespace pi {

/* Semaphore defines & functions */
#define	OSA_SEM_FIFO		1
#define	OSA_SEM_PRIORITY	2
#define	OSA_SEM_TIMEOUT		4

#ifdef USE_C
    #ifdef __cplusplus
    extern "C" {
    #endif
#endif

OSA_RESULT	osa_sem_init(void);
OSA_RESULT	osa_sem_release(void);

OSA_RESULT	osa_sem_create(OSA_HANDLE *hSem, ri32 iInitVal=0, ru32 mode=0);
OSA_RESULT	osa_sem_delete(OSA_HANDLE hSem);
OSA_RESULT	osa_sem_wait(OSA_HANDLE hSem);
OSA_RESULT	osa_sem_wait_timeout(OSA_HANDLE hSem, ru32 iTimeout=OSA_WAIT_INFINITE);
OSA_RESULT	osa_sem_signal(OSA_HANDLE hSem);
OSA_RESULT	osa_sem_getValue(OSA_HANDLE hSem, ri32 *piVal);

#ifdef USE_C
    #ifdef __cplusplus
    }
    #endif
#endif

} // end namespace pi

#endif /* end of __RTK_OSA_SEMAPHORE_H__ */

