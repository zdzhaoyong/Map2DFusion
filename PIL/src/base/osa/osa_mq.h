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

#ifndef __RTK_OSA_MQ_H__
#define __RTK_OSA_MQ_H__

#include "osa.h"

namespace pi {

/* MQ create mode defines */
#define	OSA_MQ_FIFO			1
#define	OSA_MQ_PRIORITY		2
#define	OSA_MQ_TIMEOUT		4

/* Define MQ priority range */
#define	OSA_MQ_PRIORITY_MIN	  0
#define	OSA_MQ_PRIORITY_MAX  31
#define OSA_MQ_PRIORITY_NORM 15

#ifdef USE_C
    #ifdef __cplusplus
    extern "C" {
    #endif
#endif

OSA_RESULT	osa_mq_init(void);
OSA_RESULT	osa_mq_release(void);

OSA_RESULT	osa_mq_create(OSA_HANDLE *hMQ, ru32 maxnum, ru32 maxlen, ru32 mode);
OSA_RESULT	osa_mq_delete(OSA_HANDLE hMQ);
OSA_RESULT	osa_mq_send(OSA_HANDLE hMQ, OSA_DATA buf, ru32 size, ru32 priority);
OSA_RESULT	osa_mq_send_timeout(OSA_HANDLE hMQ, OSA_DATA buf, ru32 size, 
					ru32 priority, ru32 timeout);
OSA_RESULT	osa_mq_receive(OSA_HANDLE hMQ, OSA_DATA buf, ru32 *size, ru32 *priority);
OSA_RESULT	osa_mq_receive_timeout(OSA_HANDLE hMQ, OSA_DATA buf, ru32 *size,
					ru32 *priority, ru32 timeout);

#ifdef USE_C
    #ifdef __cplusplus
    }
    #endif
#endif

} // end namespace pi

#endif /* end of __RTK_OSA_MQ_H__ */

