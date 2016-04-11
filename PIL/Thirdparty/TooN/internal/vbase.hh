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

namespace Internal{
template<int Size, class Precision, int Stride, class Mem> struct GenericVBase;

////////////////////////////////////////////////////////////////////////////////
//
// Slice holding class
//
struct Default{};

template<int Stride, class Ptr=Default, class CPtr=Default, class Ref=Default, class CRef=Default>
struct SliceVBase {

	// this class is really just a typedef
	template<int Size, typename Precision>
	struct VLayout
		: public GenericVBase<Size, Precision, Stride, VectorSlice<Size, Precision, Ptr, CPtr, Ref, CRef> > {
		typedef typename VectorSlice<Size, Precision, Ptr, CPtr, Ref, CRef>::PointerType PointerType;
	
		VLayout(PointerType d, int length, int stride)
			:GenericVBase<Size, Precision, Stride, VectorSlice<Size, Precision, Ptr, CPtr, Ref, CRef> >(d, length, stride){
		}

		template<class Op>
		VLayout(const Operator<Op>& op)
			:GenericVBase<Size, Precision, Stride, VectorSlice<Size, Precision> >(op) {}
	};

};

template<int Stride>
struct SliceVBase<Stride, Default, Default, Default, Default> {

	// this class is really just a typedef
	template<int Size, typename Precision>
	struct VLayout
		: public GenericVBase<Size, Precision, Stride, VectorSlice<Size, Precision> > {

		typedef typename VectorSlice<Size, Precision>::PointerType PointerType;
	
		VLayout(PointerType d, int length, int stride)
			:GenericVBase<Size, Precision, Stride, VectorSlice<Size, Precision> >(d, length, stride){
		}

		template<class Op>
		VLayout(const Operator<Op>& op)
			:GenericVBase<Size, Precision, Stride, VectorSlice<Size, Precision> >(op) {}
	};

};

////////////////////////////////////////////////////////////////////////////////
//
// Classes for Vectors owning memory
//

struct VBase {

	// this class is really just a typedef
	template<int Size, class Precision>
	struct VLayout 
		: public GenericVBase<Size, Precision, 1, VectorAlloc<Size, Precision> > {
	
		VLayout(){}

		VLayout(int s)
			:GenericVBase<Size, Precision, 1, VectorAlloc<Size, Precision> >(s)
		{}

		template<class Op>
		VLayout(const Operator<Op>& op)
			:GenericVBase<Size, Precision, 1, VectorAlloc<Size, Precision> >(op) {}
	};
};

////////////////////////////////////////////////////////////////////////////////
//
// Generic implementation
//

template<int Size, typename Precision, int Stride, typename Mem> struct GenericVBase: public Mem, public StrideHolder<Stride>
{	
	int stride() const{
		return StrideHolder<Stride>::stride();
	}

	//Optional constuctors
	GenericVBase(){}

	GenericVBase(int s)
	:Mem(s)
	{}

	typedef typename Mem::PointerType PointerType;
	typedef typename Mem::ConstPointerType ConstPointerType;
	typedef typename Mem::ReferenceType ReferenceType;
	typedef typename Mem::ConstReferenceType ConstReferenceType;

	GenericVBase(PointerType d, int length, int stride)
	:Mem(d, length),StrideHolder<Stride>(stride){
	}
	
	template<class Op>
	GenericVBase(const Operator<Op> & op) : Mem(op), StrideHolder<Stride>(op) {}

	using Mem::data;
	using Mem::size;

	ReferenceType operator[](int i) {
		Internal::check_index(size(), i);
		return data()[i * stride()];
	}

	ConstReferenceType operator[](int i) const {
		Internal::check_index(size(), i);
		return data()[i * stride()];
	}

	typedef SliceVBase<Stride, PointerType, ConstPointerType, ReferenceType, ConstReferenceType> SliceBase;
	typedef SliceVBase<Stride, ConstPointerType, ConstPointerType, ConstReferenceType, ConstReferenceType> ConstSliceBase;


	//Completely generic Vector slice operations below:
	template<int Start, int Length> 
	Vector<Length, Precision, SliceBase> slice(int start, int length){
		Internal::CheckSlice<Size, Start, Length>::check(size(), start, length);	
		return Vector<Length, Precision, SliceBase>(data() + stride() * (Start==Dynamic?start:Start), Length==Dynamic?length:Length, stride(), Slicing());
	}

	template<int Start, int Length> 
	const Vector<Length, const Precision, ConstSliceBase> slice(int start, int length) const{
		Internal::CheckSlice<Size, Start, Length>::check(size(), start, length);	
		return Vector<Length, const Precision, ConstSliceBase>(data() + stride() * (Start==Dynamic?start:Start), Length==Dynamic?length:Length, stride(), Slicing());
	}

	

	//Special case slice operations
	template<int Start, int Length> Vector<Length, Precision, SliceBase> slice(){
		Internal::CheckSlice<Size, Start, Length>::check();
		return slice<Start, Length>(Start, Length);
	}

	template<int Start, int Length> const Vector<Length, const Precision, ConstSliceBase> slice() const {
		Internal::CheckSlice<Size, Start, Length>::check();
		return slice<Start, Length>(Start, Length);
	}

	Vector<Dynamic, Precision, SliceBase> slice(int start, int length){
		return slice<Dynamic, Dynamic>(start, length);
	}

	const Vector<Dynamic, const Precision, ConstSliceBase> slice(int start, int length) const{
		return slice<Dynamic, Dynamic>(start, length);
	}
		
	//Other slices below
	const Matrix<1, Size, const Precision, Slice<1,Stride> > as_row() const{
		return Matrix<1, Size, const Precision, Slice<1,Stride> >(data(), 1, size(), 1, stride(), Slicing());
	}

	Matrix<1, Size, Precision, Slice<1,Stride> > as_row(){
		return Matrix<1, Size, Precision, Slice<1,Stride> >(data(), 1, size(), 1, stride(), Slicing());
	}

	const Matrix<Size, 1, const Precision, Slice<Stride,1> > as_col() const{
		return Matrix<Size, 1, const Precision, Slice<Stride,1> >(data(), size(), 1, stride(), 1, Slicing());
	}

	Matrix<Size, 1, Precision, Slice<Stride,1> > as_col(){
		return Matrix<Size, 1, Precision, Slice<Stride,1> >(data(), size(), 1, stride(), 1, Slicing());
	}

	typedef Vector<Size, Precision, SliceBase> as_slice_type;
	
	Vector<Size, Precision, SliceBase> as_slice(){                 
		return Vector<Size, Precision, SliceBase>(data(), size(), stride(), Slicing());         
	}

	const Vector<Size, const Precision, ConstSliceBase> as_slice() const {                 
		return Vector<Size, const Precision, ConstSliceBase>(data(), size(), stride(), Slicing());         
	}

	DiagonalMatrix<Size,Precision, SliceBase> as_diagonal() {
		return DiagonalMatrix<Size, Precision, SliceBase> (data(), size(), stride(), Slicing());
	}

	const DiagonalMatrix<Size,const Precision, ConstSliceBase> as_diagonal() const {
		return DiagonalMatrix<Size, const Precision, ConstSliceBase> (data(), size(), stride(), Slicing());
	}

};

}

}
