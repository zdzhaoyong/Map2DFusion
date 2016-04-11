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

#ifndef __RTK_OSA_TSD_H__
#define __RTK_OSA_TSD_H__

#include "osa.h"

namespace pi {

#ifdef USE_C
    #ifdef __cplusplus
    extern "C" {
    #endif
#endif

/* Thread specific data Defines & functions */
OSA_RESULT	osa_tsd_init(void);
OSA_RESULT	osa_tsd_release(void);

OSA_RESULT	osa_tsd_set(OSA_DATA Data);
OSA_RESULT	osa_tsd_get(OSA_DATA *Data);

#ifdef USE_C
    #ifdef __cplusplus
    }
    #endif
#endif

} // end of namespace pi

#endif /* end of __RTK_OSA_TSD_H__ */

