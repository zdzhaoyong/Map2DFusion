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

#ifndef __RTK_OSA_ERR_H__
#define __RTK_OSA_ERR_H__

namespace pi {

#define E_OSA_OK                0           /* all right */

#define E_OSA_MAGIC             -1          /* magic number error */
#define E_OSA_MEM               -2          /* malloc memory fail */
#define E_OSA_DEADLOCK          -3          /* dead lock */
#define E_OSA_LL_FUNC_ERR       -4          /* OSA low-level function error */

#define E_OSA_T_SUSPEND         -142        /* thread is suspend */
#define E_OSA_T_STOP            -143        /* thread is stoped */
#define E_OSA_T_BADHANDLE       -144        /* handle error */
#define E_OSA_T_NOSUSPEND       -145        /* didn't suspend */
#define E_OSA_T_BADPRIORITY     -146        /* priority out of range */
#define E_OSA_T_NOSTOP          -147        /* thread didn't stoped */
#define E_OSA_T_TSD             -148        /* failed to operate tsd */
#define E_OSA_T_KILL            -149        /* failed to kill thread */
#define E_OSA_T_DELETE          -150        /* failed to delete thread */
#define E_OSA_T_WAIT            -151        /* failed to wait */
#define E_OSA_T_RESUME          -152        /* failed to resume */
#define E_OSA_T_RUNNING         -153        /* Thread is running */

#define E_OSA_SEM_CREATE        -30         /* failed to create semaphore */
#define E_OSA_SEM_HANDLE        -31         /* semaphore's handle error */
#define E_OSA_SEM_WAIT          -32         /* wait failed */
#define E_OSA_SEM_TIMEOUT       -33         /* wait timeout */
#define E_OSA_SEM_SIGNAL        -34         /* Failed to signal */


#define E_OSA_MU_HANDLE         -50         /* mutex handle error */
#define E_OSA_MU_LOCK           -51         /* lock failed */
#define E_OSA_MU_TIMEOUT        -52         /* wait time out */
#define E_OSA_MU_UNLOCK         -53         /* failed unlock */

#define E_OSA_CV_HANDLE         -70         /* cv handle error */
#define E_OSA_CV_WAIT           -71         /* cv wait failed */
#define E_OSA_CV_SIGNAL         -72         /* cv sginal failed */
#define E_OSA_CV_BROADCAST      -73         /* cv broadcast failed */
#define E_OSA_CV_TIMEOUT        -74         /* cv wait time-out */


#define E_OSA_MQ_HANDLE         -90         /* mq handle error */
#define E_OSA_MQ_MAXNUM         -91         /* mq full */
#define E_OSA_MQ_TIMEOUT        -92         /* mq timeout */


#define E_OSA_TM_HANDLE         -110        /* Timer handle error */
#define E_OSA_TM_RESOLUTION     -111        /* time resolution error */
#define E_OSA_TM_CREATE         -112        /* Failed to create timer */
#define E_OSA_TM_SET            -113        /* Failed to set time */

} // end namespace pi

#endif  /* end of __RTK_OSA_ERR_H__ */

