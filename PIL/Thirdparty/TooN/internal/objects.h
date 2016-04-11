// -*- c++ -*-

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

namespace TooN {

namespace Internal{
	// dummy structs that are used in 0-ary operators
	struct Zero;
	struct SizedZero;
	struct RCZero;
	template<class P> struct Identity;
	template<class P> struct SizedIdentity;

	template<int S, class P, class B, class Ps> class ScalarsVector;	
	template<int R, int C, class P, class B, class Ps> class ScalarsMatrix;	
	template<int R, int C, class P, class B, class Ps> class AddIdentity;	
	template<class P> class Scalars;	
	template<class P> class SizedScalars;	
	template<class P> class RCScalars;
	
	///@internal
	///@brief This class represents 1 and only in all its forms.
	///@ingroup gInternal
	struct One{

		One(){} ///<This constructor does nothing. This allows const One to be declared with no initializer.
		///Generic cast to anything
		template<class C> operator C() const
		{
			return 1;
		}
	};
	template<class Rhs> Rhs operator*(One, const Rhs& v){return v;}   ///<Multiplies One by something.
	template<class Lhs> Lhs operator*(const Lhs& v, One){return v;}   ///<Multiplies something by One
	template<class Rhs> Rhs operator+(One, const Rhs& v){return 1+v;} ///<Adds something to One
	template<class Lhs> Lhs operator+(const Lhs& v, One){return v+1;} ///<Adds One to something
	template<class Rhs> Rhs operator-(One, const Rhs& v){return 1-v;} ///<Subtracts something from One
	template<class Lhs> Lhs operator-(const Lhs& v, One){return v-1;} ///<Subtracts One from something.

	///Returns negative One.
	inline int operator-(const One&)
	{
		return -1;
	}
	
	///@internal
	///@brief For an instance \e i of type C, what is the type of \e -i?
	///Usually the answer is that is it the same type.
	///@ingroup gInternal
	template<class C> struct NegType
	{
		typedef C Type; ///<The type of -C
	};

	/**@internal
	   @brief The type of -One
	   @ingroup gInternal
	*/
	template<> struct NegType<One>
	{
		///One really repersents 1. Therefore -One is not the same type
		///as One.
		typedef int Type;
	};
}

///@internal
///@brief Does One behave as a field with respect to Rhs?
///Answer: it does is Rhs forms a field.
///@ingroup gInternal
template<class Rhs> struct Field<Internal::One, Rhs>
{
	///One can be converted in to anything, so the resulting type is
	///a field if the other type is a field.
	static const int is = Field<Rhs,Rhs>::is;
};

///@internal
///@brief Does One behave as a field with respect to Lhs?
///Answer: it does is Lhs forms a field.
///@ingroup gInternal
template<class Lhs> struct Field<Lhs, Internal::One>
{
	///One can be converted in to anything, so the resulting type is
	///a field if the other type is a field.
	static const int is = Field<Lhs,Lhs>::is;
};

////////////////////
// Zero
////////////////////



template<> struct Operator<Internal::SizedZero>;
template<> struct Operator<Internal::RCZero>;


///@internal
///@brief Object which behaves like a block of zeros. See TooN::Zeros.
///@ingroup gInternal
template<> struct Operator<Internal::Zero> {
	///@name Operator members
	///@{
	template<int Size, class Precision, class Base>
	void eval(Vector<Size, Precision, Base>& v) const {
		for(int i=0; i < v.size(); i++) {
			v[i]= 0;
		}
	}

	template<int R, int C, class P, class B>
	void eval(Matrix<R,C,P,B>& m) const {
		for(int r=0; r<m.num_rows(); r++){
			for(int c=0; c<m.num_cols(); c++){
				m(r,c)=0;
			}
		}
	}
	///@}

	template<int R, int C, class P, class B>
	bool notequal(Matrix<R,C,P,B>& m) const {
		for(int r=0; r<m.num_rows(); r++)
			for(int c=0; c<m.num_cols(); c++)
				if(m[r][c] != 0)
					return 1;
		
		return 0;
	}


	template<int S, class P, class B>
	bool notequal(Vector<S,P,B>& v) const {
		for(int i=0; i<v.size(); i++)
			if(v[i] != 0)
				return 1;
		return 0;
	}

	///Generate a sized Zero object for constructing dynamic vectors.
	Operator<Internal::SizedZero> operator()(int s);
	///Generate a sized Zero object for constructing dynamic matrices.
	Operator<Internal::RCZero> operator()(int r, int c);
	
};

///@internal
///@brief Variant of the Zeros object which holds two sizes for constructing dynamic matrices.
///@ingroup gInternal
template<> struct Operator<Internal::RCZero> : public Operator<Internal::Zero> {

	///@name Operator members determining the size.
	///@{
	Operator(int r, int c) : my_rows(r), my_cols(c) {}

	const int my_rows;
	const int my_cols;
	
	int num_rows() const {return my_rows;}
	int num_cols() const {return my_cols;}
	///@}
};

///@internal
///@brief Variant of the Zeros object which holds a size for constructing dynamic vectors.
///@ingroup gInternal
template<> struct Operator<Internal::SizedZero> : public Operator<Internal::Zero> {

	///@name Operator members determining the size for vectors and square matrices.
	///@{
	Operator(int s)	: my_size(s) {}
		
	const int my_size;
	
	int size() const {return my_size;}
	int num_rows() const {return my_size;}
	int num_cols() const {return my_size;}
	///@}
};

inline Operator<Internal::SizedZero> Operator<Internal::Zero>::operator()(int s){
	return Operator<Internal::SizedZero>(s);
}

inline Operator<Internal::RCZero> Operator<Internal::Zero>::operator()(int r, int c){
	return Operator<Internal::RCZero>(r,c);
}


//////////////
// Identity
//////////////

///@internal
///@brief Operator to construct a new matrix with idendity added 
///@ingroup gInternal
template<int R, int C, class P, class B, class Precision> struct Operator<Internal::AddIdentity<R,C,P,B,Precision> >
{
	const Precision s;   ///<Scale of the identity matrix
	const Matrix<R,C,P,B>& m; ///<matrix to which the identity should be added
	bool invert_m; ///<Whether the identity should be added to + or - m
	
	///@name Construction
	///@{
	Operator(Precision s_, const Matrix<R,C,P,B>& m_, bool b)
		:s(s_),m(m_),invert_m(b){}
	///@}

	///@name Operator members
	///@{
	template<int R1, int C1, class P1, class B1>
	void eval(Matrix<R1,C1,P1,B1>& mm) const{
		for(int r=0; r < m.num_rows(); r++)
			for(int c=0; c < m.num_cols(); c++)
				if(invert_m)
					mm[r][c] = -m[r][c];
				else
					mm[r][c] = m[r][c];

		for(int i=0; i < m.num_rows(); i++)
				mm[i][i] += (P)s;
	}
	///@}

	///@name Sized operator members
	///@{
	int num_rows() const
	{
		return m.num_rows();
	}
	int num_cols() const
	{
		return m.num_cols();
	}
	///@}
};

///@internal
///@brief Object which behaves like an Identity matrix. See TooN::Identity.
///@ingroup gInternal
template<class Pr> struct Operator<Internal::Identity<Pr> > {
	
	///@name Scalable operators members
	///@{

	typedef Pr Precision;
	template<class Pout, class Pmult> Operator<Internal::Identity<Pout> > scale_me(const Pmult& m) const
	{
		return Operator<Internal::Identity<Pout> >(val*m);
	}
	///}
	
	///<Scale of the identity matrix.
	const Precision val;

	///@name Construction
	///@{
	Operator(const Precision& v)
		:val(v)
	{}

	Operator()
	{}
	///}

	///@name Operator members
	///@{
	template<int R, int C, class P, class B>
	void eval(Matrix<R,C,P,B>& m) const {
		SizeMismatch<R, C>::test(m.num_rows(), m.num_cols());
		
		for(int r=0; r<m.num_rows(); r++){
			for(int c=0; c<m.num_cols(); c++){
				m(r,c)=0;
			}
		}
		
		for(int r=0; r < m.num_rows(); r++) {
			m(r,r) = (P)val;
		}
	}
	
	template<int Rows, int Cols, typename P, typename B>
	void plusequals(Matrix<Rows, Cols, P, B>& m) const
	{
		SizeMismatch<Rows, Cols>::test(m.num_rows(), m.num_cols());
		for(int i=0; i < m.num_rows(); i++)
			m[i][i] += (P)val;
	}

	template <int Rows, int Cols, typename P1, typename B1> 
	Operator<Internal::AddIdentity<Rows,Cols,P1,B1,Precision> > add(const Matrix<Rows,Cols, P1, B1>& m) const
	{
		SizeMismatch<Rows, Cols>::test(m.num_rows(), m.num_cols());
		return Operator<Internal::AddIdentity<Rows,Cols,P1,B1,Precision> >(val, m, 0);
	}

	template <int Rows, int Cols, typename P1, typename B1> 
	Operator<Internal::AddIdentity<Rows,Cols,P1,B1,Precision> > rsubtract(const Matrix<Rows,Cols, P1, B1>& m) const
	{
		SizeMismatch<Rows, Cols>::test(m.num_rows(), m.num_cols());
		return Operator<Internal::AddIdentity<Rows,Cols,P1,B1,Precision> >(-val, m, 0);
	}

	template <int Rows, int Cols, typename P1, typename B1> 
	Operator<Internal::AddIdentity<Rows,Cols,P1,B1,Precision> > lsubtract(const Matrix<Rows,Cols, P1, B1>& m) const
	{
		SizeMismatch<Rows, Cols>::test(m.num_rows(), m.num_cols());
		return Operator<Internal::AddIdentity<Rows,Cols,P1,B1,Precision> >(val, m, 1);
	}
	///@}
	
	///@name Sizeable operator members
	///@{
	Operator<Internal::SizedIdentity<Precision> > operator()(int s){
		return Operator<Internal::SizedIdentity<Precision> >(s, val);
	}
	///@}
};
	
///@internal
///@brief A variant of Identity which holds a size, allowing dynamic matrices to be constructed
///@ingroup gInternal
template<class Precision> struct Operator<Internal::SizedIdentity<Precision> > 
	: public  Operator<Internal::Identity<Precision> > {

	using Operator<Internal::Identity<Precision> >::val;
	
	///@name Constructors
	///@{
	Operator(int s, const Precision& v)
		:Operator<Internal::Identity<Precision> > (v), my_size(s)
	{}
	///@}

	///@name Sized operator members
	///@{
	const int my_size;
	int num_rows() const {return my_size;}
	int num_cols() const {return my_size;}
	///@}

	///@name Scalable operator members
	///@{
	template<class Pout, class Pmult> Operator<Internal::SizedIdentity<Pout> > scale_me(const Pmult& m) const
	{
		return Operator<Internal::SizedIdentity<Pout> >(my_size, val*m);
	}
	///@}
};
////////////////////////////////////////////////////////////////////////////////
//
// Addition of scalars to vectors and matrices
//

	
///@internal
///@brief Operator to construct a new vector a a vector with a scalar added to every element
///@ingroup gInternal
template<int S, class P, class B, class Precision> struct Operator<Internal::ScalarsVector<S,P,B,Precision> >
{
	const Precision s;      ///<Scalar to add
	const Vector<S,P,B>& v; ///<Vector to be added to.
	const bool invert_v;    ///<Whether to use + or - \c v

	///@name Constructors
	///@{
	Operator(Precision s_, const Vector<S,P,B>& v_, bool inv)
		:s(s_),v(v_),invert_v(inv){}
	///@}

	///@name Operator members
	///@{
	template<int S1, class P1, class B1>
	void eval(Vector<S1,P1,B1>& vv) const{
		for(int i=0; i < v.size(); i++)
			if(invert_v)
				vv[i] = s - v[i];
			else
				vv[i] = s + v[i];
	}
	///@}

	///@name Sized operator members
	///@{
	int size() const
	{
		return v.size();
	}
	///@}
};

///@internal
///@brief Operator to construct a new matrix a a matrix with a scalar added to every element
///@ingroup gInternal
template<int R, int C, class P, class B, class Precision> struct Operator<Internal::ScalarsMatrix<R,C,P,B,Precision> >
{
	const Precision s;        ///<Scalar to add
	const Matrix<R,C,P,B>& m; ///<Vector to be added to.
	const bool invert_m;      ///<Whether to use + or - \c m
	///@name Operator members
	///@{
	Operator(Precision s_, const Matrix<R,C,P,B>& m_, bool inv)
		:s(s_),m(m_),invert_m(inv){}
	template<int R1, int C1, class P1, class B1>
	void eval(Matrix<R1,C1,P1,B1>& mm) const{
		for(int r=0; r < m.num_rows(); r++)
			for(int c=0; c < m.num_cols(); c++)
				if(invert_m)
					mm[r][c] = s - m[r][c];
				else
					mm[r][c] = s + m[r][c];
	}
	///@}

	///@name Sized operator members
	///@{
	int num_rows() const
	{
		return m.num_rows();
	}
	int num_cols() const
	{
		return m.num_cols();
	}
	///@}
};

///@internal
///@brief Generic scalars object. Knows how to be added, knows how to deal with += and so on.
///See TooN::Ones
///@ingroup gInternal
template<class P> struct Operator<Internal::Scalars<P> >
{	
	///@name Scalable operator members
	///@{
	typedef P Precision;
	///@}

	const Precision s; ///<Value of the scalar being represented.  
	
	///@name Constructors
	///@{
	Operator(Precision s_)
		:s(s_){}

	Operator()
	{}
	///@}

	////////////////////////////////////////
	//
	// All applications for vector
	//
	///@name Operator members
	///@{

	template <int Size, typename P1, typename B1> 
	void eval(Vector<Size, P1, B1>& v) const
	{
		for(int i=0; i < v.size(); i++)
			v[i] = (P1)s;
	}

	template <int Size, typename P1, typename B1> 
	void plusequals(Vector<Size, P1, B1>& v) const
	{
		for(int i=0; i < v.size(); i++)
			v[i] += (P1)s;
	}

	template <int Size, typename P1, typename B1>
	void minusequals(Vector<Size, P1, B1>& v) const
	{
		for(int i=0; i < v.size(); ++i)
			v[i] -= (P1)s;
	}

	template <int Size, typename P1, typename B1> 
	Operator<Internal::ScalarsVector<Size,P1,B1,Precision> > add(const Vector<Size, P1, B1>& v) const
	{
		return Operator<Internal::ScalarsVector<Size,P1,B1,Precision> >(s, v, 0);
	}

	template <int Size, typename P1, typename B1> 
	Operator<Internal::ScalarsVector<Size,P1,B1,Precision> > rsubtract(const Vector<Size, P1, B1>& v) const
	{
		return Operator<Internal::ScalarsVector<Size,P1,B1,Precision> >(-s, v, 0);
	}

	template <int Size, typename P1, typename B1> 
	Operator<Internal::ScalarsVector<Size,P1,B1,Precision> > lsubtract(const Vector<Size, P1, B1>& v) const
	{
		return Operator<Internal::ScalarsVector<Size,P1,B1,Precision> >(s, v, 1);
	}

	////////////////////////////////////////
	//
	// All applications for matrix
	//

	template <int Rows, int Cols, typename P1, typename B1> 
	void eval(Matrix<Rows,Cols, P1, B1>& m) const
	{
		for(int r=0; r < m.num_rows(); r++)
			for(int c=0; c < m.num_cols(); c++)
				m[r][c] = s;
	}

	template <int Rows, int Cols, typename P1, typename B1> 
	void plusequals(Matrix<Rows,Cols, P1, B1>& m) const
	{
		for(int r=0; r < m.num_rows(); r++)
			for(int c=0; c < m.num_cols(); c++)
				m[r][c] += (P1)s;
	}

	template <int Rows, int Cols, typename P1, typename B1> 
	void minusequals(Matrix<Rows,Cols, P1, B1>& m) const
	{
		for(int r=0; r < m.num_rows(); r++)
			for(int c=0; c < m.num_cols(); c++)
				m[r][c] -= (P1)s;
	}

	template <int Rows, int Cols, typename P1, typename B1> 
	Operator<Internal::ScalarsMatrix<Rows,Cols,P1,B1,Precision> > add(const Matrix<Rows,Cols, P1, B1>& v) const
	{
		return Operator<Internal::ScalarsMatrix<Rows,Cols,P1,B1,Precision> >(s, v, 0);
	}


	template <int Rows, int Cols, typename P1, typename B1> 
	Operator<Internal::ScalarsMatrix<Rows,Cols,P1,B1,typename Internal::NegType<P>::Type> > rsubtract(const Matrix<Rows,Cols, P1, B1>& v) const
	{
		return Operator<Internal::ScalarsMatrix<Rows,Cols,P1,B1,typename Internal::NegType<P>::Type > >(-s, v, 0);
	}

	template <int Rows, int Cols, typename P1, typename B1> 
	Operator<Internal::ScalarsMatrix<Rows,Cols,P1,B1,Precision> > lsubtract(const Matrix<Rows,Cols, P1, B1>& v) const
	{
		return Operator<Internal::ScalarsMatrix<Rows,Cols,P1,B1,Precision> >(s, v, 1);
	}
	///@}
	////////////////////////////////////////
	//
	// Create sized versions for initialization
	//

	///@name Sizeable operators members
	///@{

	Operator<Internal::SizedScalars<Precision> > operator()(int size) const
	{
		return Operator<Internal::SizedScalars<Precision> > (s,size);
	}

	Operator<Internal::RCScalars<Precision> > operator()(int r, int c) const
	{
		return Operator<Internal::RCScalars<Precision> > (s,r,c);
	}
	///@}

	///@name Scalable operator members
	///@{
	template<class Pout, class Pmult> Operator<Internal::Scalars<Pout> > scale_me(const Pmult& m) const
	{
		return Operator<Internal::Scalars<Pout> >(s*m);
	}
	///@}
};

///@internal
///@brief Variant of the Operator<Internal::Scalars> object which holds a size to construct dynamic vectors or square matrices.
///@ingroup gInternal 
template<class P> struct Operator<Internal::SizedScalars<P> >: public Operator<Internal::Scalars<P> >
{
	using Operator<Internal::Scalars<P> >::s;
	///@name Sized operator members
	///@{
	const int my_size;
	int size() const {
		return my_size;
	}
	int num_rows() const {
		return my_size;
	}
	int num_cols() const {
		return my_size;
	}
	///@}

	///@name Constructors
	///@{
	Operator(P s, int sz)
		:Operator<Internal::Scalars<P> >(s),my_size(sz){}
	///@}
		
	///@name Scalable operator members
	///@{
	template<class Pout, class Pmult> Operator<Internal::SizedScalars<Pout> > scale_me(const Pmult& m) const
	{
		return Operator<Internal::SizedScalars<Pout> >(s*m, my_size);
	}
	///@}

private:
	void operator()(int);
	void operator()(int,int);
};

		
///@internal
///@brief Variant of Scalars (see TooN::Ones) which holds two sizes to construct dynamic matrices.
///@ingroup gInternal
template<class P> struct Operator<Internal::RCScalars<P> >: public Operator<Internal::Scalars<P> >
{
	using Operator<Internal::Scalars<P> >::s;

	///@name Operator members
	///@{
	const int my_rows, my_cols;
	int num_rows() const {
		return my_rows;
	}
	int num_cols() const {
		return my_cols;
	}
		
	Operator(P s, int r, int c)
		:Operator<Internal::Scalars<P> >(s),my_rows(r),my_cols(c)
	{}
		
	template<class Pout, class Pmult> Operator<Internal::RCScalars<Pout> > scale_me(const Pmult& m) const
	{
		return Operator<Internal::RCScalars<Pout> >(s*m, my_rows, my_cols);
	}

	///@}
private:
	void operator()(int);
	void operator()(int,int);
};


////////////////////////////////////////////////////////////////////////////////
//
// How to scale scalable operators
//
	
template<template<class> class Op, class Pl, class Pr> 
Operator<Op<typename Internal::MultiplyType<Pl, Pr>::type > >
operator*(const Pl& l, const Operator<Op<Pr> >& r)
{
	return r.template scale_me<typename Internal::MultiplyType<Pl, Pr>::type, Pl>(l); 
}

template<template<class> class Op, class Pl, class Pr> 
Operator<Op<typename Internal::MultiplyType<Pl, Pr>::type > >
operator*(const Operator<Op<Pl> >& l, const Pr&  r)
{
	return l.template scale_me<typename Internal::MultiplyType<Pl, Pr>::type>(r); 
}

template<template<class> class Op, class Pl, class Pr> 
Operator<Op<typename Internal::DivideType<Pl, Pr>::type > >
operator/(const Operator<Op<Pl> >& l, const Pr&  r)
{
	return l.template scale_me<typename Internal::MultiplyType<Pl, Pr>::type, Pl>(static_cast<typename Internal::DivideType<Pl,Pr>::type>(1)/r); 
}


template<class Op>
Operator<Op> operator-(const Operator<Op>& o)
{
	return o.template scale_me<typename Operator<Op>::Precision>(-1);
}

//Special case for negating One
template<template<class>class Op>
Operator<Op<DefaultPrecision> > operator-(const Operator<Op<Internal::One> >& o)
{
	return o.template scale_me<DefaultPrecision>(-1);
}

/**This function is used to add a scalar to every element of a vector or
   matrix. For example:
   @code
   Vector<3> v;
   ...
   ...
   v += Ones * 3; //Add 3 to every element of v;
   @endcode
   Both + and += are supported on vectors,matrices and slices.

   For construction of dynamic vectors and matrices, a size needs to be given:
   @code
       Vector<3> v_static = Ones;    
	   Vector<>  v_dynamic = Ones(3); //Construct a 3x1 vector full one 1s
       Matrix<3> m_static = Ones;     
	   Matrix<>  m_dynamic = Ones(3,4); //Construct a 3x4 matrix
   @endcode
   @ingroup gLinAlg
*/
static const Operator<Internal::Scalars<Internal::One> > Ones;


/**This function is used to initialize vectors and matrices to zero.
   For construction of dynamic vectors and matrices, a size needs to be given.
   For example:
   @code
       Vector<3> v_static = Zeros;
	   Vector<>  v_dynamic = Zeros(3); //Construct a 3x1 vector
       Matrix<3> m_static = Zeros;
	   Matrix<>  m_dynamic = Zeros(3,4); //Construct a 3x4 matrix
   @endcode
   @ingroup gLinAlg
*/
static Operator<Internal::Zero> Zeros;

/**This function is used to add a scalar to the diagonal of a matrix,
   or to construct matrices.
   For example:
   @code
   Matrix<3> v;
   ...
   ...
   Matrix<3> u = v  + Identity * 4;
   @endcode
   Both + and += are supported. For assignment, if the matrix is non-square,
   then all elements off the leading diagonal are set to zero.
   For construction of dynamic matrices, a size needs to be given:
   @code
       Matrix<3> m_static = Identity;     
	   Matrix<>  m_dynamic = Identity(3); //Construct a 3x3 matrix
   @endcode
   @ingroup gLinAlg
*/

static Operator<Internal::Identity<Internal::One> > Identity;

}

