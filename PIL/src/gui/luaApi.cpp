#ifndef PIL_ARM

#include <lua/LuaMachine.h>
#include <QWidget>
#include <QMainWindow>

#include "Win3D.h"
#include "controls/SvarTable.h"
#include "controls/qFlightInstruments.h"

using namespace pi;
using namespace std;

//class QObjectHolder
//{
//    QObjectHolder(QObject* object)
//    {
//       obj=object;
//    }

//    QObjectHolder(const string& type,char* name,char* parent)
//    {
//        obj=CreateWidgetByType(type,name,parent);
//        setObjectName(name);
//    }

//    int GetName

//    QObject* obj;
//};

QWidget* GetWidgetByName(char* name)
{
    SvarWithType<QObject*>& inst=SvarWithType<QObject*>::instance();

    if(inst.exist(name))
        return (QWidget*)inst[name];
    else return 0;
}

void GetWidgetSize(char* name,Point2d* result)
{
    SvarWithType<QObject*>& inst=SvarWithType<QObject*>::instance();

    if(inst.exist(name))
    {
        QWidget* widget=(QWidget*)inst[name];
        QSize size=widget->size();
        result->x=size.width();
        result->y=size.height();
    }
}

bool ShowWidget(char* name)
{
    SvarWithType<QObject*>& inst=SvarWithType<QObject*>::instance();
    if(inst.exist(name))
    {
        QWidget* widget=(QWidget*)inst[name];
        widget->show();
        return 1;
    }
    else return 0;
}

bool setMinimumSize(char* name,int width,int height)
{
    SvarWithType<QObject*>& inst=SvarWithType<QObject*>::instance();
    if(inst.exist(name))
    {
        QWidget* widget=(QWidget*)inst[name];
        widget->setMinimumSize(width,height);
        return 1;
    }
    else return 0;
}

bool setLayout(char* widgetName,char* layoutName)
{
    SvarWithType<QObject*>& inst=SvarWithType<QObject*>::instance();
    if(inst.exist(widgetName)&&inst.exist(layoutName))
    {
        QWidget* widget=(QWidget*)inst[widgetName];
        QLayout* layout=(QLayout*)inst[layoutName];
        widget->setLayout(layout);
        return true;
    }
    return false;
}

bool addWidget(char* layoutName,char* widgetName)
{
    SvarWithType<QObject*>& inst=SvarWithType<QObject*>::instance();
    if(inst.exist(widgetName)&&inst.exist(layoutName))
    {
        QWidget* widget=(QWidget*)inst[widgetName];
        QLayout* layout=(QLayout*)inst[layoutName];
        layout->addWidget(widget);
        return true;
    }
    return false;
}

bool addTab(char* tabName,char* widgetName)
{
    SvarWithType<QObject*>& inst=SvarWithType<QObject*>::instance();
    if(inst.exist(widgetName)&&inst.exist(tabName))
    {
        QWidget* widget=(QWidget*)inst[widgetName];
        QTabWidget* tab=(QTabWidget*)inst[tabName];
        tab->addTab(widget,widgetName);
        return true;
    }
    return false;
}

QWidget* CreateWidgetByType(const string& type,char* name,char* parent_name)
{
    SvarWithType<QObject*>& inst=SvarWithType<QObject*>::instance();

    if(inst.exist(name))
    {
        MSG_WARN("Widget has already existed. You may need to use another name.");
        return (QWidget*)inst[name];
    }
//    if(!inst.exist(parent_name))
//    {
//        MSG_ERROR("Can't find parent!");
//        return NULL;
//    }
    QWidget* parent=GetWidgetByName(parent_name);


//    cout<<"Creating "<<type<<" "<<name<<" parent:"<<parent<<endl;
    QWidget* result=NULL;
    if(type=="Win3D")
    {
        result=new Win3D((QWidget*)parent);
    }
    if(type=="SvarTable")
    {
        result=new SvarTable((QWidget*)parent);
    }
    if(type=="SvarWidget")
    {
        result=new SvarWidget(parent);
    }
    if(type=="QTabWidget")
    {
        result=new QTabWidget((QWidget*)parent);
    }
    if(type=="QADI")
    {
        result=new QADI((QWidget*)parent);
    }
    if(type=="QCompass")
    {
        result=new QCompass((QWidget*)parent);
    }
    if(type=="QWidget")
    {
        result=new QWidget((QWidget*)parent);
    }
    else if(type=="QCheckBox")
    {
        result=new QCheckBox((QWidget*)parent);
    }
    else if(type=="QTableWidget")
    {
        result=new QTableWidget((QWidget*)parent);
    }
    else if(type=="QMenu")
    {
        result=new QMenu(parent);
    }
    else if(type=="QLabel")
    {
        result=new QLabel(name,parent);
    }
    else if(type=="QKeyValueListView")
    {
        result=new QKeyValueListView(parent);
    }
    else if(type=="QVBoxLayout")
    {
        result=(QWidget*)new QVBoxLayout(parent);
    }
    else if(type=="QHBoxLayout")
    {
        result=(QWidget*)new QHBoxLayout(parent);
    }
    else if(type=="QMainWindow")
    {
        result=(QWidget*)new QMainWindow(parent);
    }
    if(!result)
    {
        MSG_ERROR("Can't creat such type!");
        return NULL;
    }
    else
    {
//        cout<<"Created "<<type<<" "<<name<<" result:"<<result<<endl;
        result->setObjectName(name);
        inst[name]=(QObject*)result;
        if(!parent) result->show();
        return result;
    }
}


class GUILuaInterface
{
public:
    GUILuaInterface()
    {
        lua.reg(lua_reg);
    }

    static void lua_reg(lua_State* ls)
    {
        _AF(CreateWidgetByType);
        _AF(GetWidgetByName);
        _AF(ShowWidget);
        _AF(setMinimumSize);
        _AF(setLayout);
        _AF(addWidget);
        _AF(addTab);
        _AF(GetWidgetSize);
        LUA_REG_CLASS3(QWidget,x,y,show);

        //! 注册子类
        LUA_REG_CLASS1(QString,size);

        fflua_register_t<QObject, ctor(QObject*)>(ls, "QObject")
                REG_F(QObject,setParent)
//                REG_F(QObject,setObjectName)
//                REG_F(QObject,objectName)
                .def((bool(*)(const QObject*, const char*,const QObject *, const char*))&QObject::disconnect
                     ,"disconnect")
                //        .def((bool(*)(const QObject*, const char*))&QObject::disconnect
                //             ,"disconnect")
//                .def((bool(*)(const QObject*, const char*, const char*,Qt::ConnectionType))&QObject::connect
//                     ,"connect")
                ;

        fflua_register_t<QAction, ctor(QObject*)>(ls, "QAction","QObject")
                REG_F(QAction,menu)
                REG_F(QAction,setMenu)
                REG_F(QAction,isChecked)
                REG_F(QAction,isEnabled)
                ;

        fflua_register_t<QWidget, ctor(QWidget*)>(ls, "QWidget", "QObject")
                REG_F(QWidget,devType) REG_F(QWidget,winId)
                REG_F(QWidget,isModal) REG_F(QWidget,isWindow)
                REG_F(QWidget,width) REG_F(QWidget,height)
                REG_F(QWidget,x) REG_F(QWidget,x)
                REG_F(QWidget,minimumHeight) REG_F(QWidget,minimumWidth)
                REG_F(QWidget,setMinimumHeight) REG_F(QWidget,setMinimumWidth)
                REG_F(QWidget,maximumHeight) REG_F(QWidget,maximumWidth)
                REG_F(QWidget,setMaximumHeight) REG_F(QWidget,setMaximumWidth)
//                .def((void(*)(int,int))&QWidget::setFixedSize,"setFixedSize")
                ;

        fflua_register_t<SvarTable, ctor(QWidget*)>(ls, "SvarTable", "QWidget")
                REG_F(QWidget,width) REG_F(QWidget,height)
                REG_F(QWidget,x) REG_F(QWidget,x)
                REG_F(QWidget,minimumHeight) REG_F(QWidget,minimumWidth)
                REG_F(QWidget,setMinimumHeight) REG_F(QWidget,setMinimumWidth)
                REG_F(QWidget,setVisible) REG_F(QWidget,isActiveWindow)
                REG_F(QWidget,setLayout)
                ;

        fflua_register_t<QVBoxLayout, ctor(QWidget*)>(ls, "QVBoxLayout", "QObject")
                REG_F(QBoxLayout,addSpacing)
                REG_F(QBoxLayout,addStretch)
                REG_F(QLayout,margin)
                REG_F(QLayout,spacing)
                REG_F(QLayout,setMargin)
                REG_F(QLayout,setSpacing)
//                .def((void(*)(int,int,int,int))&QLayout::setContentsMargins,"setContentsMargins")
                REG_F(QLayout,getContentsMargins)
//                .def((bool(*)(QWidget*,Qt::Alignment))&QLayout::setAlignment,"setAlignment")
                REG_F(QLayout,setMenuBar)
                REG_F(QLayout,menuBar)
                ;

        fflua_register_t<QHBoxLayout, ctor(QWidget*)>(ls, "QHBoxLayout", "QObject")
                REG_F(QBoxLayout,addSpacing)
                REG_F(QBoxLayout,addStretch)
                REG_F(QLayout,margin)
                REG_F(QLayout,spacing)
                REG_F(QLayout,setMargin)
                REG_F(QLayout,setSpacing)
//                .def((void(*)(int,int,int,int))&QLayout::setContentsMargins,"setContentsMargins")
                REG_F(QLayout,getContentsMargins)
//                .def((bool(*)(QWidget*,Qt::Alignment))&QLayout::setAlignment,"setAlignment")
                REG_F(QLayout,setMenuBar)
                REG_F(QLayout,menuBar)
//                REG_F(QBoxLayout,addWidget)
                ;
        fflua_register_t<QMenuBar, ctor(QWidget*)>(ls, "QMenuBar", "QWidget")
                REG_F(QMenuBar,setNativeMenuBar)
                REG_F(QMenuBar,isNativeMenuBar)
                REG_F(QMenuBar,heightForWidth)
                REG_F(QMenuBar,isDefaultUp)
                REG_F(QMenuBar,setDefaultUp)
                REG_F(QMenuBar,setActiveAction)
                REG_F(QMenuBar,activeAction)
                ;

        fflua_register_t<SvarTable, ctor(QWidget*)>(ls, "SvarTable", "QWidget")
                REG_F(QWidget,width) REG_F(QWidget,height)
                REG_F(QWidget,x) REG_F(QWidget,x)
                REG_F(QWidget,minimumHeight) REG_F(QWidget,minimumWidth)
                REG_F(QWidget,setMinimumHeight) REG_F(QWidget,setMinimumWidth);

    }
};

GUILuaInterface guiLuaInterface;
#endif
