#ifndef SPTR_H
#define SPTR_H

#define USE_TR1
//#define USE_BOOST

#ifdef USE_TR1

#include <tr1/memory>

#define SPtr std::tr1::shared_ptr
#define WPtr std::tr1::weak_ptr

#else

#ifdef USE_BOOST

#include <boost/shared_ptr.hpp>
#define SPtr boost::shared_ptr
#define WPtr boost::weak_ptr

#else

#include <tr1/memory>
template <class T>
struct SPtr:public std::tr1::shared_ptr<T>
{

}

#endif //USE_BOOST

#endif //USE_TR1

#endif // SPTR_H
