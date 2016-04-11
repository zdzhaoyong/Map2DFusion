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
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "base/types/types.h"
#include "base/Svar/DataStream.h"
#include "base/system/format/format.h"
#include "DateTime.h"


namespace pi {


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int g_timeZone = 8;     ///< time zone (default)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int is_leap(int y) {
    y += 1900;
    return (y % 4) == 0 && ((y % 100) != 0 || (y % 400) == 0);
}

ri64 time_utc(struct tm *tm)
{
    static const unsigned ndays[2][12] = {
        {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
    };

    ri64 res = 0;
    int i;

    for (i = 70; i < tm->tm_year; ++i)
        res += is_leap(i) ? 366 : 365;

    for (i = 0; i < tm->tm_mon; ++i)
        res += ndays[is_leap(tm->tm_year)][i];

    res += tm->tm_mday - 1;
    res *= 24;

    res += tm->tm_hour;
    res *= 60;

    res += tm->tm_min;
    res *= 60;

    res += tm->tm_sec;
    return res;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

DateTime::DateTime()
{
    clear();
}

DateTime::DateTime(DateTime_Type dtt)
{
    timeZone = g_timeZone;
    setCurrentDateTime(dtt);
}

DateTime::DateTime(const DateTime &dt)
{
    setDateTime(dt);
}

DateTime::DateTime(ri32 y, ri32 m, ri32 d)
{
    time_t rawtime;
    struct tm *timeinfo;
    struct timeval tm_val;

    time(&rawtime);
    gettimeofday(&tm_val, NULL);
    timeinfo = localtime(&rawtime);

    year        = y;
    month       = m;
    day         = d;

    hour        = timeinfo->tm_hour;
    min         = timeinfo->tm_min;
    sec         = timeinfo->tm_sec;

    nano_sec    = tm_val.tv_usec*1000;

    timeZone    = g_timeZone;
    dt_type     = DATETIME_LOCAL;
}

DateTime::DateTime(ri32 h, ri32 min_, ri32 s, ri32 ns)
{
    time_t rawtime;
    struct tm *timeinfo;
    struct timeval tm_val;

    time(&rawtime);
    gettimeofday(&tm_val, NULL);
    timeinfo = localtime(&rawtime);

    year        = timeinfo->tm_year + 1900;
    month       = timeinfo->tm_mon  + 1;
    day         = timeinfo->tm_mday;

    hour        = h;
    min         = min_;
    sec         = s;

    nano_sec    = ns;

    timeZone    = g_timeZone;
    dt_type     = DATETIME_LOCAL;
}

DateTime::DateTime(ri32 y, ri32 m, ri32 d,
                   ri32 h, ri32 min_, ri32 s,
                   ri32 ns)
{
    year        = y;
    month       = m;
    day         = d;

    hour        = h;
    min         = min_;
    sec         = s;

    nano_sec    = ns;

    timeZone    = g_timeZone;
    dt_type     = DATETIME_LOCAL;
}

DateTime::~DateTime()
{

}

int DateTime::clear(void)
{
    year     = 1970;
    month    = 1;
    day      = 1;

    hour     = 0;
    min      = 0;
    sec      = 0;
    nano_sec = 0;

    timeZone = g_timeZone;
    dt_type  = DATETIME_UTC;
}


int DateTime::setDateTime(const DateTime &dt)
{
    year    = dt.year;
    month   = dt.month;
    day     = dt.day;

    hour     = dt.hour;
    min      = dt.min;
    sec      = dt.sec;
    nano_sec = dt.nano_sec;

    timeZone = dt.timeZone;
    dt_type  = dt.dt_type;
}

int DateTime::setDate(ri32 y, ri32 m, ri32 d,
            DateTime_Type dtt)
{
    year        = y;
    month       = m;
    day         = d;

    dt_type     = dtt;

    return 0;
}

int DateTime::setTime(ri32 h, ri32 min_, ri32 s, ri32 ns)
{
    hour        = h;
    min         = min_;
    sec         = s;

    nano_sec    = ns;

    return 0;
}

int DateTime::setTime(ri32 h, ri32 min_, double fs)
{
    hour        = h;
    min         = min_;
    sec         = (ri32)(fs);

    nano_sec    = (ri32)( (fs - sec)*1000000000 );

    return 0;
}


int DateTime::setDateTime(ri32 y, ri32 m, ri32 d,
                          ri32 h, ri32 min_, ri32 s,
                          ri32 ns,
                          DateTime_Type dtt)
{
    year        = y;
    month       = m;
    day         = d;

    hour        = h;
    min         = min_;
    sec         = s;
    nano_sec    = ns;

    dt_type     = dtt;

    return 0;
}

int DateTime::setCurrentDate(DateTime_Type dtt)
{
    time_t rawtime;
    struct tm *timeinfo;
    struct timeval tm_val;

    time(&rawtime);
    gettimeofday(&tm_val, NULL);
    if( dtt == DATETIME_LOCAL ) {
        rawtime = rawtime + timeZone*3600;
    }

    timeinfo = gmtime(&rawtime);

    year        = timeinfo->tm_year + 1900;
    month       = timeinfo->tm_mon  + 1;
    day         = timeinfo->tm_mday;

    dt_type = dtt;

    return 0;
}

int DateTime::setCurrentTime(DateTime_Type dtt)
{
    time_t rawtime;
    struct tm *timeinfo;
    struct timeval tm_val;

    time(&rawtime);
    gettimeofday(&tm_val, NULL);
    if( dtt == DATETIME_LOCAL ) {
        rawtime = rawtime + timeZone*3600;
    }

    timeinfo = gmtime(&rawtime);

    hour        = timeinfo->tm_hour;
    min         = timeinfo->tm_min;
    sec         = timeinfo->tm_sec;

    nano_sec    = tm_val.tv_usec*1000;

    dt_type     = dtt;

    return 0;
}

int DateTime::setCurrentDateTime(DateTime_Type dtt)
{
    time_t rawtime;
    struct tm *timeinfo;
    struct timeval tm_val;

    time(&rawtime);
    gettimeofday(&tm_val, NULL);
    if( dtt == DATETIME_LOCAL ) {
        rawtime = rawtime + timeZone*3600;
    }

    timeinfo = gmtime(&rawtime);


    year        = timeinfo->tm_year + 1900;
    month       = timeinfo->tm_mon  + 1;
    day         = timeinfo->tm_mday;

    hour        = timeinfo->tm_hour;
    min         = timeinfo->tm_min;
    sec         = timeinfo->tm_sec;

    nano_sec    = tm_val.tv_usec*1000;

    dt_type     = dtt;

    return 0;
}



int DateTime::timeFromString(const char *fmt, const char *sTime)
{
    // FIXME: not implemented
    return 0;
}

int DateTime::dateFromString(const char *fmt, const char *sDate)
{
    // FIXME: not implemented
    return 0;
}

int DateTime::dateTimeFromString(const char *fmt, const char *sDateTime)
{
    // FIXME: not implemented
    return 0;
}



double DateTime::diffTime(DateTime &t0)
{
    struct tm   tm0, tm1;
    time_t      tt0, tt1;
    double      dt;

    if( dt_type == t0.dt_type ) {
        t0.toTM(&tm0);
    } else {
        DateTime t = t0;

        if( t0.dt_type == DATETIME_LOCAL ) {
            t.toUTC().toTM(&tm0);
        }
        if( t0.dt_type == DATETIME_UTC ) {
            t.toLocalTime().toTM(&tm0);
        }
    }

    toTM(&tm1);

    tt0 = time_utc(&tm0);
    tt1 = time_utc(&tm1);
    dt  = difftime(tt1, tt0);

    if( t0.nano_sec > nano_sec ) {
        dt = dt - 1.0 + 1.0*(t0.nano_sec - nano_sec)/1.0e9;
    } else {
        dt = dt + 1.0*(nano_sec - t0.nano_sec)/1.0e9;
    }

    return dt;
}

const int DateTime::toTM(struct tm *t) const
{
    t->tm_year = year  - 1900;
    t->tm_mon  = month - 1;
    t->tm_mday = day;

    t->tm_hour = hour;
    t->tm_min  = min;
    t->tm_sec  = sec;

    t->tm_isdst = 0;
}

const ri64 DateTime::toTime_t(void) const
{
    ri64        t;
    struct tm   tm;

    // get seconds since 1970/1/1
    toTM(&tm);
    t = time_utc(&tm);

    if( dt_type == DATETIME_LOCAL )
        t = t - timeZone*3600;

    return t;
}

int DateTime::fromTime_t(ri64 t)
{
    time_t      tt;
    struct tm   *tm1;

    if( dt_type == DATETIME_UTC )
        tt = t;
    else if ( dt_type == DATETIME_LOCAL )
        tt = t + timeZone*3600;

    tm1 = gmtime(&tt);

    year  = tm1->tm_year + 1900;
    month = tm1->tm_mon  + 1;
    day   = tm1->tm_mday;

    hour = tm1->tm_hour;
    min  = tm1->tm_min;
    sec  = tm1->tm_sec;

    nano_sec = 0;
}

const double DateTime::toTimeStampF(void) const
{
    double      ts;
    ri64        t;
    struct tm   tm;

    // get seconds since 1970/1/1
    toTM(&tm);
    t = time_utc(&tm);

    // convert to UTC time
    if( dt_type == DATETIME_LOCAL )
        t = t - timeZone*3600;

    // get timestamp in double
    ts = t + 1.0*nano_sec/1e9;

    return ts;
}

const ri64 DateTime::toTimeStamp(void) const
{
    ri64        t, ts;
    struct tm   tm;

    // get seconds since 1970/1/1
    toTM(&tm);
    t = time_utc(&tm);

    if( dt_type == DATETIME_LOCAL )
        t = t - timeZone*3600;

    ts = t*1000000 + nano_sec/1000;

    return ts;
}

int DateTime::fromTimeStampF(double ts)
{
    time_t      t;
    ri64        _ts;
    struct tm   *tm1;

    _ts = ts * 1e6;
    if( dt_type == DATETIME_UTC )
        t = _ts/1000000;
    else if ( dt_type == DATETIME_LOCAL )
        t = _ts/1000000 + timeZone*3600;

    tm1 = gmtime(&t);

    year  = tm1->tm_year + 1900;
    month = tm1->tm_mon  + 1;
    day   = tm1->tm_mday;

    hour = tm1->tm_hour;
    min  = tm1->tm_min;
    sec  = tm1->tm_sec;

    nano_sec = (_ts % 1000000)*1000;

    return 0;
}

int DateTime::fromTimeStamp(ri64 ts)
{
    time_t      t;
    struct tm   *tm1;

    if( dt_type == DATETIME_UTC )
        t = ts/1000000;
    else if ( dt_type == DATETIME_LOCAL )
        t = ts/1000000 + timeZone*3600;

    tm1 = gmtime(&t);

    year  = tm1->tm_year + 1900;
    month = tm1->tm_mon  + 1;
    day   = tm1->tm_mday;

    hour = tm1->tm_hour;
    min  = tm1->tm_min;
    sec  = tm1->tm_sec;

    nano_sec = (ts % 1000000)*1000;

    return 0;
}

DateTime& DateTime::toUTC(void)
{
    if( dt_type == DATETIME_LOCAL ) {
        struct tm   tm0, *tm1;
        time_t      tt0;

        toTM(&tm0);

        // get second from 1970/1/1 and convert to UTC
        tt0 = time_utc(&tm0);
        tt0 = tt0 - timeZone*3600;
        tm1 = gmtime(&tt0);

        year  = tm1->tm_year + 1900;
        month = tm1->tm_mon  + 1;
        day   = tm1->tm_mday;

        hour = tm1->tm_hour;
        min  = tm1->tm_min;
        sec  = tm1->tm_sec;

        dt_type = DATETIME_UTC;
    }

    return *this;
}

DateTime& DateTime::toLocalTime(void)
{
    if( dt_type == DATETIME_UTC ) {
        struct tm   tm0, *tm1;
        time_t      tt0;

        toTM(&tm0);

        tt0 = time_utc(&tm0);
        tt0 = tt0 + timeZone*3600;
        tm1 = gmtime(&tt0);

        year  = tm1->tm_year + 1900;
        month = tm1->tm_mon  + 1;
        day   = tm1->tm_mday;

        hour = tm1->tm_hour;
        min  = tm1->tm_min;
        sec  = tm1->tm_sec;

        dt_type = DATETIME_LOCAL;
    }

    return *this;
}

DateTime& DateTime::operator = (const DateTime &other)
{
    setDateTime(other);

    return *this;
}


bool DateTime::operator != (const DateTime &other) const
{
    ri64    t1, t2;

    t1 = toTimeStamp();
    t2 = other.toTimeStamp();

    if( t1 != t2 ) return true;
    else return false;
}

bool DateTime::operator == (const DateTime &other) const
{
    ri64    t1, t2;

    t1 = toTimeStamp();
    t2 = other.toTimeStamp();

    if( t1 == t2 ) return true;
    else return false;
}

bool DateTime::operator < ( const DateTime &other ) const
{
    ri64    t1, t2;

    t1 = toTimeStamp();
    t2 = other.toTimeStamp();

    if( t1 < t2 ) return true;
    else return false;
}

bool DateTime::operator <= ( const DateTime &other ) const
{
    ri64    t1, t2;

    t1 = toTimeStamp();
    t2 = other.toTimeStamp();

    if( t1 <= t2 ) return true;
    else return false;
}

bool DateTime::operator > ( const DateTime &other ) const
{
    ri64    t1, t2;

    t1 = toTimeStamp();
    t2 = other.toTimeStamp();

    if( t1 > t2 ) return true;
    else return false;
}

bool DateTime::operator >= ( const DateTime &other ) const
{
    ri64    t1, t2;

    t1 = toTimeStamp();
    t2 = other.toTimeStamp();

    if( t1 >= t2 ) return true;
    else return false;
}

std::ostream& operator << (std::ostream &os, const DateTime &dt)
{
    char    c1, c2;

    if( dt.dt_type == DateTime::DATETIME_LOCAL ) c1 = 'L';
    else                               c1 = 'U';
    if( dt.timeZone >= 0 ) c2 = '+';
    else                   c2 = '-';

    return os << fmt::sprintf("%04d-%02d-%02d %02d:%02d:%02d.%09d %c(%c%02d)",
                              dt.year, dt.month, dt.day,
                              dt.hour, dt.min, dt.sec,
                              dt.nano_sec,
                              c1, c2, dt.timeZone);
}

void DateTime::print(void)
{
    char    c1, c2;

    if( dt_type == DATETIME_LOCAL ) c1 = 'L';
    else                            c1 = 'U';
    if( timeZone >= 0 ) c2 = '+';
    else                c2 = '-';

    printf("%04d-%02d-%02d %02d:%02d:%02d.%09d %c(%c%02d)",
           year, month, day,
           hour, min, sec,
           nano_sec,
           c1, c2, timeZone);
}

int DateTime::read(FILE *fp)
{
    int i;

    if( fp == NULL ) {
        return -1;
    }

    if( fread(&year,    sizeof(ri32), 1, fp) < 1 ) return -1;
    if( fread(&month,   sizeof(ri32), 1, fp) < 1 ) return -1;
    if( fread(&day,     sizeof(ri32), 1, fp) < 1 ) return -1;

    if( fread(&hour,    sizeof(ri32), 1, fp) < 1 ) return -1;
    if( fread(&min,     sizeof(ri32), 1, fp) < 1 ) return -1;
    if( fread(&sec,     sizeof(ri32), 1, fp) < 1 ) return -1;
    if( fread(&nano_sec, sizeof(ri32), 1, fp) < 1 ) return -1;

    if( fread(&timeZone, sizeof(ri32), 1, fp) < 1 ) return -1;
    if( fread(&i,       sizeof(ri32), 1, fp) < 1 ) return -1;
    dt_type = (DateTime_Type) i;

    return 0;
}

int DateTime::write(FILE *fp)
{
    ri32 i;

    if( fp == NULL ) {
        return -1;
    }

    fwrite(&year,       sizeof(ri32), 1, fp);
    fwrite(&month,      sizeof(ri32), 1, fp);
    fwrite(&day,        sizeof(ri32), 1, fp);

    fwrite(&hour,       sizeof(ri32), 1, fp);
    fwrite(&min,        sizeof(ri32), 1, fp);
    fwrite(&sec,        sizeof(ri32), 1, fp);
    fwrite(&nano_sec,   sizeof(ri32), 1, fp);

    fwrite(&timeZone,   sizeof(ri32), 1, fp);

    i = dt_type;
    fwrite(&i,          sizeof(ri32), 1, fp);

    return 0;
}

int DateTime::toStream(RDataStream &s, int isSingleDS)
{
    ri32    dt;

    if( isSingleDS ) {
        // init stream
        s.resize(0);

        // write magic number & version
        s.setMagicNum(0xEABF);
        s.setVerNum(0x0001);
    }

    // write date/time to stream
    s << year << month << day << hour << min << sec << nano_sec << timeZone;

    // write date/time type to stream
    dt = dt_type;
    s << dt;

    return 0;
}

int DateTime::fromStream(RDataStream &s, int isSingleDS)
{
    ri32    dt;
    ru32    d_magic, d_ver;

    if( isSingleDS ) {
        s.seek(0);

        // get magic number & version
        s.getHeader(d_magic, d_ver);

        if( d_magic != 0xEABF ) {
            dbg_pe("Magic number error! %x\n", d_magic);
            return -1;
        }
    }

    // read date/time
    if( 0 != s.read(year) )     return -2;
    if( 0 != s.read(month) )    return -2;
    if( 0 != s.read(day) )      return -2;
    if( 0 != s.read(hour) )     return -2;
    if( 0 != s.read(min) )      return -2;
    if( 0 != s.read(sec) )      return -2;
    if( 0 != s.read(nano_sec) ) return -2;
    if( 0 != s.read(timeZone) ) return -2;

    // read date/time type
    if( 0 != s.read(dt) )       return -2;
    dt_type = (DateTime_Type) dt;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int set_timeZone(int tz)
{
    g_timeZone = tz;
}

int get_timeZone(void)
{
    return g_timeZone;
}


} // end of namespace pi
