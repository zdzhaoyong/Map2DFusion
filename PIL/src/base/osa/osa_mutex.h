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


#ifndef __RTK_OSA_MUTEX_H__
#define __RTK_OSA_MUTEX_H__

#include "osa.h"

namespace pi {

/* Mutex defines & functions */
#define	OSA_MU_FIFO			1
#define	OSA_MU_TIMEOUT		4

#ifdef USE_C
    #ifdef __cplusplus
    extern "C" {
    #endif
#endif

OSA_RESULT	osa_mu_init(void);
OSA_RESULT	osa_mu_release(void);

OSA_RESULT	osa_mu_create(OSA_HANDLE *hMu, ru32 mode=0);
OSA_RESULT	osa_mu_delete(OSA_HANDLE hMu);
OSA_RESULT	osa_mu_lock(OSA_HANDLE hMu);
OSA_RESULT	osa_mu_lock_timeout(OSA_HANDLE hMu, ru32 iTime=OSA_WAIT_INFINITE);
OSA_RESULT	osa_mu_unlock(OSA_HANDLE hMu);
OSA_RESULT	osa_mu_isLock(OSA_HANDLE hMu);

#ifdef USE_C
    #ifdef __cplusplus
    }
    #endif
#endif

} // end namespace pi

#endif /* end of __RTK_OSA_MUTEX_H__ */

