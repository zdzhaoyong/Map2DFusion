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
	 
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <errno.h>
#include <sys/timeb.h>
#include <sys/types.h>

#include "../osa.h"
#include "../_osa_in.h"

namespace pi {
	  

typedef struct MqBuf {
    ru32		priority;
    ru32		leng;
    ru8		*buf;
} MqBuf_t;

typedef struct MqHandle {
    ru32		magic;
    list_t      list;

    ru32		maxnum;
    ru32		maxsize;
    ru32		mode;
	
    MqBuf_t     *msgbuf;

    ru32		count;

    sem_t       hsem_read;
    sem_t       hsem_write;
    sem_t       hmux;

    ru32		iread;
    ru32		iwrite;
} MqHandle_t;

static MqHandle_t 	*g_MqList = NULL;
static sem_t 		g_ListMux;


/******************************************************************************
 *	Local: Check mutex handle
 *
 *	Parameter:
 *		hMu				[in] input handle
 *		handle			[out] inner used handle
 *
 *	Return value:
 *		0				Success
 *		E_OSA_MU_HANDLE	handle error
 *****************************************************************************/
inline OSA_RESULT _osa_mq_checkHandle(OSA_HANDLE hMq, MqHandle_t **handle)
{
    if( NULL == handle )
        return -1;

    *handle = (MqHandle_t *) hMq;

    try {
        if( OSA_MQ_MAGIC == (*handle)->magic )
            return 0;
        else
            return -1;
    } catch( ... ) {
        return -1;
    }

    return -1;
}

/******************************************************************************
 *	Initialize message queue
 *
 *	Parameter:
 *		None
 *
 *	Return vlaue:
 *		0						Success
 *****************************************************************************/
OSA_RESULT osa_mq_init(void)
{
    d3(printf( "osa_mq_init:\n"));
	
    g_MqList = (MqHandle_t*) malloc(sizeof(MqHandle_t));
	memset(g_MqList, 0, sizeof(MqHandle_t));
    g_MqList->magic = OSA_MQ_MAGIC;
	INIT_LIST_HEAD( &(g_MqList->list) );

	sem_init(&g_ListMux, 0, 1);

    d3(printf( "osa_mq_init: end\n"));

	return 0;
}

/******************************************************************************
 *	De-init message queue
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0						Success
 *****************************************************************************/
OSA_RESULT osa_mq_release(void)
{
	MqHandle_t *phandle;
	list_t *plist;
	MqBuf_t *pbuf;
    ru32 i;

    d3(printf( "osa_mq_release:\n"));

    if( g_MqList != NULL ) {
        plist = &(g_MqList->list);
        while(plist->prev != plist->next ) {
            phandle = list_entry( &(plist->next), MqHandle_t, list);

            osa_mq_delete( (OSA_HANDLE) phandle);
        }

        memset(g_MqList, 0, sizeof(MqHandle_t));
        free(g_MqList);

        /* delete global linux mutex */
        sem_destroy( &g_ListMux );
    }

    d3(printf( "osa_mq_release: end\n"));

	return 0;
}

/******************************************************************************
 *	Create a new Message queue
 *
 *	Parameter:
 *		hMQ					[out] MQ handle
 *		maxnum				[in] Max message count
 *		maxlen				[in] Max message size
 *		mode				[in] Create flags 
 *								OSA_MQ_FIFO	Receive message follow squence
 *								OSA_MQ_PRIORITY  High priority will place first
 *								OSA_MQ_TIMEOUT	send & receive can wait timeout
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT osa_mq_create(OSA_HANDLE *hMQ, ru32 maxnum, ru32 maxlen, ru32 mode)
{
    MqHandle_t  *phandle;
    MqBuf_t     *pbuf;
    ru32      i;

    d3(printf("osa_mq_create:\n"));
    d3(printf("\tmaxnum:%d maxlen:%d mode:%d\n",
						maxnum, maxlen, mode));
	
    phandle = (MqHandle_t*) malloc(sizeof(MqHandle_t));
	memset(phandle, 0, sizeof(MqHandle_t));
    phandle->magic = OSA_MQ_MAGIC;
	INIT_LIST_HEAD( &(phandle->list) );

	/* alloc buffer */
    phandle->msgbuf = (MqBuf_t*) malloc(sizeof(MqBuf_t) * maxnum);
	for(i=0; i<maxnum; i++) {
		pbuf = &phandle->msgbuf[i];
		pbuf->priority = 0;
		pbuf->leng = 0;
        pbuf->buf = (ru8*) malloc(maxlen);
	}
	
	phandle->maxnum = maxnum;
	phandle->maxsize = maxlen;
	phandle->count = 0;
	phandle->mode = mode;
	phandle->iread = 0;
	phandle->iwrite = 0;

	sem_init(&(phandle->hsem_read), 0, 0);
	sem_init(&(phandle->hsem_write), 0, maxnum+10);
	sem_init(&(phandle->hmux), 0, 1);
	
    *hMQ = (OSA_HANDLE) phandle;

	/* insert new item into list */
    if( g_MqList ) {
        sem_wait( &(g_ListMux) );
        list_add( &(phandle->list), &(g_MqList->list) );
        sem_post( &(g_ListMux) );
    }

	return 0;
}

/******************************************************************************
 *	Delete given message queue
 *
 *	Parameter:
 *		hMQ				[in] Message Queue handle
 *
 *	Return value:
 *		0				Success
 *****************************************************************************/
OSA_RESULT osa_mq_delete(OSA_HANDLE hMQ)
{
    MqHandle_t  *phandle = NULL;
    MqBuf_t     *pbuf;
    ru32      i;

    if( _osa_mq_checkHandle(hMQ, &phandle) ) {
        d1(printf("osa_mq_delete: Input handle error!\n"));
        return E_OSA_MQ_HANDLE;
	}

	/* delete from list */
    if( g_MqList ) {
        sem_wait( &g_ListMux );
        list_del( &(phandle->list) );
        sem_post( &g_ListMux );
    }

    // delete sem
	sem_destroy( &(phandle->hsem_read) );
	sem_destroy( &(phandle->hsem_write) );
	sem_destroy( &(phandle->hmux) );

	/* delete circule buffer */
	for(i=0; i<phandle->maxnum; i++) {
		pbuf = &phandle->msgbuf[i];
		free(pbuf->buf);
	}
	free(phandle->msgbuf);

	memset(phandle, 0, sizeof(MqHandle_t));
	free(phandle);

	return 0;
}

/******************************************************************************
 *	Write data to message queue
 *
 *	Parameter:
 *		hMQ					[in] MQ handle
 *		buf					[in] Data buffer
 *		size				[in] Data size
 *		priority			[in] message's priority
 *****************************************************************************/
OSA_RESULT	osa_mq_send(OSA_HANDLE hMQ, OSA_DATA buf, ru32 size, ru32 priority)
{
    MqHandle_t *phandle = NULL;
	MqBuf_t *pbuf;

    if( _osa_mq_checkHandle(hMQ, &phandle) ) {
        d1(printf("osa_mq_send: Input handle error!\n"));
        return E_OSA_MQ_HANDLE;
	}

	/* enter exclusive area */
	sem_wait( &(phandle->hmux) );

	/* check if buffer is full */
	if( phandle->count >= phandle->maxnum ) {
		sem_post( &(phandle->hmux) );
        d2(printf("osa_mq_send: reach max number!\n"));
        d3(printf("osa_mq_send: max_num:%d count:%d\n",
							phandle->maxnum, phandle->count));
        return E_OSA_MQ_MAXNUM;
	}

	/* check size */
	if( size > phandle->maxsize ) {
        d2(printf("osa_mq_send: input size exceed max size!\n"));
        d3(printf("osa_mq_send: maxsize:%d size:%d\n",
							phandle->maxsize, size));
		size = phandle->maxsize;
	}

	pbuf = &( phandle->msgbuf[phandle->iwrite++] );
	pbuf->leng = size;
	pbuf->priority = priority;
	memcpy(pbuf->buf, buf, size);

	/* ring to first item */
	if( phandle->iwrite == phandle->maxnum )
		phandle->iwrite = 0;

	phandle->count ++;

	/* leave exclusive area */
	sem_post( &(phandle->hmux) );

	/* add one to semaphore */
	sem_post( &(phandle->hsem_read) );

	return 0;
}

OSA_RESULT	osa_mq_send_timeout(OSA_HANDLE hMQ, OSA_DATA buf, ru32 size,
                    ru32 priority, ru32 timeout)
{
    /* FIXME: not implemented */
	return 0;	
}

/******************************************************************************
 *	Get a message
 *
 *	Parameter:
 *		hMQ				[in] MQ handle
 *		buf				[out] Data buf
 *		size			[out] Message size
 *		priority		[out] message priority
 *
 *	Return vlaue:
 *		0				Success
 *		E_OSA_MQ_HANDLE	Input handle error
 *****************************************************************************/
OSA_RESULT	osa_mq_receive(OSA_HANDLE hMQ, OSA_DATA buf, ru32 *size, ru32 *priority)
{
    MqHandle_t *phandle = NULL;
	MqBuf_t *pbuf;

    d3(printf("osa_mq_receive:\n"));
	
    if( _osa_mq_checkHandle(hMQ, &phandle) ) {
        d1(printf("osa_mq_receive: Input handle error!\n"));
        return E_OSA_MQ_HANDLE;
	}

	/* wait for buffer slot */
	sem_wait( &(phandle->hsem_read) );

	sem_wait( &(phandle->hmux) );
	pbuf = &( phandle->msgbuf[phandle->iread++] );
	*size = pbuf->leng;
	*priority = pbuf->priority;
	memcpy(buf, pbuf->buf, *size);

	if( phandle->iread == phandle->maxnum )
		phandle->iread = 0;

	phandle->count --;
	sem_post( &(phandle->hmux) );

	return 0;
}

/******************************************************************************
 *	Get a message with timeout
 *
 *	Parameter:
 *		hMQ				[in] MQ handle
 *		buf				[out] Data buf
 *		size			[out] Message size
 *		priority		[out] message priority
 *		timeout			[in] time out value
 *
 *	Return vlaue:
 *		0				Success
 *		E_OSA_MQ_HANDLE	Input handle error
 *		E_OSA_MQ_TIMEOUT	Wait timeout
 *****************************************************************************/
OSA_RESULT	osa_mq_receive_timeout(OSA_HANDLE hMQ, OSA_DATA buf, ru32 *size,
                    ru32 *priority, ru32 timeout)
{
	return 0;
}


} // end of namespace pi

#endif
