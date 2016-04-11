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
#include <sys/syscall.h>
#include <sys/types.h>


#include "../osa.h"
#include "../_osa_in.h"


namespace pi {

int osa_get_pid(ri64 *pid)
{
    *pid = getpid();
    return 0;
}


int osa_get_tid(ri64 *tid)
{
#ifdef SYS_gettid
    *tid = syscall(SYS_gettid);
#else
    #error "SYS_gettid unavailable on this system"
#endif

    return 0;
}

int osa_get_hostname(char *hn, int maxLen)
{
    return gethostname(hn, maxLen);
}

int osa_get_hostname(std::string &hn)
{
    char    hostname[1024];
    int     ret = -1;

    ret = gethostname(hostname, 1024);
    hn = hostname;

    return ret;
}

} // end of namespace pi

#endif
