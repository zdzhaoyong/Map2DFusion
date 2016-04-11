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

#ifndef TOON_INCLUDE_WLS_H
#define TOON_INCLUDE_WLS_H

#include <TooN/TooN.h>
#include <TooN/Cholesky.h>
#include <TooN/helpers.h>

#include <cmath>

namespace TooN {

/// Performs Gauss-Newton weighted least squares computation.
/// @param Size The number of dimensions in the system
/// @param Precision The numerical precision used (double, float etc)
/// @param Decomposition The class used to invert the inverse Covariance matrix (must have one integer size and one typename precision template arguments) this is Cholesky by default, but could also be SQSVD
/// @ingroup gEquations
template <int Size=Dynamic, class Precision=DefaultPrecision,
		  template<int DecompSize, class DecompPrecision> class Decomposition = Cholesky>
class WLS {
public:

	/// Default constructor or construct with the number of dimensions for the Dynamic case
	WLS(int size=0) :
		my_C_inv(size,size),
		my_vector(size),
		my_decomposition(size),
		my_mu(size)
	{
		clear();
	}

	/// Clear all the measurements and apply a constant regularisation term. 
	void clear(){
		my_C_inv = Zeros;
		my_vector = Zeros;
	}

	/// Applies a constant regularisation term. 
	/// Equates to a prior that says all the parameters are zero with \f$\sigma^2 = \frac{1}{\text{val}}\f$.
	/// @param val The strength of the prior
	void add_prior(Precision val){
		for(int i=0; i<my_C_inv.num_rows(); i++){
			my_C_inv(i,i)+=val;
		}
	}
  
	/// Applies a regularisation term with a different strength for each parameter value. 
	/// Equates to a prior that says all the parameters are zero with \f$\sigma_i^2 = \frac{1}{\text{v}_i}\f$.
	/// @param v The vector of priors
	template<class B2>
	void add_prior(const Vector<Size,Precision,B2>& v){
		SizeMismatch<Size,Size>::test(my_C_inv.num_rows(), v.size());
		for(int i=0; i<my_C_inv.num_rows(); i++){
			my_C_inv(i,i)+=v[i];
		}
	}

	/// Applies a whole-matrix regularisation term. 
	/// This is the same as adding the \f$m\f$ to the inverse covariance matrix.
	/// @param m The inverse covariance matrix to add
	template<class B2>
	void add_prior(const Matrix<Size,Size,Precision,B2>& m){
		my_C_inv+=m;
	}

	/// Add a single measurement 
	/// @param m The value of the measurement
	/// @param J The Jacobian for the measurement \f$\frac{\partial\text{m}}{\partial\text{param}_i}\f$
	/// @param weight The inverse variance of the measurement (default = 1)
	template<class B2>
	inline void add_mJ(Precision m, const Vector<Size, Precision, B2>& J, Precision weight = 1) {
		
		//Upper right triangle only, for speed
		for(int r=0; r < my_C_inv.num_rows(); r++)
		{
			double Jw = weight * J[r];
			my_vector[r] += m * Jw;
			for(int c=r; c < my_C_inv.num_rows(); c++)
				my_C_inv[r][c] += Jw * J[c];
		}
	}

	/// Add multiple measurements at once (much more efficiently)
	/// @param m The measurements to add
	/// @param J The Jacobian matrix \f$\frac{\partial\text{m}_i}{\partial\text{param}_j}\f$
	/// @param invcov The inverse covariance of the measurement values
	template<int N, class B1, class B2, class B3>
	inline void add_mJ(const Vector<N,Precision,B1>& m,
					   const Matrix<Size,N,Precision,B2>& J,
					   const Matrix<N,N,Precision,B3>& invcov){
		const Matrix<Size,N,Precision> temp =  J * invcov;
		my_C_inv += temp * J.T();
		my_vector += temp * m;
	}

	/// Add multiple measurements at once (much more efficiently)
	/// @param m The measurements to add
	/// @param J The Jacobian matrix \f$\frac{\partial\text{m}_i}{\partial\text{param}_j}\f$
	/// @param invcov The inverse covariance of the measurement values
	template<int N, class B1, class B2, class B3>
	inline void add_mJ_rows(const Vector<N,Precision,B1>& m,
					   const Matrix<N,Size,Precision,B2>& J,
					   const Matrix<N,N,Precision,B3>& invcov){
		const Matrix<Size,N,Precision> temp =  J.T() * invcov;
		my_C_inv += temp * J;
		my_vector += temp * m;
	}

	/// Add a single measurement at once with a sparse Jacobian (much, much more efficiently)
	/// @param m The measurements to add
	/// @param J1 The first block of the Jacobian matrix \f$\frac{\partial\text{m}_i}{\partial\text{param}_j}\f$
	/// @param index1 starting index for the first block
	/// @param invcov The inverse covariance of the measurement values
	template<int N, typename B1>
	inline void add_sparse_mJ(const Precision m,
					   const Vector<N,Precision,B1>& J1, const int index1,
					   const Precision weight = 1){
		//Upper right triangle only, for speed
		for(int r=0; r < J1.size(); r++)
		{
			double Jw = weight * J1[r];
			my_vector[r+index1] += m * Jw;
			for(int c = r; c < J1.size(); c++)
				my_C_inv[r+index1][c+index1] += Jw * J1[c];
		}
	}

	/// Add multiple measurements at once with a sparse Jacobian (much, much more efficiently)
	/// @param m The measurements to add
	/// @param J1 The first block of the Jacobian matrix \f$\frac{\partial\text{m}_i}{\partial\text{param}_j}\f$
	/// @param index1 starting index for the first block
	/// @param invcov The inverse covariance of the measurement values
	template<int N, int S1, class P1, class P2, class P3, class B1, class B2, class B3>
	inline void add_sparse_mJ_rows(const Vector<N,P1,B1>& m,
					   const Matrix<N,S1,P2,B2>& J1, const int index1,
					   const Matrix<N,N,P3,B3>& invcov){
		const Matrix<S1,N,Precision> temp1 = J1.T() * invcov;
		const int size1 = J1.num_cols();
		my_C_inv.slice(index1, index1, size1, size1) += temp1 * J1;
		my_vector.slice(index1, size1) += temp1 * m;
	}

	/// Add multiple measurements at once with a sparse Jacobian (much, much more efficiently)
	/// @param m The measurements to add
	/// @param J1 The first block of the Jacobian matrix \f$\frac{\partial\text{m}_i}{\partial\text{param}_j}\f$
	/// @param index1 starting index for the first block
	/// @param J2 The second block of the Jacobian matrix \f$\frac{\partial\text{m}_i}{\partial\text{param}_j}\f$
	/// @param index2 starting index for the second block
	/// @param invcov The inverse covariance of the measurement values
	template<int N, int S1, int S2, class B1, class B2, class B3, class B4>
	inline void add_sparse_mJ_rows(const Vector<N,Precision,B1>& m,
					   const Matrix<N,S1,Precision,B2>& J1, const int index1,
					   const Matrix<N,S2,Precision,B3>& J2, const int index2,
					   const Matrix<N,N,Precision,B4>& invcov){
		const Matrix<S1,N,Precision> temp1 = J1.T() * invcov;
		const Matrix<S2,N,Precision> temp2 = J2.T() * invcov;
		const Matrix<S1,S2,Precision> mixed = temp1 * J2;
		const int size1 = J1.num_cols();
		const int size2 = J2.num_cols();
		my_C_inv.slice(index1, index1, size1, size1) += temp1 * J1;
		my_C_inv.slice(index2, index2, size2, size2) += temp2 * J2;
		my_C_inv.slice(index1, index2, size1, size2) += mixed;
		my_C_inv.slice(index2, index1, size2, size1) += mixed.T();
		my_vector.slice(index1, size1) += temp1 * m;
		my_vector.slice(index2, size2) += temp2 * m;
	}

	/// Process all the measurements and compute the weighted least squares set of parameter values
	/// stores the result internally which can then be accessed by calling get_mu()
	void compute(){
	
		//Copy the upper right triangle to the empty lower-left.
		for(int r=1; r < my_C_inv.num_rows(); r++)
			for(int c=0; c < r; c++)
				my_C_inv[r][c] = my_C_inv[c][r];

		my_decomposition.compute(my_C_inv);
		my_mu=my_decomposition.backsub(my_vector);
	}

	/// Combine measurements from two WLS systems
	/// @param meas The measurements to combine with
	void operator += (const WLS& meas){
		my_vector+=meas.my_vector;
		my_C_inv += meas.my_C_inv;
	}

	/// Returns the inverse covariance matrix
	Matrix<Size,Size,Precision>& get_C_inv() {return my_C_inv;}
	/// Returns the inverse covariance matrix
	const Matrix<Size,Size,Precision>& get_C_inv() const {return my_C_inv;}
	Vector<Size,Precision>& get_mu(){return my_mu;}  ///<Returns the update. With no prior, this is the result of \f$J^\dagger e\f$.
	const Vector<Size,Precision>& get_mu() const {return my_mu;} ///<Returns the update. With no prior, this is the result of \f$J^\dagger e\f$.
	Vector<Size,Precision>& get_vector(){return my_vector;} ///<Returns the  vector \f$J^{\mathsf T} e\f$
	const Vector<Size,Precision>& get_vector() const {return my_vector;} ///<Returns the  vector \f$J^{\mathsf T} e\f$
	Decomposition<Size,Precision>& get_decomposition(){return my_decomposition;} ///< Return the decomposition object used to compute \f$(J^{\mathsf T}  J + P)^{-1}\f$
	const Decomposition<Size,Precision>& get_decomposition() const {return my_decomposition;} ///< Return the decomposition object used to compute \f$(J^{\mathsf T}  J + P)^{-1}\f$


private:
	Matrix<Size,Size,Precision> my_C_inv;
	Vector<Size,Precision> my_vector;
	Decomposition<Size,Precision> my_decomposition;
	Vector<Size,Precision> my_mu;

	// comment out to allow bitwise copying
	// WLS( WLS& copyof );
	// int operator = ( WLS& copyof );
};

}

#endif
