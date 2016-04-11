namespace TooN{



namespace Internal
{
	template<int N, int Size, class P, class B> struct VectorFiller
	{
		Vector<Size, P, B>& v;
		VectorFiller<N-1, Size, P, B>* parent;
		bool underfill;

		VectorFiller(Vector<Size, P, B>& v_, VectorFiller<N-1, Size, P, B>* p)
		:v(v_),parent(p),underfill(N<v.size())
		{
		}

		VectorFiller<N+1, Size, P, B> operator,(const P& p)
		{
			Internal::CheckOverFill<N, Size>::check(v.size());
			v[N] = p;
			return VectorFiller<N+1, Size, P, B>(v, this);
		}

		~VectorFiller()
		{
			#ifndef TOON_NDEBUG_FILL
				if(underfill)
				{
					#ifdef TOON_TEST_INTERNALS
						throw Internal::Underfill();
					#else
						std::cerr << "TooN: underfilled vector\n";
						std::abort();
					#endif
				}
				else if(parent)
					parent->underfill = 0;
			#endif
		}
	};

	template<int Size, class P, class B> struct VectorStartFill
	{
		Vector<Size, P, B>& v;
		VectorStartFill(Vector<Size, P, B> & v_)
		:v(v_){}

		VectorFiller<1, Size, P, B> operator=(const P& p)
		{
			Internal::CheckOverFill<1, Size>::check(v.size());
			v[0] = p;
			return VectorFiller<1, Size, P, B>(v, 0);
		}
	};


	template<int N, int R, int C, class P, class B> struct MatrixFiller
	{
		Matrix<R, C, P, B>& m;
		MatrixFiller<N-1, R, C, P, B>* parent;
		int r, c;
		bool underfill;

		MatrixFiller(Matrix<R, C, P, B>& m_, MatrixFiller<N-1, R, C, P, B>*p, int r_, int c_)
		:m(m_),parent(p),r(r_),c(c_),underfill(r < m.num_rows())
		{}

		MatrixFiller<N+1, R, C, P, B> operator,(const P& p)
		{
			Internal::CheckMOverFill<N, R, C>::check(m.num_rows() * m.num_cols());
			m[r][c] = p;
			c++;
			if(c == m.num_cols())
			{
				c=0;
				r++;
			}		

			return MatrixFiller<N+1, R, C, P, B>(m, this, r, c);
		}

		~MatrixFiller()
		{
			#ifndef TOON_NDEBUG_FILL
				if(underfill)
				{
					#ifdef TOON_TEST_INTERNALS
						throw Internal::Underfill();
					#else
						std::cerr << "TooN: underfilled matrix\n";
						std::abort();
					#endif
				}
				else if(parent)
					parent->underfill = 0;
			#endif
		}
	};

	template<int R, int C, class P, class B> struct MatrixStartFill
	{
		Matrix<R, C, P, B>& m;
		MatrixStartFill(Matrix<R, C, P, B> & m_)
		:m(m_){}

		MatrixFiller<1, R, C, P, B> operator=(const P& p)
		{
			Internal::CheckMOverFill<0, R, C>::check(m.num_rows() * m.num_cols());
			m[0][0] = p;
			return MatrixFiller<1, R, C, P, B>(m, 0, 0, 1);
		}
	};

}

/**Set up a matrix for filling. Uses the following syntax:
@code
	Matrix<2,2> m;
	Fill(m) = 1, 2,
	          3, 4;
@endcode
Overfill is detected at compile time if possible, underfill
is detected at run-time. The checks can not be optimized out
for dynamic matrices, so define \c TOON_NDEBUG_FILL to prevent
the checks from being used.
@param m Matrix to fill
@ingroup gLinAlg
*/
template<int R, int C, class Precision, class Base> Internal::MatrixStartFill<R, C, Precision, Base> Fill(Matrix<R, C, Precision, Base>& m)
{
	return m;
}

/**Set up a vector for filling. Uses the following syntax:
@code
	Vector<2> v;
	Fill(v) = 1, 2;
@endcode
Overfill is detected at compile time if possible, underfill
is detected at run-time. The checks can not be optimized out
for dynamic vectors, so define \c TOON_NDEBUG_FILL to prevent
the checks from being used.
@param v Vector to fill
@ingroup gLinAlg
*/
template<int Size, class Precision, class Base> Internal::VectorStartFill<Size, Precision, Base> Fill(Vector<Size, Precision, Base>& v)
{
	return v;
}

}
