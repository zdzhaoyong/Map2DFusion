#ifndef LUAMACHINE_H
#define LUAMACHINE_H

#include <lua/fflua/fflua.h>
#include <base/types/SPtr.h>

#define lua LuaMachine::get_fflua()

#define _AF(a) fflua_register_t<>(ls).def(&a,#a);
#define REG_C(c) fflua_register_t<c,ctor()>(ls,#c)
#define REG_F(c,f) .def(&c::f,#f)

#define LUA_REG_CLASS(C) REG_C(C)
#define LUA_REG_CLASS1(C,F1) REG_C(C) REG_F(C,F1)
#define LUA_REG_CLASS2(C,F1,F2) REG_C(C) REG_F(C,F1) REG_F(C,F2)
#define LUA_REG_CLASS3(C,F1,F2,F3) REG_C(C) REG_F(C,F1) REG_F(C,F2) REG_F(C,F3)
#define LUA_REG_CLASS4(C,F1,F2,F3,F4) REG_C(C) REG_F(C,F1) REG_F(C,F2) REG_F(C,F3) \
    REG_F(C,F4)
#define LUA_REG_CLASS5(C,F1,F2,F3,F4,F5) REG_C(C) REG_F(C,F1) REG_F(C,F2) REG_F(C,F3) \
    REG_F(C,F4) REG_F(C,F5)
#define LUA_REG_CLASS6(C,F1,F2,F3,F4,F5,F6) REG_C(C) REG_F(C,F1) REG_F(C,F2) REG_F(C,F3) \
    REG_F(C,F4) REG_F(C,F5) REG_F(C,F6)

class LuaMachineImpl;

class LuaMachine
{
public:
    LuaMachine();
    virtual ~LuaMachine(){}
    lua_State* L();
    static inline pi::fflua_t& get_fflua()
    {
        static pi::fflua_t* inst=0;
        if(inst==0)
        {
            inst=new pi::fflua_t;
        }
        return *inst;
    }

    void start(){}
    int  loadFile(const std::string& filename);

    int     GetInt(const std::string& name);
    double  GetDouble(const std::string& name);
    std::string  GetString(const std::string& name);

private:
    SPtr<LuaMachineImpl> data;
};

#endif // LUAMACHINE_H
