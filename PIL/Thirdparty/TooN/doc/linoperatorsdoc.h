/*
    Copyright (c) 2005 Paul Smith

	Permission is granted to copy, distribute and/or modify this document under
	the terms of the GNU Free Documentation License, Version 1.2 or any later
	version published by the Free Software Foundation; with no Invariant
	Sections, no Front-Cover Texts, and no Back-Cover Texts.

    You should have received a copy of the GNU Free Documentation License
    License along with this library; if not, write to the Free Software
    Foundation, Inc.
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

*/
// A proxy version of the Matrix class,
// cleaned up to present a comprehensible
// version of the Mector interface

#ifdef DOXYGEN_INCLUDE_ONLY_FOR_DOCS

/// All classes and functions are within this namespace
namespace TooN
{

/// @name Arithmetic operations
//@{

/// Unary minus. Returns the negative of the argument.
/// @code
/// Vector<4> a = 1,2,3,4;
/// Vector<4> b = -a; // b will now be [-1 -2 -3 -4]
/// @endcode
/// @relates Vector
template<int Size>
Vector<Size> operator-(const Vector<Size>& v);

/// Add two vectors. Returns the sum of the two vectors.
/// @code
/// Vector<4> a = 1,2,3,4;
/// Vector<4> b = 5,5,5,5;
/// Vector<4> c = a + b;  // c will now be [6 7 8 9]
/// @endcode
/// @relates Vector
template<int Size>
Vector<Size> operator+(const Vector<Size>& lhs, const Vector<Size>& rhs);

/// Add a vector to another one. Returns the modified vector.
/// @code
/// Vector<4> a = 1,2,3,4;
/// Vector<4> b = 5,5,5,5;
/// a += b;   // a will now be [6 7 8 9]
/// @endcode
/// @relates Vector
template<int Size>
Vector<Size>& operator+= (Vector<Size>& lhs, const Vector<Size>& rhs);

/// Subtract two vectors. Returns the dfference of the two vectors.
/// @code
/// Vector<4> a = 1,2,3,4;
/// Vector<4> b = 5,5,5,5;
/// Vector<4> c = b + a;  // c will now be [4 3 2 1]
/// @endcode
/// @relates Vector
template<int Size>
Vector<Size> operator-(const Vector<Size>& lhs, const Vector<Size>& rhs);

/// Subtract one vector from another one. Returns the modified vector.
/// @code
/// Vector<4> a = 1,2,3,4;
/// Vector<4> b = 5,5,5,5;
/// a -= b;   // a will now be [-4 -3 -2 -1]
/// @endcode
/// @relates Vector
template<int Size>
Vector<Size>& operator-= (Vector<Size>& lhs, const Vector<Size>& rhs);

/// Multiply a vector by a scalar. Both left- and right- multiply are allowed.
/// @code
/// Vector<4> a = 1,2,3,4;
/// Vector<4> b = a * 2.0;  // now b = [2 4 6 8]
/// @endcode
/// @relates Vector
template<int Size>
Vector<Size> operator* (const Vector<Size>& lhs, double rhs);

/// Multiply a vector by a scalar. Both left- and right- multiply are allowed.
/// @code
/// Vector<4> a = 1,2,3,4;
/// Vector<4> b = 3.0 * a;  // now b = [3 6 9 12]
/// @endcode
/// @relates Vector
template<int Size>
Vector<Size> operator* (double lhs, const Vector<Size>& rhs);

/// Multiply a vector by a scalar. Returns the modified vector.
/// @code
/// Vector<4> a = 1,2,3,4;
/// a *= 2.0;     // now a = [2 4 6 8]
/// @endcode
/// @relates Vector
template<int Size>
Vector<Size>& operator*= (const Vector<Size>& lhs, const double& rhs);

/// Divide a vector by a scalar. 
/// @code
/// Vector<4> a = 1,2,3,4;
/// Vector<4> b = a / 2.0;  // now b = [0.5 1.0 1.5 2.0]
/// @endcode
/// @relates Vector
template<int Size>
Vector<Size> operator/ (const Vector<Size>& lhs, double rhs);

/// Divide vector by a scalar. Returns the modified vector.
/// @code
/// Vector<4> a = 1,2,3,4;
/// a /= 2.0;     // now a = [0.5 1.0 1.5 2.0]
/// @endcode
/// @relates Vector
template<int Size>
Vector<Size>& operator/= (const Vector<Size>& lhs, const double& rhs);

/// The dot product of two vectors.
/// @code
/// Vector<4> a = 1,2,3,4;
/// Vector<4> b = 5,6,7,8;
/// double d = a*b;  // now d = 1*5 + 2*6 + 3*7 + 4*8 = 70
/// @endcode
/// @relates Vector
template<int Size>
double operator* (const Vector<Size>& lhs, const Vector<Size>& rhs);

/// The cross product of two 3-vectors. The vectors must be fixed-size.
/// @code
/// Vector<3> a = 1,2,3;
/// Vector<3> b = 5,6,7;
/// double d = a^b;  // now d = [-4 8 4]
/// @endcode
/// @relates Vector
Vector<3> operator^ (const Vector<3>& lhs, const Vector<3>& rhs);

/// Add two matrices. Returns the sum of the two matrices.
/// @relates Matrix
template<int Rows, int Cols>
Matrix<Rows, Cols> operator+(const Matrix<Rows, Cols>& lhs, const Matrix<Rows, Cols>& rhs);

/// Add a matrix to another one. Returns the modified matrix.
/// @relates Matrix
template<int Rows, int Cols>
Matrix<Rows, Cols>& operator+= (Matrix<Rows, Cols>& lhs, const Matrix<Rows, Cols>& rhs);

/// Subtract two matrices. Returns the dfference of the two matrices.
/// @relates Matrix
template<int Rows, int Cols>
Matrix<Rows, Cols> operator-(const Matrix<Rows, Cols>& lhs, const Matrix<Rows, Cols>& rhs);

/// Subtract one matrix from another one. Returns the modified matrix.
/// @relates Matrix
template<int Rows, int Cols>
Matrix<Rows, Cols>& operator-= (Matrix<Rows, Cols>& lhs, const Matrix<Rows, Cols>& rhs);

/// Multiply a matrix by a scalar. Both left- and right- multiply are allowed.
/// @relates Matrix
template<int Rows, int Cols>
Matrix<Rows, Cols> operator* (const Matrix<Rows, Cols>& lhs, double rhs);

/// Multiply a matrix by a scalar. Both left- and right- multiply are allowed.
/// @relates Matrix
template<int rows, int Cols>
Matrix<Rows, Cols> operator* (double lhs, const Matrix<Rows, Cols>& rhs);

/// Multiply a matrix by a scalar. Returns the modified matrix.
/// @relates Matrix
template<int Rows, int Cols>
Matrix<Rows, Cols>& operator*= (const Matrix<Rows, Cols>& lhs, const double& rhs);

/// Divide a matrix by a scalar. 
/// @relates Matrix
template<int Rows, int Cols>
Matrix<Rows, Cols> operator/ (const Matrix<Rows, Cols>& lhs, double rhs);

/// Divide matrix by a scalar. Returns the modified matrix.
/// @relates Matrix
template<int Rows, int Cols>
Matrix<Rows, Cols>& operator/= (const Matrix<Rows, Cols>& lhs, const double& rhs);

/// Multiply a matrix by a vector (with the vector on the right).
/// This interprets the vector as a column vector.
/// @relates Matrix
template<int Rows, int Cols>
Vector<Rows> operator* (const Matrix<Rows, Cols>& lhs, const Vector<Cols>& rhs);

/// Multiply a vector by a matrix (with the vector on the left).
/// This interprets the vector as a row vector.
/// @relates Matrix
template<int Rows, int Cols>
Vector<Cols> operator* (const Vector<Rows>& lhs, const Matrix<Rows, Cols>& rhs);

/// Multiply a matrix by a matrix.
/// @relates Matrix
template<int Rows, int Inter, int Cols>
Matrix<Cols> operator* (const Matrix<Inter, Rows>& lhs, const Matrix<Inter, Cols>& rhs);

/// Multiply two vectors, treating the one vector as representing a diagonal matricx. This
/// has the effect of an element-by-element multiplication of two vectors, i.e. if 
/// \f$a\f$, \f$b\f$ and \f$c\f$ are three (equally-sized) vectors, \f$c_i = a_i b_i\f$.
/// @relates Vector
template<int Size>
Vector<Size> diagmult(const Vector<Size>& lhs, const Vector<Size>& rhs);

/// Multiplication treating the vector as representing a diagonal matrix on the left of the matrix.
/// @relates Matrix
template<int Rows, Cols>
Matrix<Rows, Cols> diagmult(const Vector<Rows>& lhs, const Matrix<Rows, Cols>& rhs);

/// Multiplication treating the vector as representing a diagonal matrix on the right of the matrix.
/// @relates Matrix
template<int Rows, Cols>
Matrix<Rows, Cols> diagmult(const Matrix<Rows, Cols>& lhs, const Vector<Cols>& rhs);


//@}


}

#endif
