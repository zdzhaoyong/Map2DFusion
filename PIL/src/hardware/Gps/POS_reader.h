#ifndef __GPS_READER_H__
#define __GPS_READER_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <vector>
#include <deque>


#include "base/types/types.h"
#include "base/time/DateTime.h"
#include "base/Svar/DataStream.h"
#include "base/osa/osa++.h"
#include "hardware/UART/UART.h"

namespace pi {


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \enum AHRS_FrameType
/// \brief The AHRS_FrameType enum
///
/// This enum indicate frame type, AHRS or Sensor data
///
enum AHRS_DataType {
    AHRS_DATA_AHRS,                         ///< AHRS frame
    AHRS_DATA_SENSOR                        ///< Sensor data frame
};

///
/// \struct AHRS_Frame
/// \brief The AHRS_Frame struct
///
/// This struct store estimated Euler angles and raw sensor data
///
class AHRS_Data
{
public:
    float       yaw, pitch, roll;           ///< degree
    float       alt, temp, press;           ///< meter, deg, pa
    ri32        imu_ps;                     ///< IMU per second
    ri32        crc_a, crcv_a;              ///< CRC received and computed
    ri64        tm_a;                       ///< received time (us)

    ri32        Ax, Ay, Az;                 ///< accelation
    ri32        Gx, Gy, Gz;                 ///< gyro
    ri32        Mx, My, Mz;                 ///< megnatic
    ri32        crc_s, crcv_s;              ///< CRC received and computed
    ri64        tm_s;                       ///< received time (us)

    ri64        tm;                         ///< received time (us)
    ri32        correct;                    ///< frame correct or not (1-correct, 0-wrong)

public:
    AHRS_Data() { clear(); }
    ~AHRS_Data() {}

    void print(void);
    void clear(void);

    int parse(uint8_t *buf);

    AHRS_Data& operator = (const AHRS_Data &other) {
        yaw         = other.yaw;
        pitch       = other.pitch;
        roll        = other.roll;
        alt         = other.alt;
        temp        = other.temp;
        press       = other.press;
        imu_ps      = other.imu_ps;
        crc_a       = other.crcv_a;
        crcv_a      = other.crcv_a;
        tm_a        = other.tm_a;

        Ax          = other.Ax;
        Ay          = other.Ay;
        Az          = other.Az;
        Gx          = other.Gx;
        Gy          = other.Gy;
        Gz          = other.Gz;
        Mx          = other.Mx;
        My          = other.My;
        Mz          = other.Mz;
        crc_s       = other.crc_s;
        crcv_s      = other.crcv_s;

        tm          = other.tm;
        correct     = other.correct;

        return *this;
    }

    ///
    /// \brief toStream
    /// \param s - byte stream
    /// \return
    ///
    int toStream(RDataStream &ds);

    ///
    /// \brief fromStream
    /// \param s - byte stream
    /// \return
    ///
    int fromStream(RDataStream &ds);
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


///
/// \brief The GPS Data class
///
/// \see http://aprs.gids.nl/nmea
/// \see http://www.gpsinformation.org/dale/nmea.htm
///
class POS_Data
{
public:
    POS_Data() {
        clear();
    }

    ~POS_Data() {}

    void clear(void) {
        lat         = 0;
        latFlag     = 0;
        lng         = 0;
        lngFlag     = 0;

        altitude    = 0.0;
        geoid_h     = 0.0;
        h           = 0.0;

        HDOP        = 99;
        nSat        = 0;
        fixQuality  = 1;

        batV        = 0.0;

        x           = 0.0;
        y           = 0.0;
        z           = 0.0;

        vx          = 0.0;
        vy          = 0.0;
        vz          = 0.0;

        ft_period   = 0;

        posAvaiable = 0;
        correct     = 0;

        ahrs.clear();
    }

    int fromGPGGA(const char *buf);
    int fromBatV(const ru8 *buf, int len);

    void print(void);

    int read(FILE *fp);
    int write(FILE *fp);

    ///
    /// \brief toStream
    /// \param s - byte stream
    /// \return
    ///
    int toStream(RDataStream &ds);

    ///
    /// \brief fromStream
    /// \param s - byte stream
    /// \return
    ///
    int fromStream(RDataStream &ds);


    POS_Data& operator = (const POS_Data &other) {
        time        = other.time;

        lat         = other.lat;
        latFlag     = other.latFlag;
        lng         = other.lng;
        lngFlag     = other.lngFlag;

        altitude    = other.altitude;
        geoid_h     = other.geoid_h;
        h           = other.h;

        HDOP        = other.HDOP;
        nSat        = other.nSat;
        fixQuality  = other.fixQuality;

        ahrs        = other.ahrs;

        x           = other.x;
        y           = other.y;
        z           = other.z;

        vx          = other.vx;
        vy          = other.vy;
        vz          = other.vz;

        ft_period   = other.ft_period;

        batV        = other.batV;

        posAvaiable = other.posAvaiable;
        correct     = other.correct;

        return *this;
    }


public:
    DateTime            time;       ///< time, hhmmss.ss (UTC)

    double              lat;        ///< latitude, degree
    double              lng;        ///< longitude, degree
    ri32                latFlag;    ///< latitude flag  ('N', 'S')
    ri32                lngFlag;    ///< longitude flag ('W', 'E')

    double              altitude;   ///< meters above mean sea level
    double              geoid_h;    ///< Height of geoid above WGS84 ellipsoid
    double              h;          ///< Height from ground

    double              HDOP;       ///< Horizontal Dilution of Precision (HDOP),
                                    ///< Relative accuracy of horizontal position

    ri32                nSat;       ///< Number of Satellites in view

    ri32                fixQuality; ///< Data is from a GPS fix
                                    ///< 0 = Invalid, 1 = GPS fix, 2 = DGPS fix

    AHRS_Data           ahrs;       ///< AHRS data

    double              x, y, z;    ///< position related to begining point
    double              vx, vy, vz; ///< Velocity in x,y,z-directions
    int                 ft_period;  ///< flight time period (in second)

    double              batV;       ///< Battery voltage

    ri32                posAvaiable;///< Postion information available
    ri32                correct;    ///< frame correct or not
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The POS_DataManager class
///
class POS_DataManager
{
public:
    POS_DataManager() { init(); }
    virtual ~POS_DataManager() { release(); }

    int open(const char *fname);            ///< open saved file
    int close(void);                        ///< close saved file
    int load(void);                         ///< read data from file
    int save(void);                         ///< save data to file

    int load(const char *fname);            ///< read data from given file
    int save(const char *fname);            ///< save data to given file

    POS_Data& addData(POS_Data &d);         ///< calculate x,y,z and then add POS data to queue

    POS_Data& processData(POS_Data &d);     ///< calculate x,y,z and then add POS data to queue

    pi::Point3d XYZ2Pos(const pi::Point3d& xyz);
    pi::Point3d Pos2XYZ(const pi::Point3d& pos);

    POS_Data& at(int idx);                      ///< get given frame's data by frame index
    POS_Data& getGPSData(int idx);              ///< get given frame's data by frame index

    int getGPSData(ri64 ts, POS_Data &d);       ///< get POS data by timestamp
    int getGPSData(DateTime &t, POS_Data &d);   ///< get POS data by datetime
    POS_Data& getGPSData(DateTime &t);          ///< get POS data by datetime
    POS_Data getGPSData(double timeS)
    {
        POS_Data dt;
        getGPSData((ri64)(timeS*1000000),dt);
        return dt;
    }

    POS_Data& front(void);                  ///< return first data
    POS_Data& back(void);                   ///< return last data

    int clear(void);                        ///< clear all data
    int size(void);                         ///< get total frame number
    int length(void);                       ///< get total frame number

    POS_Data& operator [](int idx);         ///< operator of []

    int setBaseDate(DateTime &dt) {
        m_baseDate = dt;
        m_bUseBaseTime = 1;
        return 0;
    }

    int getBaseDate(DateTime &dt) {
        dt = m_baseDate;
        return 0;
    }

    int getTS_MinMax(ri64 &tsMin_, ri64 &tsMax_) {
        tsMin_ = tsMin;
        tsMax_ = tsMax;

        return 0;
    }

protected:
    void init(void);
    void release(void);

protected:
    DateTime                m_baseDate;     ///< base date
    std::vector<POS_Data>   m_data;         ///< data array
    std::vector<ri64>       m_tsArray;      ///< time stamp array
    RMutex                  *m_mutex;       ///< read/write lock

    ri64                    tsMin, tsMax;   ///< begining & ending timestamp
    double                  z0, z1;         ///< begining & current height
    double                  lat0, lng0;     ///< begining & current lat
    double                  lat1, lng1;     ///< begining & current lng
    ri64                    ft_beg;         ///< timestamp of flight begining (z>10 m, unit: second)

    int                     m_bUseBaseTime; ///< use base-time or not
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The POS_Reader class
///
/// Read POS data from UART or file
///
class POS_Reader : public RThread
{
public:
    POS_Reader();
    virtual ~POS_Reader();

    ///
    /// \brief get UART object
    /// \return UART object
    ///
    UART* getUART(void) {
        return uart;
    }

    ///
    /// \brief set UART object
    /// \param u - UART pointer
    ///
    void setUART(UART *u) {
        uart = u;
    }


    ///
    /// \brief set autosave file name
    /// \param fname - autosave file name
    ///
    void setAutosaveFile(const std::string &fname);

    ///
    /// \brief get autosave file name
    /// \return autosave file name
    ///
    std::string getAutosaveFile(void);

    ///
    /// \brief setDataType
    /// \param dt
    ///
    void setDataType(int dt) {
        dataType = dt;
    }

    ///
    /// \brief getDataType
    /// \return
    ///
    int getDataType(void) {
        return dataType;
    }

    ///
    /// \brief begin receiving thread
    /// \return
    ///
    int begin(void);

    ///
    /// \brief stop receiving thread
    /// \return
    ///
    int stop(void);

    ///
    /// \brief Determin frame available or not
    /// \return 0 - no frame, 1 - frame available
    ///
    int frame_ready(void);

    ///
    /// \brief Get current frame number in queue
    /// \return frame number
    ///
    int frame_num(void);

    ///
    /// \brief Clear all frames in the queue
    /// \return 0 - Success
    ///
    int frame_clear(void);

    ///
    /// \brief Get one frame from queue
    /// \return frame
    ///
    POS_Data frame_get(void);

    ///
    /// \brief Get one frame from queue
    /// \param f - AHRS_Frame
    /// \return 0 - Success, 1 - Failed
    ///
    int frame_get(POS_Data &f);

    ///
    /// \brief Get newest frame and clear frame queue
    /// \param f - AHRS_Frame
    /// \return 0 - Success, 1 - Failed
    ///
    int frame_get_last(POS_Data &f);

    ///
    /// \brief UART receiving thread
    /// \param arg - thread input arguments
    /// \return NOT USED
    ///
    int thread_func(void *arg=NULL);

    int thread_fun_binData(void *arg=NULL);
    int thread_fun_asciiData(void *arg=NULL);

    ///
    /// \brief parse input frame (binary data)
    /// \param buf - buffer address
    /// \param len - data length
    /// \return
    ///
    int parseFrame_binData(ru8 *buf, int len);


    ///
    /// \brief parse input frame (ASCII data)
    /// \param buf - buffer address
    /// \param len - data length
    /// \return
    ///
    int parseFrame_asciiData(ru8 *buf, int len);

    virtual void addFrame(POS_Data& frame);


protected:
    UART                    *uart;              ///< UART object

    int                     dataType;           ///< data type (0 - binary, 1 - ASCII)
    int                     portType;           ///< port type (0 - true port,1 virtual port)

    std::deque<POS_Data>    frameQueue;         ///< Frame queue
    RMutex                  *mutex;             ///< Frame queue mutex

    std::string             m_fnAutoSave;       ///< auto-save file name
    FILE                    *m_fpAutoSave;      ///< auto-save file handle
};

} // end of namespace pi

#endif // end of __GPS_READER_H__
