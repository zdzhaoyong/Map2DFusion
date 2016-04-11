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


#ifndef TOON_INCLUDE_HELPERS_H
#define TOON_INCLUDE_HELPERS_H

#include <TooN/TooN.h>
#include <TooN/gaussian_elimination.h>
#include <cmath>
#include <functional>
#include <utility>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2 0.707106781186547524401
#endif

#ifdef WIN32
namespace std {
	inline int isfinite( const double & v ){ return _finite(v); }
	inline int isfinite( const float & v ){ return _finite(v); }
	inline int isnan( const double & v ){ return _isnan(v); }
	inline int isnan( const float & v ){ return _isnan(v); }
}
#endif

namespace TooN {
	
	///Invert a matrix.
	///
	///For sizes other than 2x2, @link gDecomps decompositions @endlink provide a suitable solition.
	///
	///@param m Matrix to invert.
	///@ingroup gDecomps
	inline Matrix<2> inv(const Matrix<2>& m)
	{
		double d = 1./(m[0][0]*m[1][1] - m[1][0]*m[0][1]);

		return Data(
		     m[1][1]*d, -m[0][1]*d,
		    -m[1][0]*d,  m[0][0]*d
		);
	}



	///\deprecated
	///@ingroup gLinAlg
	template<int Size, class Precision, class Base> TOON_DEPRECATED void Fill(Vector<Size, Precision, Base>& v, const Precision& p)
	{
		for(int i=0; i < v.size(); i++)
			v[i]= p;
	}

	///\deprecated
	///@ingroup gLinAlg
	template<int Rows, int Cols, class Precision, class Base> TOON_DEPRECATED void Fill(Matrix<Rows, Cols, Precision, Base>& m, const Precision& p)
	{
		for(int i=0; i < m.num_rows(); i++)
			for(int j=0; j < m.num_cols(); j++)
				m[i][j] = p;
	}

	///Compute the \f$L_2\f$ norm of \e v
	///@param v \e v
	///@ingroup gLinAlg
	template<int Size, class Precision, class Base> inline Precision norm(const Vector<Size, Precision, Base>& v)
	{
		using std::sqrt;
		return sqrt(v*v);
	}

	///Compute the \f$L_2^2\f$ norm of \e v
	///@param v \e v
	///@ingroup gLinAlg
	template<int Size, class Precision, class Base> inline Precision norm_sq(const Vector<Size, Precision, Base>& v)
	{
		return v*v;
	}
	
	///Compute the \f$L_1\f$ norm of \e v
	///@param v \e v
	///@ingroup gLinAlg
	template<int Size, class Precision, class Base> inline Precision norm_1(const Vector<Size, Precision, Base>& v)
	{
		using std::abs;
		Precision n = 0;
		for(int i=0; i < v.size(); i++)
			n += abs(v[i]);
		return n;
	}

	///Compute the \f$L_\infty\f$ norm of \e v
	///@param v \e v
	///@ingroup gLinAlg
	template<int Size, class Precision, class Base> inline Precision norm_inf(const Vector<Size, Precision, Base>& v)
	{
		using std::abs;
		using std::max;
		Precision n = 0;
		n = abs(v[0]);

		for(int i=1; i < v.size(); i++)
			n = max(n, abs(v[i]));
		return n;
	}
	
	///Compute the \f$L_2\f$ norm of \e v.
	///Synonym for norm()
	///@param v \e v
	///@ingroup gLinAlg
	template<int Size, class Precision, class Base> inline Precision norm_2(const Vector<Size, Precision, Base>& v)
	{
		return norm(v);
	}
	



	///Compute a the unit vector \f$\hat{v}\f$.
	///@param v \e v
	///@ingroup gLinAlg
	template<int Size, class Precision, class Base> inline Vector<Size, Precision> unit(const Vector<Size, Precision, Base> & v)
	{
		using std::sqrt;
		return TooN::operator*(v,(1/sqrt(v*v)));
	}
	
	//Note because of the overload later, this function will ONLY receive sliced vectors. Therefore
	//a copy can be made, which is still a slice, so operating on the copy operates on the original
	//data.
	///Normalize a vector in place
	///@param v Vector to normalize
	///@ingroup gLinAlg
	template<int Size, class Precision, class Base> inline void normalize(Vector<Size, Precision, Base> v)
	{
		using std::sqrt;
		v /= sqrt(v*v);
	}
	
	//This overload is required to operate on non-slice vectors
	/**
		\overload
	*/  
	template<int Size, class Precision> inline void normalize(Vector<Size, Precision> & v)
	{
		normalize(v.as_slice());
	}

	///For a vector \e v of length \e i, return \f$[v_1, v_2, \cdots, v_{i-1}] / v_i \f$
	///@param v \e v
	///@ingroup gLinAlg
	// Don't remove the +0 in the first template parameter of the return type. It is a work around for a Visual Studio 2010 bug:
	// https://connect.microsoft.com/VisualStudio/feedback/details/735283/vc-2010-parse-error-in-template-parameters-using-ternary-operator
	template<int Size, typename Precision, typename Base> inline Vector<(Size==Dynamic?Dynamic:Size-1)+0, Precision> project( const Vector<Size, Precision, Base> & v){
		static const int Len = (Size==Dynamic?Dynamic:Size-1);
		return TooN::operator/(v.template slice<0, Len>(0 , v.size() - 1) , v[v.size() - 1]);
	}

	//This should probably be done with an operator to prevent an extra new[] for dynamic vectors.
	///For a vector \e v of length \e i, return \f$[v_1, v_2, \cdots, v_{i}, 1]\f$
	///@param v \e v
	///@ingroup gLinAlg
	// Don't remove the +0 in the first template parameter of the return type. It is a work around for a Visual Studio 2010 bug:
	// https://connect.microsoft.com/VisualStudio/feedback/details/735283/vc-2010-parse-error-in-template-parameters-using-ternary-operator
	template<int Size, typename Precision, typename Base> inline Vector<(Size==Dynamic?Dynamic:Size+1)+0, Precision> unproject( const Vector<Size, Precision, Base> & v){
		Vector<(Size==Dynamic?Dynamic:Size+1), Precision> result(v.size()+1);
		static const int Len = (Size==Dynamic?Dynamic:Size);
		result.template slice<0, Len>(0, v.size()) = v;
		result[v.size()] = 1;
		return result;
	}
	
	/**
       \overload
	*/
	template<int R, int C, typename Precision, typename Base> inline Matrix<R-1, C, Precision> project( const Matrix<R,C, Precision, Base> & m){
        Matrix<R-1, C, Precision> result = m.slice(0,0,R-1,m.num_cols());
        for( int c = 0; c < m.num_cols(); ++c ) {
            result.slice(0,c,R-1,1) /= m[R-1][c];
        }
        return result;
    }

    template<int C, typename Precision, typename Base> inline Matrix<-1, C, Precision> project( const Matrix<-1,C, Precision, Base> & m){
        Matrix<-1, C, Precision> result = m.slice(0,0,m.num_rows()-1,m.num_cols());
        for( int c = 0; c < m.num_cols(); ++c ) {
            result.slice(0,c,m.num_rows()-1,1) /= m[m.num_rows()-1][c];
        }
        return result;
    }

	/**
       \overload
	*/
    template<int R, int C, typename Precision, typename Base> inline Matrix<R+1, C, Precision> unproject( const Matrix<R, C, Precision, Base> & m){
        Matrix<R+1, C, Precision> result;
        result.template slice<0,0,R,C>() = m;
        result[R] = Ones;
        return result;
    }

    template<int C, typename Precision, typename Base> inline Matrix<-1, C, Precision> unproject( const Matrix<-1, C, Precision, Base> & m){
        Matrix<-1, C, Precision> result( m.num_rows()+1, m.num_cols() );
        result.slice(0,0,m.num_rows(),m.num_cols()) = m;
        result[m.num_rows()] = Ones;
        return result;
    }

	/// Frobenius (root of sum of squares) norm of input matrix \e m
	///@param m \e m
	///@ingroup gLinAlg
	template <int R, int C, typename P, typename B>
	P inline norm_fro( const Matrix<R,C,P,B> & m ){
		using std::sqrt;
		P n = 0;
		for(int r = 0; r < m.num_rows(); ++r)
			for(int c = 0; c < m.num_cols(); ++c)
				n += m[r][c] * m[r][c];

		return sqrt(n);
	}

	/// \e L<sub>&#8734;</sub> (row sum) norm of input matrix m
	/// computes the maximum of the sums of absolute values over rows
	///@ingroup gLinAlg
	template <int R, int C, typename P, typename B>
	P inline norm_inf( const Matrix<R,C,P,B> & m ){
		using std::abs;
		using std::max;
		P n = 0;
		for(int r = 0; r < m.num_rows(); ++r){
			P s = 0;
			for(int c = 0; c < m.num_cols(); ++c)
				s += abs(m(r,c));
			n = max(n,s);
		}
		return n;
	}
	
	/// \e L<sub>1</sub> (col sum) norm of input matrix m
	/// computes the maximum of the sums of absolute values over columns
	///@ingroup gLinAlg
	template <int R, int C, typename P, typename B>
	P inline norm_1( const Matrix<R,C,P,B> & m ){
		using std::abs;
		using std::max;
		P n = 0;
		for(int c = 0; c < m.num_cols(); ++c){
			P s = 0;
			for(int r = 0; r < m.num_rows(); ++r)
				s += abs(m(r,c));
			n = max(n,s);
		}
		return n;
	}

	namespace Internal {
		///@internal
		///@brief Exponentiate a matrix using a the Taylor series
		///This will not work if the norm of the matrix is too large.
		template <int R, int C, typename P, typename B>
		inline Matrix<R, C, P> exp_taylor( const Matrix<R,C,P,B> & m ){
			TooN::SizeMismatch<R, C>::test(m.num_rows(), m.num_cols());
			Matrix<R,C,P> result = TooN::Zeros(m.num_rows(), m.num_cols());
			Matrix<R,C,P> f = TooN::Identity(m.num_rows());
			P k = 1;
			while(norm_inf((result+f)-result) > 0){
				result += f;
				f = (m * f) / k;
				k += 1;
			}
			return result;
		}

		///@internal
		///@brief Logarithm of a matrix using a the Taylor series
		///This will not work if the norm of the matrix is too large.
		template <int R, int C, typename P, typename B>
		inline Matrix<R, C, P> log_taylor( const Matrix<R,C,P,B> & m ){
			TooN::SizeMismatch<R, C>::test(m.num_rows(), m.num_cols());
			Matrix<R,C,P> X = m - TooN::Identity * 1.0;
			Matrix<R,C,P> F = X;
			Matrix<R,C,P> sum = TooN::Zeros(m.num_rows(), m.num_cols());
			P k = 1;
			while(norm_inf((sum+F/k)-sum) > 0){
				sum += F/k;
				F = -F*X;
				k += 1;
			}
			return sum;
		}

	};
	
	/// computes the matrix exponential of a matrix m by 
	/// scaling m by 1/(powers of 2), using Taylor series and 
	/// squaring again.
	/// @param m input matrix, must be square
	/// @return result matrix of the same size/type as input
	/// @ingroup gLinAlg
	template <int R, int C, typename P, typename B>
	inline Matrix<R, C, P> exp( const Matrix<R,C,P,B> & m ){
		using std::max;
		SizeMismatch<R, C>::test(m.num_rows(), m.num_cols());
		const P l = log2(norm_inf(m));
		const int s = max(0,(int)ceil(l));
		Matrix<R,C,P> result = Internal::exp_taylor(m/(1<<s));
		for(int i = 0; i < s; ++i)
			result = result * result;
		return result;
	}
	
	/// computes a matrix square root of a matrix m by
	/// the product form of the Denman and Beavers iteration
	/// as given in Chen et al. 'Approximating the logarithm of a matrix to specified accuracy', 
	/// J. Matrix Anal Appl, 2001. This is used for the matrix
	/// logarithm function, but is useable by on its own.
	/// @param m input matrix, must be square
	/// @return a square root of m of the same size/type as input
	/// @ingroup gLinAlg
	template <int R, int C, typename P, typename B>
	inline Matrix<R, C, P> sqrt( const Matrix<R,C,P,B> & m){
		SizeMismatch<R, C>::test(m.num_rows(), m.num_cols());
		Matrix<R,C,P> M = m;
		Matrix<R,C,P> Y = m;
		Matrix<R,C,P> M_inv(m.num_rows(), m.num_cols());
		const Matrix<R,C,P> id = Identity(m.num_rows());
		do {
			M_inv = gaussian_elimination(M, id);
			Y = Y * (id + M_inv) * 0.5;
			M = 0.5 * (id + (M + M_inv) * 0.5);
		} while(norm_inf(M - M_inv) > 0);
		return Y;
	}
	
	/// computes the matrix logarithm of a matrix m using the inverse scaling and 
	/// squaring method. The overall approach is described in
	/// Chen et al. 'Approximating the logarithm of a matrix to specified accuracy', 
	/// J. Matrix Anal Appl, 2001, but this implementation only uses a simple
	/// taylor series after the repeated square root operation.
	/// @param m input matrix, must be square
	/// @return the log of m of the same size/type as input
	/// @ingroup gLinAlg
	template <int R, int C, typename P, typename B>
	inline Matrix<R, C, P> log( const Matrix<R,C,P,B> & m){
		int counter = 0;
		Matrix<R,C,P> A = m;
		while(norm_inf(A - Identity*1.0) > 0.5){
			++counter;
			A = sqrt(A);
		}
		return Internal::log_taylor(A) * pow(2.0, counter);
	}
	
	/// Returns true if every element is finite
	///@ingroup gLinAlg
	template<int S, class P, class B> bool isfinite(const Vector<S, P, B>& v)
	{ 
		using std::isfinite;
		for(int i=0; i < v.size(); i++)
			if(!isfinite(v[i]))
				return 0;
		return 1;
	}

	/// Returns true if any element is NaN
	///@ingroup gLinAlg
	template<int S, class P, class B> bool isnan(const Vector<S, P, B>& v)
	{ 
		using std::isnan;
		for(int i=0; i < v.size(); i++)
			if(isnan(v[i]))
				return 1;
		return 0;
	}

	/// Symmetrize a matrix 
	///@param m \e m
	///@return \f$ \frac{m + m^{\mathsf T}}{2} \f$
	///@ingroup gLinAlg
	template<int Rows, int Cols, typename Precision, typename Base>
	void Symmetrize(Matrix<Rows,Cols,Precision,Base>& m){
		SizeMismatch<Rows,Cols>::test(m.num_rows(), m.num_cols());
		for(int r=0; r<m.num_rows()-1; r++){
			for(int c=r+1; c<m.num_cols(); c++){
				const Precision temp=(m(r,c)+m(c,r))/2;
				m(r,c)=temp;
				m(c,r)=temp;
			}
		}
	}
	
	/// computes the trace of a square matrix
	///@ingroup gLinAlg
	template<int Rows, int Cols, typename Precision, typename Base>
	Precision trace(const Matrix<Rows, Cols, Precision, Base> & m ){
		SizeMismatch<Rows,Cols>::test(m.num_rows(), m.num_cols());
		Precision tr = 0;
		for(int i = 0; i < m.num_rows(); ++i)
			tr += m(i,i);
		return tr;
	}

	/// creates an returns a cross product matrix M from a 3 vector v, such that for all vectors w, the following holds: v ^ w = M * w
	/// @param vec the 3 vector input
	/// @return the 3x3 matrix to set to the cross product matrix
	///@ingroup gLinAlg
	template<int Size, class P, class B> inline TooN::Matrix<3, 3, P> cross_product_matrix(const Vector<Size, P, B>& vec)
	{
		SizeMismatch<Size,3>::test(vec.size(), 3);

		TooN::Matrix<3, 3, P> result;

		result(0,0) = 0; 
		result(0,1) = -vec[2]; 
		result(0,2) = vec[1];
		result(1,0) = vec[2]; 
		result(1,1) = 0; 
		result(1,2) = -vec[0];
		result(2,0) = -vec[1]; 
		result(2,1) = vec[0]; 
		result(2,2) = 0;

		return result;
	}

    namespace Internal {
        template<int Size, typename Precision, typename Base, typename Func, typename Ret> inline Ret accumulate( const Vector<Size, Precision, Base> & v )  {
            Func func;
            if( v.size() == 0 ) {
                return func.null(); // What should we return, exception?
            }
            func.initialise( v[0], 0 );
            for( int ii = 1; ii < v.size(); ii++ ) {
                func( v[ii], ii );
            }
            return func.ret();
        }

        template<int R, int C, typename Precision, typename Base, typename Func, typename Ret> inline Ret accumulate( const Matrix<R, C, Precision, Base> & m )  {
            Func func;
            if( m.num_rows() == 0 || m.num_cols() == 0) {
                return func.null(); // What should we return, exception?
            }
            func.initialise( m[0][0], 0, 0 );
            for(int r=0; r<m.num_rows(); r++){
                for(int c=0; c<m.num_cols(); c++){
                    func( m[r][c], r, c );
                }
            }
            return func.ret();
        }
        template<int R, int C, typename Precision, typename Base, typename Func, typename Ret> inline Ret accumulate_horizontal( const Matrix<R, C, Precision, Base> & m ) {
            Func func( m.num_rows() );
            if( m.num_cols() == 0 || m.num_rows() == 0 ) {
                func.null(); // What should we return, exception?
            }
            for(int r=0; r<m.num_rows(); r++){
                func.initialise( m[r][0], r, 0 );
                for(int c=1; c<m.num_cols(); c++){
                    func( m[r][c], r, c );
                }
            }
            return func.ret();
        }
        template<int R, int C, typename Precision, typename Base, typename Func, typename Ret> inline Ret accumulate_vertical( const Matrix<R, C, Precision, Base> & m ) {
            Func func( m.num_cols() );
            if( m.num_cols() == 0 || m.num_rows() == 0 ) {
                func.null(); // What should we return, exception?
            }
            for(int c=0; c<m.num_cols(); c++){
                func.initialise( m[0][c], 0, c );
                for(int r=1; r<m.num_rows(); r++){
                    func( m[r][c], r, c );
                }
            }
            return func.ret();
        }        

        template<typename Precision, typename ComparisonFunctor>
        class accumulate_functor_vector {
            Precision bestVal;
        public:
            Precision null() {
                return 0;
            }
            void initialise( Precision initialVal, int ) {
                bestVal = initialVal;
            }
            void operator()( Precision curVal, int ) {
                if( ComparisonFunctor()( curVal, bestVal ) ) {
                    bestVal = curVal;
                }
            }
            Precision ret() { return bestVal; }            
        };
        template<typename Precision, typename ComparisonFunctor>
        class accumulate_element_functor_vector {
            Precision bestVal;
            int nBestIndex;
        public:
            std::pair<Precision,int> null() {
                return std::pair<Precision,int>( 0, 0 );
            }
            void initialise( Precision initialVal, int nIndex ) {
                bestVal = initialVal;
                nBestIndex = nIndex;
            }
            void operator()( Precision curVal, int nIndex ) {
                if( ComparisonFunctor()( curVal, bestVal ) ) {
                    bestVal = curVal;
                    nBestIndex = nIndex;
                }
            }
            std::pair<Precision,int> ret() {
                return std::pair<Precision,int>( bestVal, nBestIndex );
            }            
        };
        template<typename Precision, typename ComparisonFunctor>
        class accumulate_functor_matrix {
            Precision bestVal;
        public:
            Precision null() {
                return 0;
            }
            void initialise( Precision initialVal, int, int ) {
                bestVal = initialVal;
            }
            void operator()( Precision curVal, int, int ) {
                if( ComparisonFunctor()( curVal, bestVal ) ) {
                    bestVal = curVal;
                }
            }
            Precision ret() { return bestVal; }            
        };
        template<typename Precision, typename ComparisonFunctor>
        class accumulate_element_functor_matrix {
            Precision bestVal;
            int nBestRow;
            int nBestCol;
        public:
            std::pair<Precision,std::pair<int,int> > null() {
                return std::pair<Precision,std::pair<int,int> >( 0, std::pair<int,int>( 0, 0 ) );
            }
            void initialise( Precision initialVal, int nRow, int nCol ) {
                bestVal = initialVal;
                nBestRow = nRow;
                nBestCol = nCol;
            }
            void operator()( Precision curVal, int nRow, int nCol ) {
                if( ComparisonFunctor()( curVal, bestVal ) ) {
                    bestVal = curVal;
                    nBestRow = nRow;
                    nBestCol = nCol;
                }
            }
            std::pair<Precision,std::pair<int,int> > ret() {
                return std::pair<Precision,std::pair<int,int> >( bestVal, 
                                                                 std::pair<int,int>( nBestRow, nBestCol ) );
            }            
        };
        template<typename Precision, typename ComparisonFunctor>
        class accumulate_vertical_functor {
            Vector<Dynamic,Precision>* bestVal;
        public:
            accumulate_vertical_functor() {
                bestVal = NULL;
            }
            accumulate_vertical_functor( int nNumCols ) {
                bestVal = new Vector<Dynamic,Precision>( nNumCols );
            }
            Vector<Dynamic,Precision> null() {
                return Vector<Dynamic,Precision>( 0 );
            }
            void initialise( Precision initialVal, int, int nCol ) {
                (*bestVal)[nCol] = initialVal;
            }
            void operator()( Precision curVal, int, int nCol ) {
                if( ComparisonFunctor()( curVal, (*bestVal)[nCol] ) ) {
                    (*bestVal)[nCol] = curVal;
                }
            }
            Vector<Dynamic,Precision> ret() {
                if( bestVal == NULL ) {
                    return null();
                }
                Vector<Dynamic,Precision> vRet = *bestVal; 
                delete bestVal;
                return vRet;
            }            
        };
        template<typename Precision, typename ComparisonFunctor>
        class accumulate_element_vertical_functor {
            Vector<Dynamic,Precision>* bestVal;
            Vector<Dynamic,Precision>* bestIndices;
        public:
            accumulate_element_vertical_functor() {
                bestVal = NULL;
                bestIndices = NULL;
            }
            accumulate_element_vertical_functor( int nNumCols ) {
                bestVal = new Vector<Dynamic,Precision>( nNumCols );
                bestIndices = new Vector<Dynamic,Precision>( nNumCols );
            }
            std::pair<Vector<Dynamic,Precision>,Vector<Dynamic,Precision> > null() {
                Vector<Dynamic,Precision> vEmpty( 0 );
                return std::pair<Vector<Dynamic,Precision>,Vector<Dynamic,Precision> >( vEmpty, vEmpty );
            }
            void initialise( Precision initialVal, int nRow, int nCol ) {
                (*bestVal)[nCol] = initialVal;
                (*bestIndices)[nCol] = nRow;
            }
            void operator()( Precision curVal, int nRow, int nCol ) {
                if( ComparisonFunctor()( curVal, (*bestVal)[nCol] ) ) {
                    (*bestVal)[nCol] = curVal;
                    (*bestIndices)[nCol] = nRow;
                }
            }
            std::pair<Vector<Dynamic,Precision>,Vector<Dynamic,Precision> > ret() {
                if( bestVal == NULL ) {
                    return null();
                }
                std::pair<Vector<Dynamic,Precision>,Vector<Dynamic,Precision> > vRet = 
                    std::pair<Vector<Dynamic,Precision>,Vector<Dynamic,Precision> > (*bestVal, *bestIndices );
                delete bestVal; bestVal = NULL;
                delete bestIndices; bestIndices = NULL;
                return vRet;
            }            
        };
        template<typename Precision, typename ComparisonFunctor>
        class accumulate_horizontal_functor {
            Vector<Dynamic,Precision>* bestVal;
        public: 
            accumulate_horizontal_functor() {
                bestVal = NULL;
            }
            accumulate_horizontal_functor( int nNumRows ) {
                bestVal = new Vector<Dynamic,Precision>( nNumRows );
            }
            Vector<Dynamic,Precision> null() {
                return Vector<Dynamic,Precision>( 0 );
            }
            void initialise( Precision initialVal, int nRow, int ) {
                (*bestVal)[nRow] = initialVal;
            }
            void operator()( Precision curVal, int nRow, int ) {
                if( ComparisonFunctor()( curVal, (*bestVal)[nRow] ) ) {
                    (*bestVal)[nRow] = curVal;
                }
            }
            Vector<Dynamic,Precision> ret() { 
                if( bestVal == NULL ) {
                    return null();
                }
                Vector<Dynamic,Precision> vRet = *bestVal;
                delete bestVal; bestVal = NULL;
                return vRet; 
            }            
        };
        template<typename Precision, typename ComparisonFunctor>
        class accumulate_element_horizontal_functor {
            Vector<Dynamic,Precision>* bestVal;
            Vector<Dynamic,Precision>* bestIndices;
        public:
            accumulate_element_horizontal_functor() {
                bestVal = NULL;
                bestIndices = NULL;
            }
            accumulate_element_horizontal_functor( int nNumRows ) {
                bestVal = new Vector<Dynamic,Precision>( nNumRows );
                bestIndices = new Vector<Dynamic,Precision>( nNumRows );
            }
            std::pair<Vector<Dynamic,Precision>,Vector<Dynamic,Precision> > null() {
                Vector<Dynamic,Precision> vEmpty( 0 );
                return std::pair<Vector<Dynamic,Precision>,Vector<Dynamic,Precision> >( vEmpty, vEmpty );
            }
            void initialise( Precision initialVal, int nRow, int nCol ) {
                (*bestVal)[nRow] = initialVal;
                (*bestIndices)[nRow] = nCol;
            }
            void operator()( Precision curVal, int nRow, int nCol ) {
                if( ComparisonFunctor()( curVal, (*bestVal)[nRow] ) ) {
                    (*bestVal)[nRow] = curVal;
                    (*bestIndices)[nRow] = nCol;
                }
            }
            std::pair<Vector<Dynamic,Precision>,Vector<Dynamic,Precision> > ret() {
                if( bestVal == NULL ) {
                    return null();
                }
                std::pair<Vector<Dynamic,Precision>,Vector<Dynamic,Precision> > vRet = 
                    std::pair<Vector<Dynamic,Precision>,Vector<Dynamic,Precision> >( *bestVal, *bestIndices );
                delete bestVal; bestVal = NULL;
                delete bestIndices; bestIndices = NULL;
                return vRet;
            }            
        };
    }


	/// Finds the minimal value of a vector.
	/// @param v a vector
	/// @return the smallest value of v
    template<int Size, typename Precision, typename Base> inline Precision min_value( const Vector<Size, Precision, Base>& v) {
        typedef Internal::accumulate_functor_vector<Precision, std::less<Precision> > vector_accumulate_functor;
        return Internal::accumulate<Size,Precision,Base,
            vector_accumulate_functor, Precision >( v ); 
    }
	/// Finds the largest value of a vector.
	/// @param v a vector
	/// @return the largest value of v
    template<int Size, typename Precision, typename Base> inline Precision max_value( const Vector<Size, Precision, Base>& v) {
        typedef Internal::accumulate_functor_vector<Precision, std::greater<Precision> > vector_accumulate_functor;
        return Internal::accumulate<Size,Precision,Base,
            vector_accumulate_functor, Precision >( v ); 
    }

	/// Finds the smallest value of a matrix.
	/// @param m a matrix
	/// @return the smallest value of m
    template<int R, int C, typename Precision, typename Base> inline Precision min_value( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_functor_matrix<Precision, std::less<Precision> > matrix_accumulate_functor;
        return Internal::accumulate<R,C,Precision,Base,
            matrix_accumulate_functor, Precision>( m );
    }
	/// Finds the largest value of a matrix.
	/// @param m a matrix
	/// @return the largest value of m
    template<int R, int C, typename Precision, typename Base> inline Precision max_value( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_functor_matrix<Precision, std::greater<Precision> > matrix_accumulate_functor;
        return Internal::accumulate<R,C,Precision,Base,
            matrix_accumulate_functor, Precision>( m );
    }
	/// Finds the smallest values of each column of a matrix.
	/// @param m a matrix
	/// @return a vector of size C
    template<int R, int C, typename Precision, typename Base> inline Vector<Dynamic,Precision> min_value_vertical( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_vertical_functor<Precision,std::less<Precision> > matrix_accumulate_vertical_functor;
        return Internal::accumulate_vertical<R,C,Precision,Base,
            matrix_accumulate_vertical_functor, Vector<Dynamic,Precision> >( m );
    }
	/// Finds the largest values of each column of a matrix.
	/// @param m a matrix
	/// @return a vector of size C
    template<int R, int C, typename Precision, typename Base> inline Vector<Dynamic,Precision> max_value_vertical( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_vertical_functor<Precision,std::greater<Precision> > matrix_accumulate_vertical_functor;
        return Internal::accumulate_vertical<R,C,Precision,Base,
            matrix_accumulate_vertical_functor, Vector<Dynamic,Precision> >( m );
    }
	/// Finds the smallest values of each row of a matrix.
	/// @param m a matrix
	/// @return a vector of size R
    template<int R, int C, typename Precision, typename Base> inline Vector<Dynamic,Precision> min_value_horizontal( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_horizontal_functor<Precision,std::less<Precision> > matrix_accumulate_horizontal_functor;
        return Internal::accumulate_horizontal<R,C,Precision,Base,
            matrix_accumulate_horizontal_functor, Vector<Dynamic,Precision> >( m );
    }
	/// Finds the largest values of each row of a matrix.
	/// @param m a matrix
	/// @return a vector of size R
    template<int R, int C, typename Precision, typename Base> inline Vector<Dynamic,Precision> max_value_horizontal( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_horizontal_functor<Precision,std::greater<Precision> > matrix_accumulate_horizontal_functor;
        return Internal::accumulate_horizontal<R,C,Precision,Base,
            matrix_accumulate_horizontal_functor, Vector<Dynamic,Precision> >( m );
    }
	/// Finds the smallest value of a vector and its index.
	/// @param v a vector
	/// @return a pair containing the smallest value and its index
    template<int Size, typename Precision, typename Base> inline std::pair<Precision,int> min_element( const Vector<Size, Precision, Base>& v) {
        typedef Internal::accumulate_element_functor_vector<Precision, std::less<Precision> > vector_accumulate_functor;
        return Internal::accumulate<Size,Precision,Base,
            vector_accumulate_functor, std::pair<Precision,int> >( v ); 
    }
	/// Finds the largest value of a vector and its index.
	/// @param v a vector
	/// @return a pair containing the largest value and its index
    template<int Size, typename Precision, typename Base> inline std::pair<Precision,int> max_element( const Vector<Size, Precision, Base>& v) {
        typedef Internal::accumulate_element_functor_vector<Precision, std::greater<Precision> > vector_accumulate_functor;
        return Internal::accumulate<Size,Precision,Base,
            vector_accumulate_functor, std::pair<Precision,int> >( v ); 
    }    
	/// Finds the smallest value of a matrix and its row and column.
	/// @param m a matrix
	/// @return a pair containing the smallest value and a pair
	/// containing its row and column
    template<int R, int C, typename Precision, typename Base> inline std::pair<Precision,std::pair<int,int> > min_element( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_element_functor_matrix<Precision, std::less<Precision> > matrix_accumulate_functor;
        typedef std::pair<Precision,std::pair<int,int> > Ret;
        return Internal::accumulate<R,C,Precision,Base,
            matrix_accumulate_functor, Ret>( m );
    }
	/// Finds the largest value of a matrix and its row and column.
	/// @param m a matrix
	/// @return a pair containing the largest value and a pair
	/// containing its row and column
    template<int R, int C, typename Precision, typename Base> inline std::pair<Precision,std::pair<int,int> > max_element( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_element_functor_matrix<Precision, std::greater<Precision> > matrix_accumulate_functor;
        typedef std::pair<Precision,std::pair<int,int> > Ret;
        return Internal::accumulate<R,C,Precision,Base,
            matrix_accumulate_functor, Ret>( m );
    }	
    /// Finds the smallest values of each column of a matrix and their
	/// indices.
	/// @param m a matrix
	/// @return a pair of vectors of size C containg the values and
	/// their indices
    template<int R, int C, typename Precision, typename Base> inline std::pair<Vector<Dynamic,Precision>,Vector<Dynamic,Precision> > min_element_vertical( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_element_vertical_functor<Precision,std::less<Precision> > matrix_accumulate_vertical_functor;
        typedef std::pair<Vector< Dynamic, Precision >,Vector< Dynamic, Precision > > Ret;
        return Internal::accumulate_vertical<R,C,Precision,Base,
            matrix_accumulate_vertical_functor, Ret >( m );
    }
    /// Finds the largest values of each column of a matrix and their
	/// indices.
	/// @param m a matrix
	/// @return a pair of vectors of size C containg the values and
	/// their indices
    template<int R, int C, typename Precision, typename Base> inline std::pair<Vector< Dynamic, Precision >,Vector< Dynamic, Precision > > max_element_vertical( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_element_vertical_functor<Precision,std::greater<Precision> > matrix_accumulate_vertical_functor;
        typedef std::pair<Vector< Dynamic, Precision >,Vector< Dynamic, Precision > > Ret;
        return Internal::accumulate_vertical<R,C,Precision,Base,
            matrix_accumulate_vertical_functor, Ret >( m );
    }
    /// Finds the smallest values of each row of a matrix and their
	/// indices.
	/// @param m a matrix
	/// @return a pair of vectors of size R containg the values and
	/// their indices
    template<int R, int C, typename Precision, typename Base> inline std::pair<Vector< Dynamic, Precision >,Vector< Dynamic, Precision > > min_element_horizontal( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_element_horizontal_functor<Precision,std::less<Precision> > matrix_accumulate_vertical_functor;
        typedef std::pair<Vector< Dynamic, Precision >,Vector< Dynamic, Precision > > Ret;
        return Internal::accumulate_horizontal<R,C,Precision,Base,
            matrix_accumulate_vertical_functor, Ret >( m );
    }
    /// Finds the largest values of each row of a matrix and their
	/// indices.
	/// @param m a matrix
	/// @return a pair of vectors of size R containg the values and
	/// their indices
    template<int R, int C, typename Precision, typename Base> inline std::pair<Vector< Dynamic, Precision >,Vector< Dynamic, Precision > > max_element_horizontal( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_element_horizontal_functor<Precision,std::greater<Precision> > matrix_accumulate_vertical_functor;
        typedef std::pair<Vector< Dynamic, Precision >,Vector< Dynamic, Precision > > Ret;
        return Internal::accumulate_horizontal<R,C,Precision,Base,
            matrix_accumulate_vertical_functor, Ret >( m );
    }
}
#endif
