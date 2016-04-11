#ifndef SYSTEM_BASE_H
#define SYSTEM_BASE_H

#include <base/system/thread/ThreadBase.h>
#include "MainWindowBase.h"

namespace pi
{


class SystemBase : public Thread, public EventHandle
{
public:
    SystemBase();
    virtual ~SystemBase();
    virtual void initialize();

    virtual void run();



protected:
    MainWindowBase* mWindow;
    int &           pause;//svar.i["SystemPause"]
};


}
#endif // SYSTEM_BASE_H
