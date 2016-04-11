#include "LuaMachine.h"
#include "RegistedClasses.h"

#include <base/time/Global_Timer.h>
#include <base/types/types.h>
#include <base/Svar/Scommand.h>

using namespace pi;

void enterTimer(const char* name)
{
    timer.enter(name);
}

void leaveTimer(const char* name)
{
    timer.leave(name);
}

Svar* GetSvar()
{
    return &svar;
}

//! lua talbe 可以自动转换为stl 对象
void dumy(map<string, string> ret, vector<int> a, list<string> b, set<int64_t> c)
{
    printf("in %s begin ------------\n", __FUNCTION__);
    for (map<string, string>::iterator it =  ret.begin(); it != ret.end(); ++it)
    {
        printf("map:%s, val:%s:\n", it->first.c_str(), it->second.c_str());
    }
    printf("in %s end ------------\n", __FUNCTION__);
}


static void lua_reg(lua_State* ls)
{
    //! 注册基类函数, ctor() 为构造函数的类型
    LUA_REG_CLASS2(base_t,dump,v);

    //! 注册子类，ctor(int) 为构造函数， foo_t为类型名称， base_t为继承的基类名称
    fflua_register_t<foo_t, ctor(int)>(ls, "foo_t", "base_t")
            REG_F(foo_t,print) REG_F(foo_t,a);

    LUA_REG_CLASS1(clazz,static_func);
    LUA_REG_CLASS4(Point3d,x,y,z,norm);
    LUA_REG_CLASS3(Point2d,x,y,norm);
    LUA_REG_CLASS4(RTK_Color,a,b,g,r);

    REG_C(Svar) REG_F(Svar,ParseFile) REG_F(Svar,ParseLine);

    _AF(dumy);
    _AF(enterTimer);
    _AF(leaveTimer);
    _AF(GetSvar);
}

void ScommandHandle(void* ptr, string sCommand, string sParams)
{
    if(sCommand=="luaLoadFile")
    {
        lua.load_file(sParams);
    }
    else if(sCommand=="luaCall")
    {
        lua.run_string(sParams);
    }
}

class LuaAPI
{
public:
    LuaAPI();
};

LuaAPI::LuaAPI()
{
    lua.reg(lua_reg);
    Scommand& inst=Scommand::instance();
    inst.RegisterCommand("luaLoadFile",ScommandHandle,this);
    inst.RegisterCommand("luaCall",ScommandHandle,this);
}

LuaAPI luaApi;


