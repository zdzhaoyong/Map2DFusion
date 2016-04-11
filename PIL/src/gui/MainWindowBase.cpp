#include "MainWindowImpl.h"
#include "MainWindowBase.h"

#include <base/Svar/Svar.h>

namespace pi {

MainWindowBase::MainWindowBase()
{
    start();
    while(!impl.get()) sleep(10);
}

void MainWindowBase::run()
{
    int argc=svar.GetInt("argc",1);
    SvarWithType<char**>& inst=SvarWithType<char**>::instance();
    char** argv=new char*[argc];
    if(inst.exist("argv"))
        argv=inst.get_var("argv",argv);
    QApplication app(argc,argv);
    impl=SPtr<MainWindowImpl>(new MainWindowImpl);
    int ret=app.exec();
    stop();
}

void MainWindowBase::call(string cmd)
{
    if(impl.get())
        impl->call(cmd);
}

QWidget* MainWindowBase::getWidget()
{
    if(impl.get())
        return impl.get();
}

}
