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

#ifndef __RTK_DATETIME_H__
#define __RTK_DATETIME_H__

#include <vector>
#include <ostream>

#include "base/types/types.h"
#include "base/Svar/DataStream.h"


namespace pi {


///
/// \brief The DateTime class
///
class DateTime
{
public:
    ///
    /// \brief The DateTime_Type enum
    ///
    enum DateTime_Type
    {
        DATETIME_LOCAL = 0,                 ///< Local time
        DATETIME_UTC,                       ///< UTC/GMT time
        DATETIME_GPS                        ///< GPS time (FIXME: not used yet)
    };


public:
    DateTime();
    DateTime(DateTime_Type dtt);
    DateTime(const DateTime &dt);
    DateTime(const char *sDT);
    DateTime(ri32 y, ri32 m, ri32 d);
    DateTime(ri32 h, ri32 min, ri32 s,
             ri32 ns);
    DateTime(ri32 y, ri32 m, ri32 d,
             ri32 h, ri32 min, ri32 s,
             ri32 ns=0);

    ~DateTime();

    ///
    /// \brief clear date/time
    /// \return
    ///
    int clear(void);

    ///
    /// \brief setDateTime by DateTime date
    /// \param dt - DateTime class data
    /// \return
    ///
    int setDateTime(const DateTime &dt);

    ///
    /// \brief setDate
    /// \param y    - year
    /// \param m    - month
    /// \param d    - day
    /// \param dtt  - date type
    /// \return
    ///
    int setDate(ri32 y, ri32 m, ri32 d,
                DateTime_Type dtt = DATETIME_LOCAL);

    ///
    /// \brief setTime
    /// \param h        - hour
    /// \param min_     - minute
    /// \param s        - second
    /// \param ns       - nano second
    /// \return
    ///
    int setTime(ri32 h, ri32 min_, ri32 s, ri32 ns=0);

    ///
    /// \brief setTime
    /// \param h        - hour
    /// \param min_     - minute
    /// \param fs       - second
    /// \return
    ///
    int setTime(ri32 h, ri32 min_, double fs);

    ///
    /// \brief setDateTime
    /// \param y        - year
    /// \param m        - month
    /// \param d        - day
    /// \param h        - hour
    /// \param min_     - minute
    /// \param s        - second
    /// \param ns       - nano second
    /// \param dtt      - date/time type (UTC, local)
    /// \return
    ///
    int setDateTime(ri32 y, ri32 m, ri32 d, ri32 h, ri32 min_, ri32 s,
                    ri32 ns=0,
                    DateTime_Type dtt = DATETIME_LOCAL);


    ///
    /// \brief set to current date
    /// \param dtt      - date/time type (UTC, local)
    /// \return
    ///
    int setCurrentDate(DateTime_Type dtt = DATETIME_LOCAL);

    ///
    /// \brief set to current time
    /// \param dtt      - date/time type (UTC, local)
    /// \return
    ///
    int setCurrentTime(DateTime_Type dtt = DATETIME_LOCAL);

    ///
    /// \brief set to current date and time
    /// \param dtt      - date/time type (UTC, local)
    /// \return
    ///
    int setCurrentDateTime(DateTime_Type dtt = DATETIME_LOCAL);


    ///
    /// \brief set time from string
    /// \param fmt      - string format
    /// \param sTime    - string contains time
    /// \return
    ///
    int timeFromString(const char *fmt, const char *sTime);

    ///
    /// \brief set date from string
    /// \param fmt      - string format
    /// \param sTime    - string contains date
    /// \return
    ///
    int dateFromString(const char *fmt, const char *sDate);

    ///
    /// \brief set date/time from string
    /// \param fmt          - string format
    /// \param sDateTime    - string contains date/time
    /// \return
    ///
    int dateTimeFromString(const char *fmt, const char *sDateTime);


    ///
    /// \brief to stdandard TM struct
    /// \param t        - tm
    /// \return
    ///
    const int toTM(struct tm *t) const;

    ///
    /// \brief toTime_t
    /// \return         - second since 1970/1/1
    ///
    const ri64 toTime_t(void) const;

    ///
    /// \brief setup date/time from time_t value
    /// \param t        - second since 1970/1/1
    /// \return
    ///
    int fromTime_t(ri64 t);

    ///
    /// \brief toTimeStampF
    /// \return         - second since 1970/1/1
    ///
    const double toTimeStampF(void) const;

    ///
    /// \brief toTimeStamp
    /// \return         - micro second since 1970/1/1
    ///
    const ri64 toTimeStamp(void) const;

    ///
    /// \brief setup date/time from time stamp
    /// \param ts       - second since 1970/1/1
    /// \return
    ///
    int fromTimeStampF(double ts);

    ///
    /// \brief setup date/time from time stamp
    /// \param ts       - micro second since 1970/1/1
    /// \return
    ///
    int fromTimeStamp(ri64 ts);




    ///
    /// \brief date/time to UCT
    /// \return
    ///
    DateTime& toUTC(void);

    ///
    /// \brief date/time to local
    /// \return
    ///
    DateTime& toLocalTime(void);


    ///
    /// \brief operator =
    /// \param other
    /// \return
    ///
    DateTime& operator = (const DateTime &other);

    bool operator != (const DateTime &other) const;
    bool operator == (const DateTime &other) const;
    bool operator <  (const DateTime &other) const;
    bool operator <= (const DateTime &other) const;
    bool operator >  (const DateTime &other) const;
    bool operator >= (const DateTime &other) const;

    friend std::ostream& operator << (std::ostream &os, const DateTime &dt);

    ///
    /// \brief diffTime - return the number of seconds elasped
    ///                     between t0 and current time
    /// \param t0 - input date/time of begin timing
    /// \return The seconds between t0 and date/time of this object
    ///
    double diffTime(DateTime &t0);

    ///
    /// \brief print to stdout
    ///
    void print(void);

    ///
    /// \brief read from file
    /// \param fp
    /// \return
    ///
    int read(FILE *fp);

    ///
    /// \brief write to file
    /// \param fp
    /// \return
    ///
    int write(FILE *fp);

    ///
    /// \brief toStream
    /// \param s - byte stream
    /// \return
    ///
    int toStream(RDataStream &s, int isSingleDS = 0);

    ///
    /// \brief fromStream
    /// \param s - byte stream
    /// \return
    ///
    int fromStream(RDataStream &s, int isSingleDS = 0);


public:
    ri32        year;               ///< year
    ri32        month;              ///< month
    ri32        day;                ///< day of the month

    ri32        hour;               ///< hour
    ri32        min;                ///< minute
    ri32        sec;                ///< second
    ru32        nano_sec;           ///< nano second

    ri32            timeZone;       ///< time zone
    DateTime_Type   dt_type;        ///< Date/time type
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief set_timeZone
///     Set current time zone from (-12 ~ 12)
///
/// \param tz - time zone different from UTC (unit: hour)
/// \return not used
///
/// \see get_timeZone
///
int set_timeZone(int tz);

///
/// \brief get_timeZone
///     Get current time zone
///
/// \return time zone (unit: hour)
///
int get_timeZone(void);

} // end of namespace pi

#endif // __RTK_DATETIME_H__

