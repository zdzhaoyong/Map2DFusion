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

#ifndef TOON_INCLUDE_SE3_H
#define TOON_INCLUDE_SE3_H

#include <TooN/so3.h>

namespace TooN {


/// Represent a three-dimensional Euclidean transformation (a rotation and a translation). 
/// This can be represented by a \f$3\times\f$4 matrix operating on a homogeneous co-ordinate, 
/// so that a vector \f$\underline{x}\f$ is transformed to a new location \f$\underline{x}'\f$
/// by
/// \f[\begin{aligned}\underline{x}' &= E\times\underline{x}\\ \begin{bmatrix}x'\\y'\\z'\end{bmatrix} &= \begin{pmatrix}r_{11} & r_{12} & r_{13} & t_1\\r_{21} & r_{22} & r_{23} & t_2\\r_{31} & r_{32} & r_{33} & t_3\end{pmatrix}\begin{bmatrix}x\\y\\z\\1\end{bmatrix}\end{aligned}\f]
/// 
/// This transformation is a member of the Special Euclidean Lie group SE3. These can be parameterised
/// six numbers (in the space of the Lie Algebra). In this class, the first three parameters are a
/// translation vector while the second three are a rotation vector, whose direction is the axis of rotation
/// and length the amount of rotation (in radians), as for SO3
/// @ingroup gTransforms
template <typename Precision = DefaultPrecision>
class SE3 {
public:
	/// Default constructor. Initialises the the rotation to zero (the identity) and the translation to zero
	inline SE3() : my_translation(Zeros) {}

	template <int S, typename P, typename A> 
	SE3(const SO3<Precision> & R, const Vector<S, P, A>& T) : my_rotation(R), my_translation(T) {}
	template <int S, typename P, typename A>
	SE3(const Vector<S, P, A> & v) { *this = exp(v); }

	template <class IP, int S, typename P, typename A> 
	SE3(const Operator<Internal::Identity<IP> >&, const Vector<S, P, A>& T) : my_translation(T) {}

	/// Returns the rotation part of the transformation as a SO3
	inline SO3<Precision>& get_rotation(){return my_rotation;}
	/// @overload
	inline const SO3<Precision>& get_rotation() const {return my_rotation;}

	/// Returns the translation part of the transformation as a Vector
	inline Vector<3, Precision>& get_translation() {return my_translation;}
	/// @overload
	inline const Vector<3, Precision>& get_translation() const {return my_translation;}

	/// Exponentiate a Vector in the Lie Algebra to generate a new SE3.
	/// See the Detailed Description for details of this vector.
	/// @param vect The Vector to exponentiate
	template <int S, typename P, typename A>
	static inline SE3 exp(const Vector<S, P, A>& vect);


	/// Take the logarithm of the matrix, generating the corresponding vector in the Lie Algebra.
	/// See the Detailed Description for details of this vector.
	static inline Vector<6, Precision> ln(const SE3& se3);
	/// @overload
	inline Vector<6, Precision> ln() const { return SE3::ln(*this); }

	inline SE3 inverse() const {
		const SO3<Precision> rinv = get_rotation().inverse();
		return SE3(rinv, -(rinv*my_translation));
	}

	/// Right-multiply by another SE3 (concatenate the two transformations)
	/// @param rhs The multipier
	template<typename P>
	inline SE3& operator *=(const SE3<P> & rhs) {
		get_translation() += get_rotation() * rhs.get_translation();
		get_rotation() *= rhs.get_rotation();
		return *this;
	}

	/// Right-multiply by another SE3 (concatenate the two transformations)
	/// @param rhs The multipier
	template<typename P>
	inline SE3<typename Internal::MultiplyType<Precision, P>::type> operator *(const SE3<P>& rhs) const { 
	    return SE3<typename Internal::MultiplyType<Precision, P>::type>(get_rotation()*rhs.get_rotation(), get_translation() + get_rotation()*rhs.get_translation()); 
	}

	inline SE3& left_multiply_by(const SE3& left) {
		get_translation() = left.get_translation() + left.get_rotation() * get_translation();
		get_rotation() = left.get_rotation() * get_rotation();
		return *this;
	}

	static inline Matrix<4,4,Precision> generator(int i){
		Matrix<4,4,Precision> result(Zeros);
		if(i < 3){
			result[i][3]=1;
			return result;
		}
		result[(i+1)%3][(i+2)%3] = -1;
		result[(i+2)%3][(i+1)%3] = 1;
		return result;
	}

  /// Returns the i-th generator times pos
  template<typename Base>
  inline static Vector<4,Precision> generator_field(int i, const Vector<4, Precision, Base>& pos)
  {
    Vector<4, Precision> result(Zeros);
    if(i < 3){
      result[i]=pos[3];
      return result;
    }
    result[(i+1)%3] = - pos[(i+2)%3];
    result[(i+2)%3] = pos[(i+1)%3];
    return result;
  }
  
	/// Transfer a matrix in the Lie Algebra from one
	/// co-ordinate frame to another. This is the operation such that for a matrix 
	/// \f$ B \f$, 
	/// \f$ e^{\text{Adj}(v)} = Be^{v}B^{-1} \f$
	/// @param M The Matrix to transfer
	template<int S, typename P2, typename Accessor>
	inline Vector<6, Precision> adjoint(const Vector<S,P2, Accessor>& vect)const;

	/// Transfer covectors between frames (using the transpose of the inverse of the adjoint)
	/// so that trinvadjoint(vect1) * adjoint(vect2) = vect1 * vect2
	template<int S, typename P2, typename Accessor>
	inline Vector<6, Precision> trinvadjoint(const Vector<S,P2,Accessor>& vect)const;
	
	///@overload
	template <int R, int C, typename P2, typename Accessor>
	inline Matrix<6,6,Precision> adjoint(const Matrix<R,C,P2,Accessor>& M)const;

	///@overload
	template <int R, int C, typename P2, typename Accessor>
	inline Matrix<6,6,Precision> trinvadjoint(const Matrix<R,C,P2,Accessor>& M)const;

private:
	SO3<Precision> my_rotation;
	Vector<3, Precision> my_translation;
};

// transfers a vector in the Lie algebra
// from one coord frame to another
// so that exp(adjoint(vect)) = (*this) * exp(vect) * (this->inverse())
template<typename Precision>
template<int S, typename P2, typename Accessor>
inline Vector<6, Precision> SE3<Precision>::adjoint(const Vector<S,P2, Accessor>& vect) const{
	SizeMismatch<6,S>::test(6, vect.size());
	Vector<6, Precision> result;
	result.template slice<3,3>() = get_rotation() * vect.template slice<3,3>();
	result.template slice<0,3>() = get_rotation() * vect.template slice<0,3>();
	result.template slice<0,3>() += get_translation() ^ result.template slice<3,3>();
	return result;
}

// tansfers covectors between frames
// (using the transpose of the inverse of the adjoint)
// so that trinvadjoint(vect1) * adjoint(vect2) = vect1 * vect2
template<typename Precision>
template<int S, typename P2, typename Accessor>
inline Vector<6, Precision> SE3<Precision>::trinvadjoint(const Vector<S,P2, Accessor>& vect) const{
	SizeMismatch<6,S>::test(6, vect.size());
	Vector<6, Precision> result;
	result.template slice<3,3>() = get_rotation() * vect.template slice<3,3>();
	result.template slice<0,3>() = get_rotation() * vect.template slice<0,3>();
	result.template slice<3,3>() += get_translation() ^ result.template slice<0,3>();
	return result;
}

template<typename Precision>
template<int R, int C, typename P2, typename Accessor>
inline Matrix<6,6,Precision> SE3<Precision>::adjoint(const Matrix<R,C,P2,Accessor>& M)const{
	SizeMismatch<6,R>::test(6, M.num_cols());
	SizeMismatch<6,C>::test(6, M.num_rows());

	Matrix<6,6,Precision> result;
	for(int i=0; i<6; i++){
		result.T()[i] = adjoint(M.T()[i]);
	}
	for(int i=0; i<6; i++){
		result[i] = adjoint(result[i]);
	}
	return result;
}

template<typename Precision>
template<int R, int C, typename P2, typename Accessor>
inline Matrix<6,6,Precision> SE3<Precision>::trinvadjoint(const Matrix<R,C,P2,Accessor>& M)const{
	SizeMismatch<6,R>::test(6, M.num_cols());
	SizeMismatch<6,C>::test(6, M.num_rows());

	Matrix<6,6,Precision> result;
	for(int i=0; i<6; i++){
		result.T()[i] = trinvadjoint(M.T()[i]);
	}
	for(int i=0; i<6; i++){
		result[i] = trinvadjoint(result[i]);
	}
	return result;
}

/// Write an SE3 to a stream 
/// @relates SE3
template <typename Precision>
inline std::ostream& operator <<(std::ostream& os, const SE3<Precision>& rhs){
	std::streamsize fw = os.width();
	for(int i=0; i<3; i++){
		os.width(fw);
		os << rhs.get_rotation().get_matrix()[i];
		os.width(fw);
		os << rhs.get_translation()[i] << '\n';
	}
	return os;
}


/// Reads an SE3 from a stream 
/// @relates SE3
template <typename Precision>
inline std::istream& operator>>(std::istream& is, SE3<Precision>& rhs){
	for(int i=0; i<3; i++){
		is >> rhs.get_rotation().my_matrix[i].ref() >> rhs.get_translation()[i];
	}
	rhs.get_rotation().coerce();
	return is;
}

//////////////////
// operator *   //
// SE3 * Vector //
//////////////////

namespace Internal {
template<int S, typename PV, typename A, typename P>
struct SE3VMult;
}

template<int S, typename PV, typename A, typename P>
struct Operator<Internal::SE3VMult<S,PV,A,P> > {
	const SE3<P> & lhs;
	const Vector<S,PV,A> & rhs;
	
	Operator(const SE3<P> & l, const Vector<S,PV,A> & r ) : lhs(l), rhs(r) {}
	
	template <int S0, typename P0, typename A0>
	void eval(Vector<S0, P0, A0> & res ) const {
		SizeMismatch<4,S>::test(4, rhs.size());
		res.template slice<0,3>()=lhs.get_rotation() * rhs.template slice<0,3>();
		res.template slice<0,3>()+=TooN::operator*(lhs.get_translation(),rhs[3]);
		res[3] = rhs[3];
	}
	int size() const { return 4; }
};

/// Right-multiply by a Vector
/// @relates SE3
template<int S, typename PV, typename A, typename P> inline
Vector<4, typename Internal::MultiplyType<P,PV>::type> operator*(const SE3<P> & lhs, const Vector<S,PV,A>& rhs){
	return Vector<4, typename Internal::MultiplyType<P,PV>::type>(Operator<Internal::SE3VMult<S,PV,A,P> >(lhs,rhs));
}

/// Right-multiply by a Vector
/// @relates SE3
template <typename PV, typename A, typename P> inline
Vector<3, typename Internal::MultiplyType<P,PV>::type> operator*(const SE3<P>& lhs, const Vector<3,PV,A>& rhs){
	return lhs.get_translation() + lhs.get_rotation() * rhs;
}

//////////////////
// operator *   //
// Vector * SE3 //
//////////////////

namespace Internal {
template<int S, typename PV, typename A, typename P>
struct VSE3Mult;
}

template<int S, typename PV, typename A, typename P>
struct Operator<Internal::VSE3Mult<S,PV,A,P> > {
	const Vector<S,PV,A> & lhs;
	const SE3<P> & rhs;
	
	Operator( const Vector<S,PV,A> & l, const SE3<P> & r ) : lhs(l), rhs(r) {}
	
	template <int S0, typename P0, typename A0>
	void eval(Vector<S0, P0, A0> & res ) const {
		SizeMismatch<4,S>::test(4, lhs.size());
		res.template slice<0,3>()=lhs.template slice<0,3>() * rhs.get_rotation();
		res[3] = lhs[3];
		res[3] += lhs.template slice<0,3>() * rhs.get_translation();
	}
	int size() const { return 4; }
};

/// Left-multiply by a Vector
/// @relates SE3
template<int S, typename PV, typename A, typename P> inline
Vector<4, typename Internal::MultiplyType<P,PV>::type> operator*( const Vector<S,PV,A>& lhs, const SE3<P> & rhs){
	return Vector<4, typename Internal::MultiplyType<P,PV>::type>(Operator<Internal::VSE3Mult<S,PV,A,P> >(lhs,rhs));
}

//////////////////
// operator *   //
// SE3 * Matrix //
//////////////////

namespace Internal {
template <int R, int C, typename PM, typename A, typename P>
struct SE3MMult;
}

template<int R, int Cols, typename PM, typename A, typename P>
struct Operator<Internal::SE3MMult<R, Cols, PM, A, P> > {
	const SE3<P> & lhs;
	const Matrix<R,Cols,PM,A> & rhs;
	
	Operator(const SE3<P> & l, const Matrix<R,Cols,PM,A> & r ) : lhs(l), rhs(r) {}
	
	template <int R0, int C0, typename P0, typename A0>
	void eval(Matrix<R0, C0, P0, A0> & res ) const {
		SizeMismatch<4,R>::test(4, rhs.num_rows());
		for(int i=0; i<rhs.num_cols(); ++i)
			res.T()[i] = lhs * rhs.T()[i];
	}
	int num_cols() const { return rhs.num_cols(); }
	int num_rows() const { return 4; }
};

/// Right-multiply by a Matrix
/// @relates SE3
template <int R, int Cols, typename PM, typename A, typename P> inline 
Matrix<4,Cols, typename Internal::MultiplyType<P,PM>::type> operator*(const SE3<P> & lhs, const Matrix<R,Cols,PM, A>& rhs){
	return Matrix<4,Cols,typename Internal::MultiplyType<P,PM>::type>(Operator<Internal::SE3MMult<R, Cols, PM, A, P> >(lhs,rhs));
}

//////////////////
// operator *   //
// Matrix * SE3 //
//////////////////

namespace Internal {
template <int Rows, int C, typename PM, typename A, typename P>
struct MSE3Mult;
}

template<int Rows, int C, typename PM, typename A, typename P>
struct Operator<Internal::MSE3Mult<Rows, C, PM, A, P> > {
	const Matrix<Rows,C,PM,A> & lhs;
	const SE3<P> & rhs;
	
	Operator( const Matrix<Rows,C,PM,A> & l, const SE3<P> & r ) : lhs(l), rhs(r) {}
	
	template <int R0, int C0, typename P0, typename A0>
	void eval(Matrix<R0, C0, P0, A0> & res ) const {
		SizeMismatch<4, C>::test(4, lhs.num_cols());
		for(int i=0; i<lhs.num_rows(); ++i)
			res[i] = lhs[i] * rhs;
	}
	int num_cols() const { return 4; }
	int num_rows() const { return lhs.num_rows(); }
};

/// Left-multiply by a Matrix
/// @relates SE3
template <int Rows, int C, typename PM, typename A, typename P> inline 
Matrix<Rows,4, typename Internal::MultiplyType<PM,P>::type> operator*(const Matrix<Rows,C,PM, A>& lhs, const SE3<P> & rhs ){
	return Matrix<Rows,4,typename Internal::MultiplyType<PM,P>::type>(Operator<Internal::MSE3Mult<Rows, C, PM, A, P> >(lhs,rhs));
}

template <typename Precision>
template <int S, typename P, typename VA>
inline SE3<Precision> SE3<Precision>::exp(const Vector<S, P, VA>& mu){
	SizeMismatch<6,S>::test(6, mu.size());
	static const Precision one_6th = 1.0/6.0;
	static const Precision one_20th = 1.0/20.0;
	using std::sqrt;
	
	SE3<Precision> result;
	
	const Vector<3,Precision> w = mu.template slice<3,3>();
	const Precision theta_sq = w*w;
	const Precision theta = sqrt(theta_sq);
	Precision A, B;
	
	const Vector<3,Precision> cross = w ^ mu.template slice<0,3>();
	if (theta_sq < 1e-8) {
		A = 1.0 - one_6th * theta_sq;
		B = 0.5;
		result.get_translation() = mu.template slice<0,3>() + 0.5 * cross;
	} else {
		Precision C;
		if (theta_sq < 1e-6) {
			C = one_6th*(1.0 - one_20th * theta_sq);
			A = 1.0 - theta_sq * C;
			B = 0.5 - 0.25 * one_6th * theta_sq;
		} else {
			const Precision inv_theta = 1.0/theta;
			A = sin(theta) * inv_theta;
			B = (1 - cos(theta)) * (inv_theta * inv_theta);
			C = (1 - A) * (inv_theta * inv_theta);
		}
		result.get_translation() = mu.template slice<0,3>() + TooN::operator*(B, cross) + TooN::operator*(C, (w ^ cross));
	}
	rodrigues_so3_exp(w, A, B, result.get_rotation().my_matrix);
	return result;
}

template <typename Precision>
inline Vector<6, Precision> SE3<Precision>::ln(const SE3<Precision>& se3) {
	using std::sqrt;
	Vector<3,Precision> rot = se3.get_rotation().ln();
	const Precision theta = sqrt(rot*rot);

	Precision shtot = 0.5;	
	if(theta > 0.00001) {
		shtot = sin(theta/2)/theta;
	}
	
	// now do the rotation
	const SO3<Precision> halfrotator = SO3<Precision>::exp(rot * -0.5);
	Vector<3, Precision> rottrans = halfrotator * se3.get_translation();
	
	if(theta > 0.001){
		rottrans -= TooN::operator*(rot, ((se3.get_translation() * rot) * (1-2*shtot) / (rot*rot)));
	} else {
		rottrans -= TooN::operator*(rot, ((se3.get_translation() * rot)/24));
	}
	
	rottrans /= (2 * shtot);
	
	Vector<6, Precision> result;
	result.template slice<0,3>()=rottrans;
	result.template slice<3,3>()=rot;
	return result;
}

template <typename Precision>
inline SE3<Precision> operator*(const SO3<Precision>& lhs, const SE3<Precision>& rhs){
	return SE3<Precision>(lhs*rhs.get_rotation(),lhs*rhs.get_translation());
}

#if 0 // should be moved to another header file

    template <class A> inline
    Vector<3> transform(const SE3& pose, const FixedVector<3,A>& x) { return pose.get_rotation()*x + pose.get_translation(); }
    
    template <class A> inline
    Vector<3> transform_inverse_depth(const SE3& pose, const FixedVector<3,A>& uvq)
    {
	const Matrix<3>& R = pose.get_rotation().get_matrix();	
	const Vector<3> DqT = R.template slice<0,0,3,2>() * uvq.template slice<0,2>() + R.T()[2] + uvq[2] * pose.get_translation();
	const double inv_z = 1.0/ DqT[2];
	return makeVector(DqT[0]*inv_z, DqT[1]*inv_z, uvq[2]*inv_z);
    }

    template <class A1, class A2> inline
    Vector<3> transform(const SE3& pose, const FixedVector<3,A1>& x, FixedMatrix<3,3,A2>& J_x)
    {
	J_x = pose.get_rotation().get_matrix();
	return pose * x;
    }


    template <class A1, class A2> inline
    Vector<3> inverse_depth_point_jacobian(const SE3& pose, const double q, const FixedVector<3,A1>& DqT, const double inv_z, FixedMatrix<3,3,A2>& J_uvq)
    {
	const Matrix<3>& R = pose.get_rotation().get_matrix();	
	const Vector<3>& T = pose.get_translation();
	const double u1 = DqT[0] * inv_z;
	J_uvq[0][0] = inv_z * (R[0][0] - u1*R[2][0]);
	J_uvq[0][1] = inv_z * (R[0][1] - u1*R[2][1]);
	J_uvq[0][2] = inv_z * (T[0] - u1 * T[2]);
	
	const double v1 = DqT[1] * inv_z;
	J_uvq[1][0] = inv_z * (R[1][0] - v1*R[2][0]);
	J_uvq[1][1] = inv_z * (R[1][1] - v1*R[2][1]);
	J_uvq[1][2] = inv_z * (T[1] - v1 * T[2]);
	
	const double q1 = q * inv_z;
	J_uvq[2][0] = inv_z * (-q1 * R[2][0]);
	J_uvq[2][1] = inv_z * (-q1 * R[2][1]);
	J_uvq[2][2] = inv_z * (1.0 - q1 * T[2]);

	return makeVector(u1, v1, q1);
    }
    

    template <class A1, class A2> inline
    Vector<3> transform_inverse_depth(const SE3& pose, const FixedVector<3,A1>& uvq, FixedMatrix<3,3,A2>& J_uvq)
    {
	const Matrix<3>& R = pose.get_rotation().get_matrix();	
	const Vector<3>& T = pose.get_translation();
	const double q = uvq[2];
	const Vector<3> DqT = R.template slice<0,0,3,2>() * uvq.template slice<0,2>() + R.T()[2] + q * T;
	const double inv_z = 1.0 / DqT[2];

	return inverse_depth_point_jacobian(pose, q, DqT, inv_z, J_uvq);
    }

    template <class A1, class A2, class A3> inline
    Vector<3> transform(const SE3& pose, const FixedVector<3,A1>& x, FixedMatrix<3,3,A2>& J_x, FixedMatrix<3,6,A3>& J_pose)
    {
	J_x = pose.get_rotation().get_matrix();
	Identity(J_pose.template slice<0,0,3,3>());
	const Vector<3> cx = pose * x;
	J_pose[0][3] = J_pose[1][4] = J_pose[2][5] = 0;
	J_pose[1][3] = -(J_pose[0][4] = cx[2]);
	J_pose[0][5] = -(J_pose[2][3] = cx[1]);
	J_pose[2][4] = -(J_pose[1][5] = cx[0]);
	return cx;
    }

    template <class A1, class A2, class A3> inline
    Vector<3> transform_inverse_depth(const SE3& pose, const FixedVector<3,A1>& uvq, FixedMatrix<3,3,A2>& J_uvq, FixedMatrix<3,6,A3>& J_pose)
    {
	const Matrix<3>& R = pose.get_rotation().get_matrix();	
	const Vector<3>& T = pose.get_translation();
	const double q = uvq[2];
	const Vector<3> DqT = R.template slice<0,0,3,2>() * uvq.template slice<0,2>() + R.T()[2] + q * T;
	const double inv_z = 1.0 / DqT[2];
	
	const Vector<3> uvq1 = inverse_depth_point_jacobian(pose, q, DqT, inv_z, J_uvq);
	const double q1 = uvq1[2];

	J_pose[0][1] = J_pose[1][0] = J_pose[2][0] = J_pose[2][1] = 0;
	J_pose[0][0] = J_pose[1][1] = q1;
	J_pose[0][2] = -uvq1[0] * q1;
	J_pose[1][2] = -uvq1[1] * q1;
	J_pose[2][2] = -q1 * q1;
	
	J_pose[0][3] = -uvq1[1]*uvq1[0];
	J_pose[0][4] = 1 + uvq1[0]*uvq1[0];
	J_pose[0][5] = -uvq1[1];
	
	J_pose[1][3] = -1 - uvq1[1]*uvq1[1];
	J_pose[1][4] = uvq1[0] * uvq1[1];
	J_pose[1][5] = uvq1[0];

	J_pose[2][3] = -uvq1[1]*q1;
	J_pose[2][4] = uvq1[0]*q1;
	J_pose[2][5] = 0;

	return uvq1;
    }

    template <class A1, class A2, class A3> inline
    Vector<2> project_transformed_point(const SE3& pose, const FixedVector<3,A1>& in_frame, FixedMatrix<2,3,A2>& J_x, FixedMatrix<2,6,A3>& J_pose)
    {
	const double z_inv = 1.0/in_frame[2];
	const double x_z_inv = in_frame[0]*z_inv;
	const double y_z_inv = in_frame[1]*z_inv;
	const double cross = x_z_inv * y_z_inv;
	J_pose[0][0] = J_pose[1][1] = z_inv;
	J_pose[0][1] = J_pose[1][0] = 0;
	J_pose[0][2] = -x_z_inv * z_inv;
	J_pose[1][2] = -y_z_inv * z_inv;
	J_pose[0][3] = -cross;
	J_pose[0][4] = 1 + x_z_inv*x_z_inv; 
	J_pose[0][5] = -y_z_inv;  
	J_pose[1][3] = -1 - y_z_inv*y_z_inv;
	J_pose[1][4] =  cross;
	J_pose[1][5] =  x_z_inv;    
	
	const TooN::Matrix<3>& R = pose.get_rotation().get_matrix();
	J_x[0][0] = z_inv*(R[0][0] - x_z_inv * R[2][0]);
	J_x[0][1] = z_inv*(R[0][1] - x_z_inv * R[2][1]);
	J_x[0][2] = z_inv*(R[0][2] - x_z_inv * R[2][2]);
	J_x[1][0] = z_inv*(R[1][0] - y_z_inv * R[2][0]);
	J_x[1][1] = z_inv*(R[1][1] - y_z_inv * R[2][1]);
	J_x[1][2] = z_inv*(R[1][2] - y_z_inv * R[2][2]);
	
	return makeVector(x_z_inv, y_z_inv);
    }


    template <class A1> inline
    Vector<2> transform_and_project(const SE3& pose, const FixedVector<3,A1>& x)
    {
	return project(transform(pose,x));
    }

    template <class A1> inline
    Vector<2> transform_and_project_inverse_depth(const SE3& pose, const FixedVector<3,A1>& uvq)
    {
	const Matrix<3>& R = pose.get_rotation().get_matrix();	
	const Vector<3>& T = pose.get_translation();
	const Vector<3> DqT = R.template slice<0,0,3,2>() * uvq.template slice<0,2>() + R.T()[2] + uvq[2] * T;
	return project(DqT);
    }

    template <class A1, class A2, class A3> inline
    Vector<2> transform_and_project(const SE3& pose, const FixedVector<3,A1>& x, FixedMatrix<2,3,A2>& J_x, FixedMatrix<2,6,A3>& J_pose)
    {
	return project_transformed_point(pose, transform(pose,x), J_x, J_pose);
    }

    template <class A1, class A2, class A3> inline
    Vector<2> transform_and_project_inverse_depth(const SE3& pose, const FixedVector<3,A1>& uvq, FixedMatrix<2,3,A2>& J_uvq, FixedMatrix<2,6,A3>& J_pose)
    {
	const Matrix<3>& R = pose.get_rotation().get_matrix();	
	const Vector<3>& T = pose.get_translation();
	const double q = uvq[2];
	const Vector<3> DqT = R.template slice<0,0,3,2>() * uvq.template slice<0,2>() + R.T()[2] + q * T;
	const Vector<2> uv = project_transformed_point(pose, DqT, J_uvq, J_pose);
	J_uvq.T()[2] = J_pose.template slice<0,0,2,3>() * pose.get_translation();
	J_pose.template slice<0,0,2,3>() *= q;
	return uv;
    }

#endif

}

#endif
