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

namespace Internal
{
// As usual, a positive integer means static and -1 means dynamic.
// The new case is that for strides, -2 means that the stride is 
// the same as num_cols/num_rows, which must be dynamically sized.

template<int, int, class, int, int, class> struct GenericMBase;

////////////////////////////////////////////////////////////////////////////////
//Closure used to acquire strides
//-1 means dynamic stride
//-2 means dynamic stride is tied to size for a normal matrix
template<int RowStride, int ColStride> struct Slice
{
  
	template<int Rows, int Cols, class Precision> struct MLayout: public GenericMBase<Rows, Cols, Precision, RowStride, ColStride, MatrixSlice<Rows, Cols, Precision> >
	{
		MLayout(Precision* p, int rows, int cols, int rowstride, int colstride)
			:GenericMBase<Rows,Cols,Precision,RowStride,ColStride,MatrixSlice<Rows, Cols, Precision> >(p, rows, cols, rowstride, colstride)
		{
		}
	};
};


template<int Rows, int Cols, bool D = (Rows == Dynamic || Cols == Dynamic)>
struct DiagSize
{
	static const int size = Dynamic;
};
template<int Rows, int Cols>
struct DiagSize<Rows, Cols, 0>
{
	static const int size = (Rows<Cols?Rows:Cols);
};

template<int Rs, int Cs, bool D = (Rs == Dynamic || Cs == Dynamic)>
struct DiagStride
{
	static const int stride = Dynamic;
};
template<int Rs, int Cs>
struct DiagStride<Rs, Cs, 0>
{
	static const int stride = Rs + Cs;
};


template<int Rows, int Cols, class Precision, int RowStride, int ColStride, class Mem> struct GenericMBase
	: public Mem, 
	RowStrideHolder<RowStride>,
	ColStrideHolder<ColStride>
{
	//Slices can never have tied strides
	static const int SliceRowStride = RowStride == -2?-1: RowStride;
	static const int SliceColStride = ColStride == -2?-1: ColStride;
	
	typedef Slice<SliceRowStride,SliceColStride> SliceBase;

	int rowstride() const {
		if(RowStride == -2) { //Normal tied stride
			return num_cols();
		} else {
			return RowStrideHolder<RowStride>::stride();
		}
	}

	int colstride() const {
		if(ColStride == -2) { //Normal tied stride
			return num_rows();
		} else {
			return ColStrideHolder<ColStride>::stride();
		}
	}

	//Optional constructors
	GenericMBase(){}

	GenericMBase(Precision* p)
	:Mem(p)
	{}


	GenericMBase(Precision* p, int r, int c, int rowstride, int colstride)
	:Mem(p, r, c),
	 RowStrideHolder<RowStride>(rowstride),
	 ColStrideHolder<ColStride>(colstride) 
	{}

	GenericMBase(int r, int c)
	:Mem(r, c) {}

	template<class Op>
	GenericMBase(const Operator<Op>& op)
		: Mem(op),
		  RowStrideHolder<RowStride>(op),
		  ColStrideHolder<ColStride>(op)
	{}

	using Mem::my_data;
	using Mem::num_cols;
	using Mem::num_rows;

	Precision& operator()(int r, int c){
		Internal::check_index(num_rows(), r);
		Internal::check_index(num_cols(), c);
		return my_data[r*rowstride() + c*colstride()];
	}

	const Precision& operator()(int r, int c) const {
		Internal::check_index(num_rows(), r);
		Internal::check_index(num_cols(), c);
		return my_data[r*rowstride() + c*colstride()];
	}

	Precision& operator[](const std::pair<int, int>& index) {
		Internal::check_index(num_rows(), index.first);
		Internal::check_index(num_cols(), index.second);
		return (*this)(index.first, index.second);
	}

	const Precision& operator[](const std::pair<int, int>& index) const {
		Internal::check_index(num_rows(), index.first);
		Internal::check_index(num_cols(), index.second);
		return (*this)(index.first, index.second);
	}

	// this is the type of vector obtained by [ ]
	typedef Vector<Cols, Precision, SliceVBase<SliceColStride> > Vec;
	typedef Vector<Cols, const Precision, SliceVBase<SliceColStride> > CVec;
	
	Vec operator[](int r) {
		Internal::check_index(num_rows(), r);
		return Vec(my_data + rowstride()* r, num_cols(), colstride(), Slicing());
	}

	const CVec operator[](int r) const {
		Internal::check_index(num_rows(), r);
		return CVec(my_data + rowstride()* r, num_cols(), colstride(), Slicing());
	}

	
	//Generic matrix slicing
	template<int Rstart, int Cstart, int Rlength, int Clength>
	Matrix<Rlength, Clength, Precision, Slice<SliceRowStride,SliceColStride> > slice(int rs, int cs, int rl, int cl){
		Internal::CheckSlice<Rows, Rstart, Rlength>::check(num_rows(), rs, rl);
		Internal::CheckSlice<Cols, Cstart, Clength>::check(num_cols(), cs, cl);

		//Always pass the size and stride as a run-time parameter. It will be ignored
		//by SliceHolder (above) if it is statically determined.
		return Matrix<Rlength, Clength, Precision, Slice<SliceRowStride,SliceColStride> >(
		       my_data+rowstride()*(Rstart==Dynamic?rs:Rstart) + colstride()*(Cstart==Dynamic?cs:Cstart), 
			   Rlength==Dynamic?rl:Rlength, 
			   Clength==Dynamic?cl:Clength, 
			   rowstride(), colstride(), Slicing());
	}

	template<int Rstart, int Cstart, int Rlength, int Clength>
	const Matrix<Rlength, Clength, const Precision, Slice<SliceRowStride,SliceColStride> > slice(int rs, int cs, int rl, int cl) const{
		Internal::CheckSlice<Rows, Rstart, Rlength>::check(num_rows(), rs, rl);
		Internal::CheckSlice<Cols, Cstart, Clength>::check(num_cols(), cs, cl);

		//Always pass the size and stride as a run-time parameter. It will be ignored
		//by SliceHolder (above) if it is statically determined.
		return Matrix<Rlength, Clength, const Precision, Slice<SliceRowStride,SliceColStride> >(
		       my_data+rowstride()*(Rstart==Dynamic?rs:Rstart) + colstride()*(Cstart==Dynamic?cs:Cstart), 
			   Rlength==Dynamic?rl:Rlength, 
			   Clength==Dynamic?cl:Clength, 
			   rowstride(), colstride(), Slicing());
	}

	//Special cases of slicing
	template<int Rstart, int Cstart, int Rlength, int Clength>
	Matrix<Rlength, Clength, Precision, Slice<SliceRowStride,SliceColStride> > slice()
	{
		//Extra checking in the static case
		Internal::CheckSlice<Rows, Rstart, Rlength>::check();
		Internal::CheckSlice<Cols, Cstart, Clength>::check();
		return slice<Rstart, Cstart, Rlength, Clength>(Rstart, Cstart, Rlength, Clength);
	}

	template<int Rstart, int Cstart, int Rlength, int Clength>
	const Matrix<Rlength, Clength, const Precision, Slice<SliceRowStride,SliceColStride> > slice() const
	{
		Internal::CheckSlice<Rows, Rstart, Rlength>::check();
		Internal::CheckSlice<Cols, Cstart, Clength>::check();
		return slice<Rstart, Cstart, Rlength, Clength>(Rstart, Cstart, Rlength, Clength);
	}

	Matrix<-1, -1, Precision, Slice<SliceRowStride,SliceColStride> > slice(int rs, int cs, int rl, int cl){
		return slice<Dynamic, Dynamic, Dynamic, Dynamic>(rs, cs, rl, cl);
	}

	const Matrix<-1, -1, const Precision, Slice<SliceRowStride,SliceColStride> > slice(int rs, int cs, int rl, int cl) const {
		return slice<Dynamic, Dynamic, Dynamic, Dynamic>(rs, cs, rl, cl);
	}

	//Other slice related functions.
	Matrix<Cols, Rows, Precision, Slice<SliceColStride,SliceRowStride> > T(){
		return Matrix<Cols, Rows, Precision, Slice<SliceColStride,SliceRowStride> >(my_data, num_cols(), num_rows(), colstride(), rowstride(), Slicing());
	}

	const Matrix<Cols, Rows, const Precision, Slice<SliceColStride,SliceRowStride> > T() const{
		return Matrix<Cols, Rows, const Precision, Slice<SliceColStride,SliceRowStride> >(my_data, num_cols(), num_rows(), colstride(), rowstride(), Slicing());
	}

	static const int DiagSize = Internal::DiagSize<Rows, Cols>::size;
	static const int DiagStride = Internal::DiagStride<SliceRowStride, SliceColStride>::stride;

	Vector<DiagSize, Precision, SliceVBase<DiagStride> > diagonal_slice()
	{
		return Vector<DiagSize, Precision, SliceVBase<DiagStride> >(my_data, std::min(num_cols(), num_rows()), rowstride() + colstride(), Slicing());
	}

	Vector<DiagSize, const Precision, SliceVBase<DiagStride> > diagonal_slice() const 
	{
		return Vector<DiagSize, const Precision, SliceVBase<DiagStride> >(my_data, std::min(num_cols(), num_rows()), rowstride() + colstride(), Slicing());
	}
};

}

////////////////////////////////////////////////////////////////////////////////
//
// Classes for Matrices owning memory
//
//
struct RowMajor
{
	template<int Rows, int Cols, class Precision> struct MLayout: public Internal::GenericMBase<Rows, Cols, Precision, (Cols==-1?-2:Cols), 1, Internal::MatrixAlloc<Rows, Cols, Precision> >
	{
		//Optional constructors.
		
		MLayout(){}

		MLayout(int rows, int cols)
			:Internal::GenericMBase<Rows, Cols, Precision, (Cols == -1 ? -2 : Cols), 1, Internal::MatrixAlloc<Rows, Cols, Precision> >(rows, cols)
		{}

		template<class Op>
		MLayout(const Operator<Op>& op)
			:Internal::GenericMBase<Rows, Cols, Precision, (Cols == -1 ? -2 : Cols), 1, Internal::MatrixAlloc<Rows, Cols, Precision> >(op)
		{}

	};
};

struct ColMajor
{
	template<int Rows, int Cols, class Precision> struct MLayout: public Internal::GenericMBase<Rows, Cols, Precision, 1, (Rows==-1?-2:Rows), Internal::MatrixAlloc<Rows, Cols, Precision> >
	{
		//Optional constructors.
		
		MLayout(){}

		MLayout(int rows, int cols)
		:Internal::GenericMBase<Rows, Cols, Precision, 1, (Rows == -1 ? -2 : Rows), Internal::MatrixAlloc<Rows, Cols, Precision> >(rows, cols)
		{}

		template<class Op>
		MLayout(const Operator<Op>& op)
			:Internal::GenericMBase<Rows, Cols, Precision, 1, (Rows == -1 ? -2 : Rows), Internal::MatrixAlloc<Rows, Cols, Precision> >(op)
		{}

	};
};

}

