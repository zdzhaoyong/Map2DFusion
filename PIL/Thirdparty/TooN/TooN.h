//-*- c++ -*-

// Copyright (C) 2005,2009 Tom Drummond (twd20@cam.ac.uk),
// Ed Rosten (er258@cam.ac.uk), Gerhard Reitmayr (gr281@cam.ac.uk)
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


#ifndef TOON_INCLUDE_TOON_H
#define TOON_INCLUDE_TOON_H
#include <iostream>
#include <cstdlib>
#include <limits>
#include <new>
#include <utility>
#include <vector>
#include <complex>
#include <TooN/internal/config.hh>
#include <TooN/internal/typeof.hh>
#include <TooN/internal/deprecated.hh>

#if defined TOON_NDEBUG || defined NDEBUG
	#define TOON_NDEBUG_MISMATCH
	#define TOON_NDEBUG_SLICE
	#define TOON_NDEBUG_SIZE
	#define TOON_NDEBUG_FILL
#endif

#ifdef TOON_INITIALIZE_RANDOM
#include <ctime>
#endif

#ifdef TOON_USE_LAPACK
	#ifndef TOON_DETERMINANT_LAPACK
		#define TOON_DETERMINANT_LAPACK 35
	#endif
#endif

///Everything lives inside this namespace
namespace TooN {

#ifdef TOON_TEST_INTERNALS
	namespace Internal
	{
		struct BadIndex{};
		struct SliceError{};
		struct StaticSliceError{};
		struct SizeMismatch{};
		struct StaticSizeMismatch{};
		struct VectorOverfill{};
		struct StaticVectorOverfill{};
		struct MatrixOverfill{};
		struct StaticMatrixOverfill{};
		struct Underfill{};
	}
#endif
	
	using std::numeric_limits;
	///Is a number a field? i.e., +, -, *, / defined.
	///
	///Specialize this to make TooN work properly with new types. See, for example functions/fadbad.h
	///
	///Specifically, is the type on the default field. Because of the conversion rules
	///of C++, TooN uses a rather loose definition of field. The a type is on the
	///default field if arithmetic works between it and any builtin numeric type. So, for
	///instance unsigned char and float are considered to be on the default field even though 
	///by themselves they form very different fields.
	///
	///See also Field.
	///
	///The reason for this is so that <code> makeVector(1, 0, 0) </code> behaves as expected
	///even though it will actually be a <code> Vector<3,int></code>.
	///
	///
	///
	///The primary reason for this is to allow SFINAE to work properly.
	///This is required if there are the following two functions:
	///@code 
	///   Vector<> * X  //Generic type X
	///   Vector<> * DiagonalMatrix<>
	///@endcode
	///If one of the functions is a substitution failure, then it will be
	///ignored, allowing the functions to coexist happily. However, not all
	///types of failure are substitution failures. TooN's type deduction happens
	///when determining the return type of the function. This is too early, so
	///the wrong kind of error in the return type deduction causes an error, rather
	///than a substitution failure. The IsField mechanism makes it the right kind of
	///error, thereby allowing a substitution failuer to occur.
	///
	///@internal
	///Internal::One is on the same field of any type which is also a field.
	///@ingroup gLinAlg
	template<class C> struct IsField
	{
		static const int value = numeric_limits<C>::is_specialized; ///<Is C a field?
	};

	template<class C> struct IsField<std::complex<C> >
	{
		static const int value = numeric_limits<C>::is_specialized; ///<Is C a field?
	};
	
	///Specialized for const types
	///@internal
	///Internal::Field determines if two classes are in the same field.
	///@ingroup gLinAlg
	template<class C> struct IsField<const C>
	{
		static const int value = IsField<C>::value; ///<Is C a field?
	};

	template<class C, class D> struct These_Types_Do_Not_Form_A_Field;
	
	///@internal
	///@brief The namaespace holding all the internal code.
	namespace Internal
	{
		///@internal
		///@brief Maximum number of bytes to be allocated on the stack.
		///new is used above this number.
		static const unsigned int max_bytes_on_stack=1000;
		///@internal
	 	///@brief A tag used to indicate that a slice is being constructed.
		///@ingroup gInternal
		struct Slicing{};
		template<int RowStride, int ColStride> struct Slice;
		template<int Size, typename Precision, int Stride, typename Mem> struct GenericVBase;
	}

	template<int Size, class Precision, class Base> struct Vector;
	template<int Rows, int Cols, class Precision, class Base> struct Matrix;
	template<int Size, class Precision, class Base> struct DiagonalMatrix;


	#ifdef DOXYGEN_INCLUDE_ONLY_FOR_DOCS
		///@internal
		///@brief This is a struct used heavily in TooN internals.
		///
		///They have two main uses. The first use is in construction and is completely hidden.
		///For an expression such as a+b, the return value of operator+ will be constructed in
		///place in the return statement, to prevent excessive copying and calls to new/delete.
		///
		///The other use is much more visible and is for objects such as TooN::Zeros and TooN::Idendity .
		///
		///The features allowed (construction, addition, etc) depend on the members present. 
		///For simplicity, general arguments are given below. If members are non-general, then the 
		///operators will simply not be applicable to all vectors or matrices.
		///
		///Operators belong to any of a number of categories depending on the members they provide.
		///The categories are:
		///
		/// - Sized operators
		///   - These know their own size and provide. 
		///     The sizes are used only in construction of dynamic vectors or
		///     matrices.
		/// - Sizeable operators
		///   - Sizeable operators are able to generate a sized operator of the same sort.
		/// - Scalable operators
		///   - These can be multiplied and divided by scalars.
		///
		///@ingroup gInternal
		template<typename T> struct Operator{
			///@name Members in the category ``sized operators''
			///@{

			///This must be provided in order to construct dynamic vectors.
			int size() const;
			///This along with num_cols() must be present in order to construct matrices.
			int num_rows() const; 
			///This along with num_rows() must be present in order to construct matrices.
			int num_cols() const;
			///@}
			
			///@name Members used by Vector
			///@{

			///This function must be present for construction and assignment
			///of vectors to work.
			template<int Size, class Precision, class Base>
			void eval(Vector<Size, Precision, Base>& v) const;

			///This must be present for vector += operator
			template <int Size, typename P1, typename B1> 
			void plusequals(Vector<Size, P1, B1>& v) const;

			///This must be present for vector -= operator
			template <int Size, typename P1, typename B1>
			void minusequals(Vector<Size, P1, B1>& v) const;

			///This function must be present for vector + operator
			///and operator + vector
			template <int Size, typename P1, typename B1> 
			Operator<T> add(const Vector<Size, P1, B1>& v) const;

			///This function must be present for vector - operator
			template <int Size, typename P1, typename B1> 
			Operator<T> rsubtract(const Vector<Size, P1, B1>& v) const;

			///This function must be present for operator - vector
			template <int Size, typename P1, typename B1> 
			Operator<T> lsubtract(const Vector<Size, P1, B1>& v) const;

			///@}

			///@name Members used by Matrix
			///@{
			///This function must be present for construction and assignment
			///of matrices to work.
			template<int R, int C, class P, class B>
			void eval(Matrix<R,C,P,B>& m) const; 

			///This function must be present for matrix + operator
			///and operator + matrix
			template <int Rows, int Cols, typename P1, typename B1> 
			Operator<T> add(const Matrix<Rows,Cols, P1, B1>& m) const;


			///This function must be present for matrix - operator
			template <int Rows, int Cols, typename P1, typename B1> 
			Operator<T> rsubtract(const Matrix<Rows,Cols, P1, B1>& m) const;

			///This function must be present for operator - matrix
			template <int Rows, int Cols, typename P1, typename B1> 
			Operator<T> lsubtract(const Matrix<Rows,Cols, P1, B1>& m) const;

			///This must be present for matrix += operator
			template <int Rows, int Cols, typename P1, typename B1> 
			void plusequals(Matrix<Rows,Cols, P1, B1>& m) const;

			///This must be present for matrix -= operator
			template <int Rows, int Cols, typename P1, typename B1> 
			void minusequals(Matrix<Rows,Cols, P1, B1>& m) const;
			///@}


			///@name Members in the category ``sizeable oberators''
			///@{

			///Create an operator that knows its size.
			///Suitable for vectors and square matrices.
			Operator<T> operator()(int size) const;
			
			///Create an operator that knows its size, suitable for matrices.
			Operator<T> operator()(int num_rows, int num_cols) const;
			///@}
			
			///@name Members in the category ``scalable operators''
			///@{
			typedef T Precision; ///<Precision of the operator's scale.
			
			///Scale the operator by a scalar and return a new opeator.
			template<class Pout, class Pmult> Operator<Internal::Identity<Pout> > scale_me(const Pmult& m) const
			{
				return Operator<Internal::Identity<Pout> >(val*m);
			}
			///@}

		};
	#else
		template<typename T> struct Operator;
	#endif

	///Template size value used to indicate dynamically sized vectors and matrices.
	static const int Dynamic = -1;
	static const int Resizable = -0x7fffffff;

	namespace Internal
	{
		template<int i, int j> struct SimpleSizer{static const int size=i;};
		template<int i> struct SimpleSizer<Dynamic, i>{static const int size=i;};
		template<int i> struct SimpleSizer<i, Dynamic>{static const int size=i;};
		template<> struct SimpleSizer<Dynamic, Dynamic>    {static const int size=-1;};

		template<int i> struct IsStatic
		{
			static const bool is = (i!=Dynamic && i != Resizable);
		};

		//Choose an output size, given a pair of input sizes. Be static if possible.
		template<int i, int j=i> struct Sizer{
			static const int size=SimpleSizer<Sizer<i>::size, Sizer<j>::size>::size;
		};

		//Choose an output size, given an input size. Be static if possible.
		//Otherwise be dynamic. Never generate a resizable vector.
		template<int i> struct Sizer<i,i>{
			static const int size = IsStatic<i>::is?i:Dynamic;
		};
	}
	
	///All TooN classes default to using this precision for computations and storage.
#ifndef TOON_DEFAULT_PRECISION
	typedef double DefaultPrecision;
#else
	typedef TOON_DEFAULT_PRECISION DefaultPrecision;
#endif

#if defined  TOON_FORTRAN_INTEGER && defined TOON_CLAPACK
	#error Error: both TOON_FORTRAN_INTEGER and TOON_CLAPACK defined
#elif defined TOON_CLAPACK
	typedef long FortranInteger;
#elif defined TOON_FORTRAN_INTEGER
	typedef TOON_FORTRAN_INTEGER FortranInteger;
#else
	typedef int FortranInteger;
#endif

}

#include <TooN/internal/debug.hh>

#include <TooN/internal/introspection.hh>


#include <TooN/internal/dchecktest.hh>
#include <TooN/internal/allocator.hh>

#include <TooN/internal/size_mismatch.hh>
#include <TooN/internal/overfill_error.hh>
#include <TooN/internal/slice_error.hh>

#include <TooN/internal/comma.hh>

#include <TooN/internal/vbase.hh>
#include <TooN/internal/vector.hh>
	
#include <TooN/internal/mbase.hh>
#include <TooN/internal/matrix.hh>
#include <TooN/internal/reference.hh>

#include <TooN/internal/make_vector.hh>
#include <TooN/internal/operators.hh>
	
#include <TooN/internal/objects.h>

#include <TooN/internal/diagmatrix.h>

#include <TooN/internal/data.hh>
#include <TooN/internal/data_functions.hh>

#include <TooN/helpers.h>
#include <TooN/determinant.h>

#endif
