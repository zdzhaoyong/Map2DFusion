#ifndef __TIME_H__
#define __TIME_H__


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>


////////////////////////////////////////////////////////////////////////////////
/// Linux
////////////////////////////////////////////////////////////////////////////////

#ifdef PIL_LINUX

#include <sys/time.h>
#include <sys/timeb.h>

namespace pi {


inline uint64_t tm_get_millis(void)
{
    struct timeval  tm_val;
    uint64_t        v;
    int             ret;

    ret = gettimeofday(&tm_val, NULL);

    v = tm_val.tv_sec*1000 + tm_val.tv_usec/1000;
    return v;
}

inline uint64_t tm_get_ms(void)
{
    struct timeval  tm_val;
    uint64_t        v;
    int             ret;

    ret = gettimeofday(&tm_val, NULL);

    v = tm_val.tv_sec*1000 + tm_val.tv_usec/1000;
    return v;
}

inline uint64_t tm_get_us(void)
{
    struct timeval  tm_val;
    uint64_t        v;
    int             ret;

    ret = gettimeofday(&tm_val, NULL);

    v = tm_val.tv_sec*1000000 + tm_val.tv_usec;
    return v;
}

inline double tm_getTimeStamp(void)
{
    struct timeval  tm_val;
    double          v;
    int             ret;

    ret = gettimeofday(&tm_val, NULL);

    v = tm_val.tv_sec + 1.0*tm_val.tv_usec/1e6;
    return v;
}


inline uint32_t tm_getTimeStampUnix(void)
{
    struct timeval  tm_val;
    uint32_t        v;
    int             ret;

    ret = gettimeofday(&tm_val, NULL);

    v = tm_val.tv_sec;
    return v;
}

inline void tm_sleep(uint32_t t)
{
    struct timespec tp;

    tp.tv_sec  = t / 1000;
    tp.tv_nsec = ( t % 1000 ) * 1000000;

    while( nanosleep(&tp, &tp) );
}

inline void tm_sleep_us(int64_t t)
{
    struct timespec tp;

    tp.tv_sec  = t / 1000000;
    tp.tv_nsec = ( t % 1000000 ) * 1000;

    while( nanosleep(&tp, &tp) );
}


} // end of namespace pi


#endif // end of PIL_LINUX


////////////////////////////////////////////////////////////////////////////////
/// Windows & Mingw
////////////////////////////////////////////////////////////////////////////////

#if defined(PIL_WINDOWS) || defined(PIL_MINGW)

#include <windows.h>


namespace pi {

inline uint64_t tm_get_millis(void)
{
    return GetTickCount();
}

inline uint64_t tm_get_ms(void)
{
    return GetTickCount();
}

inline uint64_t tm_get_us(void)
{
    FILETIME        t;
    uint64_t        t_ret;

    // get UTC time
    GetSystemTimeAsFileTime(&t);

    t_ret = 0;

    t_ret |= t.dwHighDateTime;
    t_ret <<= 32;
    t_ret |= t.dwLowDateTime;

    // convert 100 ns to [ms]
    return t_ret/10;
}

inline double tm_getTimeStamp(void)
{
    FILETIME        t;
    uint64_t        t_ret;
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
}


inline uint32_t tm_getTimeStampUnix(void)
{
    FILETIME        t;
    uint64_t        t_ret;
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
}

inline void tm_sleep(uint32_t t)
{
    Sleep(t);
}

inline void tm_sleep_us(int64_t t)
{
    Sleep(t/1000);
}

} // end of namespace pi


#endif // end of PIL_WINDOWS || PIL_MINGW


#endif // end of __TIME_H__
