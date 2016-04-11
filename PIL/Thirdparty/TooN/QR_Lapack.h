#ifndef TOON_INCLUDE_QR_LAPACK_H
#define TOON_INCLUDE_QR_LAPACK_H


#include <TooN/TooN.h>
#include <TooN/lapack.h>
#include <utility>

namespace TooN{

/**
Performs %QR decomposition.

@warning this will only work if the number of columns is greater than 
the number of rows!

The QR decomposition operates on a matrix A. It can be performed with
or without column pivoting. In general:
\f[
AP = QR
\f]
Where \f$P\f$ is a permutation matrix constructed to permute the columns
of A. In practise, \f$P\f$ is stored as a vector of integer elements.

With column pivoting, the elements of the leading diagonal of \f$R\f$ will
be sorted from largest in magnitude to smallest in magnitude.

@ingroup gDecomps
*/
template<int Rows=Dynamic, int Cols=Rows, class Precision=double>
class QR_Lapack{

	private:
		static const int square_Size = (Rows>=0 && Cols>=0)?(Rows<Cols?Rows:Cols):Dynamic;

	public:	
		/// Construct the %QR decomposition of a matrix. This initialises the class, and
		/// performs the decomposition immediately.
		/// @param m The matrix to decompose
		/// @param p Whether or not to perform pivoting
		template<int R, int C, class P, class B> 
		QR_Lapack(const Matrix<R,C,P,B>& m, bool p=0)
		:copy(m),tau(square_size()), 
		 Q(square_size(), square_size()), 
		 do_pivoting(p), 
		 pivot(Zeros(m.num_cols()))
		{
			//pivot is set to all zeros, which means all columns are free columns
			//and can take part in column pivoting.

			compute();
		}
		
		///Return R
		const Matrix<Rows, Cols, Precision, ColMajor>& get_R()
		{
			return copy;
		}
		
		///Return Q
		const Matrix<square_Size, square_Size, Precision, ColMajor>& get_Q()
		{
			return Q;
		}	

		///Return the permutation vector. The definition is that column \f$i\f$ of A is
		///column \f$P(i)\f$ of \f$QR\f$.
		const Vector<Cols, int>& get_P()
		{
			return pivot;
		}

	private:

		void compute()
		{	
			FortranInteger M = copy.num_rows();
			FortranInteger N = copy.num_cols();
			
			FortranInteger LWORK=-1;
			FortranInteger INFO;
			FortranInteger lda = M;

			Precision size;
			
			//Set up the pivot vector
			if(do_pivoting)
				pivot = Zeros;
			else
				for(int i=0; i < pivot.size(); i++)
					pivot[i] = i+1;

			
			//Compute the working space
			geqp3_(&M, &N, copy.get_data_ptr(), &lda, pivot.get_data_ptr(), tau.get_data_ptr(), &size, &LWORK, &INFO);

			LWORK = (FortranInteger) size;

			Precision* work = new Precision[LWORK];
			
			geqp3_(&M, &N, copy.get_data_ptr(), &lda, pivot.get_data_ptr(), tau.get_data_ptr(), work, &LWORK, &INFO);


			if(INFO < 0)
				std::cerr << "error in QR, INFO was " << INFO << std::endl;

			//The upper "triangle+" of copy is R
			//The lower right and tau contain enough information to reconstruct Q
			
			//LAPACK provides a handy function to do the reconstruction
			Q = copy.template slice<0,0,square_Size, square_Size>(0,0,square_size(), square_size());
			
			FortranInteger K = square_size();
			M=K;
			N=K;
			lda = K;
			orgqr_(&M, &N, &K, Q.get_data_ptr(), &lda, tau.get_data_ptr(), work, &LWORK, &INFO);

			if(INFO < 0)
				std::cerr << "error in QR, INFO was " << INFO << std::endl;

			delete [] work;
			
			//Now zero out the lower triangle
			for(int r=1; r < square_size(); r++)
				for(int c=0; c<r; c++)
					copy[r][c] = 0;

			//Now fix the pivot matrix.
			//We need to go from FORTRAN to C numbering. 
			for(int i=0; i < pivot.size(); i++)
				pivot[i]--;
		}

		Matrix<Rows, Cols, Precision, ColMajor> copy;
		Vector<square_Size, Precision> tau;
		Matrix<square_Size, square_Size, Precision, ColMajor> Q;
		bool do_pivoting;
		Vector<Cols, FortranInteger> pivot;
		

		int square_size()
		{
			return std::min(copy.num_rows(), copy.num_cols());	
		}
};

}


#endif
