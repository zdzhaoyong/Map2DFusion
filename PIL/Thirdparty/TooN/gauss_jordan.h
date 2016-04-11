// -*- c++ -*-

//    Copyright (C) 2009 Ed Rosten (er258@cam.ac.uk)
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


#ifndef TOON_INC_GAUSS_JORDAN_H
#define TOON_INC_GAUSS_JORDAN_H

#include <utility>
#include <cmath>
#include <TooN/TooN.h>

namespace TooN
{
/// Perform Gauss-Jordan reduction on m
///
/// If m is of the form \f$[A | I ]\f$, then after reduction, m
/// will be \f$[ I | A^{-1}]\f$. There is no restriction on the input, 
/// in that the matrix augmenting A does not need to be I, or square.
/// The reduction is performed using elementary row operations and 
/// partial pivoting.
///
/// @param m The matrix to be reduced.
/// @ingroup gDecomps
template<int R, int C, class Precision, class Base> void gauss_jordan(Matrix<R, C, Precision, Base>& m)
{
	using std::swap;

	//Loop over columns to reduce.
	for(int col=0; col < m.num_rows(); col++)
	{
		//Reduce the current column to a single element


		//Search down the current column in the lower triangle for the largest
		//absolute element (pivot).  Then swap the pivot row, so that the pivot
		//element is on the diagonal. The benchmarks show that it is actually
		//faster to swap whole rows than it is to access the rows via indirection 
		//and swap the indirection element. This holds for both pointer indirection
		//and using a permutation vector over rows.
		{
		  using std::abs;
			int pivotpos = col;
			double pivotval = abs(m[pivotpos][col]);
			for(int p=col+1; p <m.num_rows(); p++)
			  if(abs(m[p][col]) > pivotval)
				{
					pivotpos = p;
					pivotval = abs(m[pivotpos][col]);
				}
			
			if(col != pivotpos)
				swap(m[col].ref(), m[pivotpos].ref());
		}

		//Reduce the current column in every row to zero, excluding elements on
		//the leading diagonal.
		for(int row = 0; row < m.num_rows(); row++)
		{
			if(row != col)
			{
				double multiple = m[row][col] / m[col][col];
		
				//We could eliminate some of the computations in the augmented
				//matrix, if the augmented half is the identity. In general, it
				//is not. 

				//Subtract the pivot row from all other rows, to make 
				//column col zero.
				m[row][col] = 0;
				for(int c=col+1; c < m.num_cols(); c++)
					m[row][c] = m[row][c] - m[col][c] * multiple;
			}
		}
	}
	
	//Final pass to make diagonal elements one. Performing this in a final
	//pass allows us to avoid any significant computations on the left-hand
	//square matrix, since it is diagonal, and ends up as the identity.
	for(int row=0;row < m.num_rows(); row++)
	{
		double mul = 1/m[row][row];

		m[row][row] = 1;

		for(int col=m.num_rows(); col < m.num_cols(); col++)
			m[row][col] *= mul;
	}
}

}
#endif
