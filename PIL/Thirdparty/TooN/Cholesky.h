// -*- c++ -*-

//     Copyright (C) 2009 Tom Drummond (twd20@cam.ac.uk)
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


#ifndef TOON_INCLUDE_CHOLESKY_H
#define TOON_INCLUDE_CHOLESKY_H

#include <TooN/TooN.h>

namespace TooN {


/**
Decomposes a positive-semidefinite symmetric matrix A (such as a covariance) into L*D*L^T, where L is lower-triangular and D is diagonal.
Also can compute the classic A = L*L^T, with L lower triangular.  The LDL^T form is faster to compute than the classical Cholesky decomposition. 
Use get_unscaled_L() and get_D() to access the individual matrices of L*D*L^T decomposition. Use get_L() to access the lower triangular matrix of the classic Cholesky decomposition L*L^T.
The decomposition can be used to compute A^-1*x, A^-1*M, M*A^-1*M^T, and A^-1 itself, though the latter rarely needs to be explicitly represented.
Also efficiently computes det(A) and rank(A).
It can be used as follows:
@code
// Declare some matrices.
Matrix<3> A = ...; // we'll pretend it is pos-def
Matrix<2,3> M;
Matrix<2> B;
Vector<3> y = make_Vector(2,3,4);
// create the Cholesky decomposition of A
Cholesky<3> chol(A);
// compute x = A^-1 * y
x = cholA.backsub(y);
//compute A^-1
Matrix<3> Ainv = cholA.get_inverse();
@endcode
@ingroup gDecomps

Cholesky decomposition of a symmetric matrix.
Only the lower half of the matrix is considered
This uses the non-sqrt version of the decomposition
giving symmetric M = L*D*L.T() where the diagonal of L contains ones
@param Size the size of the matrix
@param Precision the precision of the entries in the matrix and its decomposition
**/
template <int Size=Dynamic, class Precision=DefaultPrecision>
class Cholesky {
public:
	Cholesky(){}

    /// Construct the Cholesky decomposition of a matrix. This initialises the class, and
    /// performs the decomposition immediately.
    /// Run time is O(N^3)
	template<class P2, class B2>
	Cholesky(const Matrix<Size, Size, P2, B2>& m)
		: my_cholesky(m) {
		SizeMismatch<Size,Size>::test(m.num_rows(), m.num_cols());
		do_compute();
	}
	
	/// Constructor for Size=Dynamic
	Cholesky(int size) : my_cholesky(size,size) {}


    /// Compute the LDL^T decomposition of another matrix.
    /// Run time is O(N^3)
	template<class P2, class B2> void compute(const Matrix<Size, Size, P2, B2>& m){
		SizeMismatch<Size,Size>::test(m.num_rows(), m.num_cols());
		SizeMismatch<Size,Size>::test(m.num_rows(), my_cholesky.num_rows());
		my_cholesky=m;
		do_compute();
	}
	
	private:
	void do_compute() {
		int size=my_cholesky.num_rows();
		for(int col=0; col<size; col++){
			Precision inv_diag = 1;
			for(int row=col; row < size; row++){
				// correct for the parts of cholesky already computed
				Precision val = my_cholesky(row,col);
				for(int col2=0; col2<col; col2++){
					// val-=my_cholesky(col,col2)*my_cholesky(row,col2)*my_cholesky(col2,col2);
					val-=my_cholesky(col2,col)*my_cholesky(row,col2);
				}
				if(row==col){
					// this is the diagonal element so don't divide
					my_cholesky(row,col)=val;
					if(val == 0){
						my_rank = row;
						return;
					}
					inv_diag=1/val;
				} else {
					// cache the value without division in the upper half
					my_cholesky(col,row)=val;
					// divide my the diagonal element for all others
					my_cholesky(row,col)=val*inv_diag;
				}
			}
		}
		my_rank = size;
	}

	public:

	/// Compute x = A^-1*v
    /// Run time is O(N^2)
	template<int Size2, class P2, class B2>
	Vector<Size, Precision> backsub (const Vector<Size2, P2, B2>& v) const {
		int size=my_cholesky.num_rows();
		SizeMismatch<Size,Size2>::test(size, v.size());

		// first backsub through L
		Vector<Size, Precision> y(size);
		for(int i=0; i<size; i++){
			Precision val = v[i];
			for(int j=0; j<i; j++){
				val -= my_cholesky(i,j)*y[j];
			}
			y[i]=val;
		}
		
		// backsub through diagonal
		for(int i=0; i<size; i++){
			y[i]/=my_cholesky(i,i);
		}

		// backsub through L.T()
		Vector<Size,Precision> result(size);
		for(int i=size-1; i>=0; i--){
			Precision val = y[i];
			for(int j=i+1; j<size; j++){
				val -= my_cholesky(j,i)*result[j];
			}
			result[i]=val;
		}

		return result;
	}

	/**overload
	*/
	template<int Size2, int C2, class P2, class B2>
	Matrix<Size, C2, Precision> backsub (const Matrix<Size2, C2, P2, B2>& m) const {
		int size=my_cholesky.num_rows();
		SizeMismatch<Size,Size2>::test(size, m.num_rows());

		// first backsub through L
		Matrix<Size, C2, Precision> y(size, m.num_cols());
		for(int i=0; i<size; i++){
			Vector<C2, Precision> val = m[i];
			for(int j=0; j<i; j++){
				val -= my_cholesky(i,j)*y[j];
			}
			y[i]=val;
		}
		
		// backsub through diagonal
		for(int i=0; i<size; i++){
			y[i]*=(1/my_cholesky(i,i));
		}

		// backsub through L.T()
		Matrix<Size,C2,Precision> result(size, m.num_cols());
		for(int i=size-1; i>=0; i--){
			Vector<C2,Precision> val = y[i];
			for(int j=i+1; j<size; j++){
				val -= my_cholesky(j,i)*result[j];
			}
			result[i]=val;
		}
		return result;
	}


    /// Compute A^-1 and store in M
    /// Run time is O(N^3)
	// easy way to get inverse - could be made more efficient
	Matrix<Size,Size,Precision> get_inverse(){
		Matrix<Size,Size,Precision>I(Identity(my_cholesky.num_rows()));
		return backsub(I);
	}
	
	///Compute the determinant.
	Precision determinant(){
		Precision answer=my_cholesky(0,0);
		for(int i=1; i<my_cholesky.num_rows(); i++){
			answer*=my_cholesky(i,i);
		}
		return answer;
	}

	template <int Size2, typename P2, typename B2>
	Precision mahalanobis(const Vector<Size2, P2, B2>& v) const {
		return v * backsub(v);
	}

	Matrix<Size,Size,Precision> get_unscaled_L() const {
		Matrix<Size,Size,Precision> m(my_cholesky.num_rows(),
					      my_cholesky.num_rows());
		m=Identity;
		for (int i=1;i<my_cholesky.num_rows();i++) {
			for (int j=0;j<i;j++) {
				m(i,j)=my_cholesky(i,j);
			}
		}
		return m;
	}
			
	Matrix<Size,Size,Precision> get_D() const {
		Matrix<Size,Size,Precision> m(my_cholesky.num_rows(),
					      my_cholesky.num_rows());
		m=Zeros;
		for (int i=0;i<my_cholesky.num_rows();i++) {
			m(i,i)=my_cholesky(i,i);
		}
		return m;
	}
	
	Matrix<Size,Size,Precision> get_L() const {
		using std::sqrt;
		Matrix<Size,Size,Precision> m(my_cholesky.num_rows(),
					      my_cholesky.num_rows());
		m=Zeros;
		for (int j=0;j<my_cholesky.num_cols();j++) {
			Precision sqrtd=sqrt(my_cholesky(j,j));
			m(j,j)=sqrtd;
			for (int i=j+1;i<my_cholesky.num_rows();i++) {
				m(i,j)=my_cholesky(i,j)*sqrtd;
			}
		}
		return m;
	}

	int rank() const { return my_rank; }

private:
	Matrix<Size,Size,Precision> my_cholesky;
	int my_rank;
};


}

#endif
