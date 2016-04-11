#ifndef MAINWINDOWBASE_H
#define MAINWINDOWBASE_H

#include "Win3D.h"
#include <base/types/SPtr.h>
#include <base/system/thread/ThreadBase.h>

class MainWindowImpl;

namespace pi {

class MainWindowBase :public pi::Thread
{
public:
    MainWindowBase();

    void call(string cmd);

    QWidget* getWidget();

protected:
    void run();
    SPtr<MainWindowImpl> impl;
};

}
#endif // MAINWINDOWBASE_H
