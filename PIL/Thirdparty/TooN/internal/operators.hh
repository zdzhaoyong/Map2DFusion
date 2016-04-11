//-*- c++ -*-

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

//////////////////////////////////////////////////////////////////////////////////////////////
//             Type  and size computation for scalar operations used in this file
//////////////////////////////////////////////////////////////////////////////////////////////

///Determine if two classes are in the same field. For the purposes of
///%TooN \c float and \c int are in the same field, since operator
///+,-,*,/ are defined for any combination of \c float and \c int. 
///Combinations of builtin types are dea;t with by IsField.
template<class L, class R> struct Field
{	
	///<Set to 1 if the two classes are in the same field.
	static const int is = IsField<L>::value & IsField<R>::value;
};

namespace Internal {

	//Automatic type deduction of return types
	///@internal
	///This function offers to return a value of type C. This function
	///is not implemented anywhere, the result is used for type deduction.
	template<class C> C gettype();
	
	
	template<class C> struct Clean
	{
		typedef C type;
	};
	
	template<class C> struct Clean<const C>
	{
		typedef C type;
	};

	template<class C> struct Clean<const C&>
	{
		typedef C type;
	};

	template<class C> struct Clean<C&>
	{
		typedef C type;
	};

	template<class L, class R> struct CField
	{
		static const int is = TooN::Field<typename Clean<L>::type, typename Clean<R>::type>::is;
	};


	//We have to use the traits here because it is not possible to 
	//check for the existence of a valid operator *, especially
	//in the presence of builtin operators. Therefore, the type is
	//only deduced if both of the input types are fields.
	template<class L, class R, int F = CField<L,R>::is> struct AddType      { typedef TOON_TYPEOF(gettype<L>()+gettype<R>()) type;};
	template<class L, class R, int F = CField<L,R>::is> struct SubtractType { typedef TOON_TYPEOF(gettype<L>()-gettype<R>()) type;};
	template<class L, class R, int F = CField<L,R>::is> struct MultiplyType { typedef TOON_TYPEOF(gettype<L>()*gettype<R>()) type;};
	template<class L, class R, int F = CField<L,R>::is> struct DivideType   { typedef TOON_TYPEOF(gettype<L>()/gettype<R>()) type;};

	template<class L, class R> struct AddType<L, R, 0>         { typedef These_Types_Do_Not_Form_A_Field<L, R> type;};
	template<class L, class R> struct SubtractType<L, R, 0>    { typedef These_Types_Do_Not_Form_A_Field<L, R> type;};
	template<class L, class R> struct MultiplyType<L, R, 0>    { typedef These_Types_Do_Not_Form_A_Field<L, R> type;};
	template<class L, class R> struct DivideType<L, R, 0>      { typedef These_Types_Do_Not_Form_A_Field<L, R> type;};


	//Mini operators for passing to Pairwise, etc
	struct Add{
		template<class A, class B, class C>      static A op(const B& b, const C& c){return b+c;}
		template<class P1, class P2> struct Return { typedef typename AddType<P1,P2>::type Type;};
	};
	struct Subtract{
		template<class A, class B, class C> static A op(const B& b, const C& c){return b-c;}
		template<class P1, class P2> struct Return { typedef typename SubtractType<P1,P2>::type Type;};
	};
	struct Multiply{
		template<class A, class B, class C> static A op(const B& b, const C& c){return b*c;}
		template<class P1, class P2> struct Return { typedef typename MultiplyType<P1,P2>::type Type;};
	};
	struct Divide{
		template<class A, class B, class C>   static A op(const B& b, const C& c){return b/c;}
		template<class P1, class P2> struct Return { typedef typename DivideType<P1,P2>::type Type;};
	};

};

//////////////////////////////////////////////////////////////////////////////////////////////
//                                       Operators
//////////////////////////////////////////////////////////////////////////////////////////////

template<class Op> struct Operator{};


//////////////////////////////////////////////////////////////////////////////////
//                         Vector <op> Vector
//////////////////////////////////////////////////////////////////////////////////

namespace Internal {
	template<typename Op,                           // the operation
			 int S1, typename P1, typename B1,      // lhs vector
			 int S2, typename P2, typename B2>      // rhs vector
	struct VPairwise;

	template <int S, typename P, typename A>        // input vector
	struct VNegate;
};

template<typename Op,                           // the operation
		 int S1, typename P1, typename B1,      // lhs vector
		 int S2, typename P2, typename B2>      // rhs vector
struct Operator<Internal::VPairwise<Op, S1, P1, B1, S2, P2, B2> > {
	const Vector<S1, P1, B1> & lhs;
	const Vector<S2, P2, B2> & rhs;

	Operator(const Vector<S1, P1, B1> & lhs_in, const Vector<S2, P2, B2> & rhs_in) : lhs(lhs_in), rhs(rhs_in) {}

	template<int S0, typename P0, typename Ba0>
	void eval(Vector<S0, P0, Ba0>& res) const
	{
		for(int i=0; i < res.size(); ++i)
			res[i] = Op::template op<P0,P1, P2>(lhs[i],rhs[i]);
	}
	int size() const {return lhs.size();}
};

// Addition Vector + Vector
template<int S1, int S2, typename P1, typename P2, typename B1, typename B2> 
Vector<Internal::Sizer<S1,S2>::size, typename Internal::AddType<P1, P2>::type> 
operator+(const Vector<S1, P1, B1>& v1, const Vector<S2, P2, B2>& v2)
{
	SizeMismatch<S1, S2>:: test(v1.size(),v2.size());
	return Operator<Internal::VPairwise<Internal::Add,S1,P1,B1,S2,P2,B2> >(v1,v2);
}

// Subtraction Vector - Vector
template<int S1, int S2, typename P1, typename P2, typename B1, typename B2> 
Vector<Internal::Sizer<S1,S2>::size, typename Internal::SubtractType<P1, P2>::type> operator-(const Vector<S1, P1, B1>& v1, const Vector<S2, P2, B2>& v2)
{
	SizeMismatch<S1, S2>:: test(v1.size(),v2.size());
	return Operator<Internal::VPairwise<Internal::Subtract,S1,P1,B1,S2,P2,B2> >(v1,v2);
}

// diagmult Vector, Vector
template <int S1, int S2, typename P1, typename P2, typename B1, typename B2>
Vector<Internal::Sizer<S1,S2>::size, typename Internal::MultiplyType<P1,P2>::type> diagmult(const Vector<S1,P1,B1>& v1, const Vector<S2,P2,B2>& v2)
{
	SizeMismatch<S1,S2>::test(v1.size(),v2.size());
	return Operator<Internal::VPairwise<Internal::Multiply,S1,P1,B1,S2,P2,B2> >(v1,v2);
}

template<int S, typename P, typename A>
struct Operator<Internal::VNegate<S, P, A> > {
	const Vector<S, P, A> & input;
	Operator( const Vector<S, P, A> & in ) : input(in) {}
	
	template<int S0, typename P0, typename A0>
	void eval(Vector<S0, P0, A0> & res) const
	{
		res = input * -1;
	}
	int size() const { return input.size(); }
};

// Negation -Vector
template <int S, typename P, typename A>
Vector<S, P> operator-(const Vector<S,P,A> & v){
	return Operator<Internal::VNegate<S,P,A> >(v);
}

// Dot product Vector * Vector
template<int Size1, typename Precision1, typename Base1, int Size2, typename Precision2, typename Base2>
typename Internal::MultiplyType<Precision1, Precision2>::type operator*(const Vector<Size1, Precision1, Base1>& v1, const Vector<Size2, Precision2, Base2>& v2){
  SizeMismatch<Size1, Size2>:: test(v1.size(),v2.size());
  const int s=v1.size();
  typename Internal::MultiplyType<Precision1, Precision2>::type result=0;
  for(int i=0; i<s; i++){
    result+=v1[i]*v2[i];
  }
  return result;
}

template <typename P1, typename P2, typename B1, typename B2>
Vector<3, typename Internal::MultiplyType<P1,P2>::type> operator^(const Vector<3,P1,B1>& v1, const Vector<3,P2,B2>& v2){
	// assume the result of adding two restypes is also a restype
	typedef typename Internal::MultiplyType<P1,P2>::type restype;

	Vector<3, restype> result;

	result[0] = v1[1]*v2[2] - v1[2]*v2[1];
	result[1] = v1[2]*v2[0] - v1[0]*v2[2];
	result[2] = v1[0]*v2[1] - v1[1]*v2[0];

	return result;
}




//////////////////////////////////////////////////////////////////////////////////
//                            Matrix <op> Matrix
//////////////////////////////////////////////////////////////////////////////////

namespace Internal {
	template<typename Op,                           // the operation
			 int R1, int C1, typename P1, typename B1,      // lhs matrix
			 int R2, int C2, typename P2, typename B2>      // rhs matrix
	struct MPairwise;

	template<int R1, int C1, typename P1, typename B1,      // lhs matrix
			 int R2, int C2, typename P2, typename B2>      // rhs matrix
	struct MatrixMultiply;

	template<int R, int C, typename P, typename A>         // input matrix
	struct MNegate;
};

template<typename Op,                           // the operation
		 int R1, int C1, typename P1, typename B1,      // lhs matrix
		 int R2, int C2, typename P2, typename B2>      // rhs matrix
struct Operator<Internal::MPairwise<Op, R1, C1, P1, B1, R2, C2, P2, B2> > {
	const Matrix<R1, C1, P1, B1> & lhs;
	const Matrix<R2, C2, P2, B2> & rhs;

	Operator(const Matrix<R1, C1, P1, B1> & lhs_in, const Matrix<R2, C2, P2, B2> & rhs_in) : lhs(lhs_in), rhs(rhs_in) {}

	template<int R0, int C0, typename P0, typename Ba0>
	void eval(Matrix<R0, C0, P0, Ba0>& res) const
	{
		for(int r=0; r < res.num_rows(); ++r){
			for(int c=0; c < res.num_cols(); ++c){
				res(r,c) = Op::template op<P0,P1, P2>(lhs(r,c),rhs(r,c));
			}
		}
	}
	int num_rows() const {return lhs.num_rows();}
	int num_cols() const {return lhs.num_cols();}
};

// Addition Matrix + Matrix
template<int R1, int R2, int C1, int C2, typename P1, typename P2, typename B1, typename B2> 
Matrix<Internal::Sizer<R1,R2>::size, Internal::Sizer<C1,C2>::size, typename Internal::AddType<P1, P2>::type> 
operator+(const Matrix<R1, C1, P1, B1>& m1, const Matrix<R2, C2, P2, B2>& m2)
{
	SizeMismatch<R1, R2>:: test(m1.num_rows(),m2.num_rows());
	SizeMismatch<C1, C2>:: test(m1.num_cols(),m2.num_cols());
	return Operator<Internal::MPairwise<Internal::Add,R1,C1,P1,B1,R2,C2,P2,B2> >(m1,m2);
}

// Subtraction Matrix - Matrix
template<int R1, int R2, int C1, int C2, typename P1, typename P2, typename B1, typename B2> 
Matrix<Internal::Sizer<R1,R2>::size, Internal::Sizer<C1,C2>::size, typename Internal::SubtractType<P1, P2>::type> 
operator-(const Matrix<R1, C1, P1, B1>& m1, const Matrix<R2, C2, P2, B2>& m2)
{
	SizeMismatch<R1, R2>:: test(m1.num_rows(),m2.num_rows());
	SizeMismatch<C1, C2>:: test(m1.num_cols(),m2.num_cols());
	return Operator<Internal::MPairwise<Internal::Subtract,R1,C1,P1,B1,R2,C2,P2,B2> >(m1,m2);
}

template<int R, int C, typename P, typename A>
struct Operator<Internal::MNegate<R,C, P, A> > {
	const Matrix<R,C,P,A> & input;
	Operator( const Matrix<R,C,P,A> & in ) : input(in) {}
	
	template<int R0, int C0, typename P0, typename A0>
	void eval(Matrix<R0,C0,P0,A0> & res) const
	{
		res = input * -1;
	}
	int num_rows() const { return input.num_rows(); }
	int num_cols() const { return input.num_cols(); }
};

// Negation -Matrix
template <int R, int C, typename P, typename A>
Matrix<R, C, P> operator-(const Matrix<R,C,P,A> & v){
	return Operator<Internal::MNegate<R,C,P,A> >(v);
}

template<int R1, int C1, typename P1, typename B1,      // lhs matrix
		 int R2, int C2, typename P2, typename B2>      // rhs matrix
struct Operator<Internal::MatrixMultiply<R1, C1, P1, B1, R2, C2, P2, B2> > {
	const Matrix<R1, C1, P1, B1> & lhs;
	const Matrix<R2, C2, P2, B2> & rhs;

	Operator(const Matrix<R1, C1, P1, B1> & lhs_in, const Matrix<R2, C2, P2, B2> & rhs_in) : lhs(lhs_in), rhs(rhs_in) {}

	template<int R0, int C0, typename P0, typename Ba0>
	void eval(Matrix<R0, C0, P0, Ba0>& res) const
	{

		for(int r=0; r < res.num_rows(); ++r) {
			for(int c=0; c < res.num_cols(); ++c) {
				res(r,c) = lhs[r] * (rhs.T()[c]);
			}
		}
	}
	int num_rows() const {return lhs.num_rows();}
	int num_cols() const {return rhs.num_cols();}
};




// Matrix multiplication Matrix * Matrix

template<int R1, int C1, int R2, int C2, typename P1, typename P2, typename B1, typename B2> 
Matrix<R1, C2, typename Internal::MultiplyType<P1, P2>::type> operator*(const Matrix<R1, C1, P1, B1>& m1, const Matrix<R2, C2, P2, B2>& m2)
{
	SizeMismatch<C1, R2>:: test(m1.num_cols(),m2.num_rows());
	return Operator<Internal::MatrixMultiply<R1,C1,P1,B1,R2,C2,P2,B2> >(m1,m2);
}

//////////////////////////////////////////////////////////////////////////////////
//                 matrix <op> vector and vv.
//////////////////////////////////////////////////////////////////////////////////


namespace Internal {
	// dummy struct for Vector * Matrix
	template<int R, int C, typename P1, typename B1, int Size, typename P2, typename B2>
	struct MatrixVectorMultiply;

	// this is distinct to cater for non commuting precision types
	template<int Size, typename P1, typename B1, int R, int C, typename P2, typename B2>
	struct VectorMatrixMultiply;

	// dummy struct for Vector * Matrix
	template<int R, int C, typename P1, typename B1, int Size, typename P2, typename B2>
	struct MatrixVectorDiagMultiply;

	// this is distinct to cater for non commuting precision types
	template<int Size, typename P1, typename B1, int R, int C, typename P2, typename B2>
	struct VectorMatrixDiagMultiply;

};

// Matrix Vector multiplication Matrix * Vector
template<int R, int C, typename P1, typename B1, int Size, typename P2, typename B2> 
struct Operator<Internal::MatrixVectorMultiply<R,C,P1,B1,Size,P2,B2> > {
	const Matrix<R,C,P1,B1>& lhs;
	const Vector<Size,P2,B2>& rhs;

	Operator(const Matrix<R,C,P1,B1>& lhs_in, const Vector<Size,P2,B2>& rhs_in) : lhs(lhs_in), rhs(rhs_in) {}

	int size() const {return lhs.num_rows();}

	template<int Sout, typename Pout, typename Bout>
	void eval(Vector<Sout, Pout, Bout>& res) const {
		for(int i=0; i < res.size(); ++i){
			res[i] = lhs[i] * rhs;
		}
	}
};

template<int R, int C, int Size, typename P1, typename P2, typename B1, typename B2>
Vector<R, typename Internal::MultiplyType<P1,P2>::type> operator*(const Matrix<R, C, P1, B1>& m, const Vector<Size, P2, B2>& v)
{
	SizeMismatch<C,Size>::test(m.num_cols(), v.size());
	return Operator<Internal::MatrixVectorMultiply<R,C,P1,B1,Size,P2,B2> >(m,v);
}
																	
// Vector Matrix multiplication Vector * Matrix
template<int R, int C, typename P1, typename B1, int Size, typename P2, typename B2> 
struct Operator<Internal::VectorMatrixMultiply<Size,P1,B1,R,C,P2,B2> > {
	const Vector<Size,P1,B1>& lhs;
	const Matrix<R,C,P2,B2>& rhs;

	Operator(const Vector<Size,P1,B1>& lhs_in, const Matrix<R,C,P2,B2>& rhs_in) : lhs(lhs_in), rhs(rhs_in) {}

	int size() const {return rhs.num_cols();}

	template<int Sout, typename Pout, typename Bout>
	void eval(Vector<Sout, Pout, Bout>& res) const {
		for(int i=0; i < res.size(); ++i){
			res[i] = lhs * rhs.T()[i];
		}
	}
};

template<int R, int C, typename P1, typename B1, int Size, typename P2, typename B2> 
Vector<C, typename Internal::MultiplyType<P1,P2>::type> operator*(const Vector<Size,P1,B1>& v,
																  const Matrix<R,C,P2,B2>& m)
{
	SizeMismatch<R,Size>::test(m.num_rows(), v.size());
	return Operator<Internal::VectorMatrixMultiply<Size,P1,B1,R,C,P2,B2> >(v,m);
}


// Matrix Vector diagonal multiplication Matrix * Vector
template<int R, int C, typename P1, typename B1, int Size, typename P2, typename B2> 
struct Operator<Internal::MatrixVectorDiagMultiply<R,C,P1,B1,Size,P2,B2> > {
	const Matrix<R,C,P1,B1>& lhs;
	const Vector<Size,P2,B2>& rhs;

	Operator(const Matrix<R,C,P1,B1>& lhs_in, const Vector<Size,P2,B2>& rhs_in) : lhs(lhs_in), rhs(rhs_in) {}

	int num_rows() const {return lhs.num_rows();}
	int num_cols() const {return lhs.num_cols();}

	template<int Rout, int Cout, typename Pout, typename Bout>
	void eval(Matrix<Rout, Cout, Pout, Bout>& res) const {
		for(int c=0; c < res.num_cols(); ++c) {
			P2 temp = rhs[c];
			for(int r=0; r < res.num_rows(); ++r) {
				res(r,c) = lhs(r,c)*temp;
			}
		}
	}
};

template<int R, int C, int Size, typename P1, typename P2, typename B1, typename B2>
Matrix<R, C, typename Internal::MultiplyType<P1,P2>::type> diagmult(const Matrix<R, C, P1, B1>& m, const Vector<Size, P2, B2>& v)
{
	SizeMismatch<C,Size>::test(m.num_cols(), v.size());
	return Operator<Internal::MatrixVectorDiagMultiply<R,C,P1,B1,Size,P2,B2> >(m,v);
}
																	
// Vector Matrix diagonal multiplication Vector * Matrix
template<int R, int C, typename P1, typename B1, int Size, typename P2, typename B2> 
struct Operator<Internal::VectorMatrixDiagMultiply<Size,P1,B1,R,C,P2,B2> > {
	const Vector<Size,P1,B1>& lhs;
	const Matrix<R,C,P2,B2>& rhs;

	Operator(const Vector<Size,P1,B1>& lhs_in, const Matrix<R,C,P2,B2>& rhs_in) : lhs(lhs_in), rhs(rhs_in) {}

	int num_rows() const {return rhs.num_rows();}
	int num_cols() const {return rhs.num_cols();}

	template<int Rout, int Cout, typename Pout, typename Bout>
	void eval(Matrix<Rout, Cout, Pout, Bout>& res) const {
		for(int r=0; r < res.num_rows(); ++r){
			const P1 temp = lhs[r];
			for(int c=0; c<res.num_cols(); ++c){
				res(r,c) = temp * rhs(r,c);
			}
		}
	}
};

template<int R, int C, typename P1, typename B1, int Size, typename P2, typename B2> 
Matrix<R, C, typename Internal::MultiplyType<P1,P2>::type> diagmult(const Vector<Size,P1,B1>& v,
																 const Matrix<R,C,P2,B2>& m)
{
	SizeMismatch<R,Size>::test(m.num_rows(), v.size());
	return Operator<Internal::VectorMatrixDiagMultiply<Size,P1,B1,R,C,P2,B2> >(v,m);
}


////////////////////////////////////////////////////////////////////////////////
//
// vector <op> scalar 
// scalar <op> vector 
// matrix <op> scalar 
// scalar <op> matrix 
//
// Except <scalar> / <matrix|vector> does not exist

namespace Internal {
	template<int Size, typename P1, typename B1, typename P2, typename Op>
	struct ApplyScalarV;

	template<int Size, typename P1, typename B1, typename P2, typename Op>
	struct ApplyScalarVL;

	template<int R, int C, typename P1, typename B1, typename P2, typename Op>
	struct ApplyScalarM;

	template<int R, int C, typename P1, typename B1, typename P2, typename Op>
	struct ApplyScalarML;
};

template<int Size, typename P1, typename B1, typename P2, typename Op>
struct Operator<Internal::ApplyScalarV<Size,P1,B1,P2,Op> > {
	const Vector<Size,P1,B1>& lhs;
	const P2& rhs;

	Operator(const Vector<Size,P1,B1>& v, const P2& s) : lhs(v), rhs(s) {}
		
	template<int S0, typename P0, typename Ba0>
	void eval(Vector<S0,P0,Ba0>& v) const {
		for(int i=0; i<v.size(); i++){
			v[i]= Op::template op<P0,P1,P2> (lhs[i],rhs);
		}
	}

	int size() const {
		return lhs.size();
	}
};

template <int Size, typename P1, typename B1, typename P2>
Vector<Size, typename Internal::Multiply::Return<P1,P2>::Type> operator*(const Vector<Size, P1, B1>& v, const P2& s){
	return Operator<Internal::ApplyScalarV<Size,P1,B1,P2,Internal::Multiply> > (v,s);
}
template <int Size, typename P1, typename B1, typename P2>
Vector<Size, typename Internal::Divide::Return<P1,P2>::Type> operator/(const Vector<Size, P1, B1>& v, const P2& s){
	return Operator<Internal::ApplyScalarV<Size,P1,B1,P2,Internal::Divide> > (v,s);
}

template<int Size, typename P1, typename B1, typename P2, typename Op>
struct Operator<Internal::ApplyScalarVL<Size,P1,B1,P2,Op> > {
	const P2& lhs;
	const Vector<Size,P1,B1>& rhs;

	Operator(const P2& s, const Vector<Size,P1,B1>& v) : lhs(s), rhs(v) {}
		
	template<int S0, typename P0, typename Ba0>
	void eval(Vector<S0,P0,Ba0>& v) const {
		for(int i=0; i<v.size(); i++){
			v[i]= Op::template op<P0,P2,P1> (lhs,rhs[i]);
		}
	}

	int size() const {
		return rhs.size();
	}
};
template <int Size, typename P1, typename B1, typename P2>
Vector<Size, typename Internal::Multiply::Return<P2,P1>::Type> operator*(const P2& s, const Vector<Size, P1, B1>& v){
	return Operator<Internal::ApplyScalarVL<Size,P1,B1,P2,Internal::Multiply> > (s,v);
}
// no left division


///////  Matrix scalar operators

template<int R, int C, typename P1, typename B1, typename P2, typename Op>
struct Operator<Internal::ApplyScalarM<R,C,P1,B1,P2,Op> > {
	const Matrix<R,C,P1,B1>& lhs;
	const P2& rhs;

	Operator(const Matrix<R,C,P1,B1>& m, const P2& s) : lhs(m), rhs(s) {}
		
	template<int R0, int C0, typename P0, typename Ba0>
	void eval(Matrix<R0,C0,P0,Ba0>& m) const {
		for(int r=0; r<m.num_rows(); r++){
			for(int c=0; c<m.num_cols(); c++){
				m(r,c)= Op::template op<P0,P1,P2> (lhs(r,c),rhs);
			}
		}
	}

	int num_rows() const {
		return lhs.num_rows();
	}
	int num_cols() const {
		return lhs.num_cols();
	}
};

template <int R, int C, typename P1, typename B1, typename P2>
Matrix<R,C, typename Internal::Multiply::Return<P1,P2>::Type> operator*(const Matrix<R,C, P1, B1>& m, const P2& s){
	return Operator<Internal::ApplyScalarM<R,C,P1,B1,P2,Internal::Multiply> > (m,s);
}
template <int R, int C, typename P1, typename B1, typename P2>
Matrix<R,C, typename Internal::Divide::Return<P1,P2>::Type> operator/(const Matrix<R,C, P1, B1>& m, const P2& s){
	return Operator<Internal::ApplyScalarM<R,C,P1,B1,P2,Internal::Divide> > (m,s);
}

template<int R, int C, typename P1, typename B1, typename P2, typename Op>
struct Operator<Internal::ApplyScalarML<R,C,P1,B1,P2,Op> > {
	const P2& lhs;
	const Matrix<R,C,P1,B1>& rhs;

	Operator( const P2& s,const Matrix<R,C,P1,B1>& m) : lhs(s), rhs(m) {}
		
	template<int R0, int C0, typename P0, typename Ba0>
	void eval(Matrix<R0,C0,P0,Ba0>& m) const {
		for(int r=0; r<m.num_rows(); r++){
			for(int c=0; c<m.num_cols(); c++){
				m(r,c)= Op::template op<P0,P1,P2> (lhs,rhs(r,c));
			}
		}
	}

	int num_rows() const {
		return rhs.num_rows();
	}
	int num_cols() const {
		return rhs.num_cols();
	}
};

template <int R, int C, typename P1, typename B1, typename P2>
Matrix<R,C, typename Internal::Multiply::Return<P2,P1>::Type> operator*(const P2& s, const Matrix<R,C, P1, B1>& m){
	return Operator<Internal::ApplyScalarML<R,C,P1,B1,P2,Internal::Multiply> > (s,m);
}

////////////////////////////////////////////////////////////////////////////////
//
// Addition of operators
//
template <int Size, typename P1, typename B1, typename Op>
Vector<Size, typename Internal::Add::Return<P1,typename Operator<Op>::Precision>::Type> operator+(const Vector<Size, P1, B1>& v, const Operator<Op>& op){
	return op.add(v);
}

template <int Size, typename P1, typename B1, typename Op>
Vector<Size, typename Internal::Add::Return<typename Operator<Op>::Precision, P1>::Type> operator+(const Operator<Op>& op, const Vector<Size, P1, B1>& v){
	return op.add(v);
}

template <int Rows, int Cols, typename P1, typename B1, typename Op>
Matrix<Rows, Cols, typename Internal::Add::Return<P1,typename Operator<Op>::Precision>::Type> operator+(const Matrix<Rows, Cols, P1, B1>& m, const Operator<Op>& op){
	return op.add(m);
}

template <int Rows, int Cols, typename P1, typename B1, typename Op>
Matrix<Rows, Cols, typename Internal::Add::Return<typename Operator<Op>::Precision,P1>::Type> operator+(const Operator<Op>& op, const Matrix<Rows, Cols, P1, B1>& m){
	return op.add(m);
}




template <int Size, typename P1, typename B1, typename Op>
Vector<Size, typename Internal::Subtract::Return<P1,typename Operator<Op>::Precision>::Type> operator-(const Vector<Size, P1, B1>& v, const Operator<Op>& op){
	return op.rsubtract(v);
}

template <int Size, typename P1, typename B1, typename Op>
Vector<Size, typename Internal::Subtract::Return<typename Operator<Op>::Precision, P1>::Type> operator-(const Operator<Op>& op, const Vector<Size, P1, B1>& v){
	return op.lsubtract(v);
}

template <int Rows, int Cols, typename P1, typename B1, typename Op>
Matrix<Rows, Cols, typename Internal::Subtract::Return<P1,typename Operator<Op>::Precision>::Type> operator-(const Matrix<Rows, Cols, P1, B1>& m, const Operator<Op>& op){
	return op.rsubtract(m);
}

template <int Rows, int Cols, typename P1, typename B1, typename Op>
Matrix<Rows, Cols, typename Internal::Subtract::Return<typename Operator<Op>::Precision,P1>::Type> operator-(const Operator<Op>& op, const Matrix<Rows, Cols, P1, B1>& m){
	return op.lsubtract(m);
}
////////////////////////////////////////////////////////////////////////////////
//
// Stream I/O operators
//

// output operator <<
template <int Size, typename Precision, typename Base>
inline std::ostream& operator<< (std::ostream& os, const Vector<Size,Precision,Base>& v){
  std::streamsize fw = os.width();
  for(int i=0; i<v.size(); i++){
    os.width(fw);
    os << v[i] << " ";
  }
  return os;
}

// operator istream& >>
template <int Size, typename Precision, typename Base>
std::istream& operator >> (std::istream& is, Vector<Size, Precision, Base>& v){
	for (int i=0; i<v.size(); i++){
		is >>  v[i];
	}
	return is;
}

template<int Rows, int Cols, typename Precision, class Base>
inline std::ostream& operator<< (std::ostream& os, const Matrix<Rows, Cols, Precision, Base>& m){
	std::streamsize fw = os.width();
	for(int i=0; i < m.num_rows(); i++)
	{
		for(int j=0; j < m.num_cols(); j++)
		{
			if(j != 0)
				os << " ";
			os.width(fw);
			os << m(i,j);
		}
		os << std::endl;
	}
	return os;
}

// operator istream& >>
template <int Rows, int Cols, typename Precision, typename Base>
std::istream& operator >> (std::istream& is, Matrix<Rows, Cols, Precision, Base>& m){
	for(int r=0; r<m.num_rows(); r++){
		for(int c=0; c < m.num_cols(); c++){
			is >> m(r,c);
		}
	}
	return is;
}


//Overloads of swap
namespace Internal
{
	TOON_CREATE_METHOD_DETECTOR(swap);
	template<class V1, class V2, bool has_swap = Has_swap_Method<V1>::Has>
	struct Swap
	{
		static void swap(V1& v1, V2& v2)
		{
			using std::swap;
			SizeMismatch<V1::SizeParameter,V2::SizeParameter>::test(v1.size(), v2.size());
			for(int i=0; i < v1.size(); i++)
				swap(v1[i], v2[i]);
		}
	};
	
	template<class V>
	struct Swap<V, V, true>
	{
		static void swap(V& v1, V& v2)
		{
			v1.swap(v2);
		}
	};

};


template<int S1, class P1, class B1, int S2, class P2, class B2>
void swap(Vector<S1, P1, B1>& v1, Vector<S2, P2, B2>& v2)
{
	Internal::Swap<Vector<S1, P1, B1>, Vector<S2, P2, B2> >::swap(v1, v2);
}


template<int S1, class P1, class B1>
void swap(Vector<S1, P1, B1>& v1, Vector<S1, P1, B1>& v2)
{
	Internal::Swap<Vector<S1, P1, B1>, Vector<S1, P1, B1> >::swap(v1, v2);
}

}
