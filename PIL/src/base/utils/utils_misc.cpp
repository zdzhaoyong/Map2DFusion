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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include <math.h>
#include <complex.h>
#include <float.h>

#ifdef PIL_LINUX
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#endif

#ifdef PIL_WINDOWS
#include <windows.h>
#endif

#include <errno.h>

#include <assert.h>
#include <inttypes.h>

#ifdef __SSSE3__
#include <tmmintrin.h>
#endif

#include <string>
#include <vector>
#include <algorithm>

#include "base/types/types.h"
#include "base/time/DateTime.h"
#include "base/time/Time.h"
#include "base/system/file_path/file_path.h"

#include "utils_str.h"
#include "utils_misc.h"



using namespace std;

namespace pi {


////////////////////////////////////////////////////////////////////////////////
/// arguments functions
////////////////////////////////////////////////////////////////////////////////

void save_arguments(int argc, char *argv[], string &fname)
{
    string      fn;
    FILE        *fp;
    int         i;
    tm          *now;
    time_t      t;
    char        str_time[200];


    fn = fname + "_args.txt";
    fp = fopen(fn.c_str(), "a+"); ASSERT(fp);

    // get current time
    time(&t);
    now = localtime(&t);
    strftime(str_time, 200, "%Y-%m-%d %H:%M:%S", now);

    fprintf(fp, "--------------- %s ---------------\n", str_time);

    for(i=0; i<argc; i++)
        fprintf(fp, "%s ", argv[i]);

    fprintf(fp, "\n\n");

    fclose(fp);
}


////////////////////////////////////////////////////////////////////////////////
/// file & path functions
////////////////////////////////////////////////////////////////////////////////

long filelength(FILE *fp)
{
    long    len;

    if( fp == NULL )
        return 0;

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    return len;
}

long filelength(const char *fname)
{
    FILE    *fp;
    long    len;

    fp = fopen(fname, "r");
    if( fp == NULL )
        return 0;

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fclose(fp);

    return len;
}


const std::string auto_filename(const char *fn_base)
{
    DateTime    t;

    t.setCurrentDateTime();

    if( fn_base == NULL ) {
        return fmt::sprintf("autosave_%04d%02d%02d_%02d%02d%02d",
                            t.year, t.month, t.day,
                            t.hour, t.min, t.sec);
    } else{
        return fmt::sprintf("%s_%04d%02d%02d_%02d%02d%02d",
                            fn_base,
                            t.year, t.month, t.day,
                            t.hour, t.min, t.sec);
    }
}

const std::string auto_filename(const std::string &fn_base)
{
    DateTime    t;

    t.setCurrentDateTime();

    if( fn_base.size() == 0 ) {
        return fmt::sprintf("autosave_%04d%02d%02d_%02d%02d%02d",
                            t.year, t.month, t.day,
                            t.hour, t.min, t.sec);
    } else{
        return fmt::sprintf("%s_%04d%02d%02d_%02d%02d%02d",
                            fn_base,
                            t.year, t.month, t.day,
                            t.hour, t.min, t.sec);
    }
}





////////////////////////////////////////////////////////////////////////////////
/// text file functions
////////////////////////////////////////////////////////////////////////////////

int readlines(const char *fn, StringArray &lns, int buf_len)
{
    FILE    *fp=NULL;

    char    *buf;
    string  s;

    // clear old data
    lns.clear();

    // alloc buffer
    buf = new char[buf_len];

    // open file
    fp = fopen(fn, "r");
    if( fp == NULL ) {
        ASSERT(fp);
        return -1;
    }

    while( !feof(fp) ) {
        // read a line
        if( NULL == fgets(buf, buf_len, fp) )
            break;

        // remove blank & CR
        s = trim(buf);

        // skip blank line
        if( s.size() < 1 )
            continue;

        // add to list
        lns.push_back(s);
    }

    // close file
    fclose(fp);

    // free array
    delete [] buf;

    return 0;
}


////////////////////////////////////////////////////////////////////////////////
/// array write/read functions
////////////////////////////////////////////////////////////////////////////////

int save_darray(const char *fn, ru64 n, double *d)
{
    FILE        *fp;

    fp = fopen(fn, "w"); ASSERT(fp);

    fwrite(&n, sizeof(ru64),    1, fp);
    fwrite(d,  sizeof(double)*n,    1, fp);
    fclose(fp);

    return 0;
}

int load_darray(const char *fn, ru64 &n, double **d)
{
    FILE        *fp;
    double      *d_;
    int         i;

    fp = fopen(fn, "r"); ASSERT(fp);

    i = fread(&n, sizeof(ru64), 1, fp);

    d_ = new double[n];
    i = fread(d_, sizeof(double)*n, 1, fp);
    fclose(fp);

    *d = d_;

    return 0;
}

int save_farray(const char *fn, ru64 n, float *d)
{
    FILE        *fp;

    fp = fopen(fn, "w"); ASSERT(fp);

    fwrite(&n, sizeof(ru64),    1, fp);
    fwrite(d,  sizeof(float)*n,     1, fp);
    fclose(fp);

    return 0;
}

int load_farray(const char *fn, ru64 &n, float **d)
{
    FILE        *fp;
    float       *d_;
    int         i;

    fp = fopen(fn, "r"); ASSERT(fp);

    i = fread(&n, sizeof(ru64), 1, fp);

    d_ = new float[n];
    i = fread(d_, sizeof(float)*n,  1, fp);
    fclose(fp);

    *d = d_;

    return 0;
}



int save_darray(const char *fn, ru64 n, ru64 m, double *d)
{
    FILE        *fp;

    fp = fopen(fn, "w"); ASSERT(fp);

    fwrite(&n, sizeof(ru64),    1, fp);
    fwrite(&m, sizeof(ru64),    1, fp);
    fwrite(d,  sizeof(double)*n*m,  1, fp);
    fclose(fp);

    return 0;
}

int load_darray(const char *fn, ru64 &n, ru64 &m, double **d)
{
    FILE        *fp;
    double      *d_;
    int         i;

    fp = fopen(fn, "r"); ASSERT(fp);

    i = fread(&n, sizeof(ru64), 1, fp);
    i = fread(&m, sizeof(ru64), 1, fp);

    d_ = new double[n*m];
    i = fread(d_, sizeof(double)*n*m, 1, fp);
    fclose(fp);

    *d = d_;

    return 0;
}

int save_farray(const char *fn, ru64 n, ru64 m, float *d)
{
    FILE        *fp;

    fp = fopen(fn, "w"); ASSERT(fp);

    fwrite(&n, sizeof(ru64),    1, fp);
    fwrite(&m, sizeof(ru64),    1, fp);
    fwrite(d,  sizeof(float)*n*m,   1, fp);
    fclose(fp);

    return 0;
}

int load_farray(const char *fn, ru64 &n, ru64 &m, float **d)
{
    FILE        *fp;
    float       *d_;
    int         i;

    fp = fopen(fn, "r"); ASSERT(fp);

    i = fread(&n, sizeof(ru64), 1, fp);
    i = fread(&m, sizeof(ru64), 1, fp);

    d_ = new float[n*m];
    i = fread(d_, sizeof(float)*n*m,  1, fp);
    fclose(fp);

    *d = d_;

    return 0;
}


////////////////////////////////////////////////////////////////////////////////
/// memory function
////////////////////////////////////////////////////////////////////////////////
void memcpy_fast(void *dst, void *src, ru32 s)
{
#ifdef __SSE2__
    ru8    *_src, *_dst;
    ru8    *end;
    ru32   _s;

    _src = (ru8*) src;
    _dst = (ru8*) dst;

    // check memory address is aligned
    assert((uintptr_t)_src % 16 == 0);

    // first part using SSE
    _s = (s/16)*16;
    end = _src + _s;
    for (; _src != end; _src += 16, _dst += 16) {
        _mm_storeu_si128((__m128i *) _dst, _mm_load_si128((__m128i *) _src));
    }

    // remainning part using normal copy
    end = (ru8*)src + s;
    for(; _src != end; _src++, _dst++) {
        *_dst = *_src;
    }
#else
    memcpy(dst,src,s);
#endif
}

void memcpy_fast_(void *dst, void *src, ru32 s)
{
#ifdef __SSE2__
    ru8    *p_src, *p_dst;
    ru32   n_fast, n_last;

    __m128i *src_vec = (__m128i*) src;
    __m128i *dst_vec = (__m128i*) dst;

    // check memory address is aligned
    assert((uintptr_t)src % 16 == 0);

    n_fast = s/64;
    n_last = s - n_fast*64;

    p_src = (ru8*) src;
    p_dst = (ru8*) dst;
    p_src += n_fast*64;
    p_dst += n_fast*64;

    // first part using SSE
    while( n_fast-- > 0 ) {
        _mm_storeu_si128(dst_vec,   _mm_load_si128(src_vec));
        _mm_storeu_si128(dst_vec+1, _mm_load_si128(src_vec+1));
        _mm_storeu_si128(dst_vec+2, _mm_load_si128(src_vec+2));
        _mm_storeu_si128(dst_vec+3, _mm_load_si128(src_vec+3));

        dst_vec += 4;
        src_vec += 4;
    }

    // remainning part using normal copy
    while( n_last-- > 0 ) {
        *p_dst = *p_src;
        p_src ++;
        p_dst ++;
    }
#else
    memcpy(dst,src,s);
#endif
}



void conv_argb8888_bgr888(ru8 *src, ru8 *dst, ru32 s)
{
    ru32 i;

    for(i=0; i<s; i++) {
        dst[0] = src[2];
        dst[1] = src[1];
        dst[2] = src[0];

        src += 4;
        dst += 3;
    }
}

// needs:
// src is 16-byte aligned
void conv_argb8888_bgr888_fast(ru8 *src, ru8 *dst, ru32 s)
{
#ifdef __SSE2__
    ru32   _s1, _s2;
    ru32   i;
    ru8    *end;

    // check memory address is aligned
    assert((uintptr_t)src % 16 == 0);

    _s1 = (s/4-1)*4;
    _s2 = s - _s1;

    // major part using SSE copy
    __m128i mask = _mm_set_epi8(
                -128, -128, -128, -128,
                12, 13, 14,
                8,  9, 10,
                4,  5,  6,
                0,  1,  2);
    end = src + _s1 * 4;
    for (; src != end; src += 16, dst += 12) {
        _mm_storeu_si128((__m128i *) dst, _mm_shuffle_epi8(_mm_load_si128((__m128i *) src), mask));
    }

    // remainning part using normal copy
    for (i=0; i<_s2; i++) {
        dst[0] = src[2];
        dst[1] = src[1];
        dst[2] = src[0];

        src += 4;
        dst += 3;
    }
#else
    conv_argb8888_bgr888(src,dst,s);
#endif
}


void conv_bgr888_argb8888(ru8 *src, ru8 *dst, ru32 s)
{
    ru32 i;

    for(i=0; i<s; i++) {
        dst[0] = 255;
        dst[1] = src[2];
        dst[2] = src[1];
        dst[3] = src[0];

        src += 3;
        dst += 4;
    }
}


void conv_bgr888_argb8888_fast(ru8 *src, ru8 *dst, ru32 s)
{
#ifdef __SSE2__
    __m128i *in_vec  = (__m128i*) src;
    __m128i *out_vec = (__m128i*) dst;

    __m128i in1, in2, in3;
    __m128i out;

    __m128i x0 = _mm_set_epi8(0,  0,  0, 0xff, 0, 0, 0, 0xff, 0, 0, 0, 0xff, 0, 0, 0, 0xff);
    __m128i x1 = _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0, 0, 0, 0);
    __m128i x2 = _mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);

    __m128i m1 = _mm_set_epi8(9,  10, 11, 0xff,  6,  7,  8, 0xff,  3,  4,  5, 0xff,  0,  1,  2, 0xff);
    __m128i m2 = _mm_set_epi8(5,   6,  7, 0xff,  2,  3,  4, 0xff, 15,  0,  1, 0xff, 12, 13, 14, 0xff);
    __m128i m3 = _mm_set_epi8(1,   2,  3, 0xff, 14, 15,  0, 0xff, 11, 12, 13, 0xff,  8,  9, 10, 0xff);
    __m128i m4 = _mm_set_epi8(13, 14, 15, 0xff, 10, 11, 12, 0xff,  7,  8,  9, 0xff,  4,  5,  6, 0xff);


    ri32 n_fast, n_last;
    ru8 *p_in, *p_out;

    // check memory address is aligned
    assert((uintptr_t)src % 16 == 0);

    n_fast = s / 16;
    n_last = s - n_fast*16;

    p_in  = src + n_fast*16*3;
    p_out = dst + n_fast*16*4;


    // fast conv
    while (n_fast-- > 0) {
        // conver first 4 pixel
        in1 = in_vec[0];
        out = _mm_shuffle_epi8(in1, m1);
        out = _mm_or_si128(out, x0);
        out_vec[0] = out;

        // convert second 4 pixel
        in2 = in_vec[1];
        in1 = _mm_and_si128(in1, x1);
        out = _mm_and_si128(in2, x2);
        out = _mm_or_si128(out, in1);
        out = _mm_shuffle_epi8(out, m2);
        out = _mm_or_si128(out, x0);
        out_vec[1] = out;

        // conver third 4 pixel
        in3 = in_vec[2];
        in2 = _mm_and_si128(in2, x1);
        out = _mm_and_si128(in3, x2);
        out = _mm_or_si128(out, in2);
        out = _mm_shuffle_epi8(out, m3);
        out = _mm_or_si128(out, x0);
        out_vec[2] = out;

        // convert forth 4 pixel
        out = _mm_shuffle_epi8(in3, m4);
        out = _mm_or_si128(out, x0);
        out_vec[3] = out;

        // point to next 16 pixel
        in_vec  += 3;
        out_vec += 4;
    }

    while( n_last-- > 0 ) {
        p_out[0] = 255;
        p_out[1] = p_in[2];
        p_out[2] = p_in[1];
        p_out[3] = p_in[0];
    }
#else
    conv_bgr888_argb8888(src,dst,s);
#endif
}


void conv_rgb888_bgr888(ru8 *src, ru8 *dst, ru32 s)
{
    ru32 i;
    for(i=0; i<s; i++) {
        dst[0] = src[2];
        dst[1] = src[1];
        dst[2] = src[0];

        src += 3;
        dst += 3;
    }
}

/* in and out must be 16-byte aligned */
// FIXME: copied data error
void conv_rgb888_bgr888_fast(ru8 *in, ru8 *out, ru32 s)
{
#ifdef __SSE2__
    __m128i *in_vec  = (__m128i*) in;
    __m128i *out_vec = (__m128i*) out;

    __m128i m1 = _mm_set_epi8(-128, 12, 13, 14, 9, 10, 11, 6, 7, 8, 3, 4, 5, 0, 1,  2);
    __m128i m2 = _mm_set_epi8(15, -128, 11, 12, 13, 8, 9, 10, 5, 6, 7, 2, 3, 4, -128, 0);
    __m128i m3 = _mm_set_epi8(13, 14, 15, 10, 11, 12, 7, 8, 9, 4, 5, 6, 1, 2, 3, -128);

    ru32 n_fast, n_last;
    ru8  *p_in, *p_out;

    // check memory address is aligned
    assert((uintptr_t)in % 16 == 0);

    n_fast = s / 48;
    n_last = s - n_fast*48;

    p_in = in;
    p_out = out;

    // fast copy
    while (n_fast-- > 0) {
        _mm_storeu_si128(out_vec,   _mm_shuffle_epi8(_mm_load_si128(in_vec),   m1));
        _mm_storeu_si128(out_vec+1, _mm_shuffle_epi8(_mm_load_si128(in_vec+1), m2));
        _mm_storeu_si128(out_vec+2, _mm_shuffle_epi8(_mm_load_si128(in_vec+2), m3));

        p_out[15] = p_in[17];
        p_out[17] = p_in[15];
        p_out[30] = p_in[32];
        p_out[32] = p_in[30];

        in_vec  += 3;
        out_vec += 3;
        p_in    += 48;
        p_out   += 48;
    }

    // remaining items using normal copy
    while(n_last-- > 0 ) {
        p_out[0] = p_in[2];
        p_out[1] = p_in[1];
        p_out[2] = p_in[0];

        p_in  += 3;
        p_out += 3;
    }
#else
    conv_rgb888_bgr888(in,out,s);
#endif
}


////////////////////////////////////////////////////////////////////////////////
/// Color pallete
////////////////////////////////////////////////////////////////////////////////

#include "color_table.h"

void get_pal_color(int pal, ru8 v, ru8 *r, ru8 *g, ru8 *b)
{
    *r = Cr[pal*256+v];
    *g = Cg[pal*256+v];
    *b = Cb[pal*256+v];
}



////////////////////////////////////////////////////////////////////////////////
/// Run program
////////////////////////////////////////////////////////////////////////////////

///
/// \brief run a given program
///
/// \param cmd              - program and arguments
///
/// \return
///         0               - success
///
int ExecProgram::run(const std::string &cmd)
{
    m_cmd = cmd;

    return start();
}

///
/// \brief stop the program
///     FIXME: only support Linux
///
/// \return
///         0               - success
///
int ExecProgram::stop(void)
{
    if( !getAlive() ) return 0;

    StringArray sa;
    sa = split_text(m_cmd, " ");

    if( sa.size() > 0 ) {
        string cmd = fmt::sprintf("killall %s", sa[0]);
        system(cmd.c_str());
    }

    return 0;
}

int ExecProgram::thread_func(void *arg)
{
    return system(m_cmd.c_str());
}




////////////////////////////////////////////////////////////////////////////////
/// test module functions
////////////////////////////////////////////////////////////////////////////////

int test_default(CParamArray *pa)
{
    printf("default test routine\n");
    return 0;
}

void print_basic_help(int argc, char *argv[],
                          TestFunctionArray fa[],
                          CParamArray &pa)
{
    int     i, j, k;
    int     tab = 30;

    fmt::print("\n");
    fmt::print("-------------------------- ");
    fmt::print_colored(fmt::GREEN, "basic usage");
    fmt::print(" --------------------------\n");

    fmt::print_colored(fmt::BLUE, "    -f"); fmt::print("              config file\n");
    fmt::print_colored(fmt::BLUE, "    -h"); fmt::print("              print usage help\n");
    fmt::print_colored(fmt::BLUE, "    -dbg_level"); fmt::print("      [0/1/2/3/4/5] debug level\n");
    fmt::print("                        1 - Error\n");
    fmt::print("                        2 - Warning\n");
    fmt::print("                        3 - Info\n");
    fmt::print("                        4 - Trace\n");
    fmt::print("                        5 - Normal\n");
    fmt::print_colored(fmt::BLUE, "    -act"); fmt::print("            [s] test module name\n");

    fmt::print("-----------------------------------------------------------------\n");
    fmt::print("\n");

    fmt::print("---------------------------- ");
    fmt::print_colored(fmt::GREEN, "modules");
    fmt::print(" ----------------------------\n");

    i=0;
    while( fa[i].f != NULL ) {
        fmt::print_colored(fmt::BLUE, "    {}", fa[i].name);

        j = strlen(fa[i].name);
        for(k=j; k<tab; k++) fmt::print(" ");

        fmt::print(" : {}\n", fa[i].desc);

        i++;
    }

    fmt::print("-----------------------------------------------------------------\n");
    fmt::print("\n");
}


int test_main(int argc, char *argv[],
              TestFunctionArray fa[], CParamArray &pa)
{
    string          act;
    int             dbg_level = 4;
    int             i, j=0;
    pi::ru64        t1=0, t2=0;
    int             ret=-1;
    string          fname;
    StringArray     sa1, sa2;

    // set signal handler
    dbg_stacktrace_setup();

    // parse input arguments
    if( argc <= 1 ) {
        ret = 0;
        goto RTK_TEST_MAIN_PRINT;
    }

    for(i=0; i<argc; i++) {
        // get config file
        if( strcmp(argv[i], "-f") == 0 ) {
            fname = argv[++i];
        }
        // print usage
        else if( strcmp(argv[i], "-h") == 0 ) {
            ret = 0;
            goto RTK_TEST_MAIN_PRINT;
        }
        // debug level
        else if( strcmp(argv[i], "-dbg_level") == 0 ) {
            dbg_level = atoi(argv[++i]);
            dbg_set_level(dbg_level);
        }
    }

    // load config file
    if( fname.length() > 0 )
        pa.load(fname + ".ini");

    // parse input argument again
    pa.set_args(argc, argv);

    // save input arguments to file
    sa1 = path_split(argv[0]);
    sa2 = path_splitext(sa1[1]);
    save_arguments(argc, argv, sa2[0]);

    // print all settings
    pa.print();

    // test actions
    act = "test_default";
    pa.s("act", act);

    // call test function
    i = 0; j = 0;
    while( fa[i].f != NULL ) {
        if( strcmp(act.c_str(), "test_default") == 0 ) {
            test_default(&pa);
            break;
        } else if( strcmp(act.c_str(), fa[i].name) == 0 ) {

            // run routine
            t1 = tm_get_us();
            ret = fa[i].f(&pa);
            t2 = tm_get_us();
            j = 1;

            break;
        }

        i++;
    }

    if( j == 0 ) {
        dbg_pe("Input arguments error!");
        goto RTK_TEST_MAIN_PRINT;
    }

    // print running time
    printf("\n---------------------------------------------------------\n");
    printf("run time = %g sec (%g min)\n",
                1.0*(t2-t1)/1000000.0,         /* sec */
                1.0*(t2-t1)/60000000.0);       /* min */
    printf("---------------------------------------------------------\n");

    goto RTK_TEST_MAIN_RET;

RTK_TEST_MAIN_PRINT:
    // print basic arguments
    print_basic_help(argc, argv, fa, pa);

    // print user provided help
    i=0;
    while( fa[i].f != NULL ) {
        if( strcmp(fa[i].name, "print_help") == 0 ) {
            ret = fa[i].f(&pa);
            break;
        }
        i++;
    }

RTK_TEST_MAIN_RET:
    return ret;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void print_basic_help_ex(int argc, char *argv[],
                         TestModuleArray ma[],
                         CParamArray &pa)
{
    int     i, j, k;
    int     tab = 30;

    fmt::print("\n");
    fmt::print("-------------------------- ");
    fmt::print_colored(fmt::GREEN, "basic usage");
    fmt::print(" --------------------------\n");

    fmt::print_colored(fmt::BLUE, "    -f"); fmt::print("              config file\n");
    fmt::print_colored(fmt::BLUE, "    -h"); fmt::print("              print usage help\n");
    fmt::print_colored(fmt::BLUE, "    -dbg_level"); fmt::print("      [0/1/2/3/4/5] debug level\n");
    fmt::print("                        1 - Error\n");
    fmt::print("                        2 - Warning\n");
    fmt::print("                        3 - Info\n");
    fmt::print("                        4 - Trace\n");
    fmt::print("                        5 - Normal\n");
    fmt::print_colored(fmt::BLUE, "    -act"); fmt::print("            [s] test module name\n");

    fmt::print("-----------------------------------------------------------------\n");
    fmt::print("\n");

    fmt::print("---------------------------- ");
    fmt::print_colored(fmt::GREEN, "modules");
    fmt::print(" ----------------------------\n");

    i=0;
    while( ma[i].m != NULL ) {
        fmt::print_colored(fmt::BLUE, "    {}", ma[i].name);

        j = strlen(ma[i].name);
        for(k=j; k<tab; k++) fmt::print(" ");

        fmt::print(" : {}\n", ma[i].desc);

        i++;
    }

    fmt::print("-----------------------------------------------------------------\n");
    fmt::print("\n");
}

int test_main(int argc, char *argv[],
              TestModuleArray ma[])
{
    CParamArray     *pa;
    int             isHelp = 0;
    string          act = "";
    int             dbg_level = 4;
    int             i, j=0;
    pi::ru64        t1=0, t2=0;
    int             ret=-1;
    string          fname;
    StringArray     sa1, sa2;

    // set signal handler
    dbg_stacktrace_setup();

    // create a gobal ParameterArray obj
    pa = pa_create();

    // parse input arguments
    if( argc <= 1 ) {
        ret = 0;
        goto RTK_TEST_MAIN_PRINT;
    }

    for(i=0; i<argc; i++) {
        // get config file
        if( strcmp(argv[i], "-f") == 0 ) {
            if( i+1 < argc )
                fname = argv[++i];
            else
                goto RTK_TEST_MAIN_PRINT;
        }
        // print usage
        else if( strcmp(argv[i], "-h") == 0 ) {
            ret = 0;
            isHelp = 1;
        }
        // debug level
        else if( strcmp(argv[i], "-dbg_level") == 0 ) {
            if( i+1 < argc ) {
                dbg_level = atoi(argv[++i]);
                dbg_set_level(dbg_level);
            } else
                goto RTK_TEST_MAIN_PRINT;
        }
        // action
        else if( strcmp(argv[i], "-act") == 0 ) {
            if( i+1 < argc ) {
                act = argv[++i];
            } else
                goto RTK_TEST_MAIN_PRINT;
        }
    }

    if( isHelp ) {
        goto RTK_TEST_MAIN_PRINT;
    }


    // load config file
    if( fname.length() > 0 )
        pa->load(fname + ".ini");

    // set argc, argv to pa
    pa->set_i("argc", argc);
    pa->set_p("argv", argv);

    // parse input argument again
    pa->set_args(argc, argv);

    // save input arguments to file
    sa1 = path_split(argv[0]);
    sa2 = path_splitext(sa1[1]);
    save_arguments(argc, argv, sa2[0]);

    // print all settings
    pa->print();

    // call test function
    i = 0; j = 0;
    while( ma[i].m != NULL ) {
        if( strcmp(act.c_str(), ma[i].name) == 0 ) {

            // run routine
            t1 = tm_get_us();
            ret = ma[i].m->run(pa);
            t2 = tm_get_us();

            j = 1;

            break;
        }

        i++;
    }

    if( j == 0 ) {
        dbg_pe("Input arguments error!");
        goto RTK_TEST_MAIN_PRINT;
    }

    // print running time
    fmt::print("\n---------------------------------------------------------\n");
    fmt::print_colored(fmt::BLUE, "run time");
    fmt::print(" = {} sec ({} min)\n",
                1.0*(t2-t1)/1000000.0,         /* sec */
                1.0*(t2-t1)/60000000.0);       /* min */
    fmt::print("---------------------------------------------------------\n");

    goto RTK_TEST_MAIN_RET;

RTK_TEST_MAIN_PRINT:
    // print basic arguments
    print_basic_help_ex(argc, argv, ma, *pa);

    if( act.size() == 0 ) return ret;

    // call module help function
    i = 0;
    while( ma[i].m != NULL ) {
        if( strcmp(act.c_str(), ma[i].name) == 0 ) {

            fmt::print("\n");
            fmt::print("=======================================\n");
            fmt::print(">> Module: ");
            fmt::print_colored(fmt::BLUE, "{}\n", ma[i].name);
            fmt::print("=======================================\n");

            // run user provided help function
            ma[i].m->help(pa);

            fmt::print("\n");

            break;
        }

        i++;
    }

RTK_TEST_MAIN_RET:
    return ret;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void svar_print_basic_help(int argc, char *argv[],
                          TestFunctionArray fa[])
{
    int     i, j, k;
    int     tab = 30;

    fmt::print("\n");
    fmt::print("-------------------------- ");
    fmt::print_colored(fmt::GREEN, "basic usage");
    fmt::print(" --------------------------\n");

    fmt::print_colored(fmt::BLUE, "    conf=conf_file"); fmt::print("           config file\n");
    fmt::print_colored(fmt::BLUE, "    help"); fmt::print("                     print usage help\n");
    fmt::print_colored(fmt::BLUE, "    dbg_level=<1,2,3,4,5>"); fmt::print("    [0/1/2/3/4/5] debug level\n");
    fmt::print("                                1 - Error\n");
    fmt::print("                                2 - Warning\n");
    fmt::print("                                3 - Info\n");
    fmt::print("                                4 - Trace\n");
    fmt::print("                                5 - Normal\n");
    fmt::print_colored(fmt::BLUE, "    act=act_name"); fmt::print("             [s] test module name\n");

    fmt::print("-----------------------------------------------------------------\n");
    fmt::print("\n");

    fmt::print("---------------------------- ");
    fmt::print_colored(fmt::GREEN, "modules");
    fmt::print(" ----------------------------\n");

    i=0;
    while( fa[i].f != NULL ) {
        fmt::print_colored(fmt::BLUE, "    {}", fa[i].name);

        j = strlen(fa[i].name);
        for(k=j; k<tab; k++) fmt::print(" ");

        fmt::print(" : {}\n", fa[i].desc);

        i++;
    }

    fmt::print("-----------------------------------------------------------------\n");
    fmt::print("\n");
}


int svar_main(int argc, char *argv[],
              TestFunctionArray fa[])
{
    string          act, actDefault;
    int             dbg_level = 4;
    int             i, j=0;
    pi::ru64        t1=0, t2=0;
    int             ret=-1;
    string          fn_conf;
    StringArray     sa1, sa2;


    // get program base name
    sa1 = path_splitext(argv[0]);
    fn_conf = sa1[0] + ".cfg";
    sa2 = path_split(sa1[0]);
    actDefault = sa2[1];

    // set argc & argv
    SvarWithType<void*>::instance()["argv"] = argv;
    svar.i["argc"] = argc;

    // parse basic input arguments
    if( argc <= 1 && 0 ) {
        ret = 0;
        goto SVAR_TEST_MAIN_PRINT;
    }

    for(i=1; i<argc; i++) {
        sa1 = split_text(argv[i], "=");

        if( sa1[0] == "conf" ) {
            if( sa1.size() > 1 ) fn_conf = sa1[1];
        } else if( sa1[0] == "help" ) {
            ret = 0;
            goto SVAR_TEST_MAIN_PRINT;
        } else if( sa1[0] == "dbg_level" ) {
            if( sa1.size() > 1 ) {
                dbg_level = atoi(sa1[1].c_str());
                dbg_set_level(dbg_level);
            }
        }
    }

    // load config file
    if( fn_conf.length() > 0 )
        svar.ParseFile(fn_conf);

    // parse input arguments
    for(i=1; i<argc; i++) {
        sa1 = split_text(argv[i], "=");

        if( sa1.size() > 1 ) {
            svar.setvar(argv[i]);
        }
    }

    // save input arguments to file
    if( svar.GetInt("Svar.saveArguments", 0) ) {
        sa2 = path_splitext(argv[0]);
        save_arguments(argc, argv, sa2[0]);
    }

    // print all settings
    svar.dumpAllVars();

    // get action name
    act = svar.GetString("act", actDefault);

    // call corresponding function
    i = 0; j = 0;
    while( fa[i].f != NULL ) {
        if( strcmp(act.c_str(), "test_default") == 0 ) {
            test_default(NULL);
            break;
        } else if( strcmp(act.c_str(), fa[i].name) == 0 ) {

            // run given routine
            t1 = tm_get_us();
            ret = fa[i].f(NULL);
            t2 = tm_get_us();
            j = 1;

            break;
        }

        i++;
    }

    if( j == 0 ) {
        dbg_pe("Input arguments error!");
        goto SVAR_TEST_MAIN_PRINT;
    }

    // print running time
    printf("\n---------------------------------------------------------\n");
    printf("run time = %g sec (%g min)\n",
                1.0*(t2-t1)/1000000.0,         /* sec */
                1.0*(t2-t1)/60000000.0);       /* min */
    printf("---------------------------------------------------------\n");

    goto SVAR_TEST_MAIN_RET;

SVAR_TEST_MAIN_PRINT:
    // print basic arguments
    svar_print_basic_help(argc, argv, fa);

    // print user provided help
    i=0;
    while( fa[i].f != NULL ) {
        if( strcmp(fa[i].name, "print_help") == 0 ) {
            ret = fa[i].f(NULL);
            break;
        }
        i++;
    }

SVAR_TEST_MAIN_RET:
    return ret;
}


} // end of namespace pi
