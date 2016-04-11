// Generated for J*C*J^T, C symmetric
template <class A1, class A2, class A3> inline void transformCovariance(const FixedMatrix<2,2,A1>& A, const FixedMatrix<2,2,A2>& B, FixedMatrix<2,2,A3>& M)
{
    M = A*B*A.T();
}

// Generated for J*C*J^T, C symmetric
template <int N, class A1, class A2, class A3> inline void transformCovariance(const FixedMatrix<2,N,A1>& A, const FixedMatrix<N,N,A2>& B, FixedMatrix<2,2,A3>& M)
{
	{	const Vector<N> ABi = B * A[0];
		M[0][0] = ABi * A[0];
		M[0][1] = M[1][0] = ABi * A[1];
	}
	M[1][1] = (B * A[1]) * A[1];
}

// Generated for J*C*J^T, C symmetric
template <int N, class A1, class A2, class A3> inline void transformCovariance(const FixedMatrix<3,N,A1>& A, const FixedMatrix<N,N,A2>& B, FixedMatrix<3,3,A3>& M)
{
	{	const Vector<N> ABi = B * A[0];
		M[0][0] = ABi * A[0];
		M[0][1] = M[1][0] = ABi * A[1];
		M[0][2] = M[2][0] = ABi * A[2];
	}
	{	const Vector<N> ABi = B * A[1];
		M[1][1] = ABi * A[1];
		M[1][2] = M[2][1] = ABi * A[2];
	}
	M[2][2] = (B * A[2]) * A[2];
}

#if 0
// Generated for J*C*J^T, C symmetric
template <int N, class A1, class A2, class A3> inline void transformCovariance(const FixedMatrix<6,N,A1>& A, const FixedMatrix<N,N,A2>& B, FixedMatrix<6,6,A3>& M)
{
	{	const Vector<N> ABi = B * A[0];
		M[0][0] = ABi * A[0];
		M[0][1] = M[1][0] = ABi * A[1];
		M[0][2] = M[2][0] = ABi * A[2];
		M[0][3] = M[3][0] = ABi * A[3];
		M[0][4] = M[4][0] = ABi * A[4];
		M[0][5] = M[5][0] = ABi * A[5];
	}
	{	const Vector<N> ABi = B * A[1];
		M[1][1] = ABi * A[1];
		M[1][2] = M[2][1] = ABi * A[2];
		M[1][3] = M[3][1] = ABi * A[3];
		M[1][4] = M[4][1] = ABi * A[4];
		M[1][5] = M[5][1] = ABi * A[5];
	}
	{	const Vector<N> ABi = B * A[2];
		M[2][2] = ABi * A[2];
		M[2][3] = M[3][2] = ABi * A[3];
		M[2][4] = M[4][2] = ABi * A[4];
		M[2][5] = M[5][2] = ABi * A[5];
	}
	{	const Vector<N> ABi = B * A[3];
		M[3][3] = ABi * A[3];
		M[3][4] = M[4][3] = ABi * A[4];
		M[3][5] = M[5][3] = ABi * A[5];
	}
	{	const Vector<N> ABi = B * A[4];
		M[4][4] = ABi * A[4];
		M[4][5] = M[5][4] = ABi * A[5];
	}
	M[5][5] = (B * A[5]) * A[5];
}
#endif
