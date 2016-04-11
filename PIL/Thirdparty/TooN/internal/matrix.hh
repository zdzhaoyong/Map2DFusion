// -*- c++ -*-

// Copyright (C) 2009 Tom Drummond (twd20@cam.ac.uk),
// Ed Rosten (er258@cam.ac.uk)
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

namespace TooN {

/**
A matrix.
Support is provided for all the usual matrix operations: 
- the (a,b) notation can be used to access an element directly
- the [] operator can be used to yield a vector from a matrix (which can be used
as an l-value)
- they can be added and subtracted
- they can be multiplied (on either side) or divided by a scalar on the right:
- they can be multiplied by matrices or vectors
- submatrices can be extracted using the templated slice() member function
- they can be transposed (and the transpose used as an l-value)
- inverse is \e not supported. Use one of the @link gDecomps matrix
decompositions @endlink instead

See individual member function documentation for examples of usage.

\par Statically-sized matrices

The library provides classes for statically and dynamically sized matrices. As
with @link Vector Vectors@endlink, statically sized matrices are more efficient,
since their size is determined at compile-time, not run-time.
To create a \f$3\times4\f$ matrix, use:
@code
Matrix<3,4> M;
@endcode
or replace 3 and 4 with the dimensions of your choice. If the matrix is square,
it can be declared as:
@code
Matrix<3> M;
@endcode
which just is a synonym for <code>Matrix<3,3></code>. Matrices can also be
constructed from pointers or static 1D or 2D arrays of doubles:
@code
  Matrix<2,3, Reference::RowMajor> M2 = Data(1,2,3,4,5,6);
@endcode

\par Dynamically-sized matrices

To create a dynamically sized matrix, use:
@code
Matrix<> M(num_rows, num_cols);
@endcode
where \a num_rows and \a num_cols are integers which will be evaluated at run
time.

Half-dynamic matriced can be constructed in either dimension:
@code
	Matrix<Dynamic, 2> M(num_rows, 2);
@endcode
note that the static dimension must be provided, but it is ignored.

@endcode

<code>Matrix<></code> is a synonym for <code> Matrix<Dynamic, Dynamic> </code>.

\par Row-major and column-major

The library supports both row major (the default - but you can change this if
you prefer) and column major layout ordering. Row major implies that the matrix
is laid out in memory in raster scan order:
\f[\begin{matrix}\text{Row major} & \text {Column major}\\
\begin{bmatrix}1&2&3\\4&5&6\\7&8&9\end{bmatrix} &
\begin{bmatrix}1&4&7\\2&5&8\\3&6&9\end{bmatrix} \end{matrix}\f]
You can override the default for a specific matrix by specifying the layout when
you construct it:
@code
Matrix<3,3,double,ColMajor> M1;
Matrix<Dynamic,Dynamic,double,RowMajor> M2(nrows, ncols);
@endcode
In this case the precision template argument must be given as it precedes the layout argument

@ingroup gLinAlg
**/
template <int Rows=Dynamic, int Cols=Rows, class Precision=DefaultPrecision, class Layout = RowMajor>
struct Matrix : public Layout::template MLayout<Rows, Cols, Precision>
{
public:

	using Layout::template MLayout<Rows, Cols, Precision>::my_data;
	using Layout::template MLayout<Rows, Cols, Precision>::num_rows;
	using Layout::template MLayout<Rows, Cols, Precision>::num_cols;

	//Use Tom's sneaky constructor hack...

	///@name Construction and destruction
	///@{

	///Construction of static matrices. Values are not initialized.
	Matrix(){}
	
	///Construction of dynamic matrices. Values are not initialized.
	Matrix(int rows, int cols) :
		Layout::template MLayout<Rows,Cols,Precision>(rows, cols)
	{}
	
	///Construction of statically sized slice matrices
	Matrix(Precision* p) :
		Layout::template MLayout<Rows, Cols, Precision>(p)
	{}

	///Construction of dynamically sized slice matrices
	Matrix(Precision* p, int r, int c) :
		Layout::template MLayout<Rows, Cols, Precision>(p, r, c)
	{}

	/// Advanced construction of dynamically sized slice matrices.
	/// Internal constructor used by GenericMBase::slice(...).
	Matrix(Precision* data, int rows, int cols, int rowstride, int colstride, Internal::Slicing)
	:Layout::template MLayout<Rows, Cols, Precision>(data, rows, cols, rowstride, colstride){}


	//See vector.hh and allocator.hh for details about why the
	//copy constructor should be default.
	///Construction from an operator.
	template <class Op>
	inline Matrix(const Operator<Op>& op)
		:Layout::template MLayout<Rows,Cols,Precision>(op)
	{
		op.eval(*this);
	}

	/// constructor from arbitrary matrix
	template<int Rows2, int Cols2, typename Precision2, typename Base2>
	inline Matrix(const Matrix<Rows2, Cols2,Precision2,Base2>& from)
	:Layout::template MLayout<Rows,Cols,Precision>(from.num_rows(), from.num_cols())
	{
	    operator=(from);
	}
	///@}

	///@name Assignment
	///@{
	/// operator = from copy
	inline Matrix& operator= (const Matrix& from)
	{
		SizeMismatch<Rows, Rows>::test(num_rows(), from.num_rows());
		SizeMismatch<Cols, Cols>::test(num_cols(), from.num_cols());

	    for(int r=0; r < num_rows(); r++)
	  	  for(int c=0; c < num_cols(); c++)
	  	  	(*this)[r][c] = from[r][c];

	    return *this;
	}

	// operator = 0-ary operator
	template<class Op> inline Matrix& operator= (const Operator<Op>& op)
	{
		op.eval(*this);
		return *this;
	}

	// operator =
	template<int Rows2, int Cols2, typename Precision2, typename Base2>
	Matrix& operator= (const Matrix<Rows2, Cols2, Precision2, Base2>& from)
	{
		SizeMismatch<Rows, Rows2>::test(num_rows(), from.num_rows());
		SizeMismatch<Cols, Cols2>::test(num_cols(), from.num_cols());

	    for(int r=0; r < num_rows(); r++)
	  	  for(int c=0; c < num_cols(); c++)
	  	  	(*this)[r][c] = from[r][c];

	    return *this;
	}
	///@}

	///@name operations on the matrix
	///@{

	Matrix& operator*=(const Precision rhs)
	{
		  for(int r=0; r < num_rows(); r++)
			  for(int c=0; c < num_cols(); c++)
			  	(*this)[r][c] *= rhs;

		  return *this;
	}

	Matrix& operator/=(const Precision rhs)
	{
		  for(int r=0; r < num_rows(); r++)
			  for(int c=0; c < num_cols(); c++)
			  	(*this)[r][c] /= rhs;

		  return *this;
	}

	template<int Rows2, int Cols2, typename Precision2, typename Base2>
	Matrix& operator+= (const Matrix<Rows2, Cols2, Precision2, Base2>& from)
	{
		SizeMismatch<Rows, Rows2>::test(num_rows(), from.num_rows());
		SizeMismatch<Cols, Cols2>::test(num_cols(), from.num_cols());

	    for(int r=0; r < num_rows(); r++)
	  	  for(int c=0; c < num_cols(); c++)
	  	  	(*this)[r][c] += from[r][c];

	    return *this;
	}

	template<class Op>
	Matrix& operator+=(const Operator<Op>& op)
	{
		op.plusequals(*this);
		return *this;
	}

	template<class Op>
	Matrix& operator-=(const Operator<Op>& op)
	{
		op.minusequals(*this);
		return *this;
	}

	template<int Rows2, int Cols2, typename Precision2, typename Base2>
	Matrix& operator-= (const Matrix<Rows2, Cols2, Precision2, Base2>& from)
	{
		SizeMismatch<Rows, Rows2>::test(num_rows(), from.num_rows());
		SizeMismatch<Cols, Cols2>::test(num_cols(), from.num_cols());

	    for(int r=0; r < num_rows(); r++)
	  	  for(int c=0; c < num_cols(); c++)
	  	  	(*this)[r][c] -= from[r][c];

	    return *this;
	}

  	template<int Rows2, int Cols2, typename Precision2, typename Base2>
	bool operator== (const Matrix<Rows2, Cols2, Precision2, Base2>& rhs) const
	{
		SizeMismatch<Rows, Rows2>::test(num_rows(), rhs.num_rows());
		SizeMismatch<Cols, Cols2>::test(num_cols(), rhs.num_cols());

	    for(int r=0; r < num_rows(); r++)
	  	  for(int c=0; c < num_cols(); c++)
		    if((*this)[r][c] != rhs[r][c])
		      return 0;
	    return 1;
	}

  	template<int Rows2, int Cols2, typename Precision2, typename Base2>
	bool operator!= (const Matrix<Rows2, Cols2, Precision2, Base2>& rhs) const
	{
		SizeMismatch<Rows, Rows2>::test(num_rows(), rhs.num_rows());
		SizeMismatch<Cols, Cols2>::test(num_cols(), rhs.num_cols());

	    for(int r=0; r < num_rows(); r++)
	  	  for(int c=0; c < num_cols(); c++)
		    if((*this)[r][c] != rhs[r][c])
		      return 1;
	    return 0;
	}

	template<class Op>
	bool operator!=(const Operator<Op>& op)
	{
		return op.notequal(*this);
	}

	
	///@}
	
	/// @name Misc
	/// @{

	/// return me as a non const reference - useful for temporaries
	Matrix& ref()
	{
		return *this;
	}
	///@}

	#ifdef DOXYGEN_INCLUDE_ONLY_FOR_DOCS
  
		/**
		Access an element from the matrix.
		The index starts at zero, i.e. the top-left element is m(0, 0).
		@code
		Matrix<2,3> m(Data(
			1,2,3
			4,5,6));
		double e = m(1,2);     // now e = 6.0;
		@endcode
		@internal
		This method is not defined by Matrix: it is inherited.
		*/
		const double& operator() (int r, int c) const;

		/**
		Access an element from the matrix.
		@param row_col <code>row_col.first</code> holds the row, <code>row_col.second</code> holds the column.
		@internal
		This method is not defined by Matrix: it is inherited.
		*/
		const double& operator[](const std::pair<int,int>& row_col) const;
		/**
			@overload
		*/
		double& operator[](const std::pair<int,int>& row_col);

		/**
		Access an element from the matrix.
		This can be used as either an r-value or an l-value. The index starts at zero,
		i.e. the top-left element is m(0, 0).
		@code
		Matrix<2,3> m(Data(
			1,2,3
			4,5,6));
		Matrix<2,3> m(d);
		m(1,2) = 8;     // now d = [1 2 3]
					  //         [4 5 8]
		@endcode
		@internal
		This method is not defined by Matrix: it is inherited.
		*/
		double& operator() (int r, int c);

		/**
		Access a row from the matrix.
		This can be used either as an r-value or an l-value. The index starts at zero,
		i.e. the first row is m[0]. To extract a column from a matrix, apply [] to the
		transpose of the matrix (see example). This can be used either as an r-value
		or an l-value. The index starts at zero, i.e. the first row (or column) is
		m[0].
		@code
		Matrix<2,3> m(Data(
			1,2,3
			4,5,6));
		Matrix<2,3> m(d);
		Vector<3> v = m[1];       // now v = [4 5 6];
		Vector<2> v2 = m.T()[0];  // now v2 = [1 4];
		@endcode
		@internal
		This method is not defined by Matrix: it is inherited.
		*/
		const Vector& operator[] (int r) const;

		/**
		Access a row from the matrix.
		This can be used either as an r-value or an l-value. The index starts at zero,
		i.e. the first row is m[0]. To extract a column from a matrix, apply [] to the
		transpose of the matrix (see example). This can be used either as an r-value
		or an l-value. The index starts at zero, i.e. the first row (or column) is
		m[0].
		@code
		Matrix<2,3> m(Data(
			1,2,3
			4,5,6));
		Matrix<2,3> m(d);
		Zero(m[0]);   // set the first row to zero
		Vector<2> v = 8,9;
		m.T()[1] = v; // now m = [0 8 0]
					//         [4 9 6]
		@endcode
		@internal
		This method is not defined by Matrix: it is inherited.
		*/
		Vector& operator[] (int r);

		/// How many rows does this matrix have?
		/// @internal
		/// This method is not defined by Matrix: it is inherited.
		int num_rows() const;

		/// How many columns does this matrix have?
		/// @internal
		/// This method is not defined by Matrix: it is inherited.
		int num_cols() const;

		/// @name Transpose and sub-matrices
		//@{
		/**
		The transpose of the matrix. This is a very fast operation--it simply
		reinterprets a row-major matrix as column-major or vice-versa. This can be
		used as an l-value.
		@code
		Matrix<2,3> m(Data(
			1,2,3
			4,5,6));
		Matrix<2,3> m(d);
		Zero(m[0]);   // set the first row to zero
		Vector<2> v = 8,9;
		m.T()[1] = v; // now m = [0 8 0]
					//         [4 9 6]
		@endcode
		@internal
		This method is not defined by Matrix: it is inherited.
		*/
		const Matrix<Cols, Rows>& T() const;

		/**
		The transpose of the matrix. This is a very fast operation--it simply
		reinterprets a row-major  matrix as column-major or vice-versa. The result can
		be used as an l-value.
		@code
		Matrix<2,3> m(Data(
			1,2,3
			4,5,6));
		Matrix<2,3> m(d);
		Vector<2> v = 8,9;
		// Set the first column to v
		m.T()[0] = v; // now m = [8 2 3]
					//         [9 5 6]
		@endcode
		<b>This means that the semantics of <code>M=M.T()</code> are broken</b>. In
		general, it is not  necessary to say <code>M=M.T()</code>, since you can use
		M.T() for free whenever you need the transpose, but if you do need to, you
		have to use the Tranpose() function defined in <code>helpers.h</code>.
		@internal
		This method is not defined by Matrix: it is inherited.
		*/
		Matrix<Cols, Rows>& T();

		/**
		Extract a sub-matrix. The matrix extracted will be begin at element
		(Rstart, Cstart) and will contain the next Rsize by Csize elements.
		@code
		Matrix<2,3> m(Data(
			1,2,3
			4,5,6
			7,8,9));
		Matrix<3> m(d);
		Extract the top-left 2x2 matrix
		Matrix<2> b = m.slice<0,0,2,2>();  // b = [1 2]
										  //     [4 5]
		@endcode
		@internal
		This method is not defined by Matrix: it is inherited.
		*/
		template<Rstart, Cstart, Rsize, Csize>
		const Matrix<Rsize, Csize>& slice() const;

		/**
		Extract a sub-matrix. The matrix extracted will be begin at element (Rstart,
		Cstart) and will contain the next Rsize by Csize elements. This can be used as
		either an r-value or an l-value.
		@code
		Matrix<2,3> m(Data(
			1,2,3
			4,5,6));
		Matrix<2,3> m(d);
		Zero(m.slice<0,2,2,1>());  // b = [1 2 0]
								  //     [4 5 0]
		@endcode
		@internal
		This method is not defined by Matrix: it is inherited.
		*/
		template<Rstart, Cstart, Rsize, Csize>
		Matrix<Rsize, Csize>& slice();

		/**
		Extract a sub-matrix with runtime location and size. The matrix extracted will
		begin at element (rstart, cstart) and will
		contain the next rsize by csize elements.
		@code
		Matrix<> m(3,3);
		Extract the top-left 2x2 matrix
		Matrix<2> b = m.slice(0,0,2,2);
		@endcode
		@internal
		This method is not defined by Matrix: it is inherited.
		*/
		const Matrix<>& slice(int rstart, int cstart, int rsize, int csize) const;

		/**
		Extract a sub-matrix with runtime location and size, which can be used as
		an l-value. The matrix extracted will be begin at element (rstart, cstart) and
		will contain the next rsize by csize elements.
		@code
		Matrix<> m(3,3);
		Zero(m.slice(0,0,2,2));
		@endcode
		@internal
		This method is not defined by Matrix: it is inherited.
		*/
		Matrix<>& slice(int rstart, int cstart, int rsize, int csize);

		//@}


	#endif
};

}
