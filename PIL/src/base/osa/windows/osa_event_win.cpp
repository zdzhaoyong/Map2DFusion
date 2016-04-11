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
#include <conio.h>

#include "../osa.h"
#include "../_osa_in.h"



namespace pi {


/******************************************************************************
 *	Local defines
 *****************************************************************************/
#define _OSA_EV_RECTHREAD_STACKSIZE         (16 * 1024)
#define	_OSA_EV_SENDTHREAD_STACKSIZE        (2 * 1024 * 1024)

#define _OSA_EV_CTRLKEY1                    0xE0
#define _OSA_EV_CTRLKEY2                    0x00

#define	MAXNUM_EVENT_MQ                     32
#define MAXLEN_EVENT_MQ                     sizeof(OSA_InputEvent_t)

/******************************************************************************
 *	Local variable
 *****************************************************************************/
static OSA_EV_InputCallback g_funEvent;

static OSA_HANDLE   g_hEventConsolMsgThread;
static OSA_HANDLE   g_hMQ;

static ru32         g_iQuit = 0;

/* Number & char key map */
static ru16 g_arrKeyTrans0[OSA_VK_KEYNUM] =
{
/* 000 - 004 */ OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 005 - 009 */ OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_BACK_SPACE, OSA_VK_TAB,		
/* 010 - 014 */ OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_ENTER, OSA_VK_NULL,		
/* 015 - 019 */ OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 020 - 024 */ OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 025 - 029 */ OSA_VK_NULL, OSA_VK_NULL, OSA_VK_ESCAPE, OSA_VK_NULL, OSA_VK_NULL,	
	
/* 030 - 034 */ OSA_VK_NULL, OSA_VK_NULL, OSA_VK_SPACE, OSA_VK_NULL, OSA_VK_NULL,	
/* 035 - 039 */ OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_COMMA,	
/* 040 - 044 */ OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,
/* 045 - 049 */ OSA_VK_MINUS, OSA_VK_PERIOD, OSA_VK_SLASH, OSA_VK_0, OSA_VK_1,
/* 050 - 054 */ OSA_VK_2, OSA_VK_3, OSA_VK_4, OSA_VK_5, OSA_VK_6,
/* 055 - 059 */ OSA_VK_7, OSA_VK_8, OSA_VK_9, OSA_VK_NULL, OSA_VK_SEMICOLON, 	
	
/* 060 - 064 */ OSA_VK_NULL, OSA_VK_EQUALS, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 
/* 065 - 069 */ OSA_VK_A, OSA_VK_B, OSA_VK_C, OSA_VK_D, OSA_VK_E,	
/* 070 - 074 */ OSA_VK_F, OSA_VK_G, OSA_VK_H, OSA_VK_I, OSA_VK_J,	
/* 075 - 079 */ OSA_VK_K, OSA_VK_L, OSA_VK_M, OSA_VK_N, OSA_VK_O,	
/* 080 - 084 */ OSA_VK_P, OSA_VK_Q, OSA_VK_R, OSA_VK_S, OSA_VK_T,	
/* 085 - 089 */ OSA_VK_U, OSA_VK_V, OSA_VK_W, OSA_VK_X, OSA_VK_Y,	
	
/* 090 - 094 */ OSA_VK_Z, OSA_VK_OPEN_BRACKET, OSA_VK_BACK_SLASH, OSA_VK_CLOSE_BRACKET, OSA_VK_NULL, 	
/* 095 - 099 */ OSA_VK_NULL, OSA_VK_NULL, OSA_VK_a, OSA_VK_b, OSA_VK_c,	
/* 100 - 104 */ OSA_VK_d, OSA_VK_e, OSA_VK_f, OSA_VK_g, OSA_VK_h,	
/* 105 - 109 */ OSA_VK_i, OSA_VK_j, OSA_VK_k, OSA_VK_l, OSA_VK_m,	
/* 110 - 114 */ OSA_VK_n, OSA_VK_o, OSA_VK_p, OSA_VK_q, OSA_VK_r,	
/* 115 - 119 */ OSA_VK_s, OSA_VK_t, OSA_VK_u, OSA_VK_v, OSA_VK_w,	
	
/* 120 - 124 */ OSA_VK_x, OSA_VK_y, OSA_VK_z, OSA_VK_NULL, OSA_VK_NULL,	
/* 125 - 129 */ OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 130 - 134 */ OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 135 - 139 */ OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL		
};

/* Editor key map */
static ru16 g_arrKeyTrans1[OSA_VK_KEYNUM] =
{
/* 000 - 004 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,		
/* 005 - 009 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,		
/* 010 - 014 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,		
/* 015 - 019 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,		
/* 020 - 024 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 025 - 029 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
	
/* 030 - 034 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 035 - 039 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 040 - 044 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 045 - 049 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 050 - 054 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 055 - 059 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
	
/* 060 - 064 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 065 - 069 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 070 - 074 */	OSA_VK_NULL, OSA_VK_HOME, OSA_VK_UP, OSA_VK_PAGE_UP, OSA_VK_NULL, 	
/* 075 - 079 */	OSA_VK_LEFT, OSA_VK_NULL, OSA_VK_RIGHT, OSA_VK_NULL, OSA_VK_GOTO_END, 	
/* 080 - 084 */	OSA_VK_DOWN, OSA_VK_PAGE_DOWN, OSA_VK_INSERT, OSA_VK_DELETE, OSA_VK_NULL, 	
/* 085 - 089 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
	
/* 090 - 094 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 095 - 099 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 100 - 104 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 105 - 109 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 110 - 114 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 115 - 119 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
	
/* 120 - 124 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 125 - 129 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 130 - 134 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 135 - 139 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL 		
};

/* Function key map */
static ru16 g_arrKeyTrans2[OSA_VK_KEYNUM] =
{
/* 000 - 004 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,		
/* 005 - 009 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,		
/* 010 - 014 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,		
/* 015 - 019 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,		
/* 020 - 024 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 025 - 029 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
	
/* 030 - 034 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 035 - 039 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 040 - 044 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 045 - 049 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 050 - 054 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 055 - 059 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_COLOR_KEY_0, 	
	
/* 060 - 064 */	OSA_VK_COLOR_KEY_1, OSA_VK_COLOR_KEY_2, OSA_VK_COLOR_KEY_3, OSA_VK_NULL, OSA_VK_NULL, 	
/* 065 - 069 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 070 - 074 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 075 - 079 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 080 - 084 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 085 - 089 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
	
/* 090 - 094 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 095 - 099 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 100 - 104 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 105 - 109 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 110 - 114 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 115 - 119 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
	
/* 120 - 124 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 125 - 129 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 130 - 134 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, 	
/* 135 - 139 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL 		
};


/******************************************************************************
 *	Local function
 *****************************************************************************/
static OSA_RESULT   _eventDefault(OSA_InputEvent_t * ev);
static ru32         _osa_ev_consol_msg_receiver(ru32 param);
static ru32         _osa_ev_sender(ru32 param);


/******************************************************************************
 *	Init event sub-system
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0				Success
 *		-1				Failed to create event Message queue
 *****************************************************************************/
OSA_RESULT	osa_ev_init(void)
{
#if 0
	OSA_RESULT		res;

	/* set default event function */
	g_funEvent = _eventDefault;

	/* create message queue */
	res = osa_mq_create(&g_hMQ, MAXNUM_EVENT_MQ, MAXLEN_EVENT_MQ, OSA_MQ_FIFO);
	if( res ) {
        d1(printf("osa_ev_init: Failed at osa_mq_create!\n"));
		return -1;
	}

	/* create win32 console io read thread */
	res = osa_t_create( &g_hEventConsolMsgThread, _osa_ev_consol_msg_receiver, 0, 
						_OSA_EV_RECTHREAD_STACKSIZE, 0);
	if( res ) {
        d1(printf("osa_ev_init: Failed to create event receiver thread!\n"));
		return -1;
	}
#endif

	return 0;
}

/******************************************************************************
 *	De-init event subsystem & release all resource
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0				Success
 *****************************************************************************/
OSA_RESULT	osa_ev_release(void)
{
#if 0
	osa_t_kill(g_hEventConsolMsgThread);
	osa_t_delete(g_hEventConsolMsgThread);

	osa_mq_delete(g_hMQ);
#endif
	return 0;
}

/******************************************************************************
 *	Ignore input event
 *
 *	Parameter:
 *		ev				[in] Event descriptor
 *
 *	Return value:
 *		0				Success
 *****************************************************************************/
OSA_RESULT	osa_ev_bypassInputEventToNative(OSA_InputEvent_t *ev)
{
	return 0;
}

/******************************************************************************
 *	Register a event input handler
 *
 *	Parameter:
 *		inputHandler	[in] Event process function ptr.
 *
 *	Return value:
 *		0				Success
 *****************************************************************************/
OSA_RESULT	osa_ev_registerInputEventHandler(OSA_EV_InputCallback inputHandler)
{
	g_funEvent = inputHandler;
	
	return 0;
}

/******************************************************************************
 *	Send message to message pool
 *
 *	Parameter:
 *		ev				[in] Message descriptor
 *
 *	Return value:
 *		0				Success
 *		-1				Failed to send message
 *****************************************************************************/
OSA_RESULT	osa_ev_sendMsg(OSA_InputEvent_t *ev)
{
	OSA_RESULT		res;
	
	res = osa_mq_send(g_hMQ, ev, sizeof(OSA_InputEvent_t), 0);
	if( res ) {
        d1(printf("osa_ev_sendMsg: Failed to send message!\n"));
		return -1;
	}
	
	return 0;
}

/******************************************************************************
 *	Local: Event send to user function
 *
 *	Parameter:
 *		lpParameter		[in] Thread input parameter
 *
 *	Return value:
 *		0				Success
 *****************************************************************************/
static ru32 _osa_ev_sender(ru32 param)
{
    OSA_RESULT          res;
	OSA_InputEvent_t	ev;
    ru32                uiMsgSize,
                        uiMsgPrority;
	
	while(1) {
		res = osa_mq_receive(g_hMQ, &ev, &uiMsgSize, &uiMsgPrority);
		if( res )
			continue;

		/* call user event receive function */
		g_funEvent(&ev);
	}
	
	return 0;
}

/******************************************************************************
 *	Local: Default event handler
 *
 *	Parameter:
 *		ev				[in] Event descriptor
 *
 *	Return value:
 *		0				Success
 *****************************************************************************/
OSA_RESULT _eventDefault(OSA_InputEvent_t *ev)
{
    d3(printf("_eventDefault: Event type: %d\n", ev->type));
	
	return 0;
}

/******************************************************************************
 *	Local: Event receiver (Thread function)
 *
 *	Parameter:
 *		lpParameter		[in] Thread input parameter
 *
 *	Return value:
 *		Default
 *****************************************************************************/
static ru32 _osa_ev_consol_msg_receiver(ru32 param)
{
    OSA_InputEvent_t    ev;
    int                 ch;
    int                 iState;

	#define _OSA_EV_ST_NONE		0
	#define _OSA_EV_ST_CTL1		1
	#define _OSA_EV_ST_CTL2		2

	iState = _OSA_EV_ST_NONE;
	
	ev.source = OSA_EV_KB;
	ev.type = OSA_EV_KB;
	
	while(1) {
		ch = _getch();
		//printf("<char-code>: %02x\n", ch);

		switch(iState) {
		case _OSA_EV_ST_NONE:
			if( ch == _OSA_EV_CTRLKEY1 ) 
				iState = _OSA_EV_ST_CTL1;
			else if( ch == _OSA_EV_CTRLKEY2 )
				iState = _OSA_EV_ST_CTL2;
			else {
				ev.code = g_arrKeyTrans0[ch];
				osa_ev_sendMsg(&ev);
			}
			break;
			
		case _OSA_EV_ST_CTL1:
			ev.code = g_arrKeyTrans1[ch];
			iState = _OSA_EV_ST_NONE;
			osa_ev_sendMsg(&ev);
			break;
			
		case _OSA_EV_ST_CTL2:
			ev.code = g_arrKeyTrans2[ch];
			iState = _OSA_EV_ST_NONE;
			osa_ev_sendMsg(&ev);
			break;
		}

		if( g_iQuit == 1 )
			break;
	}

	return 0;
}

OSA_RESULT	osa_ev_quit(void)
{
	g_iQuit = 1;
	
	return 0;
}

} // end of namespace pi

#endif
