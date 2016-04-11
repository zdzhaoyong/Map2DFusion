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


pthread_key_t _osa_tid_key  = -1;
pthread_key_t _osa_data_key = -1;


/******************************************************************************
 *	Local: create thread specifice data
 *
 *  Parameter:
 *		threadvar				 Thread's TSD
 *
 *  Return value:
 *		0						 Success
 *****************************************************************************/
OSA_RESULT _osa_tsd_create(pthread_key_t *threadVar)
{
	return ( pthread_key_create(threadVar, NULL) );
}
 
/******************************************************************************
 *  Local: delete thread specifice data
 *
 *  Parameter:
 * 	 threadvar				 Thread's TSD
 *
 *  Return value:
 * 	 0						 Success
 *****************************************************************************/
OSA_RESULT _osa_tsd_delete(pthread_key_t *threadVar)
{
    return ( pthread_key_delete(*threadVar) );
}
 
/******************************************************************************
 *  Local: set thread specifice data
 *
 *	Parameter:
 *		threadvar				Thread's TSD
 *		value					TSD
 *
 *  Return value:
 *		0						Success
 *		E_OSA_T_TSD				Failed to operate tsd
 *****************************************************************************/
OSA_RESULT _osa_tsd_set(pthread_key_t *threadVar, size_t value)
{
    ri32  ret;
 
	ret = pthread_setspecific(*threadVar, (void *)value);
	if ( 0 != ret ) {
        d1(printf("_osa_tsd_set: failed to set specific\n"));
		return -1;
	}
	 
	return 0;
}
 
/******************************************************************************
 *  Local: get thread specifice data
 *
 *  Parameter:
 * 	 threadvar				 [in] Thread's TSD
 * 	 value					 [out] TSD
 *
 *  Return value:
 * 	 0						 Success
 *****************************************************************************/
OSA_RESULT _osa_tsd_get(pthread_key_t *threadVar, size_t *value)
{
    *value = (size_t) pthread_getspecific(*threadVar);
	 
	return 0;
}

/******************************************************************************
 *  Globle: Initialize tsd sub-system
 *
 *  Parameter:
 *		NONE
 *
 *  Return value:
 *		0							Success
 *		E_OSA_T_TSD					Failed to operate tsd
 *****************************************************************************/
OSA_RESULT	 osa_tsd_init(void)
{
    ri32 ret;

	/* create Thread's local data */
    ret = _osa_tsd_create(&_osa_data_key);
	if(ret != 0) {
        d1(printf("osa_tsd_init: error allocate thread(_osa_data_key) \n"));
		return -1;
	}

    ret = _osa_tsd_create(&_osa_tid_key);
	if(ret != 0) {
        d1(printf("osa_tsd_init: error allocate tsd (_osa_tid_key)\n"));
		return -1;
	}

	return 0;
}

/******************************************************************************
 *	Globle: De-Initialize TSD
 *
 *	Parameter:
 *		NONE
 *
 *	Return value:
 *		0						Success
 *		E_OSA_T_TSD				Failed to operate TSD
 *****************************************************************************/
OSA_RESULT	 osa_tsd_release(void)
{
    ri32 ret;

    d3(printf( "osa_tsd_release:\n"));
	
    ret = _osa_tsd_delete(&_osa_data_key);
	if( 0 != ret ) {
        d1(printf("osa_tsd_release: error delete tsd(_osa_data_key)\n"));
        return E_OSA_T_TSD;
	}

    ret = _osa_tsd_delete(&_osa_tid_key);
	if( 0 != ret ) {
        d1(printf("osa_tsd_release: error delete tsd(_osa_tid_key)\n"));
        return E_OSA_T_TSD;
	}

    d3(printf("osa_tsd_release: end\n"));

	return 0;
}

/******************************************************************************
 *	Globle: Set TSD data
 *
 *	Parameter:
 *		Data				[in] Input data
 *
 *	Return value:
 *		0					Successful
 *		E_OSA_T_TSD			Failed to operate TSD
 *****************************************************************************/
OSA_RESULT	 osa_tsd_set(OSA_DATA Data)
{
    ri32 ret;

    // check tsd key
    if( _osa_data_key == -1 ) {
        _osa_tsd_create(&_osa_data_key);
    }

    // set tsd data
    ret = pthread_setspecific(_osa_data_key, (void *) Data);
	if( 0 == ret ) {
        d1(printf("osa_tsd_set: Failed at pthread_setspecific!\n"));
        return E_OSA_T_TSD;
	}
	
	return 0;
}

/******************************************************************************
 *	Globle: Set TSD data
 *
 *	Parameter:
 *		Data				[in] Input data
 *
 *	Return value:
 *		0					Successful
 *		E_OSA_T_TSD			Failed to operate TSD
 *****************************************************************************/
OSA_RESULT	 osa_tsd_get(OSA_DATA *Data)
{
    // check tsd key
    if( _osa_data_key == -1 ) {
        d1(printf("osa_tsd_get: key didn't exist!\n"));
        return -1;
    }

    // get tsd data
    *Data = (OSA_DATA) pthread_getspecific(_osa_data_key);
	
	return 0;
}

} // end of namespace pi

#endif
