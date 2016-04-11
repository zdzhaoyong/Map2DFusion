#include "LuaMachine.h"
#include <base/debug/debug_config.h>
#include <base/system/file_path/file_path.h>
#include <iostream>

using namespace pi;
using namespace std;

class LuaMachineImpl
{
public:
    LuaMachineImpl()
    {
        L=luaL_newstate();
        if(L==NULL)
        {
            MSG_ERROR("Fail to create Lua state!");
        }
        luaL_openlibs(L);
    }
public:
    lua_State* L;
};

LuaMachine::LuaMachine()
{
    data=SPtr<LuaMachineImpl>(new LuaMachineImpl());
}

lua_State* LuaMachine::L()
{
    return data->L;
}

int  LuaMachine::loadFile(const std::string& filename)
{
    int ret=luaL_loadfile(data->L,filename.c_str());
    if(ret!=0) return ret;
    ret = lua_pcall(data->L,0,0,0) ;
    return ret;
}

int  LuaMachine::GetInt(const std::string& name)
{
    MSG_WARN("Trying to load a interger.\n");
    lua_State* l=data->L;
    lua_getglobal(l,name.c_str());
    if(lua_isnoneornil(l,-1))
    {
        MSG_ERROR("LUA value is NIL!");
    }
    if(!lua_isnumber(l,-1))
    {
        MSG_ERROR("LUA value is not a number!");
    }
    int result=lua_tointeger(l,-1);
    lua_pop(l,1);
    return result;
}

double  LuaMachine::GetDouble(const std::string& name)
{
    lua_State* l=data->L;
    lua_getglobal(l,name.c_str());
    double result=lua_tonumber(l,-1);
    lua_pop(l,1);
    return result;
}

string  LuaMachine::GetString(const std::string& name)
{
    lua_State* L=data->L;
    lua_getglobal(L,name.c_str());
    const char* result=lua_tostring(L,-1);
    lua_pop(L,1);
    return result;
}

