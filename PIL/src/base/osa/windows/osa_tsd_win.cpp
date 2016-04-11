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

#include <windows.h>
#include <stdio.h>

#include "base/types/FastList.h"
#include "../osa.h"
#include "../_osa_in.h"


namespace pi {

DWORD g_TlsData = 0;
int   g_TlsInit = 0;


/******************************************************************************
 *	TSD submodule init
 *
 *	Parameter:
 *		None
 *
 *	Return Value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_tsd_init(void)
{
    if( !g_TlsInit ) {
        g_TlsData = TlsAlloc();
    }

	return 0;
}

/******************************************************************************
 *	TSD sub-module de-initialize
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_tsd_release(void)
{
    if( g_TlsInit )
        TlsFree(g_TlsData);

	return 0;
}

/******************************************************************************
 *	Set TSD data
 *
 *	Parameter:
 *		Data				[in] Input data
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_tsd_set(OSA_DATA Data)
{
	// if user didn't init the osa then init it
    osa_tsd_init();
	
	TlsSetValue(g_TlsData, (LPVOID) Data);

	return 0;
}

/******************************************************************************
 *	Get TSD data
 *
 *	Parameter:
 *		Data				[out] Output data
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_tsd_get(OSA_DATA *Data)
{
	// if user didn't init the osa then init it
    osa_tsd_init();
	
	*Data = (OSA_DATA) TlsGetValue(g_TlsData);

	return 0;
}

}

#endif
