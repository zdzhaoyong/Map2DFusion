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

#ifdef PIL_LINUX


#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <errno.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/poll.h>

#include "../osa.h"
#include "../_osa_in.h"


namespace pi {

/******************************************************************************
 *	Local defines
 *****************************************************************************/
#define	MAXNUM_EVENT_MQ					32
#define MAXLEN_EVENT_MQ					sizeof(OSA_InputEvent_t)

/******************************************************************************
 *	Local variable
 *****************************************************************************/
static OSA_EV_InputCallback g_funEvent;

static OSA_HANDLE       g_hEventThread = 0;
static OSA_HANDLE       g_hEventSendThread = 0;
static OSA_HANDLE       g_hMQ = 0;

static struct termios   g_origTermios;

static ru32           g_iEvQuit = 0;
static ru32           g_iEvStat = 1;

/******************************************************************************
 *	Key map
 *****************************************************************************/
/* Number & char key map */
static ru16 g_arrKeyTrans0[OSA_VK_KEYNUM] =
{
/* 000 - 004 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,
/* 005 - 009 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_BACK_SPACE, OSA_VK_TAB,
/* 010 - 014 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_ENTER, OSA_VK_NULL,
/* 015 - 019 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,
/* 020 - 024 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,
/* 025 - 029 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_ESCAPE, OSA_VK_NULL, OSA_VK_NULL,
	
/* 030 - 034 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_SPACE, OSA_VK_NULL, OSA_VK_NULL,
/* 035 - 039 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_COMMA,
/* 040 - 044 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,
/* 045 - 049 */	OSA_VK_MINUS, OSA_VK_PERIOD, OSA_VK_SLASH, OSA_VK_0, OSA_VK_1,
/* 050 - 054 */	OSA_VK_2, OSA_VK_3, OSA_VK_4, OSA_VK_5, OSA_VK_6,
/* 055 - 059 */	OSA_VK_7, OSA_VK_8, OSA_VK_9, OSA_VK_NULL, OSA_VK_SEMICOLON,
	
/* 060 - 064 */	OSA_VK_NULL, OSA_VK_EQUALS, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,
/* 065 - 069 */	OSA_VK_A, OSA_VK_B, OSA_VK_C, OSA_VK_D, OSA_VK_E,
/* 070 - 074 */	OSA_VK_F, OSA_VK_G, OSA_VK_H, OSA_VK_I, OSA_VK_J,
/* 075 - 079 */	OSA_VK_K, OSA_VK_L, OSA_VK_M, OSA_VK_N, OSA_VK_O,
/* 080 - 084 */	OSA_VK_P, OSA_VK_Q, OSA_VK_R, OSA_VK_S, OSA_VK_T,
/* 085 - 089 */	OSA_VK_U, OSA_VK_V, OSA_VK_W, OSA_VK_X, OSA_VK_Y,
	
/* 090 - 094 */	OSA_VK_Z, OSA_VK_OPEN_BRACKET, OSA_VK_BACK_SLASH, OSA_VK_CLOSE_BRACKET, OSA_VK_NULL,
/* 095 - 099 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_a, OSA_VK_b, OSA_VK_c,
/* 100 - 104 */	OSA_VK_d, OSA_VK_e, OSA_VK_f, OSA_VK_g, OSA_VK_h,
/* 105 - 109 */	OSA_VK_i, OSA_VK_j, OSA_VK_k, OSA_VK_l, OSA_VK_m,
/* 110 - 114 */	OSA_VK_n, OSA_VK_o, OSA_VK_p, OSA_VK_q, OSA_VK_r,
/* 115 - 119 */	OSA_VK_s, OSA_VK_t, OSA_VK_u, OSA_VK_v, OSA_VK_w,
	
/* 120 - 124 */	OSA_VK_x, OSA_VK_y, OSA_VK_z, OSA_VK_NULL, OSA_VK_NULL,
/* 125 - 129 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,
/* 130 - 134 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,
/* 135 - 139 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL
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
/* 065 - 069 */	OSA_VK_UP, OSA_VK_DOWN, OSA_VK_RIGHT, OSA_VK_LEFT, OSA_VK_NULL,
/* 070 - 074 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,
/* 075 - 079 */	OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL, OSA_VK_NULL,
/* 080 - 084 */	OSA_VK_COLOR_KEY_0, OSA_VK_COLOR_KEY_1, OSA_VK_COLOR_KEY_2, OSA_VK_COLOR_KEY_3, OSA_VK_NULL,
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
static OSA_RESULT	_eventDefault(OSA_InputEvent_t * ev);
static ri32 		_osa_ev_receiver(OSA_T_ARGS param);
static ri32		_osa_ev_sender(OSA_T_ARGS param);


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
OSA_RESULT	osa_ev_init(ru32 flags)
{
    OSA_RESULT		res;

    d3(printf("osa_ev_init:\n"));
	
	/* set default event function */
	g_funEvent = _eventDefault;

	/* create message queue */
    res = osa_mq_create(&g_hMQ, MAXNUM_EVENT_MQ, MAXLEN_EVENT_MQ, OSA_MQ_FIFO);
	if( res ) {
        d1(printf("osa_ev_init: Failed at osa_mq_create!\n"));
		return -1;
	}

    // create sender thread
    res = osa_t_create( &g_hEventSendThread, _osa_ev_sender, 0);
	if( res ) {
        d1(printf("osa_ev_init: Failed to create event message sender thread!\n"));
		return -1;
	}

    // create terminal event receiver thread
    if( flags & OSA_EV_INIT_TERM ) {
        res = osa_t_create( &g_hEventThread, _osa_ev_receiver, 0);
        if( res ) {
            d1(printf("osa_ev_init: Failed to create event receiver thread!\n"));
            return -1;
        }
    }

    d3(printf("osa_ev_init: end\n"));
	
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
    d3(printf("osa_ev_release:\n"));

    if( g_iEvStat ) {
        // kill & delete receiver thread
        if( g_hEventThread ) {
            d3(printf("osa_ev_release: kill & delete Event receiver thread\n"));
            osa_t_kill(g_hEventThread);
            osa_t_delete(g_hEventThread);
        }

        // kill & delete sender thread
        if( g_hEventSendThread ) {
            d3(printf("osa_ev_release: kill & delete Event sender thread\n"));
            osa_t_kill(g_hEventSendThread);
            osa_t_delete(g_hEventSendThread);
        }

        // delete message queue
        osa_mq_delete(g_hMQ);

        /* restore old termios settings */
        if (tcsetattr(fileno(stdin), TCSANOW, &g_origTermios) < 0) {
            d1(printf("osa_ev_release: Failed at tcsetattr!\n"));
            return -1;
        }

        // set user loop to terminate
        g_iEvQuit = 1;

        // set event status
        g_iEvStat = 0;
    }

    d3(printf("osa_ev_release: end\n"));
	
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
static ri32 _osa_ev_sender(OSA_T_ARGS param)
{
    OSA_RESULT          res;
    OSA_InputEvent_t	ev;
    ru32              uiMsgSize,
                        uiMsgPrority;

    d3(printf("_osa_ev_sender: \n"));
	
	while(1) {
        res = osa_mq_receive(g_hMQ, &ev, &uiMsgSize, &uiMsgPrority);
		if( res )
			continue;

		/* call user event receive function */
		g_funEvent(&ev);

        // if quit
        if( g_iEvQuit )
            break;
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
 *	Local: Get a char from stdin
 *
 *	Parameter:
 *		fd				[in] stdin's File id
 *
 *	Return vaulue:
 *		-1				Failed at poll
 *		other			Char code
 *****************************************************************************/
static inline int __getchar(int fd)
{
	#if 0
	struct pollfd 	pfd;
	int				cc;

	pfd.fd = fd;
	pfd.events = POLLIN;

	cc = poll(&pfd, 1, 0);
	if (cc < 0) {
        d1(printf(1, "__getchar: failed at poll!\n"));
		return -1;
	}
	#endif
	
    return getchar();
}

static inline int __get_chars(int *key_buf)
{
    int key;
    int last_key=0;

    for (;;) {
        key = getchar();

        if( key == -1 ) {
            if( last_key > 0 ) {
                key_buf[last_key] = 0;
                return last_key;
            }

            usleep(20000);
            continue;
        } else {
            key_buf[last_key++] = key;
        }
    }

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
static ri32 _osa_ev_receiver(OSA_T_ARGS param)
{
    OSA_InputEvent_t ev;

	int				fd;
	struct termios 	newTermios;

    int             key_len;
    int             key_buf[100];

    d3(printf("_osa_ev_receiver:\n"));
	
	/* get current termios settings */
	fd = fileno(stdin);
	if (tcgetattr(fd, &g_origTermios) < 0) {
        d1(printf("_osa_ev_receiver: failed at tcgetattr!\n"));
		return -1;
	}

    /* set termios flags */
    memcpy(&newTermios, &g_origTermios, sizeof(struct termios));
    newTermios.c_lflag &= ~(ECHO|ICANON);
    newTermios.c_cc[VTIME] = 0;
    newTermios.c_cc[VMIN] = 0;

	if (tcsetattr(fd, TCSANOW, &newTermios) < 0) {
        d1(printf("_osa_ev_receiver: Failed at tcsetattr!\n"));
		return -1;
	}
	
    /* set event source/type */
    ev.source = OSA_EV_KB;
    ev.type   = OSA_EV_KB;
	
    /* loop for ever */
	while(1) {
        // get key sequence
        key_len = __get_chars(key_buf);

        if( key_len < 1 ) continue;

        printf("key: (%2d)  ", key_len);
        for(int i=0; i<key_len; i++)
            printf("%3d(0x%02x) ", key_buf[i], key_buf[i]);
        printf("\n");

        // convert key
        if( key_len == 1 ) {
            ev.code = g_arrKeyTrans0[ key_buf[0] ];
        } else if ( key_len == 3 ) {
            ev.code = g_arrKeyTrans1[ key_buf[2] ];
        } else
            continue;

        // send to message queue
        osa_ev_sendMsg(&ev);

        // if quit event receive loop
        if( g_iEvQuit )
            break;
	}

	return 0;
}


OSA_RESULT	osa_ev_loop(void)
{
	while(1) {
        osa_t_sleep(100);
		
        if( g_iEvQuit )
			break;
	}

	return 0;
}

OSA_RESULT	osa_ev_quit(void)
{
    osa_ev_release();

	return 0;
}

} // end namespace pi

#endif
