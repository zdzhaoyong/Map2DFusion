// -*- c++ -*-

// Copyright (C) 2009 Tom Drummond (twd20@cam.ac.uk),
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

namespace TooN {

// class to generate compile time error
// general case which doesn't exist
template<int Size1, int Size2>
struct SizeMismatch_;

// special cases which do exist
template<int Size>
struct SizeMismatch_<Size,Size>{
  static inline void test(int, int){}
};

template<int Size>
struct SizeMismatch_<Dynamic,Size>{
  static inline void test(int size1, int size2){
    if(size1!=size2){
	  #ifdef TOON_TEST_INTERNALS
	  	throw Internal::SizeMismatch();
	  #elif !defined TOON_NDEBUG_SIZE
		  std::cerr << "TooN Size Mismatch" << std::endl;
		  std::abort();
	  #endif
    }
  }
};

template<int Size>
struct SizeMismatch_<Size,Dynamic>{
  static inline void test(int size1, int size2){
    if(size1!=size2){
	  #ifdef TOON_TEST_INTERNALS
	  	throw Internal::SizeMismatch();
	  #elif !defined TOON_NDEBUG_SIZE
		  std::cerr << "TooN Size Mismatch" << std::endl;
		  std::abort();
	  #endif
    }
  }
};

template <>
struct SizeMismatch_<Dynamic,Dynamic>{
  static inline void test(int size1, int size2){
    if(size1!=size2){
	  #ifdef TOON_TEST_INTERNALS
	  	throw Internal::SizeMismatch();
	  #elif !defined TOON_NDEBUG_SIZE
		  std::cerr << "TooN Size Mismatch" << std::endl;
		  std::abort();
	  #endif
    }
  }
};

#if 0
namespace Internal
{
	struct BadSize;
}
#endif

#ifdef TOON_TEST_INTERNALS
template<int Size1, int Size2>
struct SizeMismatch_
{
	static inline void test(int, int)
	{
		throw Internal::StaticSizeMismatch();
	}
};
#endif

template<int Size1, int Size2>
struct SizeMismatch
{
	static inline void test(int s1, int s2)
	{
		SizeMismatch_< (Size1 == Dynamic || Size1 == Resizable)?Dynamic:Size1,
		               (Size2 == Dynamic || Size2 == Resizable)?Dynamic:Size2 >::test(s1, s2);
	}
};

}
