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

#ifndef TOON_INCLUDE_SO3_H
#define TOON_INCLUDE_SO3_H

#include <TooN/TooN.h>
#include <TooN/helpers.h>
#include <cassert>

namespace TooN {

template <typename Precision> class SO3;
template <typename Precision> class SE3;
template <typename Precision> class SIM3;

template<class Precision> inline std::istream & operator>>(std::istream &, SO3<Precision> & );
template<class Precision> inline std::istream & operator>>(std::istream &, SE3<Precision> & );
template<class Precision> inline std::istream & operator>>(std::istream &, SIM3<Precision> & );

/// Class to represent a three-dimensional rotation matrix. Three-dimensional rotation
/// matrices are members of the Special Orthogonal Lie group SO3. This group can be parameterised
/// three numbers (a vector in the space of the Lie Algebra). In this class, the three parameters are the
/// finite rotation vector, i.e. a three-dimensional vector whose direction is the axis of rotation
/// and whose length is the angle of rotation in radians. Exponentiating this vector gives the matrix,
/// and the logarithm of the matrix gives this vector.
/// @ingroup gTransforms
template <typename Precision = DefaultPrecision>
class SO3 {
public:
	friend std::istream& operator>> <Precision> (std::istream& is, SO3<Precision> & rhs);
	friend std::istream& operator>> <Precision> (std::istream& is, SE3<Precision> & rhs);
	friend std::istream& operator>> <Precision> (std::istream& is, SIM3<Precision> & rhs);
	friend class SE3<Precision>;
	friend class SIM3<Precision>;

	/// Default constructor. Initialises the matrix to the identity (no rotation)
	SO3() : my_matrix(Identity) {}
	
	/// Construct from the axis of rotation (and angle given by the magnitude).
	template <int S, typename P, typename A>
	SO3(const Vector<S, P, A> & v) { *this = exp(v); }
	
	/// Construct from a rotation matrix.
	template <int R, int C, typename P, typename A>
	SO3(const Matrix<R,C,P,A>& rhs) { *this = rhs; }
	
	/// creates an SO3 as a rotation that takes Vector a into the direction of Vector b
	/// with the rotation axis along a ^ b. If |a ^ b| == 0, it creates the identity rotation.
	/// An assertion will fail if Vector a and Vector b are in exactly opposite directions. 
	/// @param a source Vector
	/// @param b target Vector
	template <int S1, int S2, typename P1, typename P2, typename A1, typename A2>
	SO3(const Vector<S1, P1, A1> & a, const Vector<S2, P2, A2> & b ){
		SizeMismatch<3,S1>::test(3, a.size());
		SizeMismatch<3,S2>::test(3, b.size());
		Vector<3, Precision> n = a ^ b;
		if(norm_sq(n) == 0) {
			//check that the vectors are in the same direction if cross product is 0. If not,
			//this means that the rotation is 180 degrees, which leads to an ambiguity in the rotation axis.
			assert(a*b>=0);
			my_matrix = Identity;
			return;
		}
		n = unit(n);
		Matrix<3> R1;
		R1.T()[0] = unit(a);
		R1.T()[1] = n;
		R1.T()[2] = n ^ R1.T()[0];
		my_matrix.T()[0] = unit(b);
		my_matrix.T()[1] = n;
		my_matrix.T()[2] = n ^ my_matrix.T()[0];
		my_matrix = my_matrix * R1.T();
	}
	
	/// Assignment operator from a general matrix. This also calls coerce()
	/// to make sure that the matrix is a valid rotation matrix.
	template <int R, int C, typename P, typename A>
	SO3& operator=(const Matrix<R,C,P,A> & rhs) {
		my_matrix = rhs;
		coerce();
		return *this;
	}
	
	/// Modifies the matrix to make sure it is a valid rotation matrix.
	void coerce() {
		my_matrix[0] = unit(my_matrix[0]);
		my_matrix[1] -= my_matrix[0] * (my_matrix[0]*my_matrix[1]);
		my_matrix[1] = unit(my_matrix[1]);
		my_matrix[2] -= my_matrix[0] * (my_matrix[0]*my_matrix[2]);
		my_matrix[2] -= my_matrix[1] * (my_matrix[1]*my_matrix[2]);
		my_matrix[2] = unit(my_matrix[2]);
		// check for positive determinant <=> right handed coordinate system of row vectors
		assert( (my_matrix[0] ^ my_matrix[1]) * my_matrix[2] > 0 ); 
	}
	
	/// Exponentiate a vector in the Lie algebra to generate a new SO3.
	/// See the Detailed Description for details of this vector.
	template<int S, typename VP, typename A> inline static SO3 exp(const Vector<S,VP,A>& vect);
	
	/// Take the logarithm of the matrix, generating the corresponding vector in the Lie Algebra.
	/// See the Detailed Description for details of this vector.
	inline Vector<3, Precision> ln() const;
	
	/// Returns the inverse of this matrix (=the transpose, so this is a fast operation)
	SO3 inverse() const { return SO3(*this, Invert()); }

	/// Right-multiply by another rotation matrix
	template <typename P>
	SO3& operator *=(const SO3<P>& rhs) {
		*this = *this * rhs;
		return *this;
	}

	/// Right-multiply by another rotation matrix
	template<typename P>
	SO3<typename Internal::MultiplyType<Precision, P>::type> operator *(const SO3<P>& rhs) const { 
	    return SO3<typename Internal::MultiplyType<Precision, P>::type>(*this,rhs); 
	}

	/// Returns the SO3 as a Matrix<3>
	const Matrix<3,3, Precision> & get_matrix() const {return my_matrix;}

	/// Returns the i-th generator.  The generators of a Lie group are the basis
	/// for the space of the Lie algebra.  For %SO3, the generators are three
	/// \f$3\times3\f$ matrices representing the three possible (linearised)
	/// rotations.
	inline static Matrix<3,3, Precision> generator(int i){
		Matrix<3,3,Precision> result(Zeros);
		result[(i+1)%3][(i+2)%3] = -1;
		result[(i+2)%3][(i+1)%3] = 1;
		return result;
	}

  /// Returns the i-th generator times pos
  template<typename Base>
  inline static Vector<3,Precision> generator_field(int i, const Vector<3, Precision, Base>& pos)
  {
    Vector<3, Precision> result;
    result[i]=0;
    result[(i+1)%3] = - pos[(i+2)%3];
    result[(i+2)%3] = pos[(i+1)%3];
    return result;
  }

	/// Transfer a vector in the Lie Algebra from one
	/// co-ordinate frame to another such that for a matrix 
	/// \f$ M \f$, the adjoint \f$Adj()\f$ obeys
	/// \f$ e^{\text{Adj}(v)} = Me^{v}M^{-1} \f$
	template <int S, typename A>
	inline Vector<3, Precision> adjoint(const Vector<S, Precision, A>& vect) const 
	{ 
		SizeMismatch<3, S>::test(3, vect.size());
		return *this * vect; 
	}

	template <typename PA, typename PB>
	inline SO3(const SO3<PA>& a, const SO3<PB>& b) : my_matrix(a.get_matrix()*b.get_matrix()) {}
	
private:
	struct Invert {};
	inline SO3(const SO3& so3, const Invert&) : my_matrix(so3.my_matrix.T()) {}
	
	Matrix<3,3, Precision> my_matrix;
};

/// Write an SO3 to a stream 
/// @relates SO3
template <typename Precision>
inline std::ostream& operator<< (std::ostream& os, const SO3<Precision>& rhs){
	return os << rhs.get_matrix();
}

/// Read from SO3 to a stream 
/// @relates SO3
template <typename Precision>
inline std::istream& operator>>(std::istream& is, SO3<Precision>& rhs){
	is >> rhs.my_matrix;
	rhs.coerce();
	return is;
}

///Compute a rotation exponential using the Rodrigues Formula.
///The rotation axis is given by \f$\vec{w}\f$, and the rotation angle must
///be computed using \f$ \theta = |\vec{w}|\f$. This is provided as a separate
///function primarily to allow fast and rough matrix exponentials using fast 
///and rough approximations to \e A and \e B.
///
///@param w Vector about which to rotate.
///@param A \f$\frac{\sin \theta}{\theta}\f$
///@param B \f$\frac{1 - \cos \theta}{\theta^2}\f$
///@param R Matrix to hold the return value.
///@relates SO3
template <typename Precision, int S, typename VP, typename VA, typename MA>
inline void rodrigues_so3_exp(const Vector<S,VP, VA>& w, const Precision A, const Precision B, Matrix<3,3,Precision,MA>& R){
    SizeMismatch<3,S>::test(3, w.size());
	{
		const Precision wx2 = (Precision)w[0]*w[0];
		const Precision wy2 = (Precision)w[1]*w[1];
		const Precision wz2 = (Precision)w[2]*w[2];
	
		R[0][0] = 1.0 - B*(wy2 + wz2);
		R[1][1] = 1.0 - B*(wx2 + wz2);
		R[2][2] = 1.0 - B*(wx2 + wy2);
	}
	{
		const Precision a = A*w[2];
		const Precision b = B*(w[0]*w[1]);
		R[0][1] = b - a;
		R[1][0] = b + a;
	}
	{
		const Precision a = A*w[1];
		const Precision b = B*(w[0]*w[2]);
		R[0][2] = b + a;
		R[2][0] = b - a;
	}
	{
		const Precision a = A*w[0];
		const Precision b = B*(w[1]*w[2]);
		R[1][2] = b - a;
		R[2][1] = b + a;
	}
}

///Perform the exponential of the matrix \f$ \sum_i w_iG_i\f$
///@param w Weightings of the generator matrices.
template <typename Precision>
template<int S, typename VP, typename VA>
inline SO3<Precision> SO3<Precision>::exp(const Vector<S,VP,VA>& w){
	using std::sqrt;
	using std::sin;
	using std::cos;
	SizeMismatch<3,S>::test(3, w.size());
	
	static const Precision one_6th = 1.0/6.0;
	static const Precision one_20th = 1.0/20.0;
	
	SO3<Precision> result;
	
	const Precision theta_sq = w*w;
	const Precision theta = sqrt(theta_sq);
	Precision A, B;
	//Use a Taylor series expansion near zero. This is required for
	//accuracy, since sin t / t and (1-cos t)/t^2 are both 0/0.
	if (theta_sq < 1e-8) {
		A = 1.0 - one_6th * theta_sq;
		B = 0.5;
	} else {
		if (theta_sq < 1e-6) {
			B = 0.5 - 0.25 * one_6th * theta_sq;
			A = 1.0 - theta_sq * one_6th*(1.0 - one_20th * theta_sq);
		} else {
			const Precision inv_theta = 1.0/theta;
			A = sin(theta) * inv_theta;
			B = (1 - cos(theta)) * (inv_theta * inv_theta);
		}
	}
	rodrigues_so3_exp(w, A, B, result.my_matrix);
	return result;
}

template <typename Precision>
inline Vector<3, Precision> SO3<Precision>::ln() const{
    using std::sqrt;
	Vector<3, Precision> result;
	
	const Precision cos_angle = (my_matrix[0][0] + my_matrix[1][1] + my_matrix[2][2] - 1.0) * 0.5;
	result[0] = (my_matrix[2][1]-my_matrix[1][2])/2;
	result[1] = (my_matrix[0][2]-my_matrix[2][0])/2;
	result[2] = (my_matrix[1][0]-my_matrix[0][1])/2;
	
	Precision sin_angle_abs = sqrt(result*result);
	if (cos_angle > M_SQRT1_2) {            // [0 - Pi/4[ use asin
		if(sin_angle_abs > 0){
			result *= asin(sin_angle_abs) / sin_angle_abs;
		}
	} else if( cos_angle > -M_SQRT1_2) {    // [Pi/4 - 3Pi/4[ use acos, but antisymmetric part
		const Precision angle = acos(cos_angle);
		result *= angle / sin_angle_abs;        
	} else {  // rest use symmetric part
		// antisymmetric part vanishes, but still large rotation, need information from symmetric part
		const Precision angle = M_PI - asin(sin_angle_abs);
		const Precision d0 = my_matrix[0][0] - cos_angle,
			d1 = my_matrix[1][1] - cos_angle,
			d2 = my_matrix[2][2] - cos_angle;
		TooN::Vector<3, Precision> r2;
		if(d0*d0 > d1*d1 && d0*d0 > d2*d2){ // first is largest, fill with first column
			r2[0] = d0;
			r2[1] = (my_matrix[1][0]+my_matrix[0][1])/2;
			r2[2] = (my_matrix[0][2]+my_matrix[2][0])/2;
		} else if(d1*d1 > d2*d2) { 			    // second is largest, fill with second column
			r2[0] = (my_matrix[1][0]+my_matrix[0][1])/2;
			r2[1] = d1;
			r2[2] = (my_matrix[2][1]+my_matrix[1][2])/2;
		} else {							    // third is largest, fill with third column
			r2[0] = (my_matrix[0][2]+my_matrix[2][0])/2;
			r2[1] = (my_matrix[2][1]+my_matrix[1][2])/2;
			r2[2] = d2;
		}
		// flip, if we point in the wrong direction!
		if(r2 * result < 0)
			r2 *= -1;
		r2 = unit(r2);
		result = TooN::operator*(angle,r2);
	} 
	return result;
}

/// Right-multiply by a Vector
/// @relates SO3
template<int S, typename P, typename PV, typename A> inline
Vector<3, typename Internal::MultiplyType<P, PV>::type> operator*(const SO3<P>& lhs, const Vector<S, PV, A>& rhs){
	return lhs.get_matrix() * rhs;
}

/// Left-multiply by a Vector
/// @relates SO3
template<int S, typename P, typename PV, typename A> inline
Vector<3, typename Internal::MultiplyType<PV, P>::type> operator*(const Vector<S, PV, A>& lhs, const SO3<P>& rhs){
	return lhs * rhs.get_matrix();
}

/// Right-multiply by a matrix
/// @relates SO3
template<int R, int C, typename P, typename PM, typename A> inline
Matrix<3, C, typename Internal::MultiplyType<P, PM>::type> operator*(const SO3<P>& lhs, const Matrix<R, C, PM, A>& rhs){
	return lhs.get_matrix() * rhs;
}

/// Left-multiply by a matrix
/// @relates SO3
template<int R, int C, typename P, typename PM, typename A> inline
Matrix<R, 3, typename Internal::MultiplyType<PM, P>::type> operator*(const Matrix<R, C, PM, A>& lhs, const SO3<P>& rhs){
	return lhs * rhs.get_matrix();
}

#if 0	// will be moved to another header file ?

template <class A> inline
Vector<3> transform(const SO3& pose, const FixedVector<3,A>& x) { return pose*x; }

template <class A1, class A2> inline
Vector<3> transform(const SO3& pose, const FixedVector<3,A1>& x, FixedMatrix<3,3,A2>& J_x)
{
	J_x = pose.get_matrix();
	return pose * x;
}

template <class A1, class A2, class A3> inline
Vector<3> transform(const SO3& pose, const FixedVector<3,A1>& x, FixedMatrix<3,3,A2>& J_x, FixedMatrix<3,3,A3>& J_pose)
{
	J_x = pose.get_matrix();
	const Vector<3> cx = pose * x;
	J_pose[0][0] = J_pose[1][1] = J_pose[2][2] = 0;
	J_pose[1][0] = -(J_pose[0][1] = cx[2]);
	J_pose[0][2] = -(J_pose[2][0] = cx[1]);
	J_pose[2][1] = -(J_pose[1][2] = cx[0]);
	return cx;
}

template <class A1, class A2, class A3> inline
Vector<2> project_transformed_point(const SO3& pose, const FixedVector<3,A1>& in_frame, FixedMatrix<2,3,A2>& J_x, FixedMatrix<2,3,A3>& J_pose)
{
	const double z_inv = 1.0/in_frame[2];
	const double x_z_inv = in_frame[0]*z_inv;
	const double y_z_inv = in_frame[1]*z_inv;
	const double cross = x_z_inv * y_z_inv;
	J_pose[0][0] = -cross;
	J_pose[0][1] = 1 + x_z_inv*x_z_inv;
	J_pose[0][2] = -y_z_inv;
	J_pose[1][0] = -1 - y_z_inv*y_z_inv;
	J_pose[1][1] =  cross;
	J_pose[1][2] =  x_z_inv;

	const TooN::Matrix<3>& R = pose.get_matrix();
	J_x[0][0] = z_inv*(R[0][0] - x_z_inv * R[2][0]);
	J_x[0][1] = z_inv*(R[0][1] - x_z_inv * R[2][1]);
	J_x[0][2] = z_inv*(R[0][2] - x_z_inv * R[2][2]);
	J_x[1][0] = z_inv*(R[1][0] - y_z_inv * R[2][0]);
	J_x[1][1] = z_inv*(R[1][1] - y_z_inv * R[2][1]);
	J_x[1][2] = z_inv*(R[1][2] - y_z_inv * R[2][2]);

	return makeVector(x_z_inv, y_z_inv);
}


template <class A1> inline
Vector<2> transform_and_project(const SO3& pose, const FixedVector<3,A1>& x)
{
	return project(transform(pose,x));
}

template <class A1, class A2, class A3> inline
Vector<2> transform_and_project(const SO3& pose, const FixedVector<3,A1>& x, FixedMatrix<2,3,A2>& J_x, FixedMatrix<2,3,A3>& J_pose)
{
	return project_transformed_point(pose, transform(pose,x), J_x, J_pose);
}

#endif

}

#endif
