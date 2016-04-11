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

#include "osa.h"
#include "osa++.h"


namespace pi {


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

OSA_RESULT RThread_thread_func(OSA_T_ARGS arg)
{
    RThread *thread = (RThread *) arg;

    thread->m_isAlive = 1;
    thread->m_res = thread->thread_func(thread->m_arg);
    thread->m_isAlive = 0;

    return 0;
}

int RThread::start(void *arg, ru32 stackSize, ru32 mode)
{
    OSA_RESULT res = -1;

    m_arg = arg;
    res = osa_t_create(&m_handle, RThread_thread_func, this,
                       stackSize, mode);
    if( res != 0 ) {
        m_handle = 0;
        setAlive(0);
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void RTimer_timerFunc(void *arg)
{
    RTimer *tm = (RTimer *) arg;

    if( tm->m_state == 0 ) {
        tm->timer_func(tm->m_arg);
        tm->m_count ++;
    }
}

int RTimer::start(ru32 iPeriod, void *arg, ru32 mode)
{
    OSA_RESULT  res;

    m_iPeriod = iPeriod;
    m_mode    = mode;
    m_arg     = arg;
    m_state   = 0;
    m_count   = 0;

    res = osa_tm_create(&m_handle, m_iPeriod,
                        RTimer_timerFunc, this,
                        m_mode);
    if( res != 0 )
        m_handle = 0;

    return res;
}

int RTimer::start(void *arg)
{
    OSA_RESULT  res;

    m_arg   = arg;
    m_state = 0;
    m_count = 0;

    res = osa_tm_create(&m_handle, m_iPeriod,
                        RTimer_timerFunc, this,
                        m_mode);
    if( res != 0 )
        m_handle = 0;

    return res;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int g_isRunning = 1;

int isRunning(void)
{
    return g_isRunning;
}

int setStop(int s)
{
    g_isRunning = s;

    return g_isRunning;
}


} // end of namespace pi
