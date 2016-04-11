//#include "Lvar.h"
//#include <base/debug/debug_config.h>
//#include <base/Svar/Svar.h>

//using namespace std;
//using namespace pi;

//Lvar::Lvar()
//{
//    state = fflua_t::get_lua_state();

////    state=luaL_newstate();
////    if(state==NULL)
////    {
////        MSG_ERROR("Fail to create Lua state!");
////    }
////    luaL_openlibs(state);
//}

//static Lvar& Lvar::instance()
//{
//    static SvarWithType* inst=0;

//    if(!inst)
//    {
//        inst = new Lvar();
//    }

//    return *inst;
//}

//bool Lvar::ParseFile(std::string sFileName)
//{
//#ifdef MUTI_THREAD
//    pi::ScopedMutex lock(mMutex);
//#endif
//    int ret=luaL_loadfile(state,sFileName.c_str());
//    if(ret!=0) return false;
//    ret = lua_pcall(state,0,0,0) ;
//    return !ret;
//}


//int& Lvar::var(const std::string& name,const int& def)
//{
////#ifdef MUTI_THREAD
////    pi::ScopedMutex lock(mMutex);
////#endif
////    lua_State* l=state;
////    lua_getglobal(l,name.c_str());
////    get_global
////            if(lua_isnoneornil(l,-1))
////    {

////    }
////    if(!lua_isnumber(l,-1))
////    {
////        MSG_ERROR("LUA value is not a number!");
////    }
////    int result=lua_tointeger(l,-1);
////    lua_pop(l,1);
////    return result;
//}
