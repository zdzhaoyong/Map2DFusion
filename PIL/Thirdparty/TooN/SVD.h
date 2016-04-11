// -*- c++ -*-

// Copyright (C) 2005,2009 Tom Drummond (twd20@cam.ac.uk)
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

#ifndef __SVD_H
#define __SVD_H

#include <TooN/TooN.h>
#include <TooN/lapack.h>

namespace TooN {

	// TODO - should this depend on precision?
static const double condition_no=1e9; // GK HACK TO GLOBAL







/**
@class SVD TooN/SVD.h
Performs %SVD and back substitute to solve equations.
Singular value decompositions are more robust than LU decompositions in the face of 
singular or nearly singular matrices. They decompose a matrix (of any shape) \f$M\f$ into:
\f[M = U \times D \times V^T\f]
where \f$D\f$ is a diagonal matrix of positive numbers whose dimension is the minimum 
of the dimensions of \f$M\f$. If \f$M\f$ is tall and thin (more rows than columns) 
then \f$U\f$ has the same shape as \f$M\f$ and \f$V\f$ is square (vice-versa if \f$M\f$ 
is short and fat). The columns of \f$U\f$ and the rows of \f$V\f$ are orthogonal 
and of unit norm (so one of them lies in SO(N)). The inverse of \f$M\f$ (or pseudo-inverse 
if \f$M\f$ is not square) is then given by
\f[M^{\dagger} = V \times D^{-1} \times U^T\f]
 
If \f$M\f$ is nearly singular then the diagonal matrix \f$D\f$ has some small values 
(relative to its largest value) and these terms dominate \f$D^{-1}\f$. To deal with 
this problem, the inverse is conditioned by setting a maximum ratio 
between the largest and smallest values in \f$D\f$ (passed as the <code>condition</code>
parameter to the various functions). Any values which are too small 
are set to zero in the inverse (rather than a large number)
 
It can be used as follows to solve the \f$M\underline{x} = \underline{c}\f$ problem as follows:
@code
// construct M
Matrix<3> M;
M[0] = makeVector(1,2,3);
M[1] = makeVector(4,5,6);
M[2] = makeVector(7,8.10);
// construct c
 Vector<3> c;
c = 2,3,4;
// create the SVD decomposition of M
SVD<3> svdM(M);
// compute x = M^-1 * c
Vector<3> x = svdM.backsub(c);
 @endcode

SVD<> (= SVD<-1>) can be used to create an SVD whose size is determined at run-time.
@ingroup gDecomps
**/
template<int Rows=Dynamic, int Cols=Rows, typename Precision=DefaultPrecision>
class SVD {
	// this is the size of the diagonal
	// NB works for semi-dynamic sizes because -1 < +ve ints
	static const int Min_Dim = Rows<Cols?Rows:Cols;
	
public:

	/// default constructor for Rows>0 and Cols>0
	SVD() {}

	/// constructor for Rows=-1 or Cols=-1 (or both)
	SVD(int rows, int cols)
		: my_copy(rows,cols),
		  my_diagonal(std::min(rows,cols)),
		  my_square(std::min(rows,cols), std::min(rows,cols))
	{}

	/// Construct the %SVD decomposition of a matrix. This initialises the class, and
	/// performs the decomposition immediately.
	template <int R2, int C2, typename P2, typename B2>
	SVD(const Matrix<R2,C2,P2,B2>& m)
		: my_copy(m),
		  my_diagonal(std::min(m.num_rows(),m.num_cols())),
		  my_square(std::min(m.num_rows(),m.num_cols()),std::min(m.num_rows(),m.num_cols()))
	{
		do_compute();
	}

	/// Compute the %SVD decomposition of M, typically used after the default constructor
	template <int R2, int C2, typename P2, typename B2>
	void compute(const Matrix<R2,C2,P2,B2>& m){
		my_copy=m;
		do_compute();
	}
	
	private:
	void do_compute(){
		Precision* const a = my_copy.my_data;
		int lda = my_copy.num_cols();
		int m = my_copy.num_cols();
		int n = my_copy.num_rows();
		Precision* const uorvt = my_square.my_data;
		Precision* const s = my_diagonal.my_data;
		int ldu;
		int ldvt = lda;
		int LWORK;
		int INFO;
		char JOBU;
		char JOBVT;

		if(is_vertical()){ // u is a
			JOBU='O';
			JOBVT='S';
			ldu = lda;
		} else { // vt is a
			JOBU='S';
			JOBVT='O';
			ldu = my_square.num_cols();
		}

		Precision* wk;

		Precision size;
		LWORK = -1;

		// arguments are scrambled because we use rowmajor and lapack uses colmajor
		// thus u and vt play each other's roles.
		gesvd_( &JOBVT, &JOBU, &m, &n, a, &lda, s, uorvt,
				 &ldvt, uorvt, &ldu, &size, &LWORK, &INFO);
	
		LWORK = (long int)(size);
		wk = new Precision[LWORK];

		gesvd_( &JOBVT, &JOBU, &m, &n, a, &lda, s, uorvt,
				 &ldvt, uorvt, &ldu, wk, &LWORK, &INFO);
	
		delete[] wk;
	}
	
	bool is_vertical(){ 
		return (my_copy.num_rows() >= my_copy.num_cols()); 
	}

	int min_dim(){ return std::min(my_copy.num_rows(), my_copy.num_cols()); }
	
	public:

	/// Calculate result of multiplying the (pseudo-)inverse of M by another matrix. 
	/// For a matrix \f$A\f$, this calculates \f$M^{\dagger}A\f$ by back substitution 
	/// (i.e. without explictly calculating the (pseudo-)inverse). 
	/// See the detailed description for a description of condition variables.
	template <int Rows2, int Cols2, typename P2, typename B2>
	Matrix<Cols,Cols2, typename Internal::MultiplyType<Precision,P2>::type >
	backsub(const Matrix<Rows2,Cols2,P2,B2>& rhs, const Precision condition=condition_no)
	{
		Vector<Min_Dim> inv_diag(min_dim());
		get_inv_diag(inv_diag,condition);
		return (get_VT().T() * diagmult(inv_diag, (get_U().T() * rhs)));
	}

	/// Calculate result of multiplying the (pseudo-)inverse of M by a vector. 
	/// For a vector \f$b\f$, this calculates \f$M^{\dagger}b\f$ by back substitution 
	/// (i.e. without explictly calculating the (pseudo-)inverse). 
	/// See the detailed description for a description of condition variables.
	template <int Size, typename P2, typename B2>
	Vector<Cols, typename Internal::MultiplyType<Precision,P2>::type >
	backsub(const Vector<Size,P2,B2>& rhs, const Precision condition=condition_no)
	{
		Vector<Min_Dim> inv_diag(min_dim());
		get_inv_diag(inv_diag,condition);
		return (get_VT().T() * diagmult(inv_diag, (get_U().T() * rhs)));
	}

	/// Calculate (pseudo-)inverse of the matrix. This is not usually needed: 
	/// if you need the inverse just to multiply it by a matrix or a vector, use 
	/// one of the backsub() functions, which will be faster.
	/// See the detailed description of the pseudo-inverse and condition variables.
	Matrix<Cols,Rows> get_pinv(const Precision condition = condition_no){
		Vector<Min_Dim> inv_diag(min_dim());
		get_inv_diag(inv_diag,condition);
		return diagmult(get_VT().T(),inv_diag) * get_U().T();
	}

	/// Calculate the product of the singular values
	/// for square matrices this is the determinant
	Precision determinant() {
		Precision result = my_diagonal[0];
		for(int i=1; i<my_diagonal.size(); i++){
			result *= my_diagonal[i];
		}
		return result;
	}
	
	/// Calculate the rank of the matrix.
	/// See the detailed description of the pseudo-inverse and condition variables.
	int rank(const Precision condition = condition_no) {
		if (my_diagonal[0] == 0) return 0;
		int result=1;
		for(int i=0; i<min_dim(); i++){
			if(my_diagonal[i] * condition <= my_diagonal[0]){
				result++;
			}
		}
		return result;
	}

	/// Return the U matrix from the decomposition
	/// The size of this depends on the shape of the original matrix
	/// it is square if the original matrix is wide or tall if the original matrix is tall
	Matrix<Rows,Min_Dim,Precision,Reference::RowMajor> get_U(){
		if(is_vertical()){
			return Matrix<Rows,Min_Dim,Precision,Reference::RowMajor>
				(my_copy.my_data,my_copy.num_rows(),my_copy.num_cols());
		} else {
			return Matrix<Rows,Min_Dim,Precision,Reference::RowMajor>
				(my_square.my_data, my_square.num_rows(), my_square.num_cols());
		}
	}

	/// Return the singular values as a vector
	Vector<Min_Dim,Precision>& get_diagonal(){ return my_diagonal; }

	/// Return the VT matrix from the decomposition
	/// The size of this depends on the shape of the original matrix
	/// it is square if the original matrix is tall or wide if the original matrix is wide
	Matrix<Min_Dim,Cols,Precision,Reference::RowMajor> get_VT(){
		if(is_vertical()){
			return Matrix<Min_Dim,Cols,Precision,Reference::RowMajor>
				(my_square.my_data, my_square.num_rows(), my_square.num_cols());
		} else {
			return Matrix<Min_Dim,Cols,Precision,Reference::RowMajor>
				(my_copy.my_data,my_copy.num_rows(),my_copy.num_cols());
		}
	}

	///Return the pesudo-inverse diagonal. The reciprocal of the diagonal elements
	///is returned if the elements are well scaled with respect to the largest element,
	///otherwise 0 is returned.
	///@param inv_diag Vector in which to return the inverse diagonal.
	///@param condition Elements must be larger than this factor times the largest diagonal element to be considered well scaled. 
	void get_inv_diag(Vector<Min_Dim>& inv_diag, const Precision condition){
		for(int i=0; i<min_dim(); i++){
			if(my_diagonal[i] * condition <= my_diagonal[0]){
				inv_diag[i]=0;
			} else {
				inv_diag[i]=static_cast<Precision>(1)/my_diagonal[i];
			}
		}
	}

private:
	Matrix<Rows,Cols,Precision,RowMajor> my_copy;
	Vector<Min_Dim,Precision> my_diagonal;
	Matrix<Min_Dim,Min_Dim,Precision,RowMajor> my_square; // square matrix (U or V' depending on the shape of my_copy)
};






/// version of SVD forced to be square
/// princiapally here to allow use in WLS
/// @ingroup gDecomps
template<int Size, typename Precision>
struct SQSVD : public SVD<Size, Size, Precision> {
	///All constructors are forwarded to SVD in a straightforward manner.
	///@name Constructors
	///@{
	SQSVD() {}
	SQSVD(int size) : SVD<Size,Size,Precision>(size, size) {}
	
	template <int R2, int C2, typename P2, typename B2>
	SQSVD(const Matrix<R2,C2,P2,B2>& m) : SVD<Size,Size,Precision>(m) {}
	///@}
};


}


#endif
