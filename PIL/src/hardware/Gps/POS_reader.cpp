#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base/system/format/format.h"
#include "base/utils/utils.h"
#include "base/debug/debug_config.h"
#include "base/time/Time.h"
#include "utils_GPS.h"
#include "POS_reader.h"

using namespace std;


namespace pi {


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int hex2dec(char c)
{
    if( c >= '0' && c <= '9' ) return c-'0';
    if( c >= 'a' && c <= 'f' ) return c-'a'+10;
    if( c >= 'A' && c <= 'F' ) return c-'A'+10;

    return 0;
}

int xstr2int(const char *str)
{
    int     hex;
    int     i;

    hex = 0;
    i = 0;

    while( str[i] != 0 ) {
        hex = hex*16 + hex2dec(str[i]);

        i++;
    }

    return hex;
}

///
/// \brief check Frame CRC
/// \param buf - data frame
/// \param len - length
/// \return 0 - correct, 1 - wrong
///
int checkFrameCRC(const ru8 *buf, int len)
{
    int crc_sum, crc_v, crc_f;
    int i;

    crc_sum = 0;
    for(i=0; i<len-1; i++) {
        crc_sum += buf[i];
    }
    crc_v = crc_sum % 256;

    crc_f = buf[len-1];

    if( crc_v == crc_f )
        return 1;
    else
        return 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void AHRS_Data::print(void)
{
    fmt::print_colored(fmt::BLUE, "IMU:   ");
    fmt::printf("yaw = %12f, pitch = %12f, roll  = %12f\n", yaw, pitch, roll);
    fmt::printf("       alt = %12f, temp  = %12f, press = %12f\n", alt, temp, press);
    fmt::printf("       imu_ps = %5d, crc = 0x%02x (%d), tm = %lld\n", imu_ps, crc_a, correct, tm_a);

    /*
    fmt::print_colored(fmt::BLUE, "SEN:   ");
    printf("Ax = %6d, Ay = %6d, Az = %6d\n", Ax, Ay, Az);
    printf("       Gx = %6d, Gy = %6d, Gz = %6d\n", Gx, Gy, Gz);
    printf("       Mx = %6d, My = %6d, Mz = %6d\n", Mx, My, Mz);
    printf("       crc = 0x%02x (%d), tm = %lld\n\n", crc_s, correct, tm_s);
    */
}

void AHRS_Data::clear(void)
{
    yaw     = 0;
    pitch   = 0;
    roll    = 0;
    alt     = 0;
    temp    = 0;
    press   = 0;
    imu_ps  = 0;
    crc_a   = 0;
    crcv_a  = 0;
    tm_a    = 0;

    Ax      = 0;
    Ay      = 0;
    Az      = 0;
    Gx      = 0;
    Gy      = 0;
    Gz      = 0;
    Mx      = 0;
    My      = 0;
    Mz      = 0;
    crc_s   = 0;
    crcv_s  = 0;
    tm_s    = 0;

    tm      = 0;
    correct = 0;
}

int AHRS_Data::parse(uint8_t *buf)
{
    int             frame_len;
    AHRS_DataType   frame_type;
    int16_t         temp;
    int             i;
    int             crc_sum;

    // frame length (in byte)
    frame_len = buf[0];

    // frame type
    if( buf[1] == 0xA1 )
        frame_type = AHRS_DATA_AHRS;
    else if( buf[1] == 0xA2 )
        frame_type = AHRS_DATA_SENSOR;

    // AHRS data
    if( frame_type == AHRS_DATA_AHRS ) {
        temp = buf[2] << 8 | buf[3];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        this->yaw = 0.1*temp;

        temp = buf[4] << 8 | buf[5];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        this->pitch = 0.1*temp;

        temp = buf[6] << 8 | buf[7];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        this->roll = 0.1*temp;

        temp = buf[8] << 8 | buf[9];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        this->alt = 0.1*temp;

        temp = buf[10] << 8 | buf[11];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        this->temp = 0.1*temp;

        temp = buf[12] << 8 | buf[13];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        this->press = 10.0*temp;

        temp = buf[14] << 8 | buf[15];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        this->imu_ps = temp;

        this->crc_a = buf[16];

        crc_sum  = 0;
        for(i=0; i<16; i++) crc_sum += buf[i];
        this->crcv_a = crc_sum % 256;

        if( this->crc_a == this->crcv_a )
            this->correct = 1;
        else
            this->correct = 0;
    } else if ( frame_type == AHRS_DATA_SENSOR ) {
        temp = buf[2] << 8 | buf[3];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        this->Ax = temp;

        temp = buf[4] << 8 | buf[5];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        this->Ay = temp;

        temp = buf[6] << 8 | buf[7];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        this->Az = temp;

        temp = buf[8] << 8 | buf[9];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        this->Gx = temp;

        temp = buf[10] << 8 | buf[11];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        this->Gy = temp;

        temp = buf[12] << 8 | buf[13];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        this->Gz = temp;

        temp = buf[14] << 8 | buf[15];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        this->Mx = temp;

        temp = buf[16] << 8 | buf[17];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        this->My = temp;

        temp = buf[18] << 8 | buf[19];
        if( temp & 0x8000 ) temp = 0 - (temp&0x7fff);
        else temp = temp & 0x7fff;
        this->Mz = temp;

        this->crc_s = buf[20];

        crc_sum  = 0;
        for(i=0; i<20; i++) crc_sum += buf[i];
        this->crcv_s = crc_sum % 256;

        if( this->correct == 1 && (this->crc_s == this->crcv_s) )
            this->correct = 1;
        else
            this->correct = 0;
    }

    if( this->correct ) return 0;
    else return -1;
}

int AHRS_Data::toStream(RDataStream &ds)
{
    // clear datastream
    ds.clear();

    // set magic number & version number
    ds.setMagicNum(0xEA92);
    ds.setVerNum(0x0001);

    // write AHRS data
    ds << yaw << pitch << roll << alt << temp << press << imu_ps
       << crc_a << crcv_a << tm_a;

    // write sensor data
    ds << Ax << Ay << Az << Gx << Gy << Gz << Mx << My << Mz
       << crc_s << crcv_s;

    // write time and flag
    ds << tm << correct;

    return 0;
}


int AHRS_Data::fromStream(RDataStream &ds)
{
    ru32    d_magic, d_ver;

    // rewind to begining
    ds.rewind();

    // get magic & version number
    ds.getMagicNum(d_magic);
    ds.getVerNum(d_ver);

    if( d_magic != 0xEA92 ) {
        dbg_pe("Input data magic number error! %x\n", d_magic);
        return -1;
    }

    // read AHRS data
    if( 0 != ds.read(yaw) )         return -2;
    if( 0 != ds.read(pitch) )       return -2;
    if( 0 != ds.read(roll) )        return -2;
    if( 0 != ds.read(alt) )         return -2;
    if( 0 != ds.read(temp) )        return -2;
    if( 0 != ds.read(press) )       return -2;
    if( 0 != ds.read(imu_ps) )      return -2;
    if( 0 != ds.read(crc_a) )       return -2;
    if( 0 != ds.read(crcv_a) )      return -2;
    if( 0 != ds.read(tm_a) )        return -2;

    // read sensor data
    if( 0 != ds.read(Ax) )          return -2;
    if( 0 != ds.read(Ay) )          return -2;
    if( 0 != ds.read(Az) )          return -2;
    if( 0 != ds.read(Gx) )          return -2;
    if( 0 != ds.read(Gy) )          return -2;
    if( 0 != ds.read(Gz) )          return -2;
    if( 0 != ds.read(Mx) )          return -2;
    if( 0 != ds.read(My) )          return -2;
    if( 0 != ds.read(Mz) )          return -2;
    if( 0 != ds.read(crc_s) )       return -2;
    if( 0 != ds.read(crcv_s) )      return -2;

    // read time and flag
    if( 0 != ds.read(tm) )          return -2;
    if( 0 != ds.read(correct) )     return -2;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


///
/// \brief parse input buffer for $GPGGA data
///
/// \see http://aprs.gids.nl/nmea
/// \see http://www.gpsinformation.org/dale/nmea.htm
///
int POS_Data::fromGPGGA(const char *buf)
{
    int ret = 0;
    int i = 0;
    unsigned char checksum = 0, checksum_i;
    int l;
    int i1, i2, i3;
    double f1, f2;

    l = strlen(buf);

    // calc checksum
    for (i=1; i < l - 3; i++) {
        if (buf[i] != ',')
            checksum ^= buf[i];
    }

    char sCRC[4];
    sCRC[0] = buf[l - 2];
    sCRC[1] = buf[l - 1];
    sCRC[2] = 0;
    checksum_i = xstr2int(sCRC);

    if( checksum_i == checksum ) {
        correct = 1;
    } else {
        correct = 0;
        ret = -1;

        dbg_pw("Input GPS $GPGGA message is wrong!\n");

        clear();
        return ret;
    }

    // parse $GPGGA frame
    StringArray sa;

    sa = split_text(buf, ",");

    // time
    int     tm_h, tm_m, tm_s;
    ri32    tm_ns;

    if( sa[1].size() == 9 ) {
        i1 = sa[1][0] - '0';
        i2 = sa[1][1] - '0';

        tm_h = i1*10 + i2;

        i1 = sa[1][2] - '0';
        i2 = sa[1][3] - '0';
        tm_m = i1*10 + i2;

        i1 = sa[1][4] - '0';
        i2 = sa[1][5] - '0';
        tm_s = i1*10 + i2;

        i1 = sa[1][7] - '0';
        i2 = sa[1][8] - '0';
        tm_ns = (i1*10 + i2)*10000000;

        time.setTime(tm_h, tm_m, tm_s, tm_ns);
    } else {
        tm_h = 0;
        tm_m = 0;
        tm_s = 0;
        tm_ns = 0;
        time.setTime(tm_h, tm_m, tm_s, tm_ns);

        ret = -1;
    }


    // latitude
    if( sa[2].size() > 1 ) {
        i1 = sa[2][0] - '0';
        i2 = sa[2][1] - '0';
        f1 = i1*10 + i2;
        f2 = atof(sa[2].c_str());
        lat = f1 + (f2-f1*100)/60.0;
    } else {
        lat = 0;

        ret = -1;
    }

    if( sa[3].size() > 0 ) {
        latFlag = sa[3][0];
        if( latFlag != 'N' ) lat = -lat;
    } else {
        latFlag = 0;

        ret = -1;
    }

    // longitude
    if( sa[4].size() > 2 ) {
        i1 = sa[4][0] - '0';
        i2 = sa[4][1] - '0';
        i3 = sa[4][2] - '0';
        f1 = i1*100 + i2*10 + i3;
        f2 = atof(sa[4].c_str());
        lng = f1 + (f2-f1*100)/60.0;
    } else {
        lng = 0;

        ret = -1;
    }

    if( sa[5].size() > 0 ) {
        lngFlag = sa[5][0];
        if( lngFlag != 'E' ) lng = -lng;
    } else {
        lngFlag = 0;

        ret = -1;
    }

    // Fix quality
    if( sa[6].size() > 0 ) {
        fixQuality = atoi(sa[6].c_str());
    } else {
        fixQuality = 1;

        ret = -1;
    }

    // Number of satellites being tracked
    if( sa[7].size() > 1 ) {
        i1 = sa[7][0] - '0';
        i2 = sa[7][1] - '0';
        nSat = (i1*10 + i2);
    } else {
        nSat = 0;

        ret = -1;
    }

    // Horizontal Dilution of Precision (HDOP)
    //  Relative accuracy of horizontal position
    if( sa[8].size() > 0 ) {
        HDOP = atof(sa[8].c_str());
    } else {
        HDOP = 10;

        ret = -1;
    }

    // Altitude
    if( sa[9].size() > 0 ) {
        altitude = atof(sa[9].c_str());
    } else {
        altitude = 0;

        ret = -1;
    }

    // Height of geoid above WGS84 ellipsoid
    if( sa[11].size() > 0 ) {
        geoid_h = atof(sa[11].c_str());
    } else {
        geoid_h = 0;

        ret = -1;
    }

    if( ret != 0 || nSat < 3 ) {
        posAvaiable = 0;
    } else {
        posAvaiable = 1;
    }

    return ret;
}

int POS_Data::fromBatV(const ru8 *buf, int len)
{
    int     v0;

    if( checkFrameCRC(buf, len) != 1 ) {
        batV = -1;
    } else {
        v0 = buf[2];
        batV = 1.0*v0/10.0;
    }

    return 0;
}

void POS_Data::print(void)
{
    // GPS data
    fmt::print_colored(fmt::BLUE, "GPS:   "); time.print(); printf("\n");
    printf("       Lat: %12f %c, Lng: %12f %c, Alt: %12f, geoid_h: %12f\n",
           lat, latFlag, lng, lngFlag, altitude, geoid_h);
    printf("       HDOP: %12f, nSat: %d, fixQuality: %d, correct: %d, posAvaiable: %d\n",
           HDOP, nSat, fixQuality, correct, posAvaiable);

    // Battery voltage
    fmt::print_colored(fmt::BLUE, "BAT_V: "); printf("%f\n", batV);

    // AHRS data
    ahrs.print();
}

int POS_Data::read(FILE *fp)
{
    return 0;
}

int POS_Data::write(FILE *fp)
{
    return 0;
}


int POS_Data::toStream(RDataStream &ds)
{
    RDataStream  ds_time, ds_ahrs;

    // clear datastream
    ds.clear();

    // set magic number & version number
    ds.setMagicNum(0x7AC3);
    ds.setVerNum(0x0001);

    ds.write((ru8*) this, sizeof(POS_Data));

    return 0;

    // serialize time & AHRS data
    time.toStream(ds_time);
    ahrs.toStream(ds_ahrs);

    // store all data to stream
    ds << ds_time;
    ds << lat << latFlag << lng << lngFlag;
    ds << altitude << geoid_h << h;
    ds << HDOP << nSat << fixQuality;
    ds << ds_ahrs;
    ds << x << y << z;
    ds << vx << vy << vz;
    ds << batV;
    ds << posAvaiable << correct;

    return 0;
}


int POS_Data::fromStream(RDataStream &ds)
{
    ru32        d_magic, d_ver;
    RDataStream  ds_time, ds_ahrs;

    // rewind to begining
    ds.rewind();

    // get magic & version number
    ds.getMagicNum(d_magic);
    ds.getVerNum(d_ver);

    if( d_magic != 0x7AC3 ) {
        dbg_pe("Input data magic number error! %x\n", d_magic);
        return -1;
    }

    ds.read((ru8*) this, sizeof(POS_Data));
    return 0;

    // read data
    if( 0 != ds.read(ds_time) )     return -2;
    if( 0 != ds.read(lat) )         return -2;
    if( 0 != ds.read(latFlag) )     return -2;
    if( 0 != ds.read(lng) )         return -2;
    if( 0 != ds.read(lngFlag) )     return -2;
    if( 0 != ds.read(altitude) )    return -2;
    if( 0 != ds.read(geoid_h) )     return -2;
    if( 0 != ds.read(h) )           return -2;

    if( 0 != ds.read(HDOP) )        return -2;
    if( 0 != ds.read(nSat) )        return -2;
    if( 0 != ds.read(fixQuality) )  return -2;

    if( 0 != ds.read(ds_ahrs) )     return -2;

    if( 0 != ds.read(x) )          return -2;
    if( 0 != ds.read(y) )          return -2;
    if( 0 != ds.read(z) )          return -2;
    if( 0 != ds.read(vx) )          return -2;
    if( 0 != ds.read(vy) )          return -2;
    if( 0 != ds.read(vz) )          return -2;

    if( 0 != ds.read(batV) )        return -2;

    if( 0 != ds.read(posAvaiable) ) return -2;
    if( 0 != ds.read(correct) )     return -2;

    if( 0 != time.fromStream(ds_time) ) return -3;
    if( 0 != ahrs.fromStream(ds_ahrs) ) return -3;

    return 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


int POS_DataManager::open(const char *fname)
{
    return 0;
}

int POS_DataManager::close(void)
{
    return 0;
}

int POS_DataManager::load(void)
{
    return 0;
}

int POS_DataManager::save(void)
{
    return 0;
}


int POS_DataManager::load(const char *fname)
{
    FILE        *fp = NULL;
    ru32        dl;

    cout<<"POS_DataManager::Loading GPS from "<<fname<<endl;

    // open file
    fp = fopen(fname, "rb");
    if( fp == NULL ) {
        dbg_pe("can not open file: %s\n", fname);
        return -1;
    }

    m_mutex->lock();

    fread(&dl, sizeof(ru32), 1, fp);

    m_data.resize(dl);
    m_tsArray.resize(dl);
    fread(m_data.data(), sizeof(POS_Data), dl, fp);

    for(int i=0; i<dl; i++) {
        m_tsArray[i] = m_data[i].time.toTimeStamp();
    }

    cout<<"GPS data loaded.\n";

    m_mutex->unlock();

    // get beginning & ending timestamp
    cout<<"Loaded "<<m_tsArray.size()<<" gps_data.\n";

    if(m_tsArray.size()){
        tsMin = m_tsArray.front();
        tsMax = m_tsArray.back();
        return 0;
    } else {
        return -2;
    }
}

int POS_DataManager::save(const char *fname)
{
    FILE        *fp = NULL;
    int         n, i;

    RDataStream  ds;
    ru32        dl;

    // open file
    fp = fopen(fname, "wb");
    if( fp == NULL ) {
        dbg_pe("can not open file: %s\n", fname);
        return -1;
    }

    // for each data
    m_mutex->lock();

    dl = m_data.size();

    fwrite(&dl,             sizeof(ru32),     1,  fp);
    fwrite(m_data.data(),   sizeof(POS_Data), dl, fp);

    m_mutex->unlock();

    // close file
    fclose(fp);

    return 0;
}

POS_Data& POS_DataManager::addData(POS_Data &d)
{
    processData(d);

    return d;
}

pi::Point3d POS_DataManager::XYZ2Pos(const pi::Point3d& xyz)
{
    if(!lng0)
    {
        POS_Data dt=at(0);
        lng0=dt.lng;
        lat0=dt.lat;
        z0=dt.altitude;
    }
    pi::Point3d result;
    calcLngLatFromDistance(lng0, lat0, xyz.x, xyz.y, result.x, result.y);
    result.z=xyz.z+z0;
//    cout<<"XYZ:"<<xyz<<",lng0:"<<lng0<<",lat0:"<<lat0<<",z0:"<<z0
//          <<",Result:"<<result<<endl;
    return result;
}

pi::Point3d POS_DataManager::Pos2XYZ(const pi::Point3d& pos)
{
    if(fabs(lng0)<0.0000001)
    {
        POS_Data dt=at(0);
        lng0=dt.lng;
        lat0=dt.lat;
        z0=dt.altitude;
        cerr<<"GPS ORG: ";
        dt.print();
    }
    pi::Point3d result;
    calcLngLatDistance(lng0, lat0, pos.x, pos.y, result.x, result.y);
    result.z=pos.z-z0;
    return result;
}

POS_Data& POS_DataManager::processData(POS_Data &d)
{
    ri64     ft_now = 0, ts_now;

    // lock mutex
    m_mutex->lock();

    // update frame date if necessary
    // FIXME: better way?
    if( m_bUseBaseTime ) {
        d.time.year  = m_baseDate.year;
        d.time.month = m_baseDate.month;
        d.time.day   = m_baseDate.day;
        //d.time.hour  +=8;
    }

    ts_now = d.time.toTimeStamp();

    // calculate & update x, y, z
    if( d.posAvaiable ) {
        int     nPos = 0;
        double  x, y, z;

        nPos = m_data.size();

        if( nPos == 0 ) {
            lat0 = d.lat;
            lng0 = d.lng;

            x = 0;
            y = 0;
            z0 = d.altitude;
            z1 = z0;
        } else {
            lat1 = d.lat;
            lng1 = d.lng;
            calcLngLatDistance(lng0, lat0, lng1, lat1,
                              x, y);

            z1 = d.altitude;
        }

        // calculate height from ground and correct x coordinate
        z   = z1 - z0;
        d.h = z;

        // copy coordinate to AHRS d
        d.x = x;
        d.y = y;
        d.z = z;

        // calculate flight time
        if( z > 10 ) {
            if( ft_beg == 0 )
                ft_beg = d.time.toTime_t();
            else {
                ft_now = d.time.toTime_t();
                d.ft_period = ft_now - ft_beg;
            }
        }

        // calculate yaw and velocites
        // FIXME: fix wrong AHRS data (disable it)
        if( 0 && z > 10 && nPos > 50 ) {
            int         calcPeriod = 5;
            POS_Data    lastPOS;
            double      lastX, lastY, lastZ;
            double      newYaw;

            // calculate yaw
            lastPOS = m_data[nPos-calcPeriod];

            lastX = lastPOS.x;
            lastY = lastPOS.y;
            lastZ = lastPOS.z;

            newYaw = atan2(y-lastY, x-lastX) - M_PI/2.0;
            if( newYaw < 0 ) newYaw += 2*M_PI;

            d.ahrs.yaw = newYaw*180.0/M_PI;

            // calculate velocites
            ri64        t0;
            double      dt;

            t0 = m_tsArray[nPos-calcPeriod];
            dt = 1.0*(ts_now - t0)/1000000.0;
            if(dt>600)//||dt<0
            {
                cout<<"Invaid detaT: "<<dt<<endl;
                d.correct=false;
            }
            d.vx = (x - lastX)/dt;
            d.vy = (y - lastY)/dt;
            d.vz = (z - lastZ)/dt;
        }
    }

    if( d.correct && d.posAvaiable ) {
        m_data.push_back(d);
        m_tsArray.push_back(ts_now);
        if( ts_now < tsMin ) tsMin = ts_now;
        if( ts_now > tsMax ) tsMax = ts_now;

        //dbg_pt("tsMin, tsMax = %f, %f", 1e-6*tsMin, 1e-6*tsMax);
    }

    POS_Data& ret_d = d; // m_data.back();

    m_mutex->unlock();

    return ret_d;
}

POS_Data& POS_DataManager::at(int idx)
{
    m_mutex->lock();

    // check length
    int n = m_data.size();
    if( idx >= n ) {
        m_mutex->unlock();
        dbg_pe("Index output range! %d\n", idx);

        POS_Data d;
        return d;
    }

    POS_Data &d = m_data.at(idx);

    m_mutex->unlock();

    return d;
}

POS_Data& POS_DataManager::getGPSData(int idx)
{
    return at(idx);
}

int POS_DataManager::getGPSData(ri64 ts, POS_Data &d)
{
    int         i, n;

    int         i1, i2;
    POS_Data    d1, d2;
    ri64        t1, t2;
    double      r1, r2;

    i = 0;

    // check time range
    if( ts < tsMin || ts > tsMax ) {
        DateTime time;
        time.fromTimeStamp(ts);
        dbg_pe("given timestamp is out of range! %lld", ts);
        std::cout<<time;

        time.fromTimeStamp(tsMin);
        std::cout<<"Range:"<<time<<"--->";
        time.fromTimeStamp( tsMax);
        std::cout<<"Range:"<<time<<"\n";


        return -1;
    }

    m_mutex->lock();

    n = m_tsArray.size();

    // find correct position
    i = 0;
    while( i < n && ts > m_tsArray.at(i) ) {
        i ++;
    }
    {
        i1 = i-1;
        i2 = i;

        // check if range is out of range
        if( i1<0 || i1 > n-1 || i2<0 || i2 > n-1 ) {
            dbg_pe("input time is out of range! ts = %lld\n", ts);
            m_mutex->unlock();
            return -2;
        }

        if(m_tsArray.at(i2)-m_tsArray.at(i1)>10000000)
        {

            cout<<"FrameTime gap too long..."<<i<<endl;
            m_data.at(i1).print();
            m_data.at(i2).print();

        }
        // calculate ratio
        t1 = m_tsArray.at(i1);
        t2 = m_tsArray.at(i2);
        d1 = m_data.at(i1);
        d2 = m_data.at(i2);

        m_mutex->unlock();

        r1 = 1.0*(t2-ts)/(t2-t1);
        r2 = 1.0 - r1;

        if( r1 < 0.0 || r1 > 1.0 || r2 < 0.0 || r2 > 1.0 ) {
            dbg_pe("input time is out of range! ts = %lld\n", ts);
            m_mutex->unlock();
            return -2;
        }

        d = d1;

        // interpolate data
        d.time.fromTimeStamp(ts);

        d.lat           = r1*d1.lat         + r2*d2.lat;
        d.lng           = r1*d1.lng         + r2*d2.lng;
        d.altitude      = r1*d1.altitude    + r2*d2.altitude;
        d.geoid_h       = r1*d1.geoid_h     + r2*d2.geoid_h;
        d.h             = r1*d1.h           + r2*d2.h;
        d.HDOP          = r1*d1.HDOP        + r2*d2.HDOP;
        d.nSat          = (d1.nSat + d2.nSat)/2;
        d.fixQuality    = d1.fixQuality;
        d.x            = r1*d1.x          + r2*d2.x;
        d.y            = r1*d1.y          + r2*d2.y;
        d.z            = r1*d1.z          + r2*d2.z;
        d.vx            = r1*d1.vx          + r2*d2.vx;
        d.vy            = r1*d1.vy          + r2*d2.vy;
        d.vz            = r1*d1.vz          + r2*d2.vz;
        d.batV          = r1*d1.batV        + r2*d2.batV;

        d.ahrs.yaw      = r1*d1.ahrs.yaw    + r2*d2.ahrs.yaw;
        d.ahrs.pitch    = r1*d1.ahrs.pitch  + r2*d2.ahrs.pitch;
        d.ahrs.roll     = r1*d1.ahrs.roll   + r2*d2.ahrs.roll;
        d.ahrs.alt      = r1*d1.ahrs.alt    + r2*d2.ahrs.alt;
        d.ahrs.temp     = r1*d1.ahrs.temp   + r2*d2.ahrs.temp;
        d.ahrs.press    = r1*d1.ahrs.press  + r2*d2.ahrs.press;

        d.ahrs.Ax       = r1*d1.ahrs.Ax     + r2*d2.ahrs.Ax;
        d.ahrs.Ay       = r1*d1.ahrs.Ay     + r2*d2.ahrs.Ay;
        d.ahrs.Az       = r1*d1.ahrs.Az     + r2*d2.ahrs.Az;
        d.ahrs.Gx       = r1*d1.ahrs.Gx     + r2*d2.ahrs.Gx;
        d.ahrs.Gy       = r1*d1.ahrs.Gy     + r2*d2.ahrs.Gy;
        d.ahrs.Gz       = r1*d1.ahrs.Gz     + r2*d2.ahrs.Gz;
        d.ahrs.Mx       = r1*d1.ahrs.Mx     + r2*d2.ahrs.Mx;
        d.ahrs.My       = r1*d1.ahrs.My     + r2*d2.ahrs.My;
        d.ahrs.Mz       = r1*d1.ahrs.Mz     + r2*d2.ahrs.Mz;
    }

    return 0;
}

int POS_DataManager::getGPSData(DateTime &t, POS_Data &d)
{
    ri64 ts = t.toTimeStamp();
    return getGPSData(ts, d);
}


POS_Data& POS_DataManager::getGPSData(DateTime &t)
{
    POS_Data    d;
    ri64        ts = t.toTimeStamp();

    getGPSData(ts, d);
    return d;
}

POS_Data& POS_DataManager::front(void)
{
    m_mutex->lock();
    POS_Data& d = m_data.front();
    m_mutex->unlock();

    return d;
}

POS_Data& POS_DataManager::back(void)
{
    m_mutex->lock();
    POS_Data& d = m_data.back();
    m_mutex->unlock();

    return d;
}

int POS_DataManager::clear(void)
{
    m_mutex->lock();
    m_data.clear();
    m_tsArray.clear();
    m_mutex->unlock();

    ft_beg = 0;

    return 0;
}

int POS_DataManager::size(void)
{
    int s;

    m_mutex->lock();
    s = m_data.size();
    m_mutex->unlock();

    return s;
}

int POS_DataManager::length(void)
{
    int s;

    m_mutex->lock();
    s = m_data.size();
    m_mutex->unlock();

    return s;
}

POS_Data& POS_DataManager::operator [](int idx)
{
    return at(idx);
}

void POS_DataManager::init(void)
{
    m_mutex = new RMutex;

    m_baseDate.setCurrentDateTime(DateTime::DATETIME_UTC);
    m_data.clear();
    m_tsArray.clear();

    ft_beg = 0;

    m_bUseBaseTime = 0;

    DateTime tmin = m_baseDate;
    tmin.year += 40;
    tsMin = tmin.toTimeStamp();
    tsMax = 0;
}

void POS_DataManager::release(void)
{
    ft_beg = 0;

    m_data.clear();
    m_tsArray.clear();

    m_bUseBaseTime = 0;
    m_baseDate.clear();

    delete m_mutex;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

POS_Reader::POS_Reader()
{
    uart = NULL;

    dataType = 0;               // binary data type or text data type

    portType =0;
    m_fpAutoSave = NULL;

    mutex = new RMutex();
}

POS_Reader::~POS_Reader()
{
    stop();

    if( mutex != NULL ) delete mutex;
}

void POS_Reader::setAutosaveFile(const std::string &fname)
{
    m_fnAutoSave = fname;
}


std::string POS_Reader::getAutosaveFile(void)
{
    return m_fnAutoSave;
}


int POS_Reader::begin(void)
{
    // open UART port
    if( 0 != uart->open() ) {
        dbg_pe("can not open port: %s (%d)\n",
               uart->port_name.c_str(), uart->port_no);
        return -1;
    }

    // open auto-save file
    if( m_fnAutoSave.size() > 0 && portType==0 ) {
        m_fpAutoSave = fopen(m_fnAutoSave.c_str(), "wt");
        if( m_fpAutoSave == NULL ) {
            dbg_pw("can not open auto-save file: %s\n", m_fnAutoSave.c_str());
        }
    }

    // start receiving thread
    if( 0 != start() ) {
        dbg_pe("can not create thread!\n");
        setAlive(0);
        return -2;
    }

    return 0;
}

int POS_Reader::stop(void)
{
    int ret;

    if( getAlive() != 0 ) {
        setAlive(0);

        ret = wait(100);
        if( ret != 0 ) kill();

        if( m_fpAutoSave != NULL ) {
            fclose(m_fpAutoSave);
            m_fpAutoSave = NULL;
        }
    }

    return 0;
}

int POS_Reader::frame_ready(void)
{
    int ret = 0;

    mutex->lock();
    if( frameQueue.size() > 0 ) ret = 1;
    mutex->unlock();

    return ret;
}

int POS_Reader::frame_num(void)
{
    return frameQueue.size();
}

int POS_Reader::frame_clear(void)
{
    mutex->lock();
    frameQueue.clear();
    mutex->unlock();

    return 0;
}

POS_Data POS_Reader::frame_get(void)
{
    POS_Data  f;

    if( frameQueue.size() > 0 ) {
        mutex->lock();
        f = frameQueue.front();
        frameQueue.pop_front();
        mutex->unlock();
    } else
        f.clear();

    return f;
}

int POS_Reader::frame_get(POS_Data &f)
{
    if( frameQueue.size() > 0 ) {
        mutex->lock();
        f = frameQueue.front();
        frameQueue.pop_front();
        mutex->unlock();

        return 0;
    } else {
        f.clear();
        return 1;
    }
}

int POS_Reader::frame_get_last(POS_Data &f)
{
    if( frameQueue.size() > 0 ) {
        mutex->lock();
        f = frameQueue.back();
        frameQueue.clear();
        mutex->unlock();

        return 0;
    } else {
        f.clear();
        return 1;
    }
}


int POS_Reader::thread_func(void *arg)
{
    if( dataType == 0 ) return thread_fun_binData(arg);
    else                return thread_fun_asciiData(arg);
}

int POS_Reader::thread_fun_binData(void *arg)
{
    uint8_t         buf[512];
    uint8_t         ub;

    int             sta;
    int             frame_len;
    int             ir;

    int             j;

    sta = 0;
    while( getAlive() ) {
        // read one byte
        j = 0;
        while ( j<1 ) {
            j = uart->read(&ub, 1);
            if( j < 0 ) return -1;
        }

        // detect frame
        if( sta == 0 ) {
            if( ub == 0xA5 ) sta = 1;
        } else if ( sta == 1 ) {
            if( ub == 0x5A ) {
                sta = 2;
                ir = 0;
            } else
                sta = 0;
        } else if ( sta == 2 ) {
            buf[ir++] = ub;
            if( ir == 1 ) frame_len = buf[0];

            if( ir >= frame_len ) {
                sta = 0;
                parseFrame_binData(buf, frame_len);
            }
        }
    }

    return 0;
}

int POS_Reader::thread_fun_asciiData(void *arg)
{
    uint8_t         buf[512];
    uint8_t         ub;

    int             sta;
    int             frame_len;
    int             ir;

    int             j;

    sta = 0;
    while( getAlive() ) {
        // read one byte
        j = 0;
        while ( j<1 ) {
            j = uart->read(&ub, 1);
            if( j < 0 ) return -1;
        }

        // detect frame
        if( sta == 0 ) {
            // detect frame begin
            if( ub == '$' ) {
                sta = 1;

                ir = 0;
                buf[ir++] = ub;
            }
        } else if ( sta == 1 ) {
            // detect frame end
            if( ub == 0x0A || ub == 0x0D ) {
                sta = 0;

                buf[ir] = 0;
                frame_len = ir;
                parseFrame_asciiData(buf, frame_len);
            } else {
                buf[ir++] = ub;
            }
        }
    }

    return 0;
}

int POS_Reader::parseFrame_binData(ru8 *buf, int len)
{
    static POS_Data frame;
    ru8             header[3] = {0xA5, 0x5A, 0x00};
    char            msg[512];
    int             i, crc;

    char            GPS_header[32];
    int             f;

    static ri64     t0 = 0, t1 = 0, t2 = 0;

    // autosave to file
    if( m_fpAutoSave != NULL ) {
        fwrite(header, 1, 2, m_fpAutoSave);
        fwrite(buf, 1, len, m_fpAutoSave);
        fflush(m_fpAutoSave);
    }

    // check frame CRC
    crc = checkFrameCRC(buf, len);
    if( crc != 1 ) {
        dbg_pw("Input frame error! (%x)\n", buf[1]);
    }

    if( buf[1] == 0xA1 ) {
        t0 = tm_get_millis();

        // AHRS data
        frame.ahrs.parse(buf);

        // check each message's delta-time
        if( t0-t1 > 20 ) {
            dbg_pw("Time between frames is too long, discard this frame! %lld, %lld - %lld\n",
                   t0, t0-t1, t0-t2);
            return -1;
        }

        // push to frame queue
        mutex->lock();
        addFrame(frame);
        mutex->unlock();

//        frame.print();
    } else if ( buf[1] == 0xB0 ) {
        // GPS message
        for(i=0; i<len-3; i++) msg[i] = buf[2+i];
        msg[len-3] = 0;

        // get frame header
        f = 0;
        for(i=0; i<32; i++) GPS_header[i] = 0;

        for(i=0; i<30; i++) {
            if( msg[i] == ',' ) {
                GPS_header[i] = 0;
                f = 1;
                break;
            } else {
                GPS_header[i] = msg[i];
            }
        }

//        fmt::print_colored(fmt::BLUE, "\nGPS data: ");
//        fmt::printf("%s\n", msg);

        // $GPGGA data
        if( f && strcmp(GPS_header, "$GPGGA") == 0 ) {
            t1 = tm_get_millis();
            frame.time.setCurrentDateTime(DateTime::DATETIME_UTC);
            frame.fromGPGGA(msg);
        }
    } else if ( buf[1] == 0xC0 ) {
        frame.fromBatV(buf, len);
    }

    return 0;
}

int POS_Reader::parseFrame_asciiData(ru8 *buf, int len)
{
    char            header[32];
    char            msg[512];
    int             i, f;

    // autosave to file
    if( m_fpAutoSave != NULL ) {
        sprintf(msg, "%s\n", buf);
        fwrite(msg, 1, strlen(msg), m_fpAutoSave);
        fflush(m_fpAutoSave);
    }

    // get frame header
    f = 0;
    for(i=0; i<30; i++) {
        if( buf[i] != ',' ) {
            header[i] = buf[i];
        } else {
            header[i] = 0;
            f = 1;
            break;
        }
    }

    if( !f ) return -1;

    // parse frame
    if( strcmp(header, "$GPGGA") == 0 ) {
        POS_Data frame;

        frame.time.setCurrentDate(DateTime::DATETIME_UTC);
        frame.fromGPGGA((const char*)buf);

        //frame.print();

        mutex->lock();
        addFrame(frame);
        mutex->unlock();
    }

    return 0;
}

void POS_Reader::addFrame(POS_Data& frame)
{
    frameQueue.push_back(frame);
}

} // end of namespace pi
