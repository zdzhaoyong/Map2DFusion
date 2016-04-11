#ifndef __AHRS_MINI__
#define __AHRS_MINI__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


#include "base/time/Timer.h"
#include "base/osa/osa++.h"
#include "hardware/UART/UART.h"

namespace pi {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct AHRS_Frame
{
    enum FrameType {
        FRAME_AHRS,
        FRAME_SENSOR
    } frame_type;

    float       yaw, pitch, roll;           // degree
    float       alt, temp, press;           // meter, deg, pa
    int         imu_ps;                     // IMU per second

    int         Ax, Ay, Az;                 // accelation
    int         Gx, Gy, Gz;                 // gyro
    int         Mx, My, Mz;                 // megnatic

    int         crc,                        // CRC received
                crc_v,                      // CRC computed
                correct;                    // frame correct

    uint64_t    tm;                         // mills when receive

public:
    void print(void);
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class AHRS_Mini : public RThread
{
public:
    AHRS_Mini();
    AHRS_Mini(int port_no);
    AHRS_Mini(const char *port_name);

    virtual ~AHRS_Mini();

    int setUART(int port_no);
    int setUART(const char *port_name);

    virtual int start(void);
    virtual int stop(void);

    int frame_ready(void);
    AHRS_Frame frame_get(AHRS_Frame::FrameType ft = AHRS_Frame::FRAME_AHRS);

    int parse_frame(uint8_t *buf);

    virtual int thread_func(void *arg);

protected:
    AHRS_Frame  frame;
    AHRS_Frame  frame_imu, frame_sen;
    UART        uart;

    int         frame_aviable;
};

} // end of namespace pi

#endif // end of __AHRS_MINI__
