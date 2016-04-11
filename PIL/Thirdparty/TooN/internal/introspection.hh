// -*- c++ -*-

// Copyright (C) 2012
// Ed Rosten (er258@cam.ac.uk)
//
// This file is part of the TooN Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

// Allocators always copy data on copy construction.
//
// When a Vector/Matrix is constructed from a different, but compatible type
// copying is done at a much higher level: above the level that knows how the
// data is laid out in memory.
//
// At this level, copy construction is required since it is only known here 
// whether data or a reference to data should be copied.


///Pretty generic SFINAE introspection generator
///@ingroup gInternal
namespace TooN{
namespace Internal{
//Fake function to pretend to return an instance of a type
template<class C>
const C& get();

//Two typedefs guaranteed to have different sizes
typedef char OneSized[1];
typedef char TwoSized[2];


//Class to give us a size 2 return value or fail on 
//substituting S
template<int S> 
struct SFINAE_dummy
{
        typedef TwoSized Type;
};


#define TOON_CREATE_THING_DETECTOR(Y, X, Z) \
template<class C>\
struct Has_##X##_##Z\
{\
        static OneSized& detect_##X##_##Z(...);\
\
        template<class S>\
        static typename SFINAE_dummy<sizeof(Y)>::Type& detect_##X##_##Z(const S&);\
\
        static const bool Has = sizeof(detect_##X##_##Z(get<C>())) == 2;\
}

#define TOON_CREATE_MEMBER_DETECTOR(X)  TOON_CREATE_THING_DETECTOR(S::X, X, Member)
#define TOON_CREATE_METHOD_DETECTOR(X)  TOON_CREATE_THING_DETECTOR(&S::X, X, Method)
#define TOON_CREATE_TYPEDEF_DETECTOR(X) TOON_CREATE_THING_DETECTOR(typename S::X, X, Typedef)

}}
