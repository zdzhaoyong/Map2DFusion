// -*- c++ -*-

// Copyright (C) 2005,2009 Tom Drummond (twd20@cam.ac.uk)
//
// This file is part of the TooN Library.	This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.	If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.	Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.	This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

#ifndef __SYMEIGEN_H
#define __SYMEIGEN_H

#include <iostream>
#include <cassert>
#include <cmath>
#include <utility>
#include <complex>
#include <TooN/lapack.h>

#include <TooN/TooN.h>

namespace TooN {
static const double root3=1.73205080756887729352744634150587236694280525381038062805580;

namespace Internal{

        using std::swap;

	///Default condition number for SymEigen::backsub, SymEigen::get_pinv and SymEigen::get_inv_diag
	static const double symeigen_condition_no=1e9;

	///@internal
	///@brief Compute eigensystems for sizes > 2
	///Helper struct for computing eigensystems, to allow for specialization on
	///2x2 matrices.
	///@ingroup gInternal
	template <int Size> struct ComputeSymEigen {

		///@internal
		///Compute an eigensystem.
		///@param m Input matrix (assumed to be symmetric)
		///@param evectors Eigen vector output
		///@param evalues Eigen values output
		template<int Rows, int Cols, typename P, typename B>
		static inline void compute(const Matrix<Rows,Cols,P, B>& m, Matrix<Size,Size,P> & evectors, Vector<Size, P>& evalues) {

			SizeMismatch<Rows, Cols>::test(m.num_rows(), m.num_cols());	 //m must be square
			SizeMismatch<Size, Rows>::test(m.num_rows(), evalues.size()); //m must be the size of the system
			

			evectors = m;
			FortranInteger N = evalues.size();
			FortranInteger lda = evalues.size();
			FortranInteger info;
			FortranInteger lwork=-1;
			P size;

			// find out how much space fortran needs
			syev_((char*)"V",(char*)"U",&N,&evectors[0][0],&lda,&evalues[0], &size,&lwork,&info);
			lwork = int(size);
			Vector<Dynamic, P> WORK(lwork);

			// now compute the decomposition
			syev_((char*)"V",(char*)"U",&N,&evectors[0][0],&lda,&evalues[0], &WORK[0],&lwork,&info);

			if(info!=0){
				std::cerr << "In SymEigen<"<<Size<<">: " << info
						<< " off-diagonal elements of an intermediate tridiagonal form did not converge to zero." << std::endl
						<< "M = " << m << std::endl;
			}
		}
	};

	///@internal
	///@brief Compute 2x2 eigensystems
	///Helper struct for computing eigensystems, specialized on 2x2 matrices.
	///@ingroup gInternal
	template <> struct ComputeSymEigen<2> {

		///@internal
		///Compute an eigensystem.
		///@param m Input matrix (assumed to be symmetric)
		///@param eig Eigen vector output
		///@param ev Eigen values output
		template<typename P, typename B>
		static inline void compute(const Matrix<2,2,P,B>& m, Matrix<2,2,P>& eig, Vector<2, P>& ev) {
			double trace = m[0][0] + m[1][1];
			//Only use the upper triangular elements.
			double det = m[0][0]*m[1][1] - m[0][1]*m[0][1]; 
			double disc = trace*trace - 4 * det;
			
			//Mathematically, disc >= 0 always.
			//Numerically, it can drift very slightly below zero.
			if(disc < 0)
				disc = 0;

			using std::sqrt;
			double root_disc = sqrt(disc);
			ev[0] = 0.5 * (trace - root_disc);
			ev[1] = 0.5 * (trace + root_disc);
			double a = m[0][0] - ev[0];
			double b = m[0][1];
			double magsq = a*a + b*b;
			if (magsq == 0) {
				eig[0][0] = 1.0;
				eig[0][1] = 0;
			} else {
				eig[0][0] = -b;
				eig[0][1] = a;
				eig[0] *= 1.0/sqrt(magsq);
			}
			eig[1][0] = -eig[0][1];
			eig[1][1] = eig[0][0];
		}
	};

    ///@internal
	///@brief Compute 3x3 eigensystems
	///Helper struct for computing eigensystems, specialized on 3x3 matrices.
	///@ingroup gInternal
	template <> struct ComputeSymEigen<3> {

		///@internal
		///Compute an eigensystem.
		///@param m Input matrix (assumed to be symmetric)
		///@param eig Eigen vector output
		///@param ev Eigen values output
		template<typename P, typename B>
		static inline void compute(const Matrix<3,3,P,B>& m, Matrix<3,3,P>& eig, Vector<3, P>& ev) {
            //method uses closed form solution of cubic equation to obtain roots of characteristic equation.
            using std::sqrt;
            using std::min;
            
			double a_norm = norm_1(m);
			double eps = 1e-7 * a_norm;

			if(a_norm == 0)
			{
				eig = TooN::Identity;
				return;
			}

            //Polynomial terms of |a - l * Identity|
            //l^3 + a*l^2 + b*l + c

            const double& a11 = m[0][0];
            const double& a12 = m[0][1];
            const double& a13 = m[0][2];

            const double& a22 = m[1][1];
            const double& a23 = m[1][2];

            const double& a33 = m[2][2];

            //From matlab:
            double a = -a11-a22-a33;
            double b = a11*a22+a11*a33+a22*a33-a12*a12-a13*a13-a23*a23;
            double c = a11*(a23*a23)+(a13*a13)*a22+(a12*a12)*a33-a12*a13*a23*2.0-a11*a22*a33;

            //Using Cardano's method:
            double p = b - a*a/3;
            double q = c + (2*a*a*a - 9*a*b)/27;

            double alpha = -q/2;

            //beta_descriminant <= 0 for real roots!
			//force to zero to avoid nasty rounding issues.
            double beta_descriminant = std::min(0.0, q*q/4 + p*p*p/27);

            double beta = sqrt(-beta_descriminant);
            double r2 = alpha*alpha  - beta_descriminant;

            ///Need A,B = cubert(alpha +- beta)
            ///Turn in to r, theta
            /// r^(1/3) * e^(i * theta/3)
            double cuberoot_r = pow(r2, 1./6);

            double theta3 = atan2(beta, alpha)/3;

            double A_plus_B = 2*cuberoot_r*cos(theta3);
            double A_minus_B = -2*cuberoot_r*sin(theta3);

            //calculate eigenvalues
            ev =  makeVector(A_plus_B, -A_plus_B/2 + A_minus_B * sqrt(3)/2, -A_plus_B/2 - A_minus_B * sqrt(3)/2) - Ones * a/3;

            if(ev[0] > ev[1])
                swap(ev[0], ev[1]);
            if(ev[1] > ev[2])
                swap(ev[1], ev[2]);
            if(ev[0] > ev[1])
                swap(ev[0], ev[1]);

			// for the vector [x y z]
			// eliminate to compute the ratios x/z and y/z
			// in both cases, the denominator is the same, so in the absence of
			// any other scaling, choose the denominator to be z and 
			// tne numerators to be x and y.
			//
			// x/z and y/z,  implies ax, ay, az which vanishes
			// if a=0
            //calculate the eigenvectors
            eig[0][0]=a12 * a23 - a13 * (a22 - ev[0]);
            eig[0][1]=a12 * a13 - a23 * (a11 - ev[0]);
            eig[0][2]=(a11-ev[0])*(a22-ev[0]) - a12*a12;
            eig[1][0]=a12 * a23 - a13 * (a22 - ev[1]);
            eig[1][1]=a12 * a13 - a23 * (a11 - ev[1]);
            eig[1][2]=(a11-ev[1])*(a22-ev[1]) - a12*a12;
            eig[2][0]=a12 * a23 - a13 * (a22 - ev[2]);
            eig[2][1]=a12 * a13 - a23 * (a11 - ev[2]);
            eig[2][2]=(a11-ev[2])*(a22-ev[2]) - a12*a12;
			
			//Check to see if we have any zero vectors
			double e0norm = norm_1(eig[0]);
			double e1norm = norm_1(eig[1]);
			double e2norm = norm_1(eig[2]);

			//Some of the vectors vanish: we're computing
			// x/z and y/z, which implies ax, ay, az which vanishes
			// if a=0;
			//
			// So compute the other two choices.
			if(e0norm < eps)
			{
				eig[0][0] += a12 * (ev[0] - a33) + a23 * a13;
				eig[0][1] += (ev[0]-a11)*(ev[0]-a33) - a13*a13;
				eig[0][2] += a23 * (ev[0] - a11) + a12 * a13;
				e0norm = norm_1(eig[0]);
			}

			if(e1norm < eps)
			{
				eig[1][0] += a12 * (ev[1] - a33) + a23 * a13;
				eig[1][1] += (ev[1]-a11)*(ev[1]-a33) - a13*a13;
				eig[1][2] += a23 * (ev[1] - a11) + a12 * a13;
				e1norm = norm_1(eig[1]);
			}

			if(e2norm < eps)
			{
				eig[2][0] += a12 * (ev[2] - a33) + a23 * a13;
				eig[2][1] += (ev[2]-a11)*(ev[2]-a33) - a13*a13;
				eig[2][2] += a23 * (ev[2] - a11) + a12 * a13;
				e2norm = norm_1(eig[2]);
			}


			//OK, a AND b might be 0
			//Check for vanishing and add in y/x, z/x which implies cx, cy, cz
			if(e0norm < eps)
			{
				eig[0][0] +=(ev[0]-a22)*(ev[0]-a33) - a23*a23;
				eig[0][1] +=a12 * (ev[0] - a33) + a23 * a13;
				eig[0][2] +=a13 * (ev[0] - a22) + a23 * a12;
				e0norm = norm_1(eig[0]);
			}

			if(e1norm < eps)
			{
				eig[1][0] +=(ev[1]-a22)*(ev[1]-a33) - a23*a23;
				eig[1][1] +=a12 * (ev[1] - a33) + a23 * a13;
				eig[1][2] +=a13 * (ev[1] - a22) + a23 * a12;
				e1norm = norm_1(eig[1]);
			}

			if(e2norm < eps)
			{
				eig[2][0] +=(ev[2]-a22)*(ev[2]-a33) - a23*a23;
				eig[2][1] +=a12 * (ev[2] - a33) + a23 * a13;
				eig[2][2] +=a13 * (ev[2] - a22) + a23 * a12;
				e2norm = norm_1(eig[2]);
			}

			//Oh, COME ON!!!
			if(e0norm < eps || e1norm < eps || e2norm < eps)
			{
				//This is blessedly rare
		
				double ns[] = {e0norm, e1norm, e2norm};
				double is[] = {0, 1, 2};
				
				//Sort them
				if(ns[0] > ns[1])
				{
					swap(ns[0], ns[1]);
					swap(is[0], is[1]);
				}
				if(ns[1] > ns[2])
				{
					swap(ns[1], ns[2]);
					swap(is[1], is[2]);
				}
				if(ns[0] > ns[1])
				{
					swap(ns[0], ns[1]);
					swap(is[0], is[1]);
				}


				if(ns[1] >= eps)
				{
					//one small one. Use the cross product of the other two
					normalize(eig[1]);
					normalize(eig[2]);
					eig[is[0]] = eig[is[1]]^eig[is[2]];
				}
				else if(ns[2] >= eps)
				{
					normalize(eig[is[2]]);
					
					//Permute vector to get a new vector with some orthogonal components.
					Vector<3> p = makeVector(eig[is[2]][1], eig[is[2]][2], eig[is[2]][0]);

					//Gram-Schmit
					Vector<3> v1 = unit(p - eig[is[2]] * (p * eig[is[2]]));
					Vector<3> v2 = v1^eig[is[2]];

					eig[is[0]] = v1;
					eig[is[1]] = v2;
				}
				else
					eig = TooN::Identity;
			}
			else
			{
  				normalize(eig[0]);
  				normalize(eig[1]);
  				normalize(eig[2]);
			}
		}
	};

};

/**
Performs eigen decomposition of a matrix.
Real symmetric (and hence square matrices) can be decomposed into
\f[M = U \times \Lambda \times U^T\f]
where \f$U\f$ is an orthogonal matrix (and hence \f$U^T = U^{-1}\f$) whose columns
are the eigenvectors of \f$M\f$ and \f$\Lambda\f$ is a diagonal matrix whose entries
are the eigenvalues of \f$M\f$. These quantities are often of use directly, and can
be obtained as follows:
@code
// construct M
Matrix<3> M(3,3);
M[0]=makeVector(4,0,2);
M[1]=makeVector(0,5,3);
M[2]=makeVector(2,3,6);

// create the eigen decomposition of M
SymEigen<3> eigM(M);
cout << "A=" << M << endl;
cout << "(E,v)=eig(A)" << endl;
// print the smallest eigenvalue
cout << "v[0]=" << eigM.get_evalues()[0] << endl;
// print the associated eigenvector
cout << "E[0]=" << eigM.get_evectors()[0] << endl;
@endcode

Further, provided the eigenvalues are nonnegative, the square root of
a matrix and its inverse can also be obtained,
@code
// print the square root of the matrix.
cout << "R=sqrtm(A)=" << eigM.get_sqrtm() << endl;
// print the square root of the matrix squared.
cout << "(should equal A), R^T*R="
     << eigM.get_sqrtm().T() * eigM.get_sqrtm() << endl;
// print the inverse of the matrix.
cout << "A^-1=" << eigM.get_pinv() << endl;
// print the inverse square root of the matrix.
cout << "C=isqrtm(A)=" << eigM.get_isqrtm() << endl;
// print the inverse square root of the matrix squared.
cout << "(should equal A^-1), C^T*C="
     << eigM.get_isqrtm().T() * eigM.get_isqrtm() << endl;
@endcode

This decomposition is very similar to the SVD (q.v.), and can be used to solve
equations using backsub() or get_pinv(), with the same treatment of condition numbers.

SymEigen<> (= SymEigen<-1>) can be used to create an eigen decomposition whose size is determined at run-time.
@ingroup gDecomps
**/
template <int Size=Dynamic, typename Precision = double>
class SymEigen {
public:
	inline SymEigen(){}

        /// Initialise this eigen decomposition but do no immediately
        /// perform a decomposition.
        ///
        /// @param m The size of the matrix to perform the eigen decomposition on.
        inline SymEigen(int m) : my_evectors(m,m), my_evalues(m) {}

	/// Construct the eigen decomposition of a matrix. This initialises the class, and
	/// performs the decomposition immediately.
	template<int R, int C, typename B>
	inline SymEigen(const Matrix<R, C, Precision, B>& m) : my_evectors(m.num_rows(), m.num_cols()), my_evalues(m.num_rows()) {
		compute(m);
	}

	/// Perform the eigen decomposition of a matrix.
	template<int R, int C, typename B>
	inline void compute(const Matrix<R,C,Precision,B>& m){
		SizeMismatch<R, C>::test(m.num_rows(), m.num_cols());
		SizeMismatch<R, Size>::test(m.num_rows(), my_evectors.num_rows());
		Internal::ComputeSymEigen<Size>::compute(m, my_evectors, my_evalues);
	}

	/// Calculate result of multiplying the (pseudo-)inverse of M by a vector.
	/// For a vector \f$b\f$, this calculates \f$M^{\dagger}b\f$ by back substitution
	/// (i.e. without explictly calculating the (pseudo-)inverse).
	/// See the SVD detailed description for a description of condition variables.
	template <int S, typename P, typename B>
	Vector<Size, Precision> backsub(const Vector<S,P,B>& rhs) const {
		return (my_evectors.T() * diagmult(get_inv_diag(Internal::symeigen_condition_no),(my_evectors * rhs)));
	}

	/// Calculate result of multiplying the (pseudo-)inverse of M by another matrix.
	/// For a matrix \f$A\f$, this calculates \f$M^{\dagger}A\f$ by back substitution
	/// (i.e. without explictly calculating the (pseudo-)inverse).
	/// See the SVD detailed description for a description of condition variables.
	template <int R, int C, typename P, typename B>
	Matrix<Size,C, Precision> backsub(const Matrix<R,C,P,B>& rhs) const {
		return (my_evectors.T() * diagmult(get_inv_diag(Internal::symeigen_condition_no),(my_evectors * rhs)));
	}

	/// Calculate (pseudo-)inverse of the matrix. This is not usually needed:
	/// if you need the inverse just to multiply it by a matrix or a vector, use
	/// one of the backsub() functions, which will be faster.
	/// See the SVD detailed description for a description of the pseudo-inverse
	/// and condition variables.
	Matrix<Size, Size, Precision> get_pinv(const double condition=Internal::symeigen_condition_no) const {
		return my_evectors.T() * diagmult(get_inv_diag(condition),my_evectors);
	}

	/// Calculates the reciprocals of the eigenvalues of the matrix.
	/// The vector <code>invdiag</code> lists the eigenvalues in order, from
	/// the largest (i.e. smallest reciprocal) to the smallest.
	/// These are also the diagonal values of the matrix \f$Lambda^{-1}\f$.
	/// Any eigenvalues which are too small are set to zero (see the SVD
	/// detailed description for a description of the and condition variables).
	Vector<Size, Precision> get_inv_diag(const double condition) const {
		Precision max_diag = -my_evalues[0] > my_evalues[my_evalues.size()-1] ? -my_evalues[0]:my_evalues[my_evalues.size()-1];
		Vector<Size, Precision> invdiag(my_evalues.size());
		for(int i=0; i<my_evalues.size(); i++){
			if(fabs(my_evalues[i]) * condition > max_diag) {
				invdiag[i] = 1/my_evalues[i];
			} else {
				invdiag[i]=0;
			}
		}
		return invdiag;
	}

	/// Returns the eigenvectors of the matrix.
	/// This returns \f$U^T\f$, so that the rows of the matrix are the eigenvectors,
	/// which can be extracted using usual Matrix::operator[]() subscript operator.
	/// They are returned in order of the size of the corresponding eigenvalue, i.e.
	/// the vector with the largest eigenvalue is first.
	Matrix<Size,Size,Precision>& get_evectors() {return my_evectors;}

	/**\overload
	*/
	const Matrix<Size,Size,Precision>& get_evectors() const {return my_evectors;}


	/// Returns the eigenvalues of the matrix.
	/// The eigenvalues are listed in order, from the smallest to the largest.
	/// These are also the diagonal values of the matrix \f$\Lambda\f$.
	Vector<Size, Precision>& get_evalues() {return my_evalues;}
	/**\overload
	*/
	const Vector<Size, Precision>& get_evalues() const {return my_evalues;}

	/// Is the matrix positive definite?
	bool is_posdef() const {
		for (int i = 0; i < my_evalues.size(); ++i) {
			if (my_evalues[i] <= 0.0)
				return false;
		}
		return true;
	}

	/// Is the matrix negative definite?
	bool is_negdef() const {
		for (int i = 0; i < my_evalues.size(); ++i) {
			if (my_evalues[i] >= 0.0)
				return false;
		}
		return true;
	}

	/// Get the determinant of the matrix
	Precision get_determinant () const {
		Precision det = 1.0;
		for (int i = 0; i < my_evalues.size(); ++i) {
			det *= my_evalues[i];
		}
		return det;
	}

	/// Calculate the square root of a matrix which is a matrix M
	/// such that M.T*M=A.
	Matrix<Size, Size, Precision> get_sqrtm () const {
		Vector<Size, Precision> diag_sqrt(my_evalues.size());
		// In the future, maybe throw an exception if an
		// eigenvalue is negative?
		for (int i = 0; i < my_evalues.size(); ++i) {
			diag_sqrt[i] = std::sqrt(my_evalues[i]);
		}
		return my_evectors.T() * diagmult(diag_sqrt, my_evectors);
	}

	/// Calculate the inverse square root of a matrix which is a
	/// matrix M such that M.T*M=A^-1.
        ///
        /// Any square-rooted eigenvalues which are too small are set
        /// to zero (see the SVD detailed description for a
        /// description of the condition variables).
	Matrix<Size, Size, Precision> get_isqrtm (const double condition=Internal::symeigen_condition_no) const {
		Vector<Size, Precision> diag_isqrt(my_evalues.size());

		// Because sqrt is a monotonic-preserving transformation,
		Precision max_diag = -my_evalues[0] > my_evalues[my_evalues.size()-1] ? (-std::sqrt(my_evalues[0])):(std::sqrt(my_evalues[my_evalues.size()-1]));
		// In the future, maybe throw an exception if an
		// eigenvalue is negative?
		for (int i = 0; i < my_evalues.size(); ++i) {
			diag_isqrt[i] = std::sqrt(my_evalues[i]);
			if(fabs(diag_isqrt[i]) * condition > max_diag) {
				diag_isqrt[i] = 1/diag_isqrt[i];
			} else {
				diag_isqrt[i] = 0;
			}
		}
		return my_evectors.T() * diagmult(diag_isqrt, my_evectors);
	}

private:
	// eigen vectors laid out row-wise so evectors[i] is the ith evector
	Matrix<Size,Size,Precision> my_evectors;

	Vector<Size, Precision> my_evalues;
};

}

#endif

