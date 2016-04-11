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
#include <stdarg.h>

#include <sys/types.h>
	 
#include "osa.h"
#include "_osa_in.h"

namespace pi {


char *_osa_str_cat(char *s_out, const char *s1, const char *s2, const char *s3)
{
    int     i, j, l1, l2, l3;

    l1 = strlen(s1);
    l2 = strlen(s2);
    l3 = strlen(s3);

    j = 0;
    for(i=0; i<l1; i++) s_out[j++] = s1[i];
    for(i=0; i<l2; i++) s_out[j++] = s2[i];
    for(i=0; i<l3; i++) s_out[j++] = s3[i];
    s_out[j] = 0;

    return s_out;
}

void osa_dbg_printf(int level,
               const char *fname, int line, const char *func,
               const char *szFmtString, ...)
{
    #define MAX_BUFF_LEN 4096

    char    *sHeader, *sTail;
    char    *sBuf1, *sBuf2;
    int     lBuf1, lBuf2;

    va_list va_params;


    // alloc string buffer
    lBuf1 = strlen(szFmtString);

    sHeader = new char[MAX_BUFF_LEN];
    sTail   = new char[MAX_BUFF_LEN];
    sBuf1   = new char[MAX_BUFF_LEN+lBuf1];
    sBuf2   = new char[MAX_BUFF_LEN+lBuf1];

#ifdef PIL_LINUX
    // generate header, tail
    if( level == 1 ) {
        sprintf(sHeader, "\033[31mERR:\033[0m  ");
        sprintf(sTail,   "      \033[35m(LINE: %5d, FILE: %s, FUNC: %s)\033[0m", line, fname, func);
    } else if (level == 2 ) {
        sprintf(sHeader, "\033[33mWARN:\033[0m ");
        sprintf(sTail,   "      \033[35m(LINE: %5d, FILE: %s, FUNC: %s)\033[0m", line, fname, func);
    } else if (level == 3 ) {
        sprintf(sHeader, "\033[36mINFO:\033[0m ");
        sprintf(sTail,   "      \033[35m(LINE: %5d, FILE: %s, FUNC: %s)\033[0m", line, fname, func);
    } else if (level == 4 ) {
        sprintf(sHeader, "\033[34m%s\033[0m >> ", func);
        sTail[0] = '\0';
    } else {
        sHeader[0] = '\0';
        sTail[0] = '\0';
    }
#endif

#ifdef PIL_WINDOWS
    // generate header, tail
    if( level == 1 ) {
        sprintf(sHeader, "ERR:  ");
        sprintf(sTail,   "      (LINE: %5d, FILE: %s, FUNC: %s)", line, fname, func);
    } else if (level == 2 ) {
        sprintf(sHeader, "WARN: ");
        sprintf(sTail,   "      (LINE: %5d, FILE: %s, FUNC: %s)", line, fname, func);
    } else if (level == 3 ) {
        sprintf(sHeader, "INFO: ");
        sprintf(sTail,   "      (LINE: %5d, FILE: %s, FUNC: %s)", line, fname, func);
    } else if (level == 4 ) {
        sprintf(sHeader, "%s >> ", func);
        sTail[0] = '\0';
    } else {
        sHeader[0] = '\0';
        sTail[0] = '\0';
    }
#endif

    // generate format string
    va_start(va_params, szFmtString);
    vsprintf(sBuf1, szFmtString, va_params);
    va_end(va_params);

    lBuf1 = strlen(sBuf1);
    if( lBuf1 > 0 )
        if( sBuf1[lBuf1-1] != '\n' ) {
            sBuf1[lBuf1] = '\n';
            sBuf1[lBuf1+1] = 0;
        }

    // concatenate final string
    _osa_str_cat(sBuf2, sHeader, sBuf1, sTail);

    lBuf2 = strlen(sBuf2);
    if( lBuf2 > 0 )
        if( sBuf2[lBuf2-1] == '\n' ) {
            sBuf2[lBuf2-1] = 0;
        }

    // output message
    puts(sBuf2);

    // free tem buffer
    delete [] sHeader;
    delete [] sTail;
    delete [] sBuf1;
    delete [] sBuf2;
}

}
