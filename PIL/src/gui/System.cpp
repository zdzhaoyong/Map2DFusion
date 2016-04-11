#include "System.h"

#include "base/Svar/Svar_Inc.h"
#include "base/debug/debug_config.h"



namespace pi {

SystemBase::SystemBase():pause(svar.i["SystemPause"])
{
    dbg_stacktrace_setup();
    mWindow=NULL;

}

SystemBase::~SystemBase()
{
    this->~Thread();
}

void SystemBase::initialize()
{

}

void SystemBase::run()
{
//    if(mWindow) mWindow->
}
}
