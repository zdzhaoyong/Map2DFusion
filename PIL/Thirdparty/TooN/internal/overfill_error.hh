namespace TooN{
namespace Internal{

template<bool b> struct overfill;
template<> struct overfill<0>{};

template<int N, int Size> struct CheckOverFill
{
	static void check(int)
	{
		#ifdef TOON_TEST_INTERNALS
			if(N >= Size)
				throw StaticVectorOverfill();
		#else
			Internal::overfill<(N>=Size)> overfilled_vector;
		#endif
	};
};

template<int N> struct CheckOverFill<N, -1>
{
	static void check(int s)
	{
		#ifdef TOON_TEST_INTERNALS
			if(N >= s)
				throw VectorOverfill();
		#elif !defined TOON_NDEBUG_FILL
			if(N >= s)
			{
				std::cerr << "TooN overfilled vector" << std::endl;
				std::abort();
			}
		#endif
	};
};


template<int N, int R, int C, bool IsDynamic=(R==-1||C==-1)> struct CheckMOverFill
{
	static void check(int)
	{
		#ifdef TOON_TEST_INTERNALS
			if(N >= R*C)
				throw StaticMatrixOverfill();
		#else
			Internal::overfill<(N>=R*C)>();
		#endif
	}
};

template<int N, int R, int C> struct CheckMOverFill<N, R, C, 1>
{
	static void check(int s)
	{
		#ifdef TOON_TEST_INTERNALS
			if(N >= s)
				throw StaticMatrixOverfill();
		#else
			if(N >= s)
			{
				std::cerr << "TooN overfilled matrix" << std::endl;
				std::abort();
			}
		#endif
	}
};

}}
