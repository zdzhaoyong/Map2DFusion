// -*- c++ -*-

// Copyright (C) 2009 Georg Klein (gk@robots.ox.ac.uk)
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

#ifndef __GR_SVD_H
#define __GR_SVD_H

#include <TooN/TooN.h>
#include <cmath>
#include <vector>
#include <algorithm>

namespace TooN
{
  
  /**
     @class GR_SVD TooN/GR_SVD.h
     Performs SVD and back substitute to solve equations.
     This code is a c++ translation of the FORTRAN routine give in 
     George E. Forsythe et al, Computer Methods for Mathematical 
     Computations, Prentice-Hall 1977. That code itself is a 
     translation of the ALGOL routine by Golub and Reinsch,
     Num. Math. 14, 403-420, 1970.
  
     N.b. the singular values returned by this routine are not sorted.
     N.b. this also means that even for MxN matrices with M<N, N 
     singular values are computed and used.
     
     The template parameters WANT_U and WANT_V may be set to false to
     indicate that U and/or V are not needed for a minor speed-up.

	 @ingroup gDecomps
  **/
  template<int M, int N = M, class Precision = DefaultPrecision, bool WANT_U = 1, bool WANT_V = 1> 
  class GR_SVD
  {
  public:
    
    template<class Precision2, class Base> GR_SVD(const Matrix<M, N, Precision2, Base> &A);
  
    static const int BigDim = M>N?M:N;
    static const int SmallDim = M<N?M:N;
    
    const Matrix<M,N,Precision>& get_U() { if(!WANT_U) throw(0); return mU;}
    const Matrix<N,N,Precision>& get_V() { if(!WANT_V) throw(0); return mV;}
    const Vector<N, Precision>& get_diagonal() {return vDiagonal;}
    
    Precision get_largest_singular_value();
    Precision get_smallest_singular_value();
    int get_smallest_singular_value_index();
    
    ///Return the pesudo-inverse diagonal. The reciprocal of the diagonal elements
    ///is returned if the elements are well scaled with respect to the largest element,
    ///otherwise 0 is returned.
    ///@param inv_diag Vector in which to return the inverse diagonal.
    ///@param condition Elements must be larger than this factor times the largest diagonal element to be considered well scaled. 
    void get_inv_diag(Vector<N>& inv_diag, const Precision condition)
    {
      Precision dMax = get_largest_singular_value();
      for(int i=0; i<N; ++i)
	inv_diag[i] = (vDiagonal[i] * condition > dMax) ? 
	  static_cast<Precision>(1)/vDiagonal[i] : 0;
    }
  
    /// Calculate result of multiplying the (pseudo-)inverse of M by another matrix. 
    /// For a matrix \f$A\f$, this calculates \f$M^{\dagger}A\f$ by back substitution 
    /// (i.e. without explictly calculating the (pseudo-)inverse). 
    /// See the detailed description for a description of condition variables.
    template <int Rows2, int Cols2, typename P2, typename B2>
    Matrix<N,Cols2, typename Internal::MultiplyType<Precision,P2>::type >
    backsub(const Matrix<Rows2,Cols2,P2,B2>& rhs, const Precision condition=1e9)
    {
      Vector<N,Precision> inv_diag;
      get_inv_diag(inv_diag,condition);
      return (get_V() * diagmult(inv_diag, (get_U().T() * rhs)));
    }

    /// Calculate result of multiplying the (pseudo-)inverse of M by a vector. 
    /// For a vector \f$b\f$, this calculates \f$M^{\dagger}b\f$ by back substitution 
    /// (i.e. without explictly calculating the (pseudo-)inverse). 
    /// See the detailed description for a description of condition variables.
    template <int Size, typename P2, typename B2>
    Vector<N, typename Internal::MultiplyType<Precision,P2>::type >
    backsub(const Vector<Size,P2,B2>& rhs, const Precision condition=1e9)
    {
      Vector<N,Precision> inv_diag;
      get_inv_diag(inv_diag,condition);
      return (get_V() * diagmult(inv_diag, (get_U().T() * rhs)));
    }

    /// Get the pseudo-inverse \f$M^{\dagger}\f$
    Matrix<N,M,Precision> get_pinv(const Precision condition=1e9)
    {
      Vector<N,Precision> inv_diag(N);
      get_inv_diag(inv_diag,condition);
      return diagmult(get_V(),inv_diag) * get_U().T();
    }

    /// Reorder the components so the singular values are in descending order
    void reorder();
    
  protected:
    void Bidiagonalize();
    void Accumulate_RHS();
    void Accumulate_LHS();
    void Diagonalize();
    bool Diagonalize_SubLoop(int k, Precision &z);
  
    Vector<N,Precision> vDiagonal;   
    Vector<BigDim, Precision> vOffDiagonal;     
    Matrix<M, N, Precision> mU;
    Matrix<N, N, Precision> mV;
    
    int nError;
    int nIterations;
    Precision anorm;
  };



  template<int M, int N, class Precision, bool WANT_U, bool WANT_V> 
  template<class Precision2, class Base> 
  GR_SVD<M, N, Precision, WANT_U, WANT_V>::GR_SVD(const Matrix<M, N, Precision2, Base> &mA)
  {
    nError = 0;
    mU = mA; 
    Bidiagonalize();
    Accumulate_RHS();
    Accumulate_LHS();
    Diagonalize();
  };

  template<int M, int N, class Precision, bool WANT_U, bool WANT_V>
  void GR_SVD<M,N,Precision, WANT_U, WANT_V>::Bidiagonalize()
  {
    using std::abs;
    using std::max;
    using std::sqrt;
    // ------------  Householder reduction to bidiagonal form
    Precision g = 0.0;
    Precision scale = 0.0;
    anorm = 0.0;
    for(int i=0; i<N; ++i) // 300
      {
	const int l = i+1; 
	vOffDiagonal[i] = scale * g;
	g = 0.0;
	Precision s = 0.0;
	scale = 0.0;
	if( i < M )
	  {
	    for(int k=i; k<M; ++k)
	      scale += abs(mU[k][i]);
	    if(scale != 0.0)
	      {
		for(int k=i; k<M; ++k)
		  {
		    mU[k][i] /= scale;
		    s += mU[k][i] * mU[k][i];
		  }
		Precision f = mU[i][i];
		g = -(f>=0?sqrt(s):-sqrt(s));
		Precision h = f * g - s;
		mU[i][i] = f - g;
		if(i!=(N-1))
		  {
		    for(int j=l; j<N; ++j)
		      {
			s = 0.0;
			for(int k=i; k<M; ++k)
			  s += mU[k][i] * mU[k][j];
			f = s / h;
			for(int k=i; k<M; ++k)
			  mU[k][j] += f * mU[k][i]; 
		      } // 150
		  }// 190
		for(int k=i; k<M; ++k)
		  mU[k][i] *= scale;
	      } // 210 
	  } // 210
	vDiagonal[i] = scale * g;
	g = 0.0;
	s = 0.0;
	scale = 0.0;
	if(!(i >= M || i == (N-1)))
	  {
	    for(int k=l; k<N; ++k)
	      scale += abs(mU[i][k]);
	    if(scale != 0.0)
	      {
		for(int k=l; k<N; k++)
		  {
		    mU[i][k] /= scale;
		    s += mU[i][k] * mU[i][k];
		  }
		Precision f = mU[i][l];
		g = -(f>=0?sqrt(s):-sqrt(s));
		Precision h = f * g - s;
		mU[i][l] = f - g;
		for(int k=l; k<N; ++k)
		  vOffDiagonal[k] = mU[i][k] / h;
		if(i != (M-1)) // 270
		  {
		    for(int j=l; j<M; ++j)
		      {
			s = 0.0;
			for(int k=l; k<N; ++k)
			  s += mU[j][k] * mU[i][k];
			for(int k=l; k<N; ++k)
			  mU[j][k] += s * vOffDiagonal[k];
		      } // 260
		  } // 270
		for(int k=l; k<N; ++k)
		  mU[i][k] *= scale;
	      } // 290
	  } // 290
	anorm = max(anorm, abs(vDiagonal[i]) + abs(vOffDiagonal[i]));
      } // 300

    // Accumulation of right-hand transformations
  }

  template<int M, int N, class Precision, bool WANT_U, bool WANT_V>
  void GR_SVD<M,N,Precision,WANT_U,WANT_V>::Accumulate_RHS()
  {
    // Get rid of fakey loop over ii, do a loop over i directly
    // This here would happen on the first run of the loop with
    // i = N-1
    mV[N-1][N-1] = static_cast<Precision>(1);
    Precision g = vOffDiagonal[N-1];
  
    // The loop
    for(int i=N-2; i>=0; --i) // 400
      {
	const int l = i + 1;
	if( g!=0) // 360
	  { 
	    for(int j=l; j<N; ++j)
	      mV[j][i] = (mU[i][j] / mU[i][l]) / g; // double division avoids possible underflow
	    for(int j=l; j<N; ++j)
	      { // 350
		Precision s = 0;
		for(int k=l; k<N; ++k)
		  s += mU[i][k] * mV[k][j];
		for(int k=l; k<N; ++k)
		  mV[k][j] += s * mV[k][i];
	      } // 350
	  } // 360
	for(int j=l; j<N; ++j)
	  mV[i][j] = mV[j][i] = 0;
	mV[i][i] = static_cast<Precision>(1);
	g = vOffDiagonal[i];
      } // 400
  }

  template<int M, int N, class Precision, bool WANT_U, bool WANT_V>
  void GR_SVD<M,N,Precision,WANT_U,WANT_V>::Accumulate_LHS()
  {
    // Same thing; remove loop over dummy ii and do straight over i
    // Some implementations start from N here
    for(int i=SmallDim-1; i>=0; --i)
      { // 500
	const int l = i+1;
	Precision g = vDiagonal[i];
	// SqSVD here uses i<N ?
	if(i != (N-1))
	  for(int j=l; j<N; ++j)
	    mU[i][j] = 0.0;
	if(g == 0.0)
	  for(int j=i; j<M; ++j)
	    mU[j][i] = 0.0;
	else
	  { // 475
	    // Can pre-divide g here
	    Precision inv_g = static_cast<Precision>(1) / g;
	    if(i != (SmallDim-1))
	      { // 460
		for(int j=l; j<N; ++j)
		  { // 450
		    Precision s = 0;
		    for(int k=l; k<M; ++k)
		      s += mU[k][i] * mU[k][j];
		    Precision f = (s / mU[i][i]) * inv_g;  // double division
		    for(int k=i; k<M; ++k)
		      mU[k][j] += f * mU[k][i];
		  } // 450
	      } // 460
	    for(int j=i; j<M; ++j)
	      mU[j][i] *= inv_g;
	  } // 475
	mU[i][i] += static_cast<Precision>(1);
      } // 500
  }
  
  template<int M, int N, class Precision,bool WANT_U, bool WANT_V>
  void GR_SVD<M,N,Precision,WANT_U,WANT_V>::Diagonalize()
  {
    // Loop directly over descending k
    for(int k=N-1; k>=0; --k)
      { // 700
	nIterations = 0;
	Precision z;
	bool bConverged_Or_Error = false;
	do
	  bConverged_Or_Error = Diagonalize_SubLoop(k, z);
	while(!bConverged_Or_Error);
	
	if(nError)
	  return;
	
	if(z < 0)
	  {
	    vDiagonal[k] = -z;
	    if(WANT_V)
	      for(int j=0; j<N; ++j)
		mV[j][k] = -mV[j][k];
	  }
      } // 700
  };


  template<int M, int N, class Precision, bool WANT_U, bool WANT_V>
  bool GR_SVD<M,N,Precision,WANT_U, WANT_V>::Diagonalize_SubLoop(int k, Precision &z)
  {
    using std::abs;
    using std::sqrt;
    const int k1 = k-1;
    // 520 is here!
    for(int l=k; l>=0; --l)
      { // 530
	const int l1 = l-1;
	if((abs(vOffDiagonal[l]) + anorm) == anorm)	
		goto line_565;
	if((abs(vDiagonal[l1]) + anorm) == anorm)
		goto line_540;
	continue;

	line_540:
	  {
	    Precision c = 0;
	    Precision s = 1.0;
	    for(int i=l; i<=k; ++i)
	      { // 560
		Precision f = s * vOffDiagonal[i];
		vOffDiagonal[i] *= c;
		if((abs(f) + anorm) == anorm)
		  break; // goto 565, effectively
		Precision g = vDiagonal[i];
		Precision h = sqrt(f * f + g * g); // Other implementations do this bit better
		vDiagonal[i] = h;
		c = g / h;
		s = -f / h;
		if(WANT_U)
		  for(int j=0; j<M; ++j)
		    {
		      Precision y = mU[j][l1];
		      Precision z = mU[j][i];
		      mU[j][l1] = y*c + z*s;
		      mU[j][i] = -y*s + z*c;
		    }
	      } // 560
	  }

	line_565:
	  {
	    // Check for convergence..
	    z = vDiagonal[k];
	    if(l == k)
	      return true; // convergence.
	    if(nIterations == 30)
	      {
		nError = k;
		return true;
	      }
	    ++nIterations;
	    Precision x = vDiagonal[l];
	    Precision y = vDiagonal[k1];
	    Precision g = vOffDiagonal[k1];
	    Precision h = vOffDiagonal[k];
	    Precision f = ((y-z)*(y+z) + (g-h)*(g+h)) / (2.0*h*y);
	    g = sqrt(f*f + 1.0);
	    Precision signed_g =  (f>=0)?g:-g;
	    f = ((x-z)*(x+z) + h*(y/(f + signed_g) - h)) / x;
		  
	    // Next QR transformation
	    Precision c = 1.0;
	    Precision s = 1.0;
	    for(int i1 = l; i1<=k1; ++i1)
	      { // 600
		const int i=i1+1;
		g = vOffDiagonal[i];    
		y = vDiagonal[i];
		h = s*g;
		g = c*g;
		z = sqrt(f*f + h*h);      
		vOffDiagonal[i1] = z;
		c = f/z;	      
		s = h/z;
		f = x*c + g*s;      
		g = -x*s + g*c;
		h = y*s;      
		y *= c;
		if(WANT_V)
		  for(int j=0; j<N; ++j)
		    {
		      Precision xx = mV[j][i1];   
		      Precision zz = mV[j][i];
		      mV[j][i1] = xx*c + zz*s; 
		      mV[j][i] = -xx*s + zz*c;
		    }
		z = sqrt(f*f + h*h);
		vDiagonal[i1] = z;
		if(z!=0)
		  {
		    c = f/z;  
		    s = h/z;
		  }
		f = c*g + s*y;
		x = -s*g + c*y;
		if(WANT_U)
		  for(int j=0; j<M; ++j)
		    {
		      Precision yy = mU[j][i1];   
		      Precision zz = mU[j][i];
		      mU[j][i1] = yy*c + zz*s; 
		      mU[j][i] = -yy*s + zz*c;
		    }
	      } // 600
	    vOffDiagonal[l] = 0;
	    vOffDiagonal[k] = f;
	    vDiagonal[k] = x;
	    return false;
	    // EO IF NOT CONVERGED CHUNK
	  } // EO IF TESTS HOLD
      } // 530
    // Code should never get here!
    throw(0);
    //return false;
  }

  
  template<int M, int N, class Precision, bool WANT_U, bool WANT_V>
  Precision GR_SVD<M,N,Precision,WANT_U,WANT_V>::get_largest_singular_value()
  {
    using std::max;
    Precision d = vDiagonal[0];
    for(int i=1; i<N; ++i) d = max(d, vDiagonal[i]);
    return d;
  }
  
  template<int M, int N, class Precision, bool WANT_U, bool WANT_V>
  Precision GR_SVD<M,N,Precision,WANT_U,WANT_V>::get_smallest_singular_value()
  {
    using std::min;
    Precision d = vDiagonal[0];
    for(int i=1; i<N; ++i) d = min(d, vDiagonal[i]);
    return d;
  }

  template<int M, int N, class Precision, bool WANT_U, bool WANT_V>
  int GR_SVD<M,N,Precision,WANT_U,WANT_V>::get_smallest_singular_value_index()
  {
    using std::min;
    int nMin=0;
    Precision d = vDiagonal[0];
    for(int i=1; i<N; ++i) 
      if(vDiagonal[i] < d)
	{
	  d = vDiagonal[i];
	  nMin = i;
	}
    return nMin;
  }

  template<int M, int N, class Precision, bool WANT_U, bool WANT_V>
  void GR_SVD<M,N,Precision,WANT_U,WANT_V>::reorder()
  {
    std::vector<std::pair<Precision, unsigned int> > vSort;
    vSort.reserve(N);
    for(unsigned int i=0; i<N; ++i)
      vSort.push_back(std::make_pair(-vDiagonal[i], i));
    std::sort(vSort.begin(), vSort.end());
    for(unsigned int i=0; i<N; ++i)
      vDiagonal[i] = -vSort[i].first;
    if(WANT_U)
      {
	Matrix<M, N, Precision> mU_copy = mU;
	for(unsigned int i=0; i<N; ++i)
	  mU.T()[i] = mU_copy.T()[vSort[i].second];
      }
    if(WANT_V)
      {
	Matrix<N, N, Precision> mV_copy = mV;
	for(unsigned int i=0; i<N; ++i)
	  mV.T()[i] = mV_copy.T()[vSort[i].second];
      }
  }

}
#endif






