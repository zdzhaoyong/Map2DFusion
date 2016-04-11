#ifndef __PI_SERIALPORT_H__
#define __PI_SERIALPORT_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

#include <deque>


namespace pi {

///
/// \brief The UART class
///
class UART
{
public:
    enum UART_OPTIONS {
        UART_NONBLOCK   = 0x01,
        UART_BUFFER     = 0x02
    };

public:
    UART();
    virtual ~UART();

    virtual int open(const std::string &portName="", int baudRate=0);
    virtual int close(void);

    virtual int write(void *d, int len, int master=1);
    virtual int read(void *d,  int len, int master=1);

    virtual int setOption(UART_OPTIONS o)  { m_options = o; }
    virtual UART_OPTIONS getOption(void) { return m_options; }

public:
    std::string     port_name;          ///< port name
                                        ///<    for Linux/Unix      - /dev/ttyACM0
                                        ///<    for Windows         - COM1

    int             baud_rate;          ///< Baud rate 9600,38400,115200, and so on
    int             byte_size;          ///< the bits in one byte 7,8..
    int             parity_sw;          ///< FIXME: windows not used yet
    int             stop_bits;          ///< stop bits 1,15,2

    int             DTR_sw;             ///< DTR switch
    int             RTS_sw;             ///< RTS switch
    int             timeout_sw;         ///< time out function

protected:
    UART_OPTIONS    m_options;          ///< options
    int             m_bufSize;          ///< buffer size

    void            *data;              ///< inner used data

    virtual int _write(void *d, int len);
    virtual int _read(void *d,  int len);
};



} // end of namespace pi

#endif // __PI_SERIALPORT_H__
