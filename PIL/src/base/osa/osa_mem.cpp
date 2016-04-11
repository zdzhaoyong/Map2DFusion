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


#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#include "base/types/FastList.h"
#include "osa.h"
#include "_osa_in.h"

namespace pi {
	  
/******************************************************************************
 *	Init memory sub-system
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_mem_init(void)
{
	return 0;
}

/******************************************************************************
 *	De-initialize mem-system
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_mem_release(void)
{
	return 0;
}

/******************************************************************************
 *	alloc a memory block
 *
 *	Parameter:
 *		iSize				[in] block size (in byte)
 *
 *	Return value:
 *		0					Success
 *		NULL				No more memory 
 *****************************************************************************/
void *		osa_mem_get(int iSize)
{
	return malloc(iSize);
}

/******************************************************************************
 *	Free a memory block
 *
 *	Parameter:
 *		mem					[in] block memory pointer
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_mem_free(void *mem)
{
	free(mem);
	return 0;
}

/******************************************************************************
 *	Set memory to given unsigned char
 *
 *	Parameter:
 *		mem					[in] Memory base address
 *		uChar				[in] unsigned char will be filled
 *		iSize				[in] memory size
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_mem_set(void *mem, ru8 uChar, ru32 iSize)
{
	memset(mem, uChar, iSize);
	return 0;
}

/******************************************************************************
 *	copy src memory to dst memory
 *
 *	Parameter:
 *		dst					[out] Dest memory base address
 *		src					[in] Source memory base address
 *		iSize				[in] memory block size
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_mem_cpy(void *dst, void *src, ru32 iSize)
{
	memcpy(dst, src, iSize);
	return 0;
}

} // end of namespace pi
