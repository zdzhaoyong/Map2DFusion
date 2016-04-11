#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base/debug/debug_config.h"
#include "base/time/Time.h"

#include "AHRS_mini.h"

namespace pi {


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void AHRS_Frame::print(void)
{
    printf("IMU: yaw = %12f, pitch = %12f, roll  = %12f\n", yaw, pitch, roll);
    printf("     alt = %12f, temp  = %12f, press = %12f\n", alt, temp, press);
    printf("     imu_ps = %5d, crc = 0x%02x (%d), tm = %lld\n\n", imu_ps, crc, correct, tm);
    printf("SEN: Ax = %6d, Ay = %6d, Az = %6d\n", Ax, Ay, Az);
    printf("     Gx = %6d, Gy = %6d, Gz = %6d\n", Gx, Gy, Gz);
    printf("     Mx = %6d, My = %6d, Mz = %6d\n", Mx, My, Mz);
    printf("     crc = 0x%02x (%d), tm = %lld\n\n", crc, correct, tm);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int AHRS_Mini::thread_func(void *arg)
{
    AHRS_Mini       *ah;

    uint8_t         buf[30];
    uint8_t         ub;

    int             sta;
    int             frame_len;
    int             ir;

    int             j;

    ah  = this;

    sta = 0;
    while( getAlive()  ) {
        // read one byte
        j = 0;
        while ( j<1 ) j = ah->uart.read(&ub, 1);

        if( sta == 0 ) {
            if( ub == 0xA5 ) sta = 1;
        } else if ( sta == 1 ) {
            if( ub == 0x5A ) {
                sta = 2;
                ir = 0;
                ah->frame.tm = tm_get_millis();
            } else
                sta = 0;
        } else if ( sta == 2 ) {
            buf[ir++] = ub;
            if( ir == 1 ) frame_len = buf[0];

            if( ir >= frame_len ) {
                sta = 0;
                ah->parse_frame(buf);
                ah->frame_aviable = 1;
            }
        }
    }

    return 0;
}

AHRS_Mini::AHRS_Mini()
{
    frame_aviable = 0;
}

AHRS_Mini::AHRS_Mini(int port_no)
{
    frame_aviable = 0;

    uart.port_no = port_no;
}

AHRS_Mini::AHRS_Mini(const char *port_name)
{
    frame_aviable = 0;

    uart.port_name = port_name;
}

AHRS_Mini::~AHRS_Mini()
{
    stop();
}

int AHRS_Mini::setUART(int port_no)
{
    uart.port_no = port_no;
}

int AHRS_Mini::setUART(const char *port_name)
{
    uart.port_name = port_name;
}

int AHRS_Mini::start(void)
{
    // open UART port
    if( 0 != uart.open() ) {
        dbg_pe("ERR: can not open port: %s (%d)\n",
               uart.port_name.c_str(), uart.port_no);
        return -1;
    }

    // start thread
    RThread::start(NULL);

    return 0;
}

int AHRS_Mini::stop(void)
{
    setAlive(0);
    wait(10);
    kill();

    return 0;
}

int AHRS_Mini::parse_frame(uint8_t *buf)
{
    int             frame_len;
    int16_t         temp;
    int             i;
    int             crc_sum;

    frame_len = buf[0];
    if( buf[1] == 0xA1 )
        frame.frame_type = AHRS_Frame::FRAME_AHRS;
    else if( buf[1] == 0xA2 )
        frame.frame_type = AHRS_Frame::FRAME_SENSOR;

    // AHRS data
    if( frame.frame_type == AHRS_Frame::FRAME_AHRS ) {
        temp = buf[2] << 8 | buf[3];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        frame.yaw = 0.1*temp;

        temp = buf[4] << 8 | buf[5];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        frame.pitch = 0.1*temp;

        temp = buf[6] << 8 | buf[7];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        frame.roll = 0.1*temp;

        temp = buf[8] << 8 | buf[9];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        frame.alt = 0.1*temp;

        temp = buf[10] << 8 | buf[11];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        frame.temp = 0.1*temp;

        temp = buf[12] << 8 | buf[13];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        frame.press = 10.0*temp;

        temp = buf[14] << 8 | buf[15];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        frame.imu_ps = temp;

        frame.crc = buf[16];

        crc_sum  = 0;
        for(i=0; i<16; i++) crc_sum += buf[i];
        frame.crc_v = crc_sum % 256;

        if( frame.crc == frame.crc_v )
            frame.correct = 1;
        else
            frame.correct = 0;

        frame_imu = frame;
    } else if ( frame.frame_type == AHRS_Frame::FRAME_SENSOR ) {
        temp = buf[2] << 8 | buf[3];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        frame.Ax = temp;

        temp = buf[4] << 8 | buf[5];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        frame.Ay = temp;

        temp = buf[6] << 8 | buf[7];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        frame.Az = temp;

        temp = buf[8] << 8 | buf[9];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        frame.Gx = temp;

        temp = buf[10] << 8 | buf[11];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        frame.Gy = temp;

        temp = buf[12] << 8 | buf[13];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        frame.Gz = temp;

        temp = buf[14] << 8 | buf[15];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        frame.Mx = temp;

        temp = buf[16] << 8 | buf[17];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        frame.My = temp;

        temp = buf[18] << 8 | buf[19];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        frame.Mz = temp;

        frame.crc = buf[20];

        crc_sum  = 0;
        for(i=0; i<20; i++) crc_sum += buf[i];
        frame.crc_v = crc_sum % 256;

        if( frame.crc == frame.crc_v )
            frame.correct = 1;
        else
            frame.correct = 0;

        frame_sen = frame;
    }

    if( frame.correct ) return 0;
    else return -1;
}

int AHRS_Mini::frame_ready(void)
{
    return frame_aviable;
}

AHRS_Frame AHRS_Mini::frame_get(AHRS_Frame::FrameType ft)
{
    frame_aviable = 0;
    if( ft == AHRS_Frame::FRAME_AHRS )
        return frame_imu;
    else
        return frame_sen;
}

} // end of namespace pi
