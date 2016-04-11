#include <lua/LuaMachine.h>
#include <lua/RegistedClasses.h>
#include <iostream>
#include <assert.h>

using namespace std;

int main()
{
    LuaMachine l;
    if(!l.L()) {cout<<"Fail to open LuaMachine.\n";return -1;}
    else
    {
        cout<<"LUA adress "<<l.L()<<endl;
    }

    lua_State* L=l.L();
    int ret=luaL_loadfile(L,"func.lua");
    if(l.loadFile("func.lua")) {cout<<"Fail to load file.\n";}

    cout<<"TestInt(1)="<<l.GetInt("TestInt")<<endl;

    cout<<"TestDouble(1)="<<l.GetDouble("TestDouble")<<endl;

//    cout<<"TestString(Success)="<<l.GetString("TestString")<<endl;

    lua.load_file("test.lua");
//    LuaAPI luaApi;

    //! 获取全局变量
    int var = 0;
    assert(0 == lua.get_global_variable("test_var", var));
    //! 设置全局变量
    assert(0 == lua.set_global_variable("test_var", ++var));

    //! 执行lua 语句
    lua.run_string("print(\"exe run_string!!\")");

    //! 调用lua函数, 基本类型作为参数
    int32_t arg1 = 1;
    float   arg2 = 2;
    double  arg3 = 3;
    string  arg4 = "4";
    lua.call<bool>("test_func", arg1, arg2, arg3,  arg4);

    //! 调用lua函数，stl类型作为参数， 自动转换为lua talbe
    vector<int> vec;        vec.push_back(100);
    list<float> lt;         lt.push_back((float)99.99);
    set<string> st;         st.insert("OhNIce");
    map<string, int> mp;    mp["key"] = 200;
    lua.call<string>("test_stl", vec, lt, st,  mp);

    //! 调用lua 函数返回 talbe，自动转换为stl结构
    vec = lua.call<vector<int> >("test_return_stl_vector");
    lt  = lua.call<list<float> >("test_return_stl_list");
    st  = lua.call<set<string> >("test_return_stl_set");
    mp  = lua.call<map<string, int> >("test_return_stl_map");
    lua.dump_stack();

    //! 调用lua函数，c++ 对象作为参数, foo_t 必须被注册过
    foo_t* foo_ptr = new foo_t(456);
    lua.call<void>("test_object", foo_ptr);

    //! 调用lua函数，c++ 对象作为返回值, foo_t 必须被注册过
    assert(foo_ptr == lua.call<foo_t*>("test_ret_object", foo_ptr));
    //! 调用lua函数，c++ 对象作为返回值, 自动转换为基类
    base_t* base_ptr = lua.call<base_t*>("test_ret_base_object", foo_ptr);
    assert(base_ptr == foo_ptr);

    lua.dump_stack();
}
