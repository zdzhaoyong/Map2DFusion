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

#include "osa.h"

namespace pi {

int osa_inited = 0;

/******************************************************************************
 *	Init OSA sub-system
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_init(void)
{
    if( osa_inited == 0 ) {
        osa_mem_init();
        osa_tsd_init();
        osa_t_init();
        osa_sem_init();
        osa_mu_init();
        osa_cv_init();
        osa_mq_init();
        osa_tm_init();
        osa_ev_init();

        osa_inited = 1;
    }

	return 0;
}

/******************************************************************************
 *	De-Init OSA sub-system
 *
 *	Parameter:
 *		None
 *
 *	Return value:
 *		0					Success
 *****************************************************************************/
OSA_RESULT	osa_release(void)
{
    if( osa_inited == 1 ) {
        osa_ev_release();
        osa_tm_release();
        osa_mq_release();
        osa_cv_release();
        osa_mu_release();
        osa_sem_release();
        osa_t_release();
        osa_tsd_release();
        osa_mem_release();

        osa_inited = 0;
    }

	return 0;
}

} // end of namespace pi
