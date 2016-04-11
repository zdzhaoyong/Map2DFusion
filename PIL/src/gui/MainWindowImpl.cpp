#include "MainWindowImpl.h"
#include <base/Svar/Scommand.h>

#include <lua/LuaMachine.h>

#include <QKeyEvent>

using namespace std;
LuaMachine luaMachine;


MainWindowImpl::MainWindowImpl(QWidget *parent):QMainWindow(parent)
{
    // setup layout
//    setupLayout();

    SvarWithType<QObject*>& inst=SvarWithType<QObject*>::instance();
    inst.insert("MainWindow",this,false);

    connect(this, SIGNAL(call_signal() ), this, SLOT(call_slot()) );
}


//int MainWindowImpl::setupLayout()
//{
//    // set window minimum size
////    this->setMinimumSize(1000, 700);
//}

void  MainWindowImpl::call(std::string cmd)
{
    cmds.push(cmd);
    emit call_signal();
}

void MainWindowImpl::call_slot()
{
    while(cmds.size())
    {
        string& cmd=(cmds.front());
        if("show"==cmd) show();
        else
            Scommand::instance().Call(cmd);
        cmds.pop();
    }
}

void MainWindowImpl::action_SvarEdit(void)
{
//    SvarWidget *sw;

//    if( SvarWithType<SvarWidget*>::instance().exist("SvarWidget") ) {
//        sw = SvarWithType<SvarWidget*>::instance()["SvarWidget"];
//    } else {
//        sw = new SvarWidget();
//        SvarWithType<SvarWidget*>::instance()["SvarWidget"] = sw;
//    }

//    sw->show();
}

void MainWindowImpl::keyPressEvent(QKeyEvent *event)
{
    int     key, key2;
//    double  v;

    key  = event->key();
    cout<<"key "<<key<<"pressed!";

//    key2 = key & 0xFFFFFF;

}

void MainWindowImpl::mousePressEvent(QMouseEvent *event)
{
#if 0
    // 1 - left
    // 2 - right
    // 4 - middle
    printf("window pressed, %d, %d, %d\n", event->button(), event->pos().x(), event->pos().y());

    if( event->button() == 1 ) {

    }
#endif
}

void MainWindowImpl::resizeEvent(QResizeEvent *event)
{
}

void MainWindowImpl::timerEvent(QTimerEvent *event)
{
}
