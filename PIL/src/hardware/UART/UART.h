#ifndef __UTILS_UART_H__
#define __UTILS_UART_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

#include <deque>

#include <base/osa/osa++.h>

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

    virtual int open(void);
    virtual int close(void);

    virtual int write(void *d, int len, int master=1);
    virtual int read(void *d,  int len, int master=1);

    virtual int setOption(UART_OPTIONS o)  { m_options = o; }
    virtual UART_OPTIONS getOption(void) { return m_options; }

public:
    int             port_no;            ///< port number - for Windows
    std::string     port_name;          ///< port name   - for Linux/Unix

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


///
/// \brief The VirtualUART class
///
/// This class only just support (read from file/write to file) at the same time
///     throug file read/write. The open file name is specified by 'UART::port_name'
///
/// \see UART
///
class VirtualUART : public UART
{
public:
    enum VirtualUART_Type {
        VUART_DEV,                  ///< real uart device
        VUART_FILE,                 ///< file
        VUART_IPC,                  ///< inter-process
        VUART_NET                   ///< network (TCP)
    };

public:
    VirtualUART();
    virtual ~VirtualUART();

    virtual void setMode(VirtualUART_Type t) { m_uartType = t; }
    virtual VirtualUART_Type getMode(void) { return m_uartType; }

    virtual int open(void);
    virtual int close(void);

    virtual int write(void *d, int len, int master=1);
    virtual int read(void *d,  int len, int master=1);

protected:
    VirtualUART_Type    m_uartType;
};


} // end of namespace pi

#endif // __UTILS_UART_H__
