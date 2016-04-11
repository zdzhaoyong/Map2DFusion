#define TOON_TEST_INTERNALS
#include <TooN/TooN.h>
#include <string>

using namespace TooN;
using namespace std;

int lineno;

template<class A, class B> struct no_duplicates
{
	typedef B type;
};


template<class A> struct no_duplicates<A, A>
{
	typedef class IgnoreMe{} type;
};

namespace TooN{namespace Internal{
struct NoError{};
}}

#define TRY lineno = __LINE__; try{

#define EXPECT_CATCH(X, Y) \
catch(no_duplicates<TooN::Internal::X, TooN::Internal::Y>::type e)\
{\
    cerr << "Test FAILED on line " << lineno << " from " << func_lineno << ". Expected " << #X << ", got " << #Y << "." << endl;\
}\

#define EXPECT(X) \
	throw TooN::Internal::NoError();\
}\
catch(TooN::Internal::X e)\
{\
    cerr << "Test OK on line " << lineno << " from " << func_lineno << endl;\
}\
EXPECT_CATCH(X, BadIndex)\
EXPECT_CATCH(X, SliceError)\
EXPECT_CATCH(X, StaticSliceError)\
EXPECT_CATCH(X, SizeMismatch)\
EXPECT_CATCH(X, StaticSizeMismatch)\
EXPECT_CATCH(X, NoError)

#define test_static_static_slices(...) test_static_static_slices_(__LINE__ , __VA_ARGS__)
template<class C> void test_static_static_slices_(int func_lineno, C v)
{
	TRY{
		v.template slice<0, 3>();
	}
	EXPECT(StaticSliceError);

	TRY{
		v.template slice<2, 2>();
	}
	EXPECT(StaticSliceError);

	TRY{
		v.template slice<-1, 1>();
	}
	EXPECT(StaticSliceError);

	TRY{
		v.template slice<0, 2>();
	}
	EXPECT(NoError)

}

#define test_static_dynamic_slices(...) test_static_dynamic_slices_(__LINE__ , __VA_ARGS__)
template<class C> void test_static_dynamic_slices_(int func_lineno, C v)
{
	TRY{
		v.template slice<0, 3>();
	}
	EXPECT(SliceError);

	TRY{
		v.template slice<2, 2>();
	}
	EXPECT(SliceError);

	TRY{
		v.template slice<-1, 1>();
	}
	EXPECT(StaticSliceError);

	TRY{
		v.template slice<0, 2>();
	}
	EXPECT(NoError);

}

#define test_dynamic_slices(...) test_dynamic_slices_(__LINE__, __VA_ARGS__)
template<class C> void test_dynamic_slices_(int func_lineno, C v)
{
	TRY{
		v.slice(0,3);
	}
	EXPECT(SliceError);

	TRY{
		v.slice(2,2);
	}
	EXPECT(SliceError);

	TRY{
		v.slice(-1,1);
	}
	EXPECT(SliceError);

	TRY{
		v.template slice(0, 2);
	}
	EXPECT(NoError);
}


#define test_index(...) test_index_(__LINE__, __VA_ARGS__)
template<class C> void test_index_(int func_lineno, C v)
{
	TRY{
		v[-2];
	}
	EXPECT(BadIndex);

	TRY{
		v[-1];
	}
	EXPECT(BadIndex);

	TRY{
		v[0];
	}
	EXPECT(NoError);

	TRY{
		v[1];
	}
	EXPECT(NoError);

	TRY{
		v[2];
	}
	EXPECT(BadIndex);
}



#define test_comma(...) test_comma_(__LINE__, __VA_ARGS__)
template<class C> void test_comma_(int func_lineno)
{
	TRY{
		Vector<4> v;
		Fill(v) = 1,2,3,4;
	}
	EXPECT(NoError);

	TRY{
		Vector<4> v(4);
		Fill(v) = 1,2,3,4;
	}
	EXPECT(NoError);

	TRY{
		Vector<4> v;
		Fill(v) = 1,2,3,4,5;
	}
	EXPECT(StaticVectorOverfill);

	TRY{
		Vector<> v(4);
		Fill(v) = 1,2,3,4,5;
	}
	EXPECT(VectorOverfill);

	TRY{
		Vector<4> v;
		Fill(v) = 1,2,3;
	}
	EXPECT(Underfill);

	TRY{
		Vector<> v(4);
		Fill(v) = 1,2,3;
	}
	EXPECT(Underfill);
}



int main()
{
	test_static_static_slices(Vector<2>());
	test_dynamic_slices(Vector<2>());
	
	test_static_static_slices(Vector<4>().slice<0,2>());
	test_dynamic_slices(Vector<4>().slice<0,2>());

	test_static_dynamic_slices(Vector<4>().slice(0,2));
	test_dynamic_slices(Vector<4>().slice(0,2));
	
	test_static_dynamic_slices(Vector<>(2));
	test_dynamic_slices(Vector<>(2));

	test_static_static_slices(Vector<>(4).slice<0,2>());
	test_dynamic_slices(Vector<>(4).slice<0,2>());

	test_static_dynamic_slices(Vector<>(4).slice(0,2));
	test_dynamic_slices(Vector<>(4).slice(0,2));

	test_index(Vector<2>());
	test_index(Vector<>(2));
}
