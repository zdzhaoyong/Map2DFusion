// -*- c++ -*-

// Copyright (C) 2011 Tom Drummond (twd20@cam.ac.uk)
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

#ifndef TOON_INCLUDE_SIM3_H
#define TOON_INCLUDE_SIM3_H

#include <TooN/se3.h>

namespace TooN {


/// Represent a three-dimensional similarity transformation (a rotation, a scale factor and a translation). 
/// This can be represented by a \f$3\times\f$4 matrix operating on a homogeneous co-ordinate, 
/// so that a vector \f$\underline{x}\f$ is transformed to a new location \f$\underline{x}'\f$
/// by
/// \f[\begin{aligned}\underline{x}' &= E\times\underline{x}\\ \begin{bmatrix}x'\\y'\\z'\end{bmatrix} &= \begin{pmatrix}s r_{11} & s r_{12} & s r_{13} & t_1\\s r_{21} & s r_{22} & s r_{23} & t_2\\s r_{31} & s r_{32} & s r_{33} & t_3\end{pmatrix}\begin{bmatrix}x\\y\\z\\1\end{bmatrix}\end{aligned}\f]
/// 
/// This transformation is a member of the Lie group SIM3. These can be parameterised with
/// seven numbers (in the space of the Lie Algebra). In this class, the first three parameters are a
/// translation vector while the second three are a rotation vector, whose direction is the axis of rotation
/// and length the amount of rotation (in radians), as for SO3. The seventh parameter is the log of the scale of the transformation.
/// @ingroup gTransforms
template <typename Precision = DefaultPrecision>
class SIM3 {
public:
	/// Default constructor. Initialises the the rotation to zero (the identity), the scale to one and the translation to zero
	inline SIM3() : my_scale(1) {}

	template <int S, typename P, typename A> 
	SIM3(const SO3<Precision> & R, const Vector<S, P, A>& T, const Precision & s) : my_se3(R,T), my_scale(s) {}
	template <int S, typename P, typename A>
	SIM3(const Vector<S, P, A> & v) { *this = exp(v); }

	/// Returns the rotation part of the transformation as a SO3
	inline SO3<Precision>& get_rotation(){return my_se3.get_rotation();}
	/// @overload
	inline const SO3<Precision>& get_rotation() const {return my_se3.get_rotation();}

	/// Returns the translation part of the transformation as a Vector
	inline Vector<3, Precision>& get_translation() {return my_se3.get_translation();}
	/// @overload
	inline const Vector<3, Precision>& get_translation() const {return my_se3.get_translation();}

	/// Returns the scale factor
	inline Precision & get_scale() { return my_scale; }
	/// @overload
	inline const Precision & get_scale() const { return my_scale; }

	/// Exponentiate a Vector in the Lie Algebra to generate a new SIM3.
	/// See the Detailed Description for details of this vector.
	/// @param vect The Vector to exponentiate
	template <int S, typename P, typename A>
	static inline SIM3 exp(const Vector<S, P, A>& vect);

	/// Take the logarithm of the matrix, generating the corresponding vector in the Lie Algebra.
	/// See the Detailed Description for details of this vector.
	static inline Vector<7, Precision> ln(const SIM3& se3);
	/// @overload
	inline Vector<7, Precision> ln() const { return SIM3::ln(*this); }

	inline SIM3 inverse() const {
		const SO3<Precision> rinv = get_rotation().inverse();
		const Precision inv_scale = 1/my_scale;
		return SIM3(rinv, -(inv_scale*(rinv*get_translation())), inv_scale);
	}

	/// Right-multiply by another SIM3 (concatenate the two transformations)
	/// @param rhs The multipier
	inline SIM3& operator *=(const SIM3& rhs) {
		get_translation() += get_rotation() * (get_scale() * rhs.get_translation());
		get_rotation() *= rhs.get_rotation();
		get_scale() *= rhs.get_scale();
		return *this;
	}

	/// Right-multiply by another SIM3 (concatenate the two transformations)
	/// @param rhs The multipier
	template<typename P>
	inline SIM3<typename Internal::MultiplyType<Precision, P>::type> operator *(const SIM3<P>& rhs) const { 
	    return SIM3<typename Internal::MultiplyType<Precision, P>::type>(get_rotation()*rhs.get_rotation(), get_translation() + get_rotation()*(get_scale()*rhs.get_translation()), get_scale()*rhs.get_scale());
	}

	inline SIM3& left_multiply_by(const SIM3& left) {
		get_translation() = left.get_translation() + left.get_rotation() * (left.get_scale() * get_translation());
		get_rotation() = left.get_rotation() * get_rotation();
		get_scale() = left.get_scale() * get_scale();
		return *this;
	}

	static inline Matrix<4,4,Precision> generator(int i){
		Matrix<4,4,Precision> result(Zeros);
		if(i < 3){
			result(i,3)=1;
			return result;
		}
		if(i < 6){
			result[(i+1)%3][(i+2)%3] = -1;
			result[(i+2)%3][(i+1)%3] = 1;
			return result;
		}
		result(0,0) = 1;
		result(1,1) = 1;
		result(2,2) = 1;
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
		if(i < 6){
			result[(i+1)%3] = - pos[(i+2)%3];
			result[(i+2)%3] = pos[(i+1)%3];
			return result;
		}
		result.template slice<0,3>() = pos.template slice<0,3>();
		return result;
	}
  
	/// Transfer a matrix in the Lie Algebra from one
	/// co-ordinate frame to another. This is the operation such that for a matrix 
	/// \f$ B \f$, 
	/// \f$ e^{\text{Adj}(v)} = Be^{v}B^{-1} \f$
	/// @param M The Matrix to transfer
	template<int S, typename P2, typename Accessor>
	inline Vector<7, Precision> adjoint(const Vector<S,P2, Accessor>& vect)const;

	/// Transfer covectors between frames (using the transpose of the inverse of the adjoint)
	/// so that trinvadjoint(vect1) * adjoint(vect2) = vect1 * vect2
	template<int S, typename P2, typename Accessor>
	inline Vector<7, Precision> trinvadjoint(const Vector<S,P2,Accessor>& vect)const;
	
	///@overload
	template <int R, int C, typename P2, typename Accessor>
	inline Matrix<7,7,Precision> adjoint(const Matrix<R,C,P2,Accessor>& M)const;

	///@overload
	template <int R, int C, typename P2, typename Accessor>
	inline Matrix<7,7,Precision> trinvadjoint(const Matrix<R,C,P2,Accessor>& M)const;

private:
	SE3<Precision> my_se3;
	Precision my_scale;
};

// transfers a vector in the Lie algebra
// from one coord frame to another
// so that exp(adjoint(vect)) = (*this) * exp(vect) * (this->inverse())
template<typename Precision>
template<int S, typename P2, typename Accessor>
inline Vector<7, Precision> SIM3<Precision>::adjoint(const Vector<S,P2, Accessor>& vect) const{
	SizeMismatch<7,S>::test(7, vect.size());
	Vector<7, Precision> result;
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
inline Vector<7, Precision> SIM3<Precision>::trinvadjoint(const Vector<S,P2, Accessor>& vect) const{
	SizeMismatch<7,S>::test(7, vect.size());
	Vector<7, Precision> result;
	result.template slice<3,3>() = get_rotation() * vect.template slice<3,3>();
	result.template slice<0,3>() = get_rotation() * vect.template slice<0,3>();
	result.template slice<3,3>() += get_translation() ^ result.template slice<0,3>();
	return result;
}

template<typename Precision>
template<int R, int C, typename P2, typename Accessor>
inline Matrix<7,7,Precision> SIM3<Precision>::adjoint(const Matrix<R,C,P2,Accessor>& M)const{
	SizeMismatch<7,R>::test(7, M.num_cols());
	SizeMismatch<7,C>::test(7, M.num_rows());

	Matrix<7,7,Precision> result;
	for(int i=0; i<7; i++){
		result.T()[i] = adjoint(M.T()[i]);
	}
	for(int i=0; i<7; i++){
		result[i] = adjoint(result[i]);
	}
	return result;
}

template<typename Precision>
template<int R, int C, typename P2, typename Accessor>
inline Matrix<7,7,Precision> SIM3<Precision>::trinvadjoint(const Matrix<R,C,P2,Accessor>& M)const{
	SizeMismatch<7,R>::test(7, M.num_cols());
	SizeMismatch<7,C>::test(7, M.num_rows());

	Matrix<7,7,Precision> result;
	for(int i=0; i<7; i++){
		result.T()[i] = trinvadjoint(M.T()[i]);
	}
	for(int i=0; i<7; i++){
		result[i] = trinvadjoint(result[i]);
	}
	return result;
}

/// Write an SIM3 to a stream 
/// @relates SIM3
template <typename Precision>
inline std::ostream& operator <<(std::ostream& os, const SIM3<Precision>& rhs){
	std::streamsize fw = os.width();
	for(int i=0; i<3; i++){
		os.width(fw);
		os << rhs.get_rotation().get_matrix()[i] * rhs.get_scale();
		os.width(fw);
		os << rhs.get_translation()[i] << '\n';
	}
	return os;
}


/// Reads an SIM3 from a stream 
/// @relates SIM3
template <typename Precision>
inline std::istream& operator>>(std::istream& is, SIM3<Precision>& rhs){
	for(int i=0; i<3; i++){
		is >> rhs.get_rotation().my_matrix[i].ref() >> rhs.get_translation()[i];
	}
	rhs.get_scale() = (norm(rhs.get_rotation().my_matrix[0]) + norm(rhs.get_rotation().my_matrix[1]) + norm(rhs.get_rotation().my_matrix[2]))/3;
	rhs.get_rotation().coerce();
	return is;
}

//////////////////
// operator *   //
// SIM3 * Vector //
//////////////////

namespace Internal {
template<int S, typename PV, typename A, typename P>
struct SIM3VMult;
}

template<int S, typename PV, typename A, typename P>
struct Operator<Internal::SIM3VMult<S,PV,A,P> > {
	const SIM3<P> & lhs;
	const Vector<S,PV,A> & rhs;
	
	Operator(const SIM3<P> & l, const Vector<S,PV,A> & r ) : lhs(l), rhs(r) {}
	
	template <int S0, typename P0, typename A0>
	void eval(Vector<S0, P0, A0> & res ) const {
		SizeMismatch<4,S>::test(4, rhs.size());
		res.template slice<0,3>()=lhs.get_rotation() * (lhs.get_scale() * rhs.template slice<0,3>());
		res.template slice<0,3>()+=TooN::operator*(lhs.get_translation(),rhs[3]);
		res[3] = rhs[3];
	}
	int size() const { return 4; }
};

/// Right-multiply by a Vector
/// @relates SIM3
template<int S, typename PV, typename A, typename P> inline
Vector<4, typename Internal::MultiplyType<P,PV>::type> operator*(const SIM3<P> & lhs, const Vector<S,PV,A>& rhs){
	return Vector<4, typename Internal::MultiplyType<P,PV>::type>(Operator<Internal::SIM3VMult<S,PV,A,P> >(lhs,rhs));
}

/// Right-multiply by a Vector
/// @relates SIM3
template <typename PV, typename A, typename P> inline
Vector<3, typename Internal::MultiplyType<P,PV>::type> operator*(const SIM3<P>& lhs, const Vector<3,PV,A>& rhs){
	return lhs.get_translation() + lhs.get_rotation() * (lhs.get_scale() * rhs);
}

//////////////////
// operator *   //
// Vector * SIM3 //
//////////////////

namespace Internal {
template<int S, typename PV, typename A, typename P>
struct VSIM3Mult;
}

template<int S, typename PV, typename A, typename P>
struct Operator<Internal::VSIM3Mult<S,PV,A,P> > {
	const Vector<S,PV,A> & lhs;
	const SIM3<P> & rhs;
	
	Operator( const Vector<S,PV,A> & l, const SIM3<P> & r ) : lhs(l), rhs(r) {}
	
	template <int S0, typename P0, typename A0>
	void eval(Vector<S0, P0, A0> & res ) const {
		SizeMismatch<4,S>::test(4, lhs.size());
		res.template slice<0,3>()= rhs.get_scale() * lhs.template slice<0,3>() * rhs.get_rotation();
		res[3] = lhs[3];
		res[3] += lhs.template slice<0,3>() * rhs.get_translation();
	}
	int size() const { return 4; }
};

/// Left-multiply by a Vector
/// @relates SIM3
template<int S, typename PV, typename A, typename P> inline
Vector<4, typename Internal::MultiplyType<P,PV>::type> operator*( const Vector<S,PV,A>& lhs, const SIM3<P> & rhs){
	return Vector<4, typename Internal::MultiplyType<P,PV>::type>(Operator<Internal::VSIM3Mult<S,PV,A,P> >(lhs,rhs));
}

//////////////////
// operator *   //
// SIM3 * Matrix //
//////////////////

namespace Internal {
template <int R, int C, typename PM, typename A, typename P>
struct SIM3MMult;
}

template<int R, int Cols, typename PM, typename A, typename P>
struct Operator<Internal::SIM3MMult<R, Cols, PM, A, P> > {
	const SIM3<P> & lhs;
	const Matrix<R,Cols,PM,A> & rhs;
	
	Operator(const SIM3<P> & l, const Matrix<R,Cols,PM,A> & r ) : lhs(l), rhs(r) {}
	
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
/// @relates SIM3
template <int R, int Cols, typename PM, typename A, typename P> inline 
Matrix<4,Cols, typename Internal::MultiplyType<P,PM>::type> operator*(const SIM3<P> & lhs, const Matrix<R,Cols,PM, A>& rhs){
	return Matrix<4,Cols,typename Internal::MultiplyType<P,PM>::type>(Operator<Internal::SIM3MMult<R, Cols, PM, A, P> >(lhs,rhs));
}

//////////////////
// operator *   //
// Matrix * SIM3 //
//////////////////

namespace Internal {
template <int Rows, int C, typename PM, typename A, typename P>
struct MSIM3Mult;
}

template<int Rows, int C, typename PM, typename A, typename P>
struct Operator<Internal::MSIM3Mult<Rows, C, PM, A, P> > {
	const Matrix<Rows,C,PM,A> & lhs;
	const SIM3<P> & rhs;
	
	Operator( const Matrix<Rows,C,PM,A> & l, const SIM3<P> & r ) : lhs(l), rhs(r) {}
	
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
/// @relates SIM3
template <int Rows, int C, typename PM, typename A, typename P> inline 
Matrix<Rows,4, typename Internal::MultiplyType<PM,P>::type> operator*(const Matrix<Rows,C,PM, A>& lhs, const SIM3<P> & rhs ){
	return Matrix<Rows,4,typename Internal::MultiplyType<PM,P>::type>(Operator<Internal::MSIM3Mult<Rows, C, PM, A, P> >(lhs,rhs));
}

namespace Internal {

/// internal function that calculates the coefficients for the Rodrigues formula for SIM3 translation
template <typename Precision>
inline Vector<3, Precision> compute_rodrigues_coefficients_sim3( const Precision & s, const Precision & t ){
	using std::exp;

	Vector<3, Precision> coeff;
	const Precision es = exp(s);

	// 4 cases for s -> 0 and/or theta -> 0
	// the Taylor expansions were calculated with Maple 12 and truncated at the 3rd power,
	// such that eps^3 < 1e-18 which results in approximately 1 + eps^3 = 1
	static const Precision eps = 1e-6;

	if(fabs(s) < eps && fabs(t) < eps){
		coeff[0] = 1 + s/2 + s*s/6;
		coeff[1] = 1/2 + s/3 - t*t/24 + s*s/8;
		coeff[2] = 1/6 + s/8 - t*t/120 + s*s/20;
	} else if(fabs(s) < eps) {
		coeff[0] = 1 + s/2 + s*s/6;
		coeff[1] = (1-cos(t))/(t*t) + (sin(t)-cos(t)*t)*s/(t*t*t)+(2*sin(t)*t-t*t*cos(t)-2+2*cos(t))*s*s/(2*t*t*t*t);
		coeff[2] = (t-sin(t))/(t*t*t) - (-t*t-2+2*cos(t)+2*sin(t)*t)*s/(2*t*t*t*t) - (-t*t*t+6*cos(t)*t+3*sin(t)*t*t-6*sin(t))*s*s/(6*t*t*t*t*t);
	} else if(fabs(t) < eps) {
		coeff[0] = (es - 1)/s;
		coeff[1] = (s*es+1-es)/(s*s) - (6*s*es+6-6*es+es*s*s*s-3*es*s*s)*t*t/(6*s*s*s*s);
		coeff[2] = (es*s*s-2*s*es+2*es-2)/(2*s*s*s) - (es*s*s*s*s-4*es*s*s*s+12*es*s*s-24*s*es+24*es-24)*t*t/(24*s*s*s*s*s);
	} else {
		const Precision a = es * sin(t);
		const Precision b = es * cos(t);
		const Precision inv_s_theta = 1/(s*s + t*t);

		coeff[0] = (es - 1)/s;
		coeff[1] = (a*s + (1-b)*t) * inv_s_theta / t;
		coeff[2] = (coeff[0] - ((b-1)*s + a*t) * inv_s_theta) / (t*t);
	}

	return coeff;
}

}

template <typename Precision>
template <int S, typename P, typename VA>
inline SIM3<Precision> SIM3<Precision>::exp(const Vector<S, P, VA>& mu){
	SizeMismatch<7,S>::test(7, mu.size());
	using std::exp;
	
	SIM3<Precision> result;

	// scale
	result.get_scale() = exp(mu[6]);
	
	// rotation
	const Vector<3,Precision> w = mu.template slice<3,3>();
	const Precision t = norm(w);
	result.get_rotation() = SO3<>::exp(w);

	// translation
	const Vector<3, Precision> coeff = Internal::compute_rodrigues_coefficients_sim3(mu[6],t); 
	const Vector<3,Precision> cross = w ^ mu.template slice<0,3>();
    result.get_translation() = coeff[0] * mu.template slice<0,3>()
            + TooN::operator*(coeff[1], cross) + TooN::operator*(coeff[2], (w ^ cross));

	return result;
}

template <typename Precision>
inline Vector<7, Precision> SIM3<Precision>::ln(const SIM3<Precision>& sim3) {
	using std::sqrt;
	using std::log;

	Vector<7, Precision> result;
	
	// rotation
	result.template slice<3,3>() = sim3.get_rotation().ln();
	const Precision theta = norm(result.template slice<3,3>());

	// scale 
	const Precision es = sim3.get_scale();
	const Precision s = log(sim3.get_scale());
	result[6] = s;

	// Translation
	const Vector<3, Precision> coeff = Internal::compute_rodrigues_coefficients_sim3(s, theta);
	const Matrix<3,3, Precision> cross = cross_product_matrix(result.template slice<3,3>());
	const Matrix<3,3, Precision> W = Identity * coeff[0] + cross * coeff[1] + cross * cross * coeff[2];
	
	result.template slice<0,3>() = gaussian_elimination(W, sim3.get_translation());

	return result;
}

#if 0
template <typename Precision>
inline SE3<Precision> operator*(const SO3<Precision>& lhs, const SE3<Precision>& rhs){
	return SE3<Precision>(lhs*rhs.get_rotation(),lhs*rhs.get_translation());
}
#endif

}

#endif
