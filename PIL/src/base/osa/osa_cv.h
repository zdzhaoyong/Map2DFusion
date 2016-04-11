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

#ifndef __OSA_CV_H__
#define __OSA_CV_H__

#include "osa.h"

namespace pi {

#ifdef USE_C
    #ifdef __cplusplus
    extern "C" {
    #endif
#endif

/* Condition variable */
OSA_RESULT	osa_cv_init(void);
OSA_RESULT	osa_cv_release(void);

OSA_RESULT	osa_cv_create(OSA_HANDLE *hCV, ru32 mode);
OSA_RESULT	osa_cv_delete(OSA_HANDLE hCV);
OSA_RESULT	osa_cv_wait(OSA_HANDLE hCV, OSA_HANDLE hMu);
OSA_RESULT	osa_cv_wait_timeout(OSA_HANDLE hCV, OSA_HANDLE hMu, ru32 timeout);
OSA_RESULT	osa_cv_signal(OSA_HANDLE hCV);
OSA_RESULT	osa_cv_broadcast(OSA_HANDLE hCV);

#ifdef USE_C
    #ifdef __cplusplus
    }
    #endif
#endif

} // end namespace pi

#endif /* end of __OSA_CV_H__ */

