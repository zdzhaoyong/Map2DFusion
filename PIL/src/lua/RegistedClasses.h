#ifndef REGISTEDCLASSES_H
#define REGISTEDCLASSES_H

#include <iostream>
#include <stdio.h>

class base_t
{
public:
    base_t():v(789){}
    void dump()
    {
        printf("in %s a:%d\n", __FUNCTION__, v);
    }
    int v;
};

class foo_t: public base_t
{
public:
    foo_t(int b):a(b)
    {
        printf("in %s b:%d this=%p\n", __FUNCTION__, b, this);
    }

    ~foo_t()
    {
        printf("in %s\n", __FUNCTION__);
    }

    void print(int64_t a, base_t* p) const
    {
        printf("in foo_t::print a:%ld p:%p\n", (long)a, p);
    }

    static void dumy()
    {
        printf("in %s\n", __FUNCTION__);
    }
    int a;
};

class clazz{
public:
    static void static_func(){
        printf("in clazz::%s end ------------\n", __FUNCTION__);
    }
};

#endif // REGISTEDCLASSES_H
