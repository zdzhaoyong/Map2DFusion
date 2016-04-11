//#ifndef LVAR_H
//#define LVAR_H
//#include <iostream>
//#include <string>

//#include <lua/fflua/fflua.h>
//#include <base/system/thread/ThreadBase.h>


//#define lvar Lvar::instance()
//#define lua  Lvar::instance().state;

//class Lvar
//{
//public:
//    Lvar();

//    /** This gives us singletons instance. \sa enter */
//    static Lvar& instance();

//    bool ParseFile(std::string sFileName);


//    template <class T> T& var(const std::string& name,const T& def);

//    bool exist(const std::string& name);
//    bool erase(const std::string& name);

//    lua_State* state;
//    pi::Mutex  mMutex;
//};

//#endif // LVAR_H
