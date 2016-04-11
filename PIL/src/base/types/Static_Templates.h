#ifndef STATIC_TEMPLATES_H
#define STATIC_TEMPLATES_H

namespace pi {

/**
********************************************
** IsPointer
********************************************
// 示例
int main()
{
    cout << IsPointer<int*>::Result << endl;
    cout << IsPointer<int>::Result << endl;
    IsPointer<int*>::ValueType i = 1;
    //IsPointer<int>::ValueType j = 1;
    // 错误：使用未定义的类型Nil
}
  */
// 仅声明
struct Nil;

// 主模板
template <typename T>
struct IsPointer
{
    enum { Result = false };
    typedef Nil ValueType;
};

// 局部特化
template <typename T>
struct IsPointer<T*>
{
    enum { Result = true };
    typedef T ValueType;
};


/**
********************************************
** IS_INT
********************************************
// 示例
int main()
{
    cout << IsPointer<int*>::Result << endl;
    cout << IsPointer<int>::Result << endl;
    IsPointer<int*>::ValueType i = 1;
    //IsPointer<int>::ValueType j = 1;
    // 错误：使用未定义的类型Nil
}
  */

template <typename T>
struct IS_INT
{
    enum { Result = false };
};

// 局部特化
struct IS_INT<int>
{
    enum { Result = true };
};


}
#endif // STATIC_TEMPLATES_H
