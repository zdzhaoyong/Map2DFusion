#ifndef __HAL_JSCOMOBO_H__
#define __HAL_JSCOMOBO_H__

#include "HAL_Joystick.h"

namespace pi {

///
/// \brief The class support integrated or combo joystick devices
///         When the device is "integrated", only one
///
class HAL_JSComobo
{
public:
    enum JS_DevType {
        JS_DEV_INTEGRATED,
        JS_DEV_COMBO,
    };

public:
    HAL_JSComobo() {
        m_devType = JS_DEV_INTEGRATED;
        m_devOpened = 0;
    }
    virtual ~HAL_JSComobo() {
        close();
    }

    int open(const std::string &confName);
    int close(void);
    int read(JS_Val *jsv);

    int isOpened(void) const { return m_devOpened; }

public:
    HAL_JoyStick        m_js0, m_js1;
    int                 m_devID0, m_devID1;

    JS_DevType          m_devType;
    int                 m_devOpened;
    int                 m_invThrottle;

    std::map<int,int>   m_channelMap0, m_channelMap1;
    std::map<int,int>   m_btnMap0, m_btnMap1;
};

} // end of namespace PI

#endif // end of __HAL_JSCOMOBO_H__
