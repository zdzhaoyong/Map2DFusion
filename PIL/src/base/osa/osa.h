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


#ifndef __RTK_OSA_H__
#define __RTK_OSA_H__

#include <stdio.h>
#include <stdint.h>

#include "base/types/types.h"

namespace pi {

typedef 	size_t	    	OSA_HANDLE;
typedef		ri32			OSA_RESULT;
typedef		void*           OSA_DATA;

#define OSA_WAIT_NO			0x00000000
#define OSA_WAIT_INFINITE	0xFFFFFFFF

} // end of namespace pi


#include "osa_err.h"
#include "osa_thread.h"
#include "osa_tsd.h"
#include "osa_semaphore.h"
#include "osa_mutex.h"
#include "osa_cv.h"
#include "osa_mq.h"
#include "osa_tm.h"
#include "osa_mem.h"
#include "osa_event.h"
#include "osa_utils.h"

namespace pi {

#ifdef USE_C
    #ifdef __cplusplus
    extern "C" {
    #endif
#endif

/* pi osa functions */
OSA_RESULT	osa_init(void);
OSA_RESULT	osa_release(void);

#ifdef USE_C
    #ifdef __cplusplus
    }
    #endif
#endif

/* define debug macros */
#ifndef CONFIG_OSA_VERBOSE
#define CONFIG_OSA_VERBOSE 1
#endif

} // end of namespace pi

#endif /* end of __RTK_OSA_H__ */

