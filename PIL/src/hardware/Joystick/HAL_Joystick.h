#ifndef __HAL_JOYSTICK_H__
#define __HAL_JOYSTICK_H__

#include "base/osa/osa++.h"


namespace pi {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define JS_AXIS_MAX_NUM     16
#define JS_BUTTON_MAX_NUM   32


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct JS_Val {
    float       AXIS[JS_AXIS_MAX_NUM];                  ///< axis values
    float       BUTTON[JS_BUTTON_MAX_NUM ];             ///< button state
    uint32_t    timeStamp;                              ///< state timestamp
    int         dataUpdated;                            ///< state has been readed or not
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class HAL_JoyStick: public RThread
{
public:
    HAL_JoyStick() {
        m_devID = -1;
        m_bOpened = 0;
    }

    HAL_JoyStick(int dev) {
        HAL_JoyStick();

        m_devID = dev;
    }

    virtual ~HAL_JoyStick() {
        close();

        m_channelMap.clear();
        m_btnMap.clear();
    }

    virtual int thread_func(void *arg);

    int open(int devID = 0);
    int close(void);

    int setChannelMap(std::vector<double> &channelMap);
    int setBtnMap(std::vector<double> &channelMap);

    int read(JS_Val *jsv);

    int isOpen(void) { return m_bOpened; }

public:
    int                 m_devType;                  // 0:joystick 1:control
    int                 m_devID;
    int                 m_devFD;

    char                m_devName[256];
    uint32_t            m_devVersion;
    uint8_t             m_numAxes;
    uint8_t             m_numBtns;

protected:
    RMutex              m_mutex;
    JS_Val              m_JSVal;
    int                 m_bOpened;

    std::map<int,int>   m_channelMap;
    std::map<int,int>   m_btnMap;
};

} // end of namespace pi

#endif // end of __HAL_JOYSTICK_H__
