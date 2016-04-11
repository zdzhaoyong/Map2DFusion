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

// Allocators always copy data on copy construction.
//
// When a Vector/Matrix is constructed from a different, but compatible type
// copying is done at a much higher level: above the level that knows how the
// data is laid out in memory.
//
// At this level, copy construction is required since it is only known here 
// whether data or a reference to data should be copied.

#ifdef __GNUC__
#define TOON_ALIGN8 __attribute__ ((aligned(8)))
#else
#define TOON_ALIGN8
#endif

namespace TooN {

namespace Internal
{

template<class Precision> struct DefaultTypes
{
	typedef Precision* PointerType;
	typedef const Precision* ConstPointerType;
	typedef Precision& ReferenceType;
	typedef const Precision& ConstReferenceType;
};


template<int Size, class Precision, bool heap> class StackOrHeap;

template<int Size, class Precision> class StackOrHeap<Size,Precision,0>
{
public:
	StackOrHeap()
	{
		debug_initialize(my_data, Size);	
	}
	Precision my_data[Size];
};

template<int Size> class StackOrHeap<Size,double,0>
{
public:
	StackOrHeap()
	{
		debug_initialize(my_data, Size);	
	}
	double my_data[Size] TOON_ALIGN8 ;
};


template<int Size, class Precision> class StackOrHeap<Size, Precision, 1>
{
	public:
		StackOrHeap()
		:my_data(new Precision[Size])
		{
			debug_initialize(my_data, Size);	
		}


		~StackOrHeap()
		{
			delete[] my_data;
		}

		Precision *my_data;

		StackOrHeap(const StackOrHeap& from)
		:my_data(new Precision[Size])
		{
			for(int i=0; i < Size; i++)
				my_data[i] = from.my_data[i];
		}
};

///@internal
///@brief This allocator object sets aside memory for a statically sized object. 
///It will
///put all the data on the stack if there are less then TooN::max_bytes_on_stack of
///data, otherwise it will use new/delete.
///@ingroup gInternal
template<int Size, class Precision> class StaticSizedAllocator: public StackOrHeap<Size, Precision, (sizeof(Precision)*Size>max_bytes_on_stack) >
{
};


///@internal
///@brief Allocate memory for a static sized Vector.
///The class switches to heap allocation automatically for large Vectors.
///Naturally, the vector is not resizable.
///@ingroup gInternal
template<int Size, class Precision> struct VectorAlloc : public StaticSizedAllocator<Size, Precision>, DefaultTypes<Precision> {
	
	///Default constructor (only for statically sized vectors)
	VectorAlloc() { }

	///Construction from a size (required by damic vectors, ignored otherwise).
	VectorAlloc(int /*s*/) { }

	///Construction from an Operator. See Operator::size().
	template<class Op>
	VectorAlloc(const Operator<Op>&) {}
	
	///Return the size of the vector.
	int size() const {
		return Size;
	}

	using StaticSizedAllocator<Size, Precision>::my_data;

	Precision *get_data_ptr()
	{
		return my_data;
	};

	const Precision *get_data_ptr() const
	{
		return my_data;
	}
	
	protected:

		Precision *data()
		{
			return my_data;
		};

		const Precision *data() const
		{
			return my_data;
		};
		
		void try_destructive_resize(int)
		{}

		template<class Op> void try_destructive_resize(const Operator<Op>&) 
		{}
};

///@internal
///@brief Allocate memory for a dynamic sized Vector.
///This is not resizable.
///@ingroup gInternal
template<class Precision> struct VectorAlloc<Dynamic, Precision>: public DefaultTypes<Precision> {
	Precision * const my_data;
	const int my_size;

	VectorAlloc(const VectorAlloc& v)
	:my_data(new Precision[v.my_size]), my_size(v.my_size)
	{ 
		for(int i=0; i < my_size; i++)
			my_data[i] = v.my_data[i];
	}

	VectorAlloc(int s)
	:my_data(new Precision[s]), my_size(s)
	{ 
		debug_initialize(my_data, my_size);	
	}

	template <class Op>
	VectorAlloc(const Operator<Op>& op) 
	: my_data(new Precision[op.size()]), my_size(op.size()) 
	{
		debug_initialize(my_data, my_size);	
	}

	int size() const {
		return my_size;
	}

	~VectorAlloc(){
		delete[] my_data;
	}

	Precision *get_data_ptr()
	{
		return my_data;
	};

	const Precision *get_data_ptr() const
	{
		return my_data;
	}

	protected:

		Precision *data()
		{
			return my_data;
		};

		const Precision *data() const
		{
			return my_data;
		};

		void try_destructive_resize(int)
		{}

		template<class Op> void try_destructive_resize(const Operator<Op>&) 
		{}
};


///@internal
///@brief Allocate memory for a resizable Vector.
///New elements available after a resize are treated as
///uninitialized. 
///@ingroup gInternal
template<class Precision> struct VectorAlloc<Resizable, Precision>: public DefaultTypes<Precision> {
	protected: 
		std::vector<Precision> numbers;
	
	public:

		VectorAlloc()
		{ 
		}

		VectorAlloc(int s)
		:numbers(s)
		{ 
			debug_initialize(data(), size());	
		}

		template <class Op>
		VectorAlloc(const Operator<Op>& op) 
		:numbers(op.size()) 
		{
			debug_initialize(data(), size());	
		}

		int size() const {
			return numbers.size();
		}

		Precision *get_data_ptr()
		{
			return data();
		};

		const Precision *get_data_ptr() const
		{
			return data();
		}

		void swap(VectorAlloc& s)
		{
			numbers.swap(s.numbers);
		}

	protected:

		Precision* data() {
			return &numbers[0];
		}

		const Precision* data()const  {
			return &numbers[0];
		}

	private:
		//Dymmy class for implementing sfinae
		//in order to test for a .size() member
		template<int S> struct SFINAE_dummy{typedef void type;};
	
	protected:

		//SFINAE implementation of try_destructive_resize
		//to avoid calling .size if it does not exist!

		//Force the function TYPE to depend on a property
		//of the Operator<Op> type, so that it simply does
		//not exist if the property is missing.
		//Therefore this method only uses .size() if it exists.
		template<class Op> 
		typename SFINAE_dummy<sizeof(&Operator<Op>::size)>::type try_destructive_resize(const Operator<Op>& op) 
		{
			try_destructive_resize(op.size());
		}
		
		//Catch-all do nothing for operators with no size method.
		template<class Op>
		void try_destructive_resize(const Op&)
		{}

		void try_destructive_resize(int newsize)
		{
			numbers.resize(newsize);
			debug_initialize(data(), newsize);
		}

	public:

		void resize(int s)
		{
			int old_size = size();
			numbers.resize(s);
			if(s > old_size)
				debug_initialize(data()+old_size, s-old_size);
		}
};

///@internal
///@brief Hold a pointer to yield a statically sized slice of a Vector.
///Not resizable.
///@ingroup gInternal
//template<int S, class Precision, class PtrType=Precision*, class ConstPtrType=const Precision*, class RefType=Precision&, class ConstRefType=const Precision&> struct VectorSlice
template<int S, class Precision, class PtrType=Precision*, class ConstPtrType=const Precision*, class RefType=Precision&, class ConstRefType=const Precision&> struct VectorSlice
{
	int size() const {
		return S;
	}

	//Optional Constructors
	
	const PtrType my_data;
	VectorSlice(PtrType p)
	:my_data(p){}

	VectorSlice(PtrType p, int /*size*/)
	:my_data(p){}

	template<class Op>
	VectorSlice(const Operator<Op>& op) : my_data(op.data()) {}
	
	protected:
		PtrType data()
		{
			return my_data;
		};

		ConstPtrType data() const
		{
			return my_data;
		};

		void try_destructive_resize(int)
		{}

		template<class Op> void try_destructive_resize(const Operator<Op>&) 
		{}

	public:
		typedef PtrType PointerType;
		typedef ConstPtrType ConstPointerType;
		typedef RefType ReferenceType;
		typedef ConstRefType ConstReferenceType;
};

///@internal
///@brief Hold a pointer to yield a dynamically sized slice of a Vector.
///Not resizable.
///@ingroup gInternal
template<class Precision, class PtrType, class ConstPtrType, class RefType, class ConstRefType> struct VectorSlice<Dynamic, Precision, PtrType, ConstPtrType, RefType, ConstRefType>
{
	const PtrType my_data;
	const int my_size;

	VectorSlice(PtrType d, int s)
	:my_data(d), my_size(s)
	{ }

	template<class Op>
	VectorSlice(const Operator<Op>& op) : my_data(op.data()), my_size(op.size()) {}

	int size() const {
		return my_size;
	}

	protected:

		PtrType data()
		{
			return my_data;
		};

		ConstPtrType data() const
		{
			return my_data;
		};

		void try_destructive_resize(int)
		{}

		template<class Op> void try_destructive_resize(const Operator<Op>&) 
		{}

	public:
		typedef PtrType PointerType;
		typedef ConstPtrType ConstPointerType;
		typedef RefType ReferenceType;
		typedef ConstRefType ConstReferenceType;
};

////////////////////////////////////////////////////////////////////////////////
//
// A class similar to StrideHolder, but to hold the size information for matrices.

///@internal
///@brief This struct holds a size using no data for static sizes.
///This struct holds a size is the size is dynamic,
///or simply recorcs the number in the type system if
///the size is static.
///@ingroup gInternal
template<int s> struct SizeHolder
{
	//Constructors ignore superfluous arguments
	SizeHolder(){}    ///<Default constrution
	SizeHolder(int){} ///<Construct from an int and discard it.

	///Simply return the statcally known size
	int size() const{
		return s;
	}
};

///@internal
///@brief This struct holds a size integer for dynamic sizes.
///@ingroup gInternal
template<> struct SizeHolder<-1>
{
	///@name Construction
	///@{
	SizeHolder(int s)
	:my_size(s){}
	///@}

	const int my_size; ///<The size
	///Return the size
	int size() const {
		return my_size;
	}
};

///@internal
///This struct holds the number of rows, only allocating space if
///necessary.
///@ingroup gInternal
template<int S> struct RowSizeHolder: private SizeHolder<S>
{
	///Construct from an int to provide a run time size if
	///necessary. 
	///@param i The size, which is discarded for the static case.
	RowSizeHolder(int i)
	:SizeHolder<S>(i){}

	RowSizeHolder()
	{}
	
	///Construct from an Operator, taking the size from the operator.
	///The size is only used in the dynamic case.
	///@param op Operator from which to determine the size.
	template<typename Op>
	RowSizeHolder(const Operator<Op>& op) : SizeHolder<S>(op.num_rows()) {}
	
	///Return the number of rows.
	int num_rows() const {return SizeHolder<S>::size();}
};


///@internal
///This struct holds the number of columns, only allocating space if
///necessary.
///@ingroup gInternal
template<int S> struct ColSizeHolder: private SizeHolder<S>
{
	///Construct from an int to provide a run time size if
	///necessary. 
	///@param i The size, which is discarded for the static case.
	ColSizeHolder(int i)
	:SizeHolder<S>(i){}

	ColSizeHolder()
	{}

	///Construct from an Operator, taking the size from the operator.
	///The size is only used in the dynamic case.
	///@param op Operator from which to determine the size.
	template<typename Op>
	ColSizeHolder(const Operator<Op>& op) : SizeHolder<S>(op.num_cols()) {}

	///Return the number of columns.
	int num_cols() const {return SizeHolder<S>::size();}	
};



template<int R, int C, class Precision, bool FullyStatic=(R>=0 && C>=0)> 
struct MatrixAlloc: public StaticSizedAllocator<R*C, Precision>
{
	MatrixAlloc(int,int)
	{}

	MatrixAlloc()
	{}

	template <class Op>
	MatrixAlloc(const Operator<Op>&)
	{}

	int num_rows() const {
		return R;
	}

	int num_cols() const {
		return C;
	}

	using  StaticSizedAllocator<R*C, Precision>::my_data;

	Precision* get_data_ptr()
	{
		return my_data;
	}

	const Precision* get_data_ptr() const 
	{
		return my_data;
	}
};


template<int R, int C, class Precision> struct MatrixAlloc<R, C, Precision, false>
	: public RowSizeHolder<R>,
	ColSizeHolder<C>
{
	Precision* const my_data;

	using RowSizeHolder<R>::num_rows;
	using ColSizeHolder<C>::num_cols;
	
	// copy constructor so guaranteed contiguous
	MatrixAlloc(const MatrixAlloc& m)
		:RowSizeHolder<R>(m.num_rows()),
		 ColSizeHolder<C>(m.num_cols()),
		 my_data(new Precision[num_rows()*num_cols()]) {
		const int size=num_rows()*num_cols();
		for(int i=0; i < size; i++) {
			my_data[i] = m.my_data[i];
		}
	}

	MatrixAlloc(int r, int c)
	:RowSizeHolder<R>(r),
	 ColSizeHolder<C>(c),
	 my_data(new Precision[num_rows()*num_cols()]) 
	{
		debug_initialize(my_data, num_rows()*num_cols());	
	}

	template <class Op>	MatrixAlloc(const Operator<Op>& op)
		:RowSizeHolder<R>(op),
		 ColSizeHolder<C>(op),
		 my_data(new Precision[num_rows()*num_cols()])
	{
		debug_initialize(my_data, num_rows()*num_cols());	
	}

	~MatrixAlloc() {
		delete[] my_data;
	}

	Precision* get_data_ptr()
	{
		return my_data;
	}

	const Precision* get_data_ptr() const 
	{
		return my_data;
	}
};


template<int R, int C, class Precision> struct MatrixSlice
	: public RowSizeHolder<R>,
	ColSizeHolder<C>
{
	Precision* const my_data;

	using RowSizeHolder<R>::num_rows;
	using ColSizeHolder<C>::num_cols;

	//Optional Constructors
	MatrixSlice(Precision* p)
	:my_data(p){}

	MatrixSlice(Precision* p, int r, int c)
		:RowSizeHolder<R>(r),
		 ColSizeHolder<C>(c),
		 my_data(p){}
	
	template<class Op>
	MatrixSlice(const Operator<Op>& op)
		:RowSizeHolder<R>(op),
		 ColSizeHolder<C>(op),
		 my_data(op.data())
	{}
};


////////////////////////////////////////////////////////////////////////////////
//
// A class similar to mem, but to hold the stride information. It is only needed
// for -1. For +int and -2, the stride is part fo teh type, or implicit.

template<int s> struct StrideHolder
{
	//Constructos ignore superfluous arguments
	StrideHolder(){}
	StrideHolder(int){}

	template<class Op>
	StrideHolder(const Operator<Op>&) {}

	int stride() const{
		return s;
	}
};

template<> struct StrideHolder<-1>
{
	StrideHolder(int s)
	:my_stride(s){}

	template<class Op>
	StrideHolder(const Operator<Op>& op) : my_stride(op.stride()) {}

	const int my_stride;
	int stride() const {
		return my_stride;
	}
};


template<int S> struct RowStrideHolder: public StrideHolder<S>
{
	RowStrideHolder(int i)
	:StrideHolder<S>(i){}

	RowStrideHolder()
	{}

	template<class Op>
	RowStrideHolder(const Operator<Op>& op)
		: StrideHolder<S>(op)
	{}

};


template<int S> struct ColStrideHolder: public StrideHolder<S>
{
	ColStrideHolder(int i)
	:StrideHolder<S>(i){}

	ColStrideHolder()
	{}

	template<class Op>
	ColStrideHolder(const Operator<Op>& op)
		: StrideHolder<S>(op)
	{}
};

}

}


#undef TOON_ALIGN8
