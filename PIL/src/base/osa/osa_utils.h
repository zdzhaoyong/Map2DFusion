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

#ifndef __OSA_UTILS_H__
#define __OSA_UTILS_H__

#include <string>

#include "osa.h"

namespace pi {

///
/// \brief get process ID
///
/// \param pid - process ID number
///
/// \return
///     0           - success
///     -1          - failed
///
int osa_get_pid(ri64 *pid);

///
/// \brief get thread ID
///
/// \param tid - thread ID number
///
/// \return
///     0           - success
///     -1          - failed
///
int osa_get_tid(ri64 *tid);

///
/// \brief get host name
///
/// \param hn           - hostname string
/// \param maxLen       - hostname max length
///
/// \return
///     0               - success
///     -1              - fialed
///
int osa_get_hostname(char *hn, int maxLen=256);

///
/// \brief get host name
///
/// \param hn           - hostname string
///
/// \return
///     0               - success
///     -1              - fialed
///
int osa_get_hostname(std::string &hn);

} // end of namespace pi

#endif // end of __OSA_UTILS_H__
