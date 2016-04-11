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

#ifndef TOON_FADBAD_INTEGATION_H
#define TOON_FADBAD_INTEGATION_H

#include <iostream>

#include <TooN/TooN.h>
#include <TooN/se3.h>
#include <TooN/se2.h>

#include <FADBAD++/fadiff.h>

namespace fadbad {

template <typename P, unsigned int N>
inline std::ostream & operator<<( std::ostream & out, const F<P, N> & val ){
	return out << val.val();
}

template <typename P, unsigned int N>
inline F<P, N> abs( const F<P, N> & val ){
    return (val.val() < 0) ? -val : val;
}

}

namespace TooN {

template<typename C, unsigned int N> struct IsField<fadbad::F<C, N> >
{
	static const int value = numeric_limits<C>::is_specialized; ///<Is C a field?
};

template <int N, typename T, typename A, unsigned D>
inline Vector<N, T> get_value( const Vector<N, fadbad::F<T, D>, A> & v ){
	Vector<N,T> result(v.size());
	for(int i = 0; i < result.size(); ++i)
		result[i] = v[i].val();
	return result;
}

template <typename P, int N, typename A>
inline Vector<N, fadbad::F<P> > make_fad_vector( const Vector<N, P, A> & val, const unsigned start = 0, const unsigned size = N ){
	Vector<N, fadbad::F<P> > result = val;
	for(unsigned i = 0, d = start; i < val.size() && d < size; ++i, ++d)
		result[i].diff(d,size);
	return result;
}

template <unsigned D, typename P, int N, typename A>
inline Vector<N, fadbad::F<P,D> > make_fad_vector( const Vector<N, P, A> & val, const unsigned start = 0 ){
	Vector<N, fadbad::F<P,D> > result = val;
	for(unsigned i = 0, d = start; i < unsigned(val.size()) && d < D; ++i, ++d)
		result[i].diff(d);
	return result;
}

template <unsigned D, typename P, int N, typename A>
inline Vector<N, P> get_derivative( const Vector<N, fadbad::F<P,D>, A> & val, const int dim ){
	Vector<N, P> r;
	for(int i = 0; i < N; ++i)
		r[i] = val[i].deriv(dim);
	return r;
}

template <unsigned D, typename P, int N, typename A>
inline Matrix<N, D, P> get_jacobian( const Vector<N, fadbad::F<P, D>,  A> & val ){
	Matrix<N, D, P> result(N, val[0].size());
	for(unsigned i = 0; i < val[0].size(); ++i)
		result.T()[i] = get_derivative( val, i );
	return result;
}

template <int R, int C, typename P, unsigned D, typename A>
inline Matrix<R, C, P> get_derivative( const Matrix<R,C, fadbad::F<P, D>, A> & val, const int dim ){
	Matrix<R, C, P> result;
	for(int r = 0; r < R; ++r)
		for(int c = 0; c < C; ++c)
			result[r][c] = val[r][c].deriv(dim);
	return result;
}

template <typename P>
inline SO3<fadbad::F<P> > make_fad_so3(int start = 0, int size = 3){
	// const Vector<3, fadbad::F<double> > p = make_fad_vector(makeVector(0.0, 0, 0), start, size);
	// return SO3<fadbad::F<double> >(p);
	SO3<fadbad::F<P> > r;
	// this is a hack
	Matrix<3,3,fadbad::F<P> > & m = const_cast<Matrix<3,3,fadbad::F<P> > &>(r.get_matrix());
	m(2,1).diff(start, size);
	m(1,2) = m(2,1) * -1;

	m(0,2).diff(start+1, size);
	m(2,0) = m(0,2) * -1;

	m(1,0).diff(start+2, size);
	m(0,1) = m(1,0) * -1;

	return r;
}

template <typename P, unsigned D>
inline SO3<fadbad::F<P, D> > make_fad_so3(int start = 0){
	// const Vector<3, fadbad::F<double> > p = make_fad_vector(makeVector(0.0, 0, 0), start, size);
	// return SO3<fadbad::F<double> >(p);
	SO3<fadbad::F<P, D> > r;
	// this is a hack
	Matrix<3,3,fadbad::F<P, D> > & m = const_cast<Matrix<3,3,fadbad::F<P, D> > &>(r.get_matrix());
	m(2,1).diff(start);
	m(1,2) = m(2,1) * -1;

	m(0,2).diff(start+1);
	m(2,0) = m(0,2) * -1;

	m(1,0).diff(start+2);
	m(0,1) = m(1,0) * -1;

	return r;
}

template <typename P>
inline SE3<fadbad::F<P> > make_fad_se3( int start = 0, int size = 6){
	return SE3<fadbad::F<P> >(make_fad_so3<P>( start+3, size ), make_fad_vector(makeVector<P>(0.0, 0, 0), start, size));
}

template <typename P, unsigned D>
inline SE3<fadbad::F<P, D> > make_fad_se3( int start = 0){
	return SE3<fadbad::F<P, D> >(make_fad_so3<P, D>( start+3 ), make_fad_vector<D>(makeVector<P>(0.0, 0, 0), start));
}

template <typename P>
inline SE2<fadbad::F<P> > make_fad_se2(int start = 0, int size = 3) {
	return SE2<fadbad::F<P> >(make_fad_vector(makeVector<P>(0.0, 0, 0), start, size));
}

template <typename P, unsigned D>
inline SE2<fadbad::F<P, D> > make_fad_se2(int start = 0) {
	return SE2<fadbad::F<P, D> >(make_fad_vector<D>(makeVector<P>(0.0, 0, 0), start));
}

template <typename P>
inline SO2<fadbad::F<P> > make_fad_so2(int start = 0, int size = 1) {
	fadbad::F<P> r = 0;
	r.diff(start,size) = 1;
	return SO2<fadbad::F<P> >(r);
}

template <typename P, unsigned D>
inline SO2<fadbad::F<P, D> > make_fad_so2(int start = 0) {
	fadbad::F<P, D> r = 0;
	r.diff(start) = 1;
	return SO2<fadbad::F<P, D> >(r);
}

template <typename P>
inline SO3<fadbad::F<P> > make_left_fad_so3( const SO3<P> & r, int start = 0, int size = 3 ){
	return make_fad_so3<P>(start, size) * r;
}

template <typename P, unsigned D>
inline SO3<fadbad::F<P, D> > make_left_fad_so3( const SO3<P> & r, int start = 0){
	return make_fad_so3<P, D>(start) * r;
}

template <typename P>
inline SE3<fadbad::F<P> > make_left_fad_se3( const SE3<P> & t,  int start = 0, int size = 6 ){
	return make_fad_se3<P>(start, size) * t;
}

template <typename P, unsigned D>
inline SE3<fadbad::F<P, D> > make_left_fad_se3( const SE3<P> & t,  int start = 0){
	return make_fad_se3<P, D>(start) * t;
}

template <typename P>
inline SO2<fadbad::F<P> > make_left_fad_so2( const SO2<P> & r, int start = 0, int size = 1 ){
	return make_fad_so2<P>(start, size) * r;
}

template <typename P, unsigned D>
inline SO2<fadbad::F<P, D> > make_left_fad_so2( const SO2<P> & r, int start = 0 ){
	return make_fad_so2<P, D>(start) * r;
}

template <typename P>
inline SE2<fadbad::F<P> > make_left_fad_se2( const SE2<P> & t, int start = 0, int size = 3 ){
	return make_fad_se2<P>(start, size) * t;
}

template <typename P, unsigned D>
inline SE2<fadbad::F<P, D> > make_left_fad_se2( const SE2<P> & t, int start = 0){
	return make_fad_se2<P, D>(start) * t;
}

}

#endif // TOON_FADBAD_INTEGATION_H

