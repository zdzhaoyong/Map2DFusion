/******************************************************************************

  Robot Toolkit ++ (RTK++)

  Copyright (c) 2007-2013 Shuhui Bu <bushuhui@nwpu.edu.cn>
  http://www.adv-ci.com

  ----------------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

#ifndef __ZY_TYPE_H__
#define __ZY_TYPE_H__

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <sstream>
#include <math.h>

//#define HAS_TOON
#ifdef HAS_TOON
#include <TooN/TooN.h>
#endif

namespace pi {

/******************************************************************************
 * basic types
 *****************************************************************************/

typedef uint8_t         ru8;
typedef uint16_t        ru16;
typedef uint32_t        ru32;
typedef uint64_t        ru64;

typedef int8_t          ri8;
typedef int16_t         ri16;
typedef int32_t         ri32;
typedef int64_t         ri64;

typedef float           rf32;
typedef double          rf64;

typedef unsigned char   byte;


/******************************************************************************
 * byte types
 *****************************************************************************/
template <class Type,int Size>
struct Array_
{
    Array_(){}
    Array_(Type def){
        for(int i=0;i<Size;i++)
            data[i]=def;
    }
    Type data[Size];

    inline friend std::ostream& operator <<(std::ostream& os,const Array_<Type,Size>& p)
    {
        for(int i=0;i<Size;i++)
            os<<p.data[i]<<" ";
        return os;
    }

    inline friend std::istream& operator >>(std::istream& os,const Array_<Type,Size>& p)
    {
        for(int i=0;i<Size;i++)
            os>>p.data[i];
        return os;
    }

    const int size(){return Size;}
};


template <int Size=2>
struct Byte
{
    byte data[Size];
};


/******************************************************************************
 * graphics types
 *****************************************************************************/
// color
struct RTK_Color {
    ru8   r, g, b, a;

    RTK_Color() {
        r = 0x00;
        g = 0x00;
        b = 0x00;
        a = 0xFF;
    }

    void set(ru8 _r, ru8 _g, ru8 _b, ru8 _a=0xff) {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <class Precision>
struct Point2_
{
    Point2_(){}
    Point2_(Precision x_,Precision y_):x(x_),y(y_){}
    inline Precision& operator[](int index)const
    {
        return ((Precision*)this)[index];
    }

    friend Point2_ operator + (const Point2_& a,const Point2_& b)
    {
        return Point2_(a.x+b.x,a.y+b.y);
    }

    friend Point2_ operator - (const Point2_& a,const Point2_& b)
    {
        return Point2_(a.x-b.x,a.y-b.y);
    }

    friend Point2_ operator -(const Point2_& a)
    {
        return Point2_(-a.x,-a.y);
    }

    friend Precision operator * (const Point2_& a,const Point2_& b)
    {
        return (a.x*b.x+a.y*b.y);
    }

    friend Point2_ operator * (const Precision& a,const Point2_& b)
    {
        return Point2_(a*b.x,a*b.y);
    }

    friend Point2_ operator * (const Point2_& b,const Precision& a)
    {
        return Point2_(a*b.x,a*b.y);
    }

    friend Point2_ operator / (const Point2_& a,const Precision& b)
    {
        return (1./b)*a;
    }

    inline Precision norm()const
    {
        return sqrt(x*x+y*y);
    }

    inline Point2_<Precision> normalize()const
    {
        if(x*x+y*y!=0)
            return (*this)*(1./norm());
        else
            return Point2_<Precision>(0,0);
    }

    friend inline std::ostream& operator <<(std::ostream& os,const Point2_& p)
    {
        os<<p.x<<" "<<p.y;
        return os;
    }

    friend inline std::istream& operator >>(std::istream& is,Point2_& p)
    {
        is>>p.x>>p.y;
        return is;
    }
    Precision x,y;
};
typedef Point2_<double> Point2d;
typedef Point2_<float>  Point2f;
typedef Point2_<int>    Point2i;


template <class Precision>
struct Point3_
{
    Point3_():x(0),y(0),z(0){}

    Point3_(Precision x_,Precision y_,Precision z_):x(x_),y(y_),z(z_){}

    inline Precision& operator[](int index)const
    {
        return ((Precision*)this)[index];
    }

    inline Precision norm()const
    {
        return sqrt(x*x+y*y+z*z);
    }

    inline Precision dot(const Point3_& a)
    {
        return x*a.x+y*a.y+z*a.z;
    }

    inline Point3_<Precision> cross(const Point3_& a)
    {
        return Point3_<Precision>(y*a.z-z*a.y,z*a.x-x*a.z,x*a.y-y*a.x);
    }

    inline Point3_<Precision> normalize()const
    {
        if(x*x+y*y+z*z!=0)
            return (*this)*(1./norm());
        else
            return Point3_<Precision>(0,0,0);
    }

    friend inline std::ostream& operator <<(std::ostream& os,const Point3_& p)
    {
        os<<p.x<<" "<<p.y<<" "<<p.z;
        return os;
    }

    friend inline std::istream& operator >>(std::istream& is,Point3_& p)
    {
        is>>p.x>>p.y>>p.z;
        return is;
    }

    friend Point3_ operator + (const Point3_& a,const Point3_& b)
    {
        return Point3_(a.x+b.x,a.y+b.y,a.z+b.z);
    }

    friend Point3_ operator - (const Point3_& a,const Point3_& b)
    {
        return Point3_(a.x-b.x,a.y-b.y,a.z-b.z);
    }

    friend Point3_ operator -(const Point3_& a)
    {
        return Point3_(-a.x,-a.y,-a.z);
    }

    friend Precision operator * (const Point3_& a,const Point3_& b)
    {
        return (a.x*b.x+a.y*b.y+a.z*b.z);
    }

    friend Point3_<Precision> operator ^ (const Point3_& a,const Point3_& b)
    {
        return Point3_(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x);
    }

    friend Point3_ operator * (const Precision& a,const Point3_& b)
    {
        return Point3_(a*b.x,a*b.y,a*b.z);
    }

    friend Point3_ operator * (const Point3_& b,const Precision& a)
    {
        return Point3_(a*b.x,a*b.y,a*b.z);
    }

    friend Point3_ operator / (const Point3_& a,const Precision& b)
    {
        return (1./b)*a;
    }

    friend inline bool operator < (const Point3_& a,const Point3_ b)
    {
        return a.x<b.x;
    }

    template <typename Scalar>
    operator Point3_<Scalar>()
    {
        return Point3_<Scalar>(x,y,z);
    }

#ifdef HAS_TOON
    operator TooN::Vector<3,Precision>& ()
    {
        return *((TooN::Vector<3,Precision>*)this);
    }
#endif

    Precision x,y,z;
};

typedef Point3_<unsigned char>  Point3ub;
typedef Point3_<float>  Point3f;
typedef Point3_<double> Point3d;
typedef Point3_<int>    Point3i;


} // end namespace pi

#endif // __RTK_TYPE_H__

