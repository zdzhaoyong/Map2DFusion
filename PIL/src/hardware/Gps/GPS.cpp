#include <fstream>
#include "base/utils/utils.h"
#include "base/Svar/Svar_Inc.h"
#include <base/system/file_path/file_path.h>

#include "GPS.h"

using namespace std;

namespace pi {

/**
    .BaseDate  - if input date is not complete, then given the base date and time
    .port_type:
            0:real uart
            1:file uart
            2:data_manager saved file
            3:NULL
    .port: UART port or file name, default: /dev/ttyUSB0
    .port_speed: baud rate, default 115200
    .date_type: binary data or text file (only for .port_type == 1)

*/

GPS::GPS(string name)
    :pathTable(0.05)
{
    // FIXME: this may make the timestamp error
    int64_t dt_time = svar.get_var<int64_t>(name+".BaseData",0);
    if( dt_time ) {
        DateTime dt;
        dt.fromTimeStamp(dt_time);
        setBaseDate(dt);
    }

    string port = svar.GetString(name+".port", "/dev/ttyUSB0");
    string ext  =pi::path_extname(port);

    if(ext==".txt")
    {
        loadFromTxt(port);
    }
    else
    {
        portType = svar.GetInt(name+".port_type", 3);
        if(portType == 2) //treat as data_manager,no thread will be run
        {
            load(port.c_str());
            for(int i=0;i<POS_DataManager::length();i++)
            {
                POS_Data pt=POS_DataManager::at(i);
                if(pt.correct)
                {
                    pathTable.Add(pt.time.toTimeStampF(),pi::Point3d(pt.x,pt.y,pt.z));
                }
            }
            return;
        }

        //treat as uart
        if( portType == 0 )
            uart = new UART;
        else if( portType == 1 )
            uart =new VirtualUART;

        if( uart ) {
            uart->port_name = port;
            uart->baud_rate = svar.GetInt(name+".port_speed", 115200);
        }

        string  fn_base = svar.GetString("fn_autosave", "");
        m_fnAutoSave = auto_filename(fn_base);

        dataType = svar.GetInt(name+".data_type", 0);
        if(dataType == 0 )   m_fnAutoSave = m_fnAutoSave + ".bin";
        else                 m_fnAutoSave = m_fnAutoSave + ".txt";

        // begin POS recving thread
        if( uart )
            begin();
    }
}

GPS::~GPS()
{
//    uart->close();
}

void GPS::addFrame(POS_Data& frame)
{
    processData(frame);
    frameQueue.push_back(frame);
    pathTable.Add(frame.time.toTimeStampF(), pi::Point3d(frame.x,frame.y,frame.z));
}

bool  GPS::loadFromTxt(const string& filename)
{
    ifstream ifs(filename.c_str());
    if(!ifs.is_open()) return false;
    string line;
    pi::Point3d pos;
    double      timestamp;

    POS_Data    posData;
    while(getline(ifs,line))
    {
        if(!line.size()) break;
        stringstream sst(line);
        sst>>timestamp>>pos;
        posData.time.fromTimeStampF(timestamp);
        posData.lng=pos.x;posData.lat=pos.y;posData.altitude=pos.z;
        posData.posAvaiable=1;
        posData.correct=1;
//        posData.print();
        addFrame(posData);
    }
//    cout<<"Loaded "<<size()<<" gps data.\n";
    return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

GPS_Reader::GPS_Reader()
{
    m_uart = NULL;
    m_isRunning = 0;
}

GPS_Reader::~GPS_Reader()
{
    // stop thread
    if( m_isRunning ) {
        setAlive(0);
        wait(10);
        kill();
    }

    m_uart = NULL;
}

int GPS_Reader::begin(void)
{
    if( m_uart == NULL ) {
        dbg_pe("UART object not set!");
        return -1;
    }

    // start receiving thread
    start();

    m_isRunning = 1;

    return 0;
}

int GPS_Reader::end(void)
{
    if( m_isRunning ) {
        setAlive(0);
        wait(10);
        kill();
    }

    m_isRunning = 0;

    return 0;
}

int GPS_Reader::thread_func(void *arg)
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
            j = m_uart->read(&ub, 1);
            if( j < 0 ) return -1;

            tm_sleep(1);
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
}

int GPS_Reader::parseFrame_asciiData(uint8_t *buf, int len)
{
    char            header[32];
    char            msg[512];
    int             i, f;

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

        RMutex m(&m_mutex);
        m_gpsQueue.push_back(frame);
    }

    return 0;
}

int GPS_Reader::getData(POS_Data &pd)
{
    RMutex m(&m_mutex);

    int n = m_gpsQueue.size();
    if( n > 0 ) {
        if( n > 1 ) {
            for(int i=0; i<n-1; i++) m_gpsQueue.pop_front();
        }

        pd = m_gpsQueue.front();
        m_gpsQueue.pop_front();

        return 0;
    }

    return -1;
}


} // end of namespace pi
