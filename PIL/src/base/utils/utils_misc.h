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

#ifndef __UTILS_MISC_H__
#define __UTILS_MISC_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>

#include "base/base.h"
#include "base/Svar/ParamArray.h"
#include "base/osa/osa++.h"
#include "base/utils/utils_str.h"

namespace pi {

////////////////////////////////////////////////////////////////////////////////
/// arguments functions
////////////////////////////////////////////////////////////////////////////////
void save_arguments(int argc, char *argv[], std::string &fname);


////////////////////////////////////////////////////////////////////////////////
/// file & path functions
////////////////////////////////////////////////////////////////////////////////
long filelength(FILE *fp);
long filelength(const char *fname);

///
/// \brief auto generate filename with date-time suffix
/// \param fn_base - file base name
/// \return auto-generated file name
///
const std::string auto_filename(const char *fn_base=NULL);

///
/// \brief auto generate filename with date-time suffix
/// \param fn_base - file base name
/// \return auto-generated file name
///
const std::string auto_filename(const std::string &fn_base);



////////////////////////////////////////////////////////////////////////////////
/// text file functions
////////////////////////////////////////////////////////////////////////////////
int readlines(const char *fn, StringArray &lns, int buf_len=8196);


////////////////////////////////////////////////////////////////////////////////
/// array write/read functions
////////////////////////////////////////////////////////////////////////////////

int save_darray(const char *fn, ru64 n,  double *d);
int load_darray(const char *fn, ru64 &n, double **d);
int save_farray(const char *fn, ru64 n,  float *d);
int load_farray(const char *fn, ru64 &n, float **d);

int save_darray(const char *fn, ru64 n,  ru64 m,  double *d);
int load_darray(const char *fn, ru64 &n, ru64 &m, double **d);
int save_farray(const char *fn, ru64 n,  ru64 m,  float *d);
int load_farray(const char *fn, ru64 &n, ru64 &m, float **d);


////////////////////////////////////////////////////////////////////////////////
/// memory function
////////////////////////////////////////////////////////////////////////////////

/// FIXME: only support 4G memory
void memcpy_fast(void *dst, void *src, ru32 s);

void conv_argb8888_bgr888     (ru8 *src, ru8 *dst, ru32 s);
void conv_argb8888_bgr888_fast(ru8 *src, ru8 *dst, ru32 s);
void conv_bgr888_argb8888     (ru8 *src, ru8 *dst, ru32 s);
void conv_bgr888_argb8888_fast(ru8 *src, ru8 *dst, ru32 s);
void conv_rgb888_bgr888       (ru8 *src, ru8 *dst, ru32 s);
void conv_rgb888_bgr888_fast  (ru8 *src, ru8 *dst, ru32 s);


////////////////////////////////////////////////////////////////////////////////
/// Color pallete
////////////////////////////////////////////////////////////////////////////////
void get_pal_color(int pal, ru8 v, ru8 *r, ru8 *g, ru8 *b);


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class ExecProgram : public RThread
{
public:
    ExecProgram() {}
    virtual ~ExecProgram() {}

    ///
    /// \brief run a given program
    ///
    /// \param cmd              - program and arguments
    ///
    /// \return
    ///         0               - success
    ///
    int run(const std::string &cmd);

    ///
    /// \brief stop the program
    ///     FIXME: only support Linux
    ///
    /// \return
    ///         0               - success
    ///
    int stop(void);


    virtual int thread_func(void *arg);

public:
    std::string         m_cmd;
};


////////////////////////////////////////////////////////////////////////////////
/// test module functions
////////////////////////////////////////////////////////////////////////////////
typedef int (*TEST_FUNC)(CParamArray *pa);

struct TestFunctionArray {
    TEST_FUNC           f;
    char                name[200];
    char                desc[200];
};

#define TEST_FUNC_DEF(f,d) {f,#f,d}



////////////////////////////////////////////////////////////////////////////////
/// test module class & functions
////////////////////////////////////////////////////////////////////////////////

class TestModule
{
public:
    TestModule() {}
    virtual ~TestModule() {}

    virtual int run(CParamArray *pa) {
        printf("Default function!\n");

        return 0;
    }

    virtual int help(CParamArray *pa) {
        printf("Default help!\n");

        return 0;
    }
};

struct TestModuleArray {
    TestModule          *m;
    char                name[200];
    char                desc[200];
    
    
};

#define TESTMODULE_DEF(f,d) {new f, #f, d}


////////////////////////////////////////////////////////////////////////////////
/// test main entry
////////////////////////////////////////////////////////////////////////////////

int test_main(int argc, char *argv[],
                  TestFunctionArray fa[],
                  CParamArray &pa);

int test_main(int argc, char *argv[], 
              TestModuleArray ma[]);

int svar_main(int argc, char *argv[],
              TestFunctionArray fa[]);

} // end of namespcae pi

#endif // end of __UTILS_MISC_H__
