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

#ifndef TOON_INCLUDE_SO2_H
#define TOON_INCLUDE_SO2_H

#include <TooN/TooN.h>
#include <TooN/helpers.h>

namespace TooN {

template <typename Precision> class SO2;
template <typename Precision> class SE2;
template <typename Precision> class SIM2;

template<typename Precision> inline std::istream & operator>>(std::istream &, SO2<Precision> & );
template<typename Precision> inline std::istream & operator>>(std::istream &, SE2<Precision> & );
template<typename Precision> inline std::istream & operator>>(std::istream &, SIM2<Precision> & );

/// Class to represent a two-dimensional rotation matrix. Two-dimensional rotation
/// matrices are members of the Special Orthogonal Lie group SO2. This group can be parameterised with
/// one number (the rotation angle).
/// @ingroup gTransforms
template<typename Precision = DefaultPrecision>
class SO2 {
	friend std::istream& operator>> <Precision>(std::istream&, SO2& );
	friend std::istream& operator>> <Precision>(std::istream&, SE2<Precision>& );
	friend std::istream& operator>> <Precision>(std::istream&, SIM2<Precision>& );

public:
	/// Default constructor. Initialises the matrix to the identity (no rotation)
	SO2() : my_matrix(Identity) {} 
	
	/// Construct from a rotation matrix.
	SO2(const Matrix<2,2,Precision>& rhs) {  
		*this = rhs; 
		coerce();
	}

	/// Construct from an angle.
	SO2(const Precision l) { *this = exp(l); }
  
	/// Assigment operator from a general matrix. This also calls coerce()
	/// to make sure that the matrix is a valid rotation matrix.
	template <int R, int C, typename P, typename A> 
	SO2& operator=(const Matrix<R,C,P,A>& rhs){
		my_matrix = rhs;
		coerce();
		return *this;
	}

	/// Modifies the matrix to make sure it is a valid rotation matrix.
	void coerce(){
		my_matrix[0] = unit(my_matrix[0]);
		my_matrix[1] -= my_matrix[0] * (my_matrix[0]*my_matrix[1]);
		my_matrix[1] = unit(my_matrix[1]);
	}

	/// Exponentiate an angle in the Lie algebra to generate a new SO2.
	inline static SO2 exp(const Precision & d){
		SO2<Precision> result;
		result.my_matrix[0][0] = result.my_matrix[1][1] = cos(d);
		result.my_matrix[1][0] = sin(d);
		result.my_matrix[0][1] = -result.my_matrix[1][0];
		return result;
	}

	/// extracts the rotation angle from the SO2
	Precision ln() const { return atan2(my_matrix[1][0], my_matrix[0][0]); }

	/// Returns the inverse of this matrix (=the transpose, so this is a fast operation)
	SO2 inverse() const { return SO2(*this, Invert()); }

	/// Self right-multiply by another rotation matrix
	template <typename P>
	SO2& operator *=(const SO2<P>& rhs){
		my_matrix=my_matrix*rhs.get_matrix();
		return *this;
	}

	/// Right-multiply by another rotation matrix
	template <typename P>
	SO2<typename Internal::MultiplyType<Precision, P>::type> operator *(const SO2<P>& rhs) const { 
		return SO2<typename Internal::MultiplyType<Precision, P>::type>(*this,rhs); 
	}

	/// Returns the SO2 as a Matrix<2>
	const Matrix<2,2,Precision>& get_matrix() const {return my_matrix;}

	/// returns generator matrix
	static Matrix<2,2,Precision> generator() {
		Matrix<2,2,Precision> result;
		result[0] = makeVector(0,-1);
		result[1] = makeVector(1,0);
		return result;
	}

private:
	struct Invert {};
	inline SO2(const SO2& so2, const Invert&) : my_matrix(so2.my_matrix.T()) {}
	template <typename PA, typename PB>
	inline SO2(const SO2<PA>& a, const SO2<PB>& b) : my_matrix(a.get_matrix()*b.get_matrix()) {}

	Matrix<2,2,Precision> my_matrix;
};

/// Write an SO2 to a stream 
/// @relates SO2
template <typename Precision>
inline std::ostream& operator<< (std::ostream& os, const SO2<Precision> & rhs){
	return os << rhs.get_matrix();
}

/// Read from SO2 to a stream 
/// @relates SO2
template <typename Precision>
inline std::istream& operator>>(std::istream& is, SO2<Precision>& rhs){
	is >> rhs.my_matrix;
	rhs.coerce();
	return is;
}

/// Right-multiply by a Vector
/// @relates SO2
template<int D, typename P1, typename PV, typename Accessor>
inline Vector<2, typename Internal::MultiplyType<P1, PV>::type> operator*(const SO2<P1> & lhs, const Vector<D, PV, Accessor> & rhs){
	return lhs.get_matrix() * rhs;
}

/// Left-multiply by a Vector
/// @relates SO2
template<int D, typename P1, typename PV, typename Accessor>
inline Vector<2, typename Internal::MultiplyType<PV,P1>::type> operator*(const Vector<D, PV, Accessor>& lhs, const SO2<P1> & rhs){
	return lhs * rhs.get_matrix();
}

/// Right-multiply by a Matrix
/// @relates SO2
template <int R, int C, typename P1, typename P2, typename Accessor> 
inline Matrix<2,C,typename Internal::MultiplyType<P1,P2>::type> operator*(const SO2<P1> & lhs, const Matrix<R,C,P2,Accessor>& rhs){
	return lhs.get_matrix() * rhs;
}

/// Left-multiply by a Matrix
/// @relates SO2
template <int R, int C, typename P1, typename P2, typename Accessor>
inline Matrix<R,2,typename Internal::MultiplyType<P1,P2>::type> operator*(const Matrix<R,C,P1,Accessor>& lhs, const SO2<P2>& rhs){
	return lhs * rhs.get_matrix();
}

}

#endif
