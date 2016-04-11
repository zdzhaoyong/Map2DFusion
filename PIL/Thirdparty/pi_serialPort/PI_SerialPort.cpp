#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>

#include <deque>

#include "PI_SerialPort.h"


#define dbg_pe printf
#define dbg_pw printf


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


#if defined(PIL_WINDOWS) || defined(PIL_MINGW)

#include <windows.h>

#define DEFAULT_READINTERVALTIMEOUT     50      /* Default read a char interval timeout in millseconds */
#define DEFAULT_READMULTIPLIER          50      /* Default read data timeout multiplier */
#define DEFAULT_READTOTALTIMEOUT        250     /* Default read total timeout */

#define DEFAULT_WRITEMULTIPLIER         50      /* Default write data timeout multiplier */
#define DEFAULT_WRITETOTALTIMEOUT       250     /* Default write total timeout */


namespace pi {

struct UART_inner_data
{
    HANDLE  m_hCom;                             /* Handle of COM Port */
    DCB     m_oDCB;                             /* COM Port Control Block */
};


UART::UART()
{
    UART_inner_data    *pd;

    // inner data
    pd = new UART_inner_data;
    pd->m_hCom = NULL;
    data = pd;

    // set default values
    port_name  = "COM1";
    baud_rate  = 115200;
    byte_size  = 8;
    parity_sw  = 1;
    stop_bits  = 1;

    timeout_sw = 0;
    DTR_sw     = 1;
    RTS_sw     = 1;
}


UART::~UART()
{
    UART_inner_data     *pd;
    int                 r;

    pd = (UART_inner_data *) data;
    if( pd->m_hCom != NULL ) {
        r = CloseHandle(pd->m_hCom);
        if( !r ) dbg_pe("UART::~UART Close port error!");
        pd->m_hCom = NULL;
    }

    delete pd;
    data = NULL;
}

int UART::open(const std::string &portName, int baudRate)
{
    UART_inner_data     *pd;

    COMMTIMEOUTS        oComTimeout;
    BOOL                fSuccess;

    int                 ret_code = 0;

    // handle structure
    pd = (UART_inner_data *) data;

    if( portName.size() ) port_name = portName;
    if( baudRate )        baud_rate = baudRate;

    // open port
    pd->m_hCom = CreateFile(
                            port_name.c_str(),                  // COM port Name
                            GENERIC_READ | GENERIC_WRITE,       // read/write flags
                            0,                                  // comm devices must be opened w/exclusive-access
                            NULL,                               // no security attributes
                            OPEN_EXISTING,                      // COM devices must use OPEN_EXISTING
                            0,                                  // no overlapped I/O
                            NULL                                // hTemplate must be NULL for COM devices
                        );

    if (pd->m_hCom == INVALID_HANDLE_VALUE) {
        // Handle the error.
        dbg_pe("UART::open CreateFile failed with wrror %d.", GetLastError());
        return 1;
    }

    // We will build on the current configuration, and skip setting the size
    //      of the input and output buffers with SetupComm.
    fSuccess = GetCommState(pd->m_hCom, &(pd->m_oDCB));
    if ( !fSuccess ) {
        dbg_pe("UART::open GetComState failed with error %d.", GetLastError());
        ret_code = 2;
        goto UART_OPEN_ERR;
    }

    // Fill in the DCB
    switch(baud_rate) {
        case 110:
            pd->m_oDCB.BaudRate = CBR_110;
            break;
        case 300:
            pd->m_oDCB.BaudRate = CBR_300;
            break;
        case 600:
            pd->m_oDCB.BaudRate = CBR_600;
            break;
        case 1200:
            pd->m_oDCB.BaudRate = CBR_1200;
            break;
        case 2400:
            pd->m_oDCB.BaudRate = CBR_2400;
            break;
        case 4800:
            pd->m_oDCB.BaudRate = CBR_4800;
            break;
        case 9600:
            pd->m_oDCB.BaudRate = CBR_9600;
            break;
        case 14400:
            pd->m_oDCB.BaudRate = CBR_14400;
            break;
        case 19200:
            pd->m_oDCB.BaudRate = CBR_19200;
            break;
        case 38400:
            pd->m_oDCB.BaudRate = CBR_38400;
            break;
        case 56000:
            pd->m_oDCB.BaudRate = CBR_56000;
            break;
        case 57600:
            pd->m_oDCB.BaudRate = CBR_57600;
            break;
        case 115200:
            pd->m_oDCB.BaudRate = CBR_115200;
            break;
        case 128000:
            pd->m_oDCB.BaudRate = CBR_128000;
            break;
        case 2560000:
            pd->m_oDCB.BaudRate = CBR_256000;
            break;
        default:
            dbg_pe("UART::open Unsupport baud rate %d.", baud_rate);;
            ret_code = 3;
            goto UART_OPEN_ERR;
    }

    if( DTR_sw )
        pd->m_oDCB.fDtrControl = DTR_CONTROL_DISABLE;
    if( RTS_sw )
        pd->m_oDCB.fRtsControl = RTS_CONTROL_DISABLE;

    pd->m_oDCB.ByteSize = byte_size;
    pd->m_oDCB.Parity   = parity_sw;
    switch( stop_bits ) {
        case 1:
            pd->m_oDCB.StopBits = ONESTOPBIT;
            break;
        case 15:
            pd->m_oDCB.StopBits = ONE5STOPBITS;
            break;
        case 2:
            pd->m_oDCB.StopBits = TWOSTOPBITS;
            break;
    }

    fSuccess = SetCommState(pd->m_hCom, &(pd->m_oDCB));
    if (!fSuccess) {
        // Handle the error.
        dbg_pe("UART::open SetComState failed with error %d.", GetLastError());
        ret_code = 4;
        goto UART_OPEN_ERR;
    }

    // timout options
    if( timeout_sw ) {
        fSuccess = GetCommTimeouts(     pd->m_hCom,         /* Handle to comm device */
                                        &oComTimeout        /* time-out values */
                                  );
        if( !fSuccess ) {
            dbg_pe("UART::open GetCommTimeouts failed with error %d.", GetLastError());
            ret_code = 5;
            goto UART_OPEN_ERR;
        }

        oComTimeout.ReadIntervalTimeout         =  DEFAULT_READINTERVALTIMEOUT;
        oComTimeout.ReadTotalTimeoutMultiplier  =  DEFAULT_READMULTIPLIER;
        oComTimeout.ReadTotalTimeoutConstant    =  DEFAULT_READTOTALTIMEOUT;
        oComTimeout.WriteTotalTimeoutMultiplier =  DEFAULT_WRITEMULTIPLIER;
        oComTimeout.WriteTotalTimeoutConstant   =  DEFAULT_WRITETOTALTIMEOUT;

        fSuccess = SetCommTimeouts(     pd->m_hCom,
                                        &oComTimeout
                                  );
        if( !fSuccess ) {
            dbg_pe("UART::open SetCommTimeouts failed with error %d.", GetLastError());
            ret_code = 6;
            goto UART_OPEN_ERR;
        }
    }

    // port state
    goto UART_OPEN_RET;

UART_OPEN_ERR:
    CloseHandle(pd->m_hCom);
    pd->m_hCom = NULL;

UART_OPEN_RET:
    return ret_code;
}

int UART::close(void)
{
    UART_inner_data     *pd;
    int                 iRes;

    // handle structure
    pd = (UART_inner_data *) data;

    if( pd->m_hCom != NULL ) {
        iRes = CloseHandle(pd->m_hCom);
        if( !iRes ) {
            dbg_pe("UART::close Close COM port error!");
            return 2;
        }

        pd->m_hCom = NULL;
    } else {
        return 1;
    }

    return 0;
}

int UART::write(void *d, int len, int master)
{
    UART_inner_data     *pd;

    BOOL                bRes;
    unsigned long       iByteWritten;

    // handle structure
    pd = (UART_inner_data *) data;

    if( pd->m_hCom != NULL ) {
        // wirte data
        bRes = WriteFile(   pd->m_hCom,         /* Handle to file */
                            d,                  /* Data buffer */
                            len,                /* Number of bytes to write */
                            &iByteWritten,      /* Number of bytes written */
                            NULL                /* overlapped buffer */
                    );
        if( !bRes ) {
            dbg_pe("UART::write Send data error, errorcode: %d", GetLastError());
            return 2;
        }

        if( iByteWritten < len ) {
            dbg_pw("UART::write Send data byte leng is not correct!");
            return 3;
        }
    } else {
        return 1;
    }

    return 0;
}

int UART::read(void *d, int len, int master)
{
    UART_inner_data     *pd;

    BOOL                bRes;
    int                 iByteRead = len;
    unsigned long       byte_read_act;

    // handle structure
    pd = (UART_inner_data *) data;

    // read data
    if( pd->m_hCom != NULL ) {
        bRes = ReadFile(    pd->m_hCom,         /* Handle to file */
                            d,                  /* Data buffer */
                            iByteRead,          /* Number of byte to read */
                            &byte_read_act,     /* Number of bytes readed */
                            NULL
                        );

        if( !bRes ) {
            dbg_pe("UART::read Read data error, errorcode: %d", GetLastError());
            return 2;
        }

        len = byte_read_act;
        if( byte_read_act < iByteRead ) {
            dbg_pw("UART::read Read data byte leng is not correct!");
            return 3;
        }
    } else {
        return 1;
    }

    return 0;
}

int UART::_write(void *d, int len)
{
    return 0;
}

int UART::_read(void *d,  int len)
{
    return 0;
}

} // end of namespace pi


#endif


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


#if defined(PIL_LINUX)

#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>

extern "C" {

inline int open_c(const char *__file, int __oflag, ...)
{
    return open(__file, __oflag);
}

} // end of extern "C"


namespace pi {

struct UART_inner_data
{
    int                 fd;
    FILE                *fp;

    std::deque<uint8_t> readBuf;
    std::deque<uint8_t> writeBuf;

public:
    UART_inner_data() {
        fd = -1;
        fp = NULL;
    }

    ~UART_inner_data() {
        if( fp != NULL ) fclose(fp);
        if( fd != -1 ) ::close(fd);

        fp = NULL;
        fd = -1;

        readBuf.clear();
        writeBuf.clear();
    }

    void clear(void) {
        readBuf.clear();
        writeBuf.clear();
    }

    int getReadBufSize(void) {
        return readBuf.size();
    }

    int putReadBuf(uint8_t *d, int len) {
        for(int i=0; i<len; i++) readBuf.push_back(d[i]);
        return len;
    }

    int getReadBuff(uint8_t *d, int len) {
        int l, i;

        if( len <= readBuf.size() ) l = len;
        else l = readBuf.size();

        for(i=0; i<l; i++) {
            d[i] = readBuf.front();
            readBuf.pop_front();
        }

        return l;
    }

    int getWriteBufSize(void) {
        return writeBuf.size();
    }

    int putWriteBuf(uint8_t *d, int len) {
        for(int i=0; i<len; i++) writeBuf.push_back(d[i]);
        return len;
    }

    int getWriteBuff(uint8_t *d, int len) {
        int l, i;

        if( len <= writeBuf.size() ) l = len;
        else l = writeBuf.size();

        for(i=0; i<l; i++) {
            d[i] = writeBuf.front();
            writeBuf.pop_front();
        }

        return l;
    }
};

UART::UART()
{
    UART_inner_data *pd;

    // inner data
    pd = new UART_inner_data;
    data = pd;

    // set default values
    port_name = "/dev/ttyACM0";

    baud_rate = 115200;
    byte_size = 8;
    parity_sw = 0;
    stop_bits = 1;

    timeout_sw = 0;
    DTR_sw     = 1;
    RTS_sw     = 1;

    m_options   = (UART_OPTIONS) (UART_NONBLOCK | UART_BUFFER);
    m_bufSize   = 32;
}

UART::~UART()
{
    UART_inner_data     *pd;
    int                 r;

    pd = (UART_inner_data *) data;
    if( pd->fd >= 0 ) {
        r = ::close(pd->fd);
        if( r == -1 ) {
            dbg_pe("close port error!\n");
        }
    }

    delete pd;
    data = NULL;
}

// Reference:
//  http://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
//  http://stackoverflow.com/questions/18108932/linux-c-serial-port-reading-writing
int UART::open(const std::string &portName, int baudRate)
{
    UART_inner_data     *pd;
    int                 r = 0;

    pd = (UART_inner_data *) data;

    if( portName.size() ) port_name = portName;
    if( baudRate )        baud_rate = baudRate;

    //pd->fd = ::open(port_name, O_RDWR|O_NOCTTY|O_SYNC);
    //pd->fd = ::open(port_name, O_RDWR| O_NONBLOCK | O_NDELAY);
    if( m_options & UART_NONBLOCK )
        pd->fd = open_c(port_name.c_str(), O_RDWR|O_NOCTTY|O_SYNC|O_NONBLOCK);
    else
        pd->fd = open_c(port_name.c_str(), O_RDWR|O_NOCTTY|O_SYNC);

    if( pd->fd < 0 ) {
        dbg_pe("open port error! (%s)\n", port_name.c_str());
        return -1;
    }

    // set port baud rate
    speed_t b_speed;
    switch(baud_rate) {
    case 110:
        b_speed = B110;
        break;
    case 300:
        b_speed = B300;
        break;
    case 600:
        b_speed = B600;
        break;
    case 1200:
        b_speed = B1200;
        break;
    case 2400:
        b_speed = B2400;
        break;
    case 4800:
        b_speed = B4800;
        break;
    case 9600:
        b_speed = B9600;
        break;
    case 19200:
        b_speed = B19200;
        break;
    case 38400:
        b_speed = B38400;
        break;
    case 57600:
        b_speed = B57600;
        break;
    case 115200:
        b_speed = B115200;
        break;
    case 230400:
        b_speed = B230400;
        break;
    case 460800:
        b_speed = B460800;
        break;
    case 500000:
        b_speed = B500000;
        break;
    case 576000:
        b_speed = B576000;
        break;
    case 921600:
        b_speed = B921600;
        break;
    case 1000000:
        b_speed = B1000000;
        break;
    default:
        dbg_pe("unsupported baud rate: %d\n", baud_rate);
        r = -2;
        goto UART_OPEN_ERR;
    }

    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if( tcgetattr(pd->fd, &tty) != 0 ) {
        dbg_pe("error %d from tcgetattr\n", errno);
        r = -3;
        goto UART_OPEN_ERR;
    }

    cfsetispeed(&tty, b_speed);
    cfsetospeed(&tty, b_speed);


    /* Setting other Port Stuff */
    tty.c_cflag     &=  ~PARENB;            // Make 8n1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;

    tty.c_cflag     &=  ~CRTSCTS;           // no flow control
    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

    if( m_options & UART_NONBLOCK ) {
        tty.c_cc[VMIN]  =   0;              // read doesn't block
        tty.c_cc[VTIME] =   0;              // 0.5 seconds read timeout
    } else {
        tty.c_cc[VMIN]  =   1;              // read doesn't block
        tty.c_cc[VTIME] =   5;              // 0.5 seconds read timeout
    }

    /* Make raw */
    cfmakeraw(&tty);
    tcflush( pd->fd, TCIFLUSH );

    if (tcsetattr (pd->fd, TCSANOW, &tty) != 0) {
        dbg_pe("error %d from tcsetattr\n", errno);
        r = -4;
        goto UART_OPEN_ERR;
    }

    goto UART_OPEN_RET;

UART_OPEN_ERR:
    ::close(pd->fd);
    pd->fd = -1;

UART_OPEN_RET:
    return r;
}

int UART::close(void)
{
    UART_inner_data     *pd;
    int                 r;

    pd = (UART_inner_data *) data;
    if( pd->fd >= 0 ) {
        r = ::close(pd->fd);
        if( r == -1 ) {
            dbg_pe("close port error!\n");
        }

        pd->fd = -1;
    }

    return r;
}


int UART::write(void *d, int len, int master)
{
    return _write(d, len);
}

int UART::read(void *d, int len, int master)
{
    UART_inner_data     *pd;
    int                 r;

    pd = (UART_inner_data *) data;
    if( pd->fd < 0 ) {
        dbg_pe("UART port not opened yet!\n");
        return -1;
    }

    if( m_options & UART_BUFFER ) {
        r = pd->getReadBuff((uint8_t*)d, len);

        if( r < 1 ) {
            int l;
            uint8_t buf[m_bufSize];

            l = _read(buf, m_bufSize);
            pd->putReadBuf(buf, l);
        }
    } else {
        r = _read(d, len);
    }

    return r;
}


int UART::_write(void *d, int len)
{
    UART_inner_data     *pd;
    int                 r;

    pd = (UART_inner_data *) data;
    if( pd->fd >= 0 ) {
        r = ::write(pd->fd, d, len);
        return r;
    } else {
        dbg_pe("UART port not opened yet!\n");
        return -1;
    }
}

int UART::_read(void *d, int len)
{
    UART_inner_data *pd = (UART_inner_data*) data;

    return ::read(pd->fd, d, len);
}

} // end of namespace pi

#endif
