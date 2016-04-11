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

#if 0

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>


#ifdef PIL_LINUX
#include <sys/time.h>
#include <sys/timeb.h>
#endif

#if defined(PIL_WINDOWS) || defined(PIL_MINGW)
#include <windows.h>
#endif


#include "base/base.h"
#include "Time.h"


namespace pi {


////////////////////////////////////////////////////////////////////////////////
/// time functions
////////////////////////////////////////////////////////////////////////////////

ru64 tm_get_millis(void)
{
#if defined(PIL_LINUX)
    struct timeval  tm_val;
    ru64            v;
    int             ret;

    ret = gettimeofday(&tm_val, NULL);

    v = tm_val.tv_sec*1000 + tm_val.tv_usec/1000;
    return v;
#endif

#if defined(PIL_WINDOWS) || defined(PIL_MINGW)
    return GetTickCount();
#endif
}

ru64 tm_get_ms(void)
{
#if defined(PIL_LINUX)
    struct timeval  tm_val;
    ru64            v;
    int             ret;

    ret = gettimeofday(&tm_val, NULL);

    v = tm_val.tv_sec*1000 + tm_val.tv_usec/1000;
    return v;
#endif

#if defined(PIL_WINDOWS) || defined(PIL_MINGW)
    return GetTickCount();
#endif
}

ru64 tm_get_us(void)
{
#if defined(PIL_LINUX)
    struct timeval  tm_val;
    ru64            v;
    int             ret;

    ret = gettimeofday(&tm_val, NULL);

    v = tm_val.tv_sec*1000000 + tm_val.tv_usec;
    return v;
#endif

#if defined(PIL_WINDOWS) || defined(PIL_MINGW)
    FILETIME        t;
    ru64            t_ret;

    // get UTC time
    GetSystemTimeAsFileTime(&t);

    t_ret = 0;

    t_ret |= t.dwHighDateTime;
    t_ret <<= 32;
    t_ret |= t.dwLowDateTime;

    // convert 100 ns to [ms]
    t_ret /= 10;

    return t_ret;
#endif
}

double tm_getTimeStamp(void)
{
#if defined(PIL_LINUX)
    struct timeval  tm_val;
    double          v;
    int             ret;

    ret = gettimeofday(&tm_val, NULL);

    v = tm_val.tv_sec + 1.0*tm_val.tv_usec/1e6;
    return v;
#endif

#if defined(PIL_WINDOWS) || defined(PIL_MINGW)
    FILETIME        t;
    ru64            t_ret;
    double          ts;

    // get UTC time
    GetSystemTimeAsFileTime(&t);

    t_ret = 0;

    t_ret |= t.dwHighDateTime;
    t_ret <<= 32;
    t_ret |= t.dwLowDateTime;

    // convert 100 ns to second
    ts = 1.0 * t_ret / 1e7;

    return ts;
#endif
}


uint32_t tm_getTimeStampUnix(void)
{
#if defined(PIL_LINUX)
    struct timeval  tm_val;
    uint32_t        v;
    int             ret;

    ret = gettimeofday(&tm_val, NULL);

    v = tm_val.tv_sec;
    return v;
#endif

#if defined(PIL_WINDOWS) || defined(PIL_MINGW)
    FILETIME        t;
    ru64            t_ret;
    uint32_t        ts;

    // get UTC time
    GetSystemTimeAsFileTime(&t);

    t_ret = 0;

    t_ret |= t.dwHighDateTime;
    t_ret <<= 32;
    t_ret |= t.dwLowDateTime;

    // convert 100 ns to second
    ts = t_ret / 10000000;

    return ts;
#endif
}

void   tm_sleep(ru32 t)
{
#if defined(PIL_LINUX)
    struct timespec tp;

    tp.tv_sec  = t / 1000;
    tp.tv_nsec = ( t % 1000 ) * 1000000;

    while( nanosleep(&tp, &tp) );
#endif

#if defined(PIL_WINDOWS) || defined(PIL_MINGW)
    Sleep(t);
#endif
}

void tm_sleep_us(ri64 t)
{
#if defined(PIL_LINUX)
    struct timespec tp;

    tp.tv_sec  = t / 1000000;
    tp.tv_nsec = ( t % 1000000 ) * 1000;

    while( nanosleep(&tp, &tp) );
#endif

#if defined(PIL_WINDOWS) || defined(PIL_MINGW)
    Sleep(t/1000);
#endif
}


} // end of namespace pi

#endif
