#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <linux/joystick.h>

#include "base/utils/utils.h"
#include "HAL_Joystick.h"

namespace pi {


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int joystick_open(const char* cp_js_dev_name, int i4_block = 1)
{
    int i4_open_flags = O_RDONLY;
    int i4_op_block = 0;
    int i4_js_fd;

    // if there is no device
    if ( !cp_js_dev_name )  {
        dbg_pe("joystic device name is NULL\n");
        return -1;
    }

    // if there is no data from device
    i4_op_block = i4_block ? 1 : 0;
    if (i4_op_block == 0) i4_open_flags |= O_NONBLOCK;

    // open the device
    i4_js_fd = open(cp_js_dev_name, i4_open_flags);

    // if it cannot open device
    if (i4_js_fd < 0) {
        dbg_pe("can not open joystick device: %s", cp_js_dev_name);
        return -1;
    }

    return i4_js_fd;
}

int joystick_close(int i4_fd)
{
    return close(i4_fd);
}

int joystick_read_ready(int i4_fd, int i4_block = 1)
{
    if( i4_block == 1 ) {
        fd_set readfd;
        int i4_ret = 0;
        struct timeval timeout = {0, 0};
        FD_ZERO(&readfd);
        FD_SET(i4_fd, &readfd);

        i4_ret = select(i4_fd + 1, &readfd, NULL, NULL, &timeout);

        if (i4_ret > 0 && FD_ISSET(i4_fd, &readfd)) {
            return 1;
        }
        else {
            return 0;
        }
    }

    return 1; /*noblock read, aways ready*/
}

int joystick_read_one_event(int i4_fd, js_event* tp_jse)
{
    int i4_rd_bytes;

    /*do not check i4_fd again*/

    i4_rd_bytes = read(i4_fd, tp_jse, sizeof(js_event));

    if (i4_rd_bytes == -1) {
        if (errno == EAGAIN) { /*when no block, it is not error*/
            return 0;
        }
        else {
            return -1;
        }
    }

    return i4_rd_bytes;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int HAL_JoyStick::thread_func(void *arg)
{
    js_event        jse;

    // if device not open
    if( m_devID < 0 ) return -1;

    m_bOpened = 1;

    // loop forever
    while( getAlive() ) {
        if ( joystick_read_ready(m_devFD) ) {
            int rc = joystick_read_one_event(m_devFD, &jse);
            if (rc > 0) {
                if ((jse.type & JS_EVENT_INIT) == JS_EVENT_INIT) {
                    if ((jse.type & ~JS_EVENT_INIT) == JS_EVENT_AXIS) {
                        RMutex m(&m_mutex);

                        m_JSVal.AXIS[m_channelMap[jse.number]] = 1.0*jse.value/32767.0;
                        m_JSVal.dataUpdated = 1;
                        m_JSVal.timeStamp = jse.time;
                    }

                    if ((jse.type & ~JS_EVENT_INIT) == JS_EVENT_BUTTON) {
                        RMutex m(&m_mutex);

                        m_JSVal.BUTTON[m_btnMap[jse.number]] = jse.value;
                        m_JSVal.dataUpdated = 1;
                        m_JSVal.timeStamp = jse.time;
                    }
                } else {
                    if (jse.type == JS_EVENT_AXIS) {
                        RMutex m(&m_mutex);

                        m_JSVal.AXIS[m_channelMap[jse.number]] = 1.0*jse.value/32767.0;
                        m_JSVal.dataUpdated = 1;
                        m_JSVal.timeStamp = jse.time;
                    } else if ( jse.type == JS_EVENT_BUTTON ) {
                        RMutex m(&m_mutex);

                        m_JSVal.BUTTON[m_btnMap[jse.number]] = jse.value;
                        m_JSVal.dataUpdated = 1;
                        m_JSVal.timeStamp = jse.time;
                    }
                }
            }

            tm_sleep(1);
        }
    }

    m_bOpened = 0;
}

int HAL_JoyStick::open(int devID)
{
    char szJS[200];

    m_devID = devID;
    m_bOpened = 0;

    // set default axis & button number
    m_numAxes = JS_AXIS_MAX_NUM;
    m_numBtns = JS_BUTTON_MAX_NUM;

    // set default map
    for(int i=0; i<JS_AXIS_MAX_NUM; i++)    m_channelMap[i] = i;
    for(int i=0; i<JS_BUTTON_MAX_NUM; i++)  m_btnMap[i] = i;

    // clear old values
    memset(&m_JSVal, 0, sizeof(JS_Val));

    // generate joystick device path
    sprintf(szJS, "/dev/input/js%d", devID);

    // open joystick
    if( m_devID >=0 ) {
        m_devFD = joystick_open(szJS, 1);
        if (m_devFD < 0) {
            dbg_pe("open joystick (%s) failed.\n", szJS);
            return -1;
        }

        // get device properties
        memset(m_devName, 0, 256);

        ioctl(m_devFD, JSIOCGNAME(256), m_devName);
        ioctl(m_devFD, JSIOCGVERSION, m_devVersion);
        ioctl(m_devFD, JSIOCGAXES, m_numAxes);
        ioctl(m_devFD, JSIOCGBUTTONS, m_numBtns);
    } else {
        dbg_pe("input devID error! %d\n", devID);
        return -2;
    }

    // start reading thread
    start();

    return 0;
}

int HAL_JoyStick::close(void)
{
    if( !m_bOpened ) -1;

    setAlive(0);
    wait(10);
    kill();

    joystick_close(m_devFD);
    m_devFD = -1;

    return 0;
}

int HAL_JoyStick::setChannelMap(std::vector<double> &channelMap)
{
    // clear old map
    m_channelMap.clear();

    // set default map
    for(int i=0; i<JS_AXIS_MAX_NUM; i++) m_channelMap[i] = i;

    // set input channelMap
    for(int i=0; i<channelMap.size(); i++) {
        m_channelMap[i] = (int) channelMap[i];
    }

    return 0;
}

int HAL_JoyStick::setBtnMap(std::vector<double> &btnMap)
{
    // clear old map
    m_btnMap.clear();

    // set default map
    for(int i=0; i<JS_BUTTON_MAX_NUM; i++) m_btnMap[i] = i;

    // set input channelMap
    for(int i=0; i<btnMap.size(); i++) {
        m_btnMap[i] = (int) btnMap[i];
    }

    return 0;
}


int HAL_JoyStick::read(JS_Val *jsv)
{
    if( !m_bOpened ) return -1;

    RMutex m(&m_mutex);

    *jsv= m_JSVal;
    m_JSVal.dataUpdated = 0;

    return 0;
}

} // end of namespace pi
