// -*- c++ -*-

// Copyright (C) 2009 Ed Rosten (er258@cam.ac.uk)
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



#ifdef TOON_TYPEOF_DECLTYPE
	#define TOON_TYPEOF(X) decltype((X))
#elif defined TOON_TYPEOF_TYPEOF
	#define TOON_TYPEOF(X) typeof((X))
#elif defined TOON_TYPEOF___TYPEOF__
	#define TOON_TYPEOF(X) __typeof__((X))
#elif defined TOON_TYPEOF_BOOST
    #include <boost/typeof/typeof.hpp>
	#define TOON_TYPEOF(X) BOOST_TYPEOF((X))
#elif (__cplusplus > 199711L ) && ! defined TOON_TYPEOF_BUILTIN
    #define TOON_TYPEOF(X) decltype((X))
#elif defined __GNUC__ && ! defined TOON_TYPEOF_BUILTIN
    #define TOON_TYPEOF(X) typeof((X))
#else
	#include <complex>
	namespace TooN{
		namespace Internal{
			#include <TooN/internal/builtin_typeof.h>
		}
	}
	#define TOON_TYPEOF(X) typename Internal::DeEnumerate<sizeof Internal::enumerate(X)>::type
#endif
