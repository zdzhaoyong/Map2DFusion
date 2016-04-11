namespace TooN{
	namespace Internal{
		template<int N, class P> class Data;
	}


	///@internal
	///@brief Object which fills a matrix some data.
	///There is no size known, since the size of the data is known at compile time.
	///Therefore if the size of the matrix is not known, then something deeply strange is
	///going on.
	///@ingroup gInternal
	template<int N, class P> struct Operator<Internal::Data<N, P> >
	{
		P vals[N];

		template<int R, int C, class T, class B>
		void eval(Matrix<R, C, T, B>& m) const
		{
			SizeMismatch<(R==-1?-1:(C==-1?-1:(R*C))), N>:: test(m.num_rows()*m.num_cols(), N);
			for(int r=0, n=0; r < R; r++)
				for(int c=0; c < C; c++, n++)
					m[r][c] = vals[n];
		}

		template<int S, typename P2, typename B>
		void eval(Vector<S, P2, B>& v) const
		{
			SizeMismatch<S, N>::test(v.size(), N);
			for(int i=0; i <N; i++)
				v[i] = vals[i];
		}
	};

	#ifdef DOXYGEN_INCLUDE_ONLY_FOR_DOCS
	
		///Package up the function arguments as some data for filling matrices.
		///Matrices are filled in row major order.
		///For example:
		///@code
		///   double theta = 2;
		///   Matrix<2> rotation = data( cos(theta), sin(theta)
		///                             -sin(theta), cos(theta));
		///@endcode
		///See also TooN::wrapMatrix().
		///@param a The first data element.
		///@ingroup gLinAlg
		inline Operator<Internal::Data<N, double> > Data(double a, ...);

		///Package up the function arguments as some data for filling matrices.
		///Any type can be uses. Matrices are filled in row-major order.
		///@code
		///   Matrix<2,2,float> rotation = data(1f, 2f, 3f, 4f);
		///@endcode
		///See also TooN::wrapMatrix().
		///@param a The first data element.
		///@ingroup gLinAlg
		template<typename Precision> inline Operator<Internal::Data<N, Precision> > Data(const Precision& a, ...);

	#endif

}
