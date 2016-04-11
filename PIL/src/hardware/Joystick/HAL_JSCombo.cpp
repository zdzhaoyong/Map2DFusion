
#include <string.h>
#include <string>

#include <base/utils/utils_str.h>
#include <base/Svar/Svar.h>
#include <base/Svar/VecParament.h>

#include "HAL_JSCombo.h"

using namespace std;

namespace pi {

int HAL_JSComobo::open(const std::string &confName)
{
    int     i, j;
    int     ret;
    string  devType = svar.GetString(confName + ".devType", "integrated");

    if( m_devOpened ) return -1;
    m_devOpened = 0;

    m_invThrottle = svar.GetInt(confName + ".invThrottle", 0);

    // stick & button integrated
    if( devType == "integrated" ) {
        m_devType = JS_DEV_INTEGRATED;

        // try to open device
        for(i=0; i<5; i++) {
            ret = m_js0.open(i);
            if( ret == 0 ) {
                // check device name
                string devName = svar.GetString(confName + ".devName",
                                                "Thrustmaster T.Flight Hotas X");
                if( devName != m_js0.m_devName ) {
                    m_js0.close();
                    continue;
                }

                // set channel & button maps
                VecParament channelMap, btnMap;

                for(j=0; j<JS_AXIS_MAX_NUM; j++)    channelMap.push_back(j);
                for(j=0; j<JS_BUTTON_MAX_NUM; j++)  btnMap.push_back(j);

                channelMap = svar.get_var(confName + ".channelMap", channelMap);
                btnMap = svar.get_var(confName + ".btnMap", btnMap);
                m_js0.setChannelMap(channelMap);
                m_js0.setBtnMap(btnMap);

                // set flags
                m_devID0 = i;
                m_devOpened = 1;

                break;
            }
        }

        // set open flag
        m_devOpened = 1;

        return 0;
    }

    // stick and throttle are separated
    if ( devType == "combo" ) {
        m_devType = JS_DEV_COMBO;

        // try to stick device
        for(i=0; i<5; i++) {
            ret = m_js0.open(i);
            if( ret == 0 ) {
                // check device name
                string devName = svar.GetString(confName + ".stick.devName",
                                                "Madcatz Saitek Pro Flight X-55 Rhino Stick");
                if( trim(devName) != trim(m_js0.m_devName) ) {
                    m_js0.close();
                    continue;
                }

                // set flags
                m_devID0 = i;

                break;
            }
        }

        if( !m_js0.isOpen() ) {
            dbg_pe("Can not open stick device for combo joystick");
            m_devOpened = 0;
            return -1;
        }

        // try to open throttle device
        for(i=0; i<5; i++) {
            if( i == m_devID0 ) continue;

            ret = m_js1.open(i);
            if( ret == 0 ) {
                // check device name
                string devName = svar.GetString(confName + ".throt.devName",
                                                "Madcatz Saitek Pro Flight X-55 Rhino Throttle");
                if( trim(devName) != trim(m_js1.m_devName) ) {
                    m_js1.close();
                    continue;
                }

                // set flags
                m_devID1 = i;

                break;
            }
        }

        if( !m_js1.isOpen() ) {
            dbg_pe("Can not open throttle device for combo joystick");
            m_devOpened = 0;
            m_js0.close();
            return -1;
        }

        // set default maps
        VecParament channelMap0, channelMap1, btnMap0, btnMap1;

        for(i=0; i<JS_AXIS_MAX_NUM; i++) {
            m_channelMap0[i] = i;
            m_channelMap1[i] = i;
        }

        for(i=0; i<JS_BUTTON_MAX_NUM; i++) {
            m_btnMap0[i] = i;
            m_btnMap1[i] = i;
        }

        // load user defined channel & button maps
        channelMap0 = svar.get_var(confName + ".stick.channelMap", channelMap0);
        btnMap0     = svar.get_var(confName + ".stick.btnMap",     btnMap0);
        channelMap1 = svar.get_var(confName + ".throt.channelMap", channelMap1);
        btnMap1     = svar.get_var(confName + ".throt.btnMap",     btnMap1);

        for(i=0; i<channelMap0.size(); i++) m_channelMap0[i] = (int) channelMap0[i];
        for(i=0; i<channelMap1.size(); i++) m_channelMap1[i] = (int) channelMap1[i];
        for(i=0; i<btnMap0.size(); i++)     m_btnMap0[i] = (int) btnMap0[i];
        for(i=0; i<btnMap1.size(); i++)     m_btnMap1[i] = (int) btnMap1[i];

        // set open flag
        m_devOpened = 1;

        return 0;
    }

    return -1;
}

int HAL_JSComobo::close(void)
{
    if( m_devOpened ) {
        if( m_devType == JS_DEV_INTEGRATED ) {
            m_js0.close();
        } else {
            m_js0.close();
            m_js1.close();
        }
    }

    m_devOpened = 0;

    return 0;
}

int HAL_JSComobo::read(JS_Val *jsv)
{
    int     i;
    int     ret = -1;

    memset(jsv, 0, sizeof(JS_Val));

    // device opened?
    if( m_devOpened ) {
        if( m_devType == JS_DEV_INTEGRATED ) {
            ret = m_js0.read(jsv);
        } else {
            JS_Val v0, v1;

            memset(&v0, 0, sizeof(JS_Val));
            memset(&v1, 0, sizeof(JS_Val));
            m_js0.read(&v0);
            m_js1.read(&v1);

            for(i=0; i<JS_AXIS_MAX_NUM; i++) {
                jsv->AXIS[m_channelMap0[i]] = v0.AXIS[i];
                jsv->AXIS[m_channelMap1[i]] = v1.AXIS[i];
            }

            for(i=0; i<JS_BUTTON_MAX_NUM; i++) {
                jsv->BUTTON[m_btnMap0[i]] = v0.BUTTON[i];
                jsv->BUTTON[m_btnMap1[i]] = v1.BUTTON[i];
            }

            if( v0.timeStamp > v1.timeStamp ) jsv->timeStamp = v0.timeStamp;
            else                              jsv->timeStamp = v1.timeStamp;

            jsv->dataUpdated = v0.dataUpdated | v1.dataUpdated;

            ret = 0;
        }

        // inverse throttle
        if( m_invThrottle ) {
            int a = abs(m_invThrottle);
            int s = m_invThrottle / a;

            jsv->AXIS[a] = jsv->AXIS[a] * s;
        }
    }

    return ret;
}

} // end of namespace pi
