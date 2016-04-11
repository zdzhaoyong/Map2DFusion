/*
    Copyright (c) 2005 Paul Smith, 2009, 2010, 2011, 2012 Edward Rosten

	Permission is granted to copy, distribute and/or modify this document under
	the terms of the GNU Free Documentation License, Version 1.2 or any later
	version published by the Free Software Foundation; with no Invariant
	Sections, no Front-Cover Texts, and no Back-Cover Texts.

    You should have received a copy of the GNU Free Documentation License
    License along with this library; if not, write to the Free Software
    Foundation, Inc.
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

*/
///////////////////////////////////////////////////////
// General Doxygen documentation
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// The main title page
/**
@mainpage

\section sIntro Introduction

The %TooN library is a set of C++ header files which provide basic numerics facilities:
	- @link TooN::Vector Vectors@endlink, @link TooN::Matrix matrices@endlink and @link gLinAlg etc @endlink
	- @link gDecomps Matrix decompositions@endlink
	- @link gOptimize Function optimization@endlink
	- @link gTransforms Parameterized matrices (eg transformations)@endlink 
	- @link gEquations linear equations@endlink
	- @link gFunctions Functions (eg automatic differentiation and numerical derivatives) @endlink

It provides classes for statically- (known at compile time) and dynamically-
(unknown at compile time) sized vectors and matrices and it can delegate
advanced functions (like large SVD or multiplication of large matrices) to
LAPACK and BLAS (this means you will need libblas and liblapack).

The library makes substantial internal use of templates to achieve run-time
speed efficiency whilst retaining a clear programming syntax.

Why use this library?
 - Because it supports statically sized vectors and matrices very efficiently.
 - Because it provides extensive type safety for statically sized vectors and matrices (you can't attempt to multiply a 3x4 matrix and a 2-vector).
 - Because it supports transposition, subscripting and slicing of matrices (to obtain a vector) very efficiently.
 - Because it interfaces well to other libraries.
 - Because it exploits LAPACK and BLAS (for which optimised versions exist on many platforms).
 - Because it is fast, \link sCramerIsBad but not at the expense of numerical stability. \endlink

\section sDesign Design philosophy of TooN

- TooN is designed to represent mathematics as closely as possible.

- TooN is a linear algebra library.
  - TooN is designed as a linear algebra library and not a generic container
	and array mathematics library. 
	
- Vectors are not matrices.
  - The Vector and Matrix objects are distinct. Vectors and matrices are closely
	related, but distinct objects which makes things like outer versus inner
	product clearer, removes ambiguity and special cases and generally makes the code
	shorter.

- TooN generally doesn't allow things which don't make much sense.
  - Why would you want to multiply or add Zeros?

- A vector is always a Vector and a matrix is always a Matrix
  - Both concrete and generic functions take variations on the Vector and Matrix class,
    no matter where the data comes from.  You will never see anything like a BaseVector.



\section sUsage How to use TooN
This section is arranged as a FAQ. Most answers include code fragments. Assume
<code>using namespace TooN;</code>.

 - \ref sDownload
 - \ref sStart
 - \ref sWindowsErrors
 - \ref sCreateVector
 - \ref sCreateMatrix
 - \ref sFunctionVector
 - \ref sGenericCode
 - \ref sConst
 - \ref sElemOps
 - \ref sInitialize
 - \ref sScalars
 - \ref ssExamples
 - \ref sSTL
 - \ref sResize
 - \ref sDebug
 - \ref sSlices
 - \ref sFuncSlices
 - \ref sPrecision
 - \ref sAutomaticDifferentiation
 - \ref sSolveLinear
 - \ref sOtherStuff
 - \ref sHandyFuncs
 - \ref sNoInplace
 - \ref sColMajor
 - \ref sWrap
 - \ref sWrap "How do I interface to other libraries?"
 - \ref sCpp11
 - \ref sImplementation

 	\subsection sDownload Getting the code and installing
	
	To get the code from cvs use:

	cvs -z3 -d:pserver:anoncvs@cvs.savannah.nongnu.org:/cvsroot/toon co TooN

	The home page for the library with a version of this documentation is at:

	http://mi.eng.cam.ac.uk/~er258/cvd/toon.html

	The code will work as-is, and comes with a default configuration, which
	should work on any system.

	On a unix system, <code>./configure && make install </code> will  install
	TooN to the correct place.  Note there is no code to be compiled, but the
	configure script performs some basic checks.

	On non-unix systems, e.g. Windows and embedded systems, you may wish to 
	configure the library manually. See \ref sManualConfiguration.

	\subsection sStart Getting started

		To begin, just in include the right file:

		@code
		#include <TooN/TooN.h>
		@endcode

		Everything lives in the <code>TooN</code> namespace.

		Then, make sure the directory containing TooN is in your compiler's
		search path. If you use any decompositions, you will need to link
		against LAPACK, BLAS and any required support libraries. On a modern
		unix system, linking against LAPACK will do this automatically.

	\subsection sWindowsErrors Comilation errors on Win32 involving TOON_TYPEOF
	
		If you get errors compiling code that uses TooN, look for the macro TOON_TYPEOF 
		in the messages. Most likely the file <code>internal/config.hh</code> is clobbered. 
		Open it and remove all the defines present there. 
		
		Also see @ref sManualConfiguration for more details on configuring TooN, 
		and @ref sConfigLapack, if you want to use LAPACK and BLAS. Define the macro
		in <code>internal/config.hh</code>.

	\subsection sCreateVector How do I create a vector?

		Vectors can be statically sized or dynamically sized.

		@code
			Vector<3> v1;    //Create a static sized vector of size 3
			Vector<>  v2(4); //Create a dynamically sized vector of size 4
			Vector<Dynamic>  v2(4); //Create a dynamically sized vector of size 4
		@endcode

		See also \ref sPrecision.


	\subsection sCreateMatrix How do I create a matrix?

		Matrices can be statically sized or dynamically sized.

		@code
			Matrix<3> m;              //A 3x3 matrix (statically sized)
			Matrix<3,2>  m;           //A 3x2 matrix (statically sized)
			Matrix<>  m(5,6);         //A 5x6 matrix (dynamically sized)
			Matrix<3,Dynamic> m(3,6); //A 3x6 matrix with a dynamic number of columns and static number of rows.
			Matrix<Dynamic,2> m(3,2); //A 2x3 matrix with a dynamic number of rows and static number of columns.
		@endcode

		See also \ref sPrecision.


	\subsection sFunctionVector How do I write a function taking a vector?
	
		To write a function taking a local copy of a vector:
		@code
			template<int Size> void func(Vector<Size> v);
		@endcode

		To write a function taking any type of vector by reference:
		@code
		template<int Size, typename Precision, typename Base> void func(const Vector<Size, Precision, Base>& v);
		@endcode
		See also \ref sPrecision, \ref sGenericCode and \ref sNoInplace


		Slices are strange types. If you want to write a function which
		uniformly accepts <code>const</code> whole objects as well as slices,
		you need to template on the precision.

		Note that constness in C++ is tricky (see \ref sConst). If you write
		the function to accept <code> Vector<3, double, B>& </code>, then you
		will not be able to pass it slices from <code> const Vector</code>s.
		If, however you write it to accept <code> Vector<3, const double, B>&
		</code>, then the only way to pass in a <code>Vector<3></code> is to
		use the <code>.as_slice()</code> method.

		See also \ref sGenericCode

	\subsection sConst What is wrong with constness?

		In TooN, the behaviour of a Vector or Matrix is controlled by the third
		template parameter. With one parameter, it owns the data, with another
		parameter, it is a slice. A static sized object uses the variable:
		@code
			 double my_data[3];
		@endcode
		to hold the data. A slice object uses:
		@code
			 double* my_data;
		@endcode
		When a Vector is made <code>const</code>, C++ inserts <code>const</code> in
		to those types.  The <code>const</code> it inserts it top level, so these
		become (respectively):
		@code
			 const double my_data[3];
			 double * const my_data;
		@endcode
		Now the types behave very differently. In the first case
		<code>my_data[0]</code> is immutable. In the second case,
		<code>my_data</code> is immutable, but
		<code>my_data[0]</code> is mutable.
		
		Therefore a slice <code>const Vector</code> behaves like an immutable
		pointer to mutable data. TooN attempts to make <code>const</code>
		objects behave as much like pointers to \e immutable data as possible.

		The semantics that TooN tries to enforce can be bypassed with 
		sufficient steps:
		@code
			//Make v look immutable
			template<class P, class B> void fake_immutable(const Vector<2, P, B>& v)
			{
				Vector<2, P, B> nonconst_v(v);
				nonconst_v[0] = 0; //Effectively mutate v
			}

			void bar()
			{
				Vector<3> v;
				...
				fake_immutable(v.slice<0,2>());
				//Now v is mutated
			}

		@endcode

		See also \ref sFunctionVector



	\subsection sElemOps What elementary operations are supported?
		
		Assignments are performed using <code>=</code>. See also 
		\ref sNoResize.

		These operators apply to vectors or matrices and scalars. 
		The operator is applied to every element with the scalar.
		@code
		*=, /=, *, / 
		@endcode
		
		Vector and vectors or matrices and matrices:
		@code
		+, -, +=, -= 
		@endcode
		
		Dot product:
		@code
		Vector * Vector
		@endcode

		Matrix multiply:
		@code
		Matrix * Matrix
		@endcode

		Matrix multiplying a column vector:
		@code
		Matrix * Vector
		@endcode

		Row vector multiplying a matrix:
		@code
		Vector * Matrix
		@endcode
		
		3x3 Vector cross product:
		@code
		Vector<3> ^ Vector<3> 
		@endcode

		All the functions listed below return slices. The slices 
		are simply references to the original data and can be used as lvalues.

		Getting the transpose of a matrix:
		@code
			Matrix.T()
		@endcode
		
		Accessing elements:
		@code
		Vector[i]     //get element i
		Matrix(i,j)   //get element i,j
		Matrix[i]     //get row i as a vector
		Matrix[i][j]  //get element i,j
		@endcode
		
		Turning vectors in to matrices:
		@code
		Vector.as_row() //vector as a 1xN matrix
		Vector.as_col() //vector as a Nx1 matrix
		@endcode

		Slicing with a start position and size:
		
		@code
		Vector.slice<Start, Length>();                         //Static slice
		Vector.slice(start, length);                           //Dynamic slice
		Matrix.slice<RowStart, ColStart, NumRows, NumCols>();  //Static slice
		Matrix.slice(rowstart, colstart, numrows, numcols);    //Dynamic slice
		@endcode
		
		Slicing diagonals:
		@code
		Matrix.diagonal_slice();                               //Get the leading diagonal as a vector.
		Vector.as_diagonal();                                  //Represent a Vector as a DiagonalMatrix
		@endcode
		
		Like other features of TooN, mixed static/dynamic slicing is allowed.
		For example:

		@code
		Vector.slice<Dynamic, 2>(3, 2);   //Slice starting at index 3, of length 2.
		@endcode

		See also \ref sSlices

	\subsection sInitialize How I initialize a vector/matrix?

		Vectors and matrices start off uninitialized (filled with random garbage).
		They can be easily filled with zeros, or ones (see also TooN::Ones):
		@code
			Vector<3> v = Zeros;
			Matrix<3> m = Zeros
			Vector<>  v2 = Zeros(2); //Note in they dynamic case, the size must be specified
			Matrix<>  m2 = Zeros(2,2); //Note in they dynamic case, the size must be specified
		@endcode

		Vectors can be filled with makeVector:
		@code
			Vector<> v = makeVector(2,3,4,5,6);
		@endcode
		
		Matrices can be initialized to the identity matrix:
		@code
			Matrix<2> m = Idendity;
			Matrix<> m2 = Identity(3);
		@endcode
		note that you need to specify the size in the dynamic case.

		Matrices can be filled from data in row-major order:
		@code
			Matrix<3> m = Data(1, 2, 3, 
			                   4, 5, 6, 
							   7, 8, 9);
		@endcode

		A less general, but visually more pleasing syntax can also be used:
		@code
			Vector<5> v;
			Fill(v) = 1,2,3,4,5; 

			Matrix<3,3> m;
			Fill(m) = 1, 2, 3, 
			          4, 5, 6, 
					  7, 8, 9;
		@endcode
		Note that underfilling is a run-time check, since it can not be detected
		at compile time.

		They can also be initialized with data from another source. See also \ref  sWrap.





	\subsection sScalars How do I add a scalar to every element of a vector/matrix? 
		
		Addition to every element is not an elementary operation in the same way
		as multiplication by a scalar. It is supported throught the ::Ones
		object:
		
		@code
			Vector<3> a, b;
			...
			b = a + Ones*3;       // b_i = a_i + 3
			a+= Ones * 3;         // a_i <- a_i + 3
		@endcode

		It is supported the same way on Matrix and slices.

	\subsection sNoResize Why does assigning mismatched dynamic vectors fail?
	
	Vectors are not generic containers, and dynamic vectors have been designed
	to have the same semantics as static vectors where possible. Therefore
	trying to assign a vector of length 2 to a vector of length 3 is an error,
	so it fails. See also \ref sResize

	\subsection sSTL How do I store Dynamic vectors in STL containers.

	As C++ does not yet support move semantics, you can only safely store
	static and resizable Vectors in STL containers.

	\subsection sResize How do I resize a dynamic vector/matrix?

	Do you really want to? If you do, then you have to declare it:

	@code
	     Vector<Resizable> v;
		 v.resize(3);
		 v = makeVector(1, 2, 3);

		 v = makeVector(1, 2); //resize
		 v = Ones(5); //resize
		 v = Zeros; // no resize
	@endcode

	The policy behind the design of TooN is that it is a linear algebra
	library, not a generic container library, so resizable Vectors are only
	created on request. They provide fewer guarantees than other vectors, so
	errors are likely to be more subtle and harder to track down.  One of the
	main purposes is to be able to store Dynamic vectors of various sizes in
	STL containers.

	Assigning vectors of mismatched sizes will cause an automatic resize. Likewise
	assigning from entities like Ones with a size specified will cause a resize.
	Assigning from an entities like Ones with no size specified will not cause
	a resize.

	They can also be resized with an explicit call to .resize().
	Resizing is efficient since it is implemented internally with
	<code>std::vector</code>.  Note that upon resize, existing data elements
	are retained but new data elements are uninitialized.

	Currently, resizable matrices are unimplemented.  If you want a resizable
	matrix, you may consider using a <code>std::vector</code>, and accessing it
	as a TooN object when appropriate. See \ref sWrap. Also, the speed and
	complexity of resizable matrices depends on the memory layout, so you may
	wish to use column major matrices as opposed to the default row major
	layout.

	\subsection sDebug What debugging options are there?

	By default, everything which is checked at compile time in the static case
	is checked at run-time in the dynamic case (with some additions). Checks can
	be disabled with various macros. Note that the optimizer will usually
	remove run-time checks on static objects if the test passes.
	
	Bounds are not checked by default. Bounds checking can be enabled by
	defining the macro \c TOON_CHECK_BOUNDS. None of these macros change the
	interface, so debugging code can be freely mixed with optimized code.

	The debugging checks can be disabled by defining either of the following macros:
		- \c TOON_NDEBUG
		- \c NDEBUG 

	Additionally, individual checks can be disabled with the following macros:
		- Static/Dynamic mismatch
			- Statically determined functions accept and ignore dynamically specified
			  sizes. Nevertheless, it is an error if they do not match.
			- Disable with \c TOON_NDEBUG_MISMATCH
		- Slices
			- Disable with \c TOON_NDEBUG_SLICE
		- Size checks (for assignment)
			- Disable with \c TOON_NDEBUG_SIZE
		- overfilling using Fill 
			- Disable with \c TOON_NDEBUG_FILL
		- underfilling using Fill (run-time check)
			- Disable with \c TOON_NDEBUG_FILL
	



	Errors are manifested to a call to <code>std::abort()</code>.

	TooN does not initialize data in a Vector or Matrix.  For debugging purposes
	the following macros can be defined:
	- \c TOON_INITIALIZE_QNAN or \c TOON_INITIALIZE_NAN Sets every element of newly defined Vectors or
	  Matrixs to quiet NaN, if it exists, and 0 otherwise. Your code will not compile
	  if you have made a Vector or Matrix of a type which cannot be constructed
	  from a number.
	- \c TOON_INITIALIZE_SNAN Sets every element of newly defined Vectors or
	  Matrixs to signalling NaN, if it exists, and 0 otherwise. 
	- \c TOON_INITIALIZE_VAL Sets every element of newly defined Vectors or
	  Matrixs to the expansion of this macro.
	- \c TOON_INITIALIZE_RANDOM Fills up newly defined Vectors and Matrixs with
	  random bytes, to trigger non repeatable behaviour. The random number
	  generator is automatically seeded with a granularity of 1 second. Your
	  code will not compile if you have a Vector or Matrix of a non-POD type.

	\subsection sSlices What are slices?

	Slices are references to data belonging to another vector or matrix. Modifying
	the data in a slice modifies the original object. Likewise, if the original 
	object changes, the change will be reflected in the slice. Slices can be
	used as lvalues. For example:

	@code
		Matrix<3> m = Identity;

		m.slice<0,0,2,2>() *= 3; //Multiply the top-left 2x2 submatrix of m by 3.

		m[2] /=10; //Divide the third row of M by 10.

		m.T()[2] +=2; //Add 2 to every element of the second column of M.

		m[1].slice<1,2>() = makeVector(3,4); //Set m_1,1 to 3 and m_1,2 to 4
		
		m[0][0]=6;
	@endcode

	Slices are usually strange types. See \ref sFunctionVector

	See also \sFuncSlices

	\subsection sPrecision Can I have a precision other than double?

	Yes!
	@code
		Vector<3, float> v;          //Static sized vector of floats
		Vector<Dynamic, float> v(4); //Dynamic sized vector of floats
		Vector<Dynamic, std::complex<double> > v(4); //Dynamic sized vector of complex numbers
	@endcode

	Likewise for matrix. By default, TooN supports all builtin types
	and std::complex. Using custom types requires some work. If the 
	custom type understands +,-,*,/ with builtin types, then specialize
	TooN::IsField on the types.

	If the type only understands +,-,*,/ with itself, then specialize
	TooN::Field on the type.

	Note that this is required so that TooN can follow the C++ promotion 
	rules. The result of multiplying a <code>Matrix<double></code> by a 
	<code>Vector<float></code> is a <code>Vector<double></code>.


	\subsection sFuncSlices How do I return a slice from a function?

	
	If you are using C++11, returning slices is now easy:
	@code
		auto sliceof(Vector<4>& v)->decltype (v.slice<1,2>())
		{
			return v.slice<1,2>();
		}
	@endcode

	If not, some tricks are required.
	Each vector has a <code>SliceBase</code> type indicating the type of a slice.

	They can be slightly tricky to use:
	@code
		Vector<2, double, Vector<4>::SliceBase> sliceof(Vector<4>& v)
		{
			return v.slice<1,2>();
		}

		template<int S, class P, class B>
		Vector<2, P, Vector<S, P, B>::SliceBase> sliceof(Vector<S, P, B>& v)
		{
			return v.template slice<1,2>();
		}

		template<int S, class P, class B>
		const Vector<2, const P, typename Vector<S, P, B>::ConstSliceBase > foo(const Vector<S, P, B>& v)
		{
			return v.template slice<1,2>();
		}

	@endcode


	\subsection sSolveLinear How do I invert a matrix / solve linear equations?
	
	You use the decomposition objects (see \ref sDecompos "below"), for example to solve Ax=b:

	@code
	Matrix<3> A;
	A[0]=makeVector(1,2,3);
	A[1]=makeVector(3,2,1);
	A[2]=makeVector(1,0,1);

	Vector<3> b = makeVector (2,3,4);

	// solve Ax=b using LU
	LU<3> luA(A);
	Vector<3> x1 = luA.backsub(b);

	// solve Ax=b using SVD
	SVD<3> svdA(A);
	Vector<3> x2 = svdA.backsub(b);
	@endcode
	
	Similarly for the other \ref sDecompos "decomposition objects"

	For 2x2 matrices, the TooN::inv function can be used.

	\subsection sDecompos  Which decomposisions are there?

	For general size matrices (not necessarily square) there are:
	@link TooN::LU LU @endlink, @link TooN::SVD SVD @endlink, @link TooN::QR QR@endlink, @link TooN::QR_Lapack LAPACK's QR@endlink and gauss_jordan()

	For square symmetric matrices there are:
	@link TooN::SymEigen SymEigen @endlink and @link TooN::Cholesky Cholesky @endlink

	If all you want to do is solve a single Ax=b then you may want gaussian_elimination()

	\subsection sOtherStuff What other stuff is there:
	
	Look at the @link modules modules @endlink.

	\subsection sHandyFuncs What handy functions are there (normalize, identity, fill, etc...)?

	See @link gLinAlg here @endlink.

	\subsection sAutomaticDifferentiation Does TooN support automatic differentiation?
	
	TooN has buildin support for <a href="http://www.fadbad.com/fadbad.html">FADBAD++</a>.
	Just do:
	@code
		#include <functions/fadbad.h>
	@endcode
	Then create matrices and vectors of FADBAD types. See functions/fadbad.h
	for available functions and parameterisations.

	TooN is type generic and so can work on any reasonable types including AD types
	if a small amount of interfacing is performed.
	See \sPrecision.



	\subsection sNoInplace Why don't functions work in place?

	Consider the function:
	@code
		void func(Vector<3>& v);
	@endcode
	It can accept a <code>Vector<3></code> by reference, and operate on it 
	in place. A <code>Vector<3></code> is a type which allocates memory on the
	stack. A slice merely references memory, and is a subtly different type. To
	write a function taking any kind of vector (including slices) you can write:

	@code
		template<class Base> void func(Vector<3, double, Base>& v);
	@endcode

	A slice is a
	temporary object, and according to the rules of C++, you can't pass a
	temporary to a function as a non-const reference. TooN provides the
	<code>.ref()</code> method to escape from this restriction, by returning a
	reference as a non-temporary. You would then have to write:
	@code
		Vector<4> v;
		...
		func(v.slice<0,3>().ref());
	@endcode
	to get func to accept the slice.

	You may also wish to consider writing functions that do not modify structures in
	place. The \c unit function of TooN computes a unit vector given an input
	vector. In the following context, the code:
	@code
		//There is some Vector, which may be a slice, etc called v;
		v = unit(v);
	@endcode
	produces exactly the same compiler output as the hypothetical
	<code>Normalize(v)</code> which operates in place (for static vectors). Consult the ChangeLog 
	entries dated ``Wed 25 Mar, 2009 20:18:16'' and ``Wed  1 Apr, 2009 16:48:45''
	for further discussion.
	

	\subsection sColMajor Can I have a column major matrix?

	Yes!
	@code
		Matrix<3, 3, double, ColMajor> m;          //3x3 Column major matrix
	@endcode

	\subsection sWrap I have a pointer to a bunch of data. How do I turn it in to a vector/matrix without copying?

	To create a vector use:
	@code
	double d[]={1,2,3,4};
	Vector<4,double,Reference> v1(d);
	Vector<Dynamic,double,Reference> v2(d,4);
	@endcode
	Or, a functional form can be used:
	@code
	double d[]={1,2,3,4};

	wrapVector<4>(d);         //Returns a Vector<4>
	wrapVector<4,double>(d);  //Returns a Vector<4>
	
	wrapVector(d,3);          //Return a Vector<Dynamic> of size 3
	wrapVector<Double>(d,3);  //Return a Vector<Dynamic> of size 3
	@endcode

	To crate a matrix use
	@code
	double d[]={1,2,3,4,5,6};
	Matrix<2,3,double,Reference::RowMajor> m1(d);
	Matrix<2,3,double,Reference::ColMajor> m2(d);
	Matrix<Dynamic, Dynamic, double, Reference::RowMajor> m3(d, 2, 3);
	Matrix<Dynamic, 3, double, Reference::RowMajor> m4(d, 2, 3); // note two size arguments are required for semi-dynamic matrices
	@endcode

	See also wrapVector() and wrapMatrix().

	\subsection sGenericCode How do I write generic code?
	
	The constructors for TooN objects are very permissive in that they 
	accept run-time size arguments for statically sized objects, and then 
	discard the values, This allows you to easily write generic code which 
	works for both static and dynamic inputs.

	Here is a function which mixes up a vector with a random matrix:
	@code
	template<int Size, class Precision, class Base> Vector<Size, Precision> mixup(const Vector<Size, Precision, Base>& v)
	{
		//Create a square matrix, of the same size as v. If v is of dynamic
		//size, then Size == Dynamic, and so Matrix will also be dynamic. In
		//this case, TooN will use the constructor arguments to select the
		//matrix size. If Size is a real size, then TooN will simply ighore
		//the constructor values.

		Matrix<Size, Size, Precision> m(v.size(), v.size());
		
		//Fill the matrix with random values that sum up to 1.
		Precision sum=0;
		for(int i=0; i < v.size(); i++)
			for(int j=0; j < v.size(); j++)
				sum += (m[i][j] = rand());
		
		m/= sum;

		return m * v;
	}
	@endcode

	Writing functions which safely accept multiple objects requires assertions
	on the sizes since they may be either static or dynamic. TooN's built in
	size check will fail at compile time if mismatched static sizes are given,
	and at run-time if mismatched dynamic sizes are given:
	
	@code
	template<int S1, class B1, int S2, class B2> void func_of_2_vectors(const Vector<S1, double, B1>& v1, const Vector<S2, double, B2>& v2)
	{
		//Ensure that vectors are the same size
		SizeMismatch<S1, S2>::test(v1.num_rows(), v2.num_rows());


	}
	@endcode

	For issues relating to constness, see \sFunctionVector and \sConst

	\subsection sCpp11 What about C++ 11 support?

	TooN compiles cleanly under C++ 11, but does not require it. It can also
	make use of some C++11 features where present. Internally, it will make use
	of \c decltype if a C++11 compiler is present and no overriding configuration
	has been set.  See  \ref stypeof for more information.

	

\subsection ssExamples Are there any examples?

Create two vectors and work out their inner (dot), outer and cross products
@code
// Initialise the vectors
Vector<3> a = makeVector(3,5,0);
Vector<3> b = makeVector(4,1,3);

// Now work out the products
double dot = a*b;                            // Dot product
Matrix<3,3> outer = a.as_col() * b.as_row(); // Outer product
Vector<3> cross = a ^ b;                     // Cross product

cout << "a:" << endl << a << endl;
cout << "b:" << endl << b << endl;
cout << "Outer:" << endl << outer << endl;
cout << "Cross:" << endl << cross << endl;
@endcode

Create a vector and a matrix and multiply the two together
@code
// Initialise a vector
Vector<3> v = makeVector(1,2,3);

// Initialise a matrix
Matrix<2,3> M(d);
M[0] = makeVector(2,4,5);
M[1] = makeVector(6,8,9);

// Now perform calculations
Vector<2> v2 = M*v;  // OK - answer is a static 2D vector
Vector<> v3 = M*v;   // OK - vector is determined to be 2D at runtime
Vector<> v4 = v*M;   // Compile error - dimensions of matrix and vector incompatible
@endcode


\subsection sImplementation How is it implemented

\subsubsection ssStatic Static-sized vectors and matrices

One aspect that makes this library efficient is that when you declare a
3-vector, all you get are 3 doubles - there's no metadata. So
<code>sizeof(Vector<3>)</code> is 24. This means that when you write
<code>Vector<3> v;</code> the data for <code>v</code> is allocated on the stack
and hence <code>new</code>/<code>delete</code>
(<code>malloc</code>/<code>free</code>) overhead is avoided. However, for large
vectors and matrices, this would be a Bad Thing since <code>Vector<1000000>
v;</code> would result in an object of 8 megabytes being allocated on the stack and
potentially overflowing it. %TooN gets around
that problem by having a cutoff at which statically sized vectors are allocated
on the heap. This is completely transparent to the programmer, the objects'
behaviour is unchanged and you still get the type safety offered by statically
sized vectors and matrices. The cutoff size at which the library changes the
representation is defined in <code>TooN.h</code> as the <code>const int
TooN::Internal::max_bytes_on_stack=1000;</code>.

When you apply the subscript operator to a <code>Matrix<3,3></code> and the
function simply returns a vector which points to the the apropriate hunk of memory as a reference
(i.e. it basically does no work apart from moving around a pointer). This avoids
copying and also allows the resulting vector to be used as an l-value. Similarly
the transpose operation applied to a matrix returns a matrix which referes to the 
same memory but with the opposite layout which also means
the transpose can be used as an l-value so <code>M1 = M2.T();</code> and
<code>M1.T() = M2;</code> do exactly the same thing.

<b> Warning: This also means that <code>M = M.T();</code> does the wrong thing.</b>
However, since .T() essentially costs nothing, it should be very rare that you need to do this.

\subsubsection ssDynamic Dynamic sized vectors and matrices

These are implemented in the obvious way using metadata with the rule that the
object that allocated on the heap also deallocates. Other objects may reference
the data (e.g. when you subscript a matrix and get a vector).

\subsection ssLazy Return value optimisation vs Lazy evaluation

When you write <code>v1 = M * v2;</code> a naive implementation will compute
<code>M * v2</code> and store the result in a temporary object. It will then
copy this temporary object into <code>v1</code>. A method often advanced to
avoid this is to have <code>M * v2</code> simply return an special object
<code>O</code> which contains references to <code>M</code> and <code>v2</code>.
When the compiler then resolves <code>v1 = O</code>, the special object computes
<code>M*v2</code> directly into <code>v1</code>. This approach is often called
lazy evaluation and the special objects lazy vectors or lazy matrices.
Stroustrup (The C++ programming language Chapter 22) refers to them as
composition closure objects or compositors.


The killer is this: <b>What if v1 is just another name for v2?</b> i.e. you
write something like <code>v = M * v;</code>. In this case the semantics have
been broken because the values of <code>v</code> are being overwritten as the
computation progresses and then the remainder of the computation is using the
new values. In this library <code>v1</code> in the expression could equally well
alias part of <code>M</code>, thus you can't even solve the problem by having a
clever check for aliasing between <code>v1</code> and <code>v2</code>. This
aliasing problem means that the only time the compiler can assume it's safe to
omit the temporary is when <code>v1</code> is being constructed (and thus cannot
alias anything else) i.e. <code>Vector<3> v1 = M * v2;</code>.

%TooN provides this optimisation by providing the compiler with the opportunity
to use a return value optimisation. It does this by making <code>M * v2</code>
call a special constructor for <code>Vector<3></code> with <code>M</code> and
<code>v2</code> as arguments. Since nothing is happening between the
construction of the temporary and the copy construction of <code>v1</code> from
the temporary (which is then destroyed), the compiler is permitted to optimise
the construction of the return value directly into <code>v1</code>.

Because a naive implemenation of this strategy would result in the vector and
matrix classes having a very large number of constructors, these classes are
provided with template constructors that take a standard form. The code that
does this, declared in the header of class <code>Vector</code> is: 

@code
	template <class Op>
	inline Vector(const Operator<Op>& op)
		: Base::template VLayout<Size, Precision> (op)
	{
		op.eval(*this);
	}
@endcode

\subsubsection ssHow How it all really works

This documentation is generated from a cleaned-up version of the interface, hiding the implementation 
that allows all of the magic to work. If you want to know more and can understand idioms like:
@code

template<int, typename, int, typename> struct GenericVBase;
template<int, typename> struct VectorAlloc;

struct VBase {
	template<int Size, class Precision>
	struct VLayout : public GenericVBase<Size, Precision, 1, VectorAlloc<Size, Precision> > {
	    ...
	};
};

template <int Size, class Precision, class Base=VBase>
class Vector: public Base::template VLayout<Size, Precision> {
   ...
};
@endcode

then take a look at the source code ... 


\section sManualConfiguration Manual configuration
	
Configuration is controlled by <code>internal/config.hh</code>. If this file is empty
then the default configuration will be used and TooN will work. There are several options.

\subsection stypeof Typeof

TooN needs a mechanism to determine the type of the result of an expression. One of the following
macros can be defined to control the behaviour:
- \c TOON_TYPEOF_DECLTYPE
  - Use the C++11 decltype operator.
- \c TOON_TYPEOF_TYPEOF
  - Use GCC's \c typeof extension. Only works with GCC and will fail with -pedantic
- \c TOON_TYPEOF___TYPEOF__
  - Use GCC's \c __typeof__ extension. Only works with GCC and will work with -pedantic
- \c TOON_TYPEOF_BOOST
  - Use the \link http://www.boost.org/doc/html/typeof.html Boost.Typeof\endlink system.
    This will work with Visual Studio if Boost is installed.
- \c TOON_TYPEOF_BUILTIN
  - The default option (does not need to be defined)
  - Only works for the standard builtin integral types and <code>std::complex<float></code> and <code>std::complex<double></code>.

Under Win32, the builtin typeof needs to be used. Comment out all the TOON_TYPEOF_ defines to use it.

If no configuration is present and C++11 is detected, then \c decltype will be used.

\subsection sConfigLapack Functions using LAPACK

Some functions use internal implementations for small sizes and may switch over
to LAPACK for larger sizes. In all cases, an equivalent method is used in terms
of accuracy (eg Gaussian elimination versus LU decomposition). If the following 
macro is defined:
- \c TOON_USE_LAPACK
then LAPACK will be used for large systems, where optional.
The individual functions are:
- TooN::determinant is controlled by \c TOON_DETERMINANT_LAPACK
  -  If the macro is undefined as or defined as -1, then LAPACK will never be
	 used. Otherwise it indicated which the size at which LAPACK should be 
	 used.

Note that these macros do not affect classes that are currently only wrappers
around LAPACK.

**/

///////////////////////////////////////////////////////
// Modules classifying classes and functions

/// @defgroup gLinAlg Linear Algebra
/// \link TooN::Vector Vector\endlink  and \link TooN::Matrix Matrix \endlink classes, and helpers.

/// @defgroup gDecomps Matrix decompositions
/// Classes to perform matrix decompositions, used to solve 
/// linear equations and provide information about matrices. 
/// Some of these are wrappers around LAPACK, others are built in.
/// provided by the LAPACK library.

/// @defgroup gTransforms Transformation matrices
/// Classes to represent particular types of transformation matrix.

/// @defgroup gEquations Linear equation solvers
/// Classes to solve linear equations.

/// @defgroup gFunctions Evaluation of functions.
/// Evaluation of useful functions.
/** 

@defgroup gOptimize Function optimization

Classes and functions to perform function optimization.

@section gOneDim One dimensional function optimization

The following functions find the minimum of a 1-D function:
 - golden_section_search()
 - brent_line_search()

@section gMultiDim Multidimensional dimensional function optimization

The following classes perform multidimensional function minimization:
 - TooN::DownhillSimplex
 - TooN::ConjugateGradient

The mode of operation is to set up a mutable class, then repeatedly call an
iterate function. This allows different sub algorithms (such as termination
conditions) to be substituted in if need be.

@internal
@defgroup gInternal TooN internals

*/
