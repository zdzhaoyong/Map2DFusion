#ifndef GPS_H
#define GPS_H

#include "POS_reader.h"
#include "PathTable.h"
#include "hardware/UART/UART.h"

namespace pi {


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The GPS class
///
class GPS : public POS_DataManager, public POS_Reader
{
public:
    /** When  means a file,then we treat it as a data manager,otherwise we open a uart
    port_type: 0:real uart 1:file uart 2:data_manager
    */
    GPS(std::string name="GPS");

    bool hasTime(int64_t tm){return (tm<=tsMax&&tm>=tsMin);}
    virtual ~GPS();

    virtual void addFrame(POS_Data& frame);
    bool    loadFromTxt(const std::string& filename);

    FastPathTable  pathTable;
};



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief GPS reader
///
/// NOTE: must set m_uart
///
class GPS_Reader : public pi::RThread
{
public:
    GPS_Reader();
    virtual ~GPS_Reader();

    virtual int begin(void);
    virtual int end(void);

    virtual int thread_func(void *arg=NULL);

    int parseFrame_asciiData(uint8_t *buf, int len);

    int getData(POS_Data &pd);

public:
    int                     m_isRunning;
    UART                    *m_uart;

    RMutex                  m_mutex;
    std::deque<POS_Data>    m_gpsQueue;
};


} // end of namespace pi

#endif // GPS_H
