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


#ifndef __RTK_OSA_EVENT_H__
#define __RTK_OSA_EVENT_H__

#include "osa.h"
#include "osa_key.h"

namespace pi {

/* define event type */
#define OSA_KEY_PRESS               0x01
#define OSA_KEY_RELEASE             0x02
#define OSA_KEY_TYPED               0x04

#define OSA_MOUSE_PRESSE            0x10
#define OSA_MOUSE_RELEASE           0x20
#define OSA_MOUSE_MOVED         	0x40
#define OSA_MOUSE_DBLCLICK          0x80

#define OSA_EV_QUIT             	0x00

/* define event source */
#define OSA_EV_RC                   0
#define OSA_EV_FP                   1
#define OSA_EV_MOUSE                2
#define OSA_EV_KB                   3

#define OSA_VK_NULL                 0
#define OSA_VK_KEYNUM       		256

/* define event init parameter */
#define OSA_EV_INIT_NULL            0x00
#define OSA_EV_INIT_TERM            0x01

/* OSA input event struct */
typedef struct OSA_InputEvent
{
    ru16        type;			/* event type */
    ru32        source;			/* event source */
    ru32        timestamp;		/* timestamp of event */

    ru16        code;			/* code */
    ru32        code_raw;       /* raw code */
	
    ri32        x_pos;			/* mouse x corridate */
    ri32        y_pos;			/* mouse y corridate */
	
    ru32        data;			/* private data */
} OSA_InputEvent_t;

#ifdef USE_C
    #ifdef	__cplusplus
    extern "C" {
    #endif
#endif

typedef OSA_RESULT (*OSA_EV_InputCallback)(OSA_InputEvent_t *ev);

OSA_RESULT	osa_ev_init(ru32 flags=OSA_EV_INIT_TERM);
OSA_RESULT	osa_ev_release(void);

OSA_RESULT	osa_ev_bypassInputEventToNative(OSA_InputEvent_t *ev);
OSA_RESULT	osa_ev_registerInputEventHandler(OSA_EV_InputCallback inputHandler);

OSA_RESULT	osa_ev_sendMsg(OSA_InputEvent_t *ev);

OSA_RESULT	osa_ev_loop(void);
OSA_RESULT	osa_ev_quit(void);

#ifdef USE_C
    #ifdef	__cplusplus
    }
    #endif
#endif

} // end namespace pi

#endif /* end of __RTK_OSA_EVENT_H__ */

