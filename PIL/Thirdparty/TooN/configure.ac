AC_INIT(TooN, version-2.0.0-beta8)
AC_COPYRIGHT([Copyright T. Drummond, E. Rosten, G. Reitmayr 2006, 2007, 2008, 2009])
AC_LANG(C++)
AC_PROG_CXX




################################################################################
#
# Useful macros
#

dnl APPEND(var, value)
dnl This appends vale to a shell variable var
define(APPEND, [$1="$$1 $2"])



dnl TEST_AND_SET_CXXFLAG(flag, [program], [run])
dnl
dnl This attempts to compile a and run program with a certain compiler flag.
dnl If no program is given, then the minimal C++ program is compiled, and 
dnl this tests just the validity of the compiler flag. 
dnl
define([TEST_AND_SET_CXXFLAG],[
	if test "$3" == ""
	then
		AC_MSG_CHECKING([if compiler flag $1 works])	
	else
		AC_MSG_CHECKING([$3])	
	fi
	save_CXXFLAGS="$CXXFLAGS"
	APPEND(CXXFLAGS, [$1])

	m4_if([$2],[],[pushdef(prog, [int main(){}])], [pushdef(prog, [$2])])

	m4_if([$4],[run],
		[AC_RUN_IFELSE([AC_LANG_SOURCE([prog])], [cvd_conf_test=1],[cvd_conf_test=0], [cvd_conf_test=0])],
		[AC_COMPILE_IFELSE([AC_LANG_SOURCE([prog])], [cvd_conf_test=1],[cvd_conf_test=0])]
	)
	
	
	popdef([prog])

	if test $cvd_conf_test = 1
	then
		AC_MSG_RESULT(yes)
		ts_success=yes
	else
		AC_MSG_RESULT(no)
		CXXFLAGS="$save_CXXFLAGS"
		ts_success=no
	fi
])










#
# Test for Package Config
#
AC_PATH_PROG(PKG_CONFIG, pkg-config)
if test "x$PKG_CONFIG" = "x"; then
	AC_MSG_WARN([Could not find pkg-config, will not create pc file.])
else
	# we need sed to find the pkg-config lib directory
	AC_CHECK_PROG(SED,sed,sed,AC_MSG_ERROR([You Must install sed]))
	AC_MSG_CHECKING([for pkg-config library dir])
	PKGCONFIG_LIBDIR="`echo $PKG_CONFIG | $SED -e 's~.*/bin/pkg-config$~~'`${libdir}/pkgconfig"

	AC_MSG_RESULT($PKGCONFIG_LIBDIR)
	AC_SUBST(PKGCONFIG_LIBDIR)
	AC_CONFIG_FILES([ TooN.pc ])

	# This will be put into the pc file
	VERSION=AC_PACKAGE_VERSION()
	AC_SUBST(VERSION)
fi


# See if we can find LAPACK
#ACX_LAPACK(
#			 LAPACK_LINKER_FLAGS="$LAPACK_LIBS $BLAS_LIBS $LIBS $FLIBS"
#			 AC_SUBST(LAPACK_LINKER_FLAGS)
#		   , AC_ERROR([Could not find LAPACK])
#		   )
#

typeof=check
AC_ARG_ENABLE(lapack, [AS_HELP_STRING([--enable-lapack],[Use LAPACK where optional])], [lapack=$enableval])
AC_ARG_ENABLE(typeof, [AS_HELP_STRING([--enable-typeof=X],[Force typeof to be decltype, __typeof__, typeof, boost or none])], [typeof=$enableval])
AC_ARG_WITH(default_precision, [AS_HELP_STRING([--with-default_precision=X],[Override default precision from double to X])], [default_precision="$withval"])

if test "$default_precision" != ""
then
	AC_DEFINE_UNQUOTED(TOON_DEFAULT_PRECISION, $default_precision)
fi

if test "$lapack" == "" 
then
	AC_CHECK_LIB(lapack, dgesvd_)
	if test $ac_cv_lib_lapack_dgesvd_ = yes
	then
		lapack=yes
	else
		lapack=no
	fi
fi

if test "$lapack" != "no" 
then
	AC_DEFINE(TOON_USE_LAPACK, 1)
	AC_SUBST(use_lapack, yes)
fi


define(TYPEOF_TEST,[
	if test "$typeof" == check
	then
		AC_MSG_CHECKING([for $1])
		AC_COMPILE_IFELSE([AC_LANG_SOURCE([
		$3
		int main()
		{
			typedef $2(int()*double()) foo;
		}
		])], [ AC_MSG_RESULT(yes);typeof=$1], AC_MSG_RESULT(no))
	fi
])

TYPEOF_TEST(decltype, decltype)
TYPEOF_TEST(__typeof__, __typeof__)
TYPEOF_TEST(typeof, typeof)
TYPEOF_TEST(boost, BOOST_TYPEOF, [#include<boost/typeof/typeof.hpp>])

if test "$typeof" == typeof
then
	AC_DEFINE(TOON_TYPEOF_TYPEOF, 1)
elif test "$typeof" == __typeof__
then
	AC_DEFINE(TOON_TYPEOF___TYPEOF__, 1)
elif test "$typeof" == decltype
then
	AC_DEFINE(TOON_TYPEOF_DECLTYPE, 1)
elif test "$typeof" == boost
then
	AC_DEFINE(TOON_TYPEOF_BOOST, 1)
fi

AC_MSG_CHECKING([for __attribute__((deprecated))])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
__attribute__((deprecated)) void foo(){}
int main()
{
	foo();
}
])], [ AC_MSG_RESULT(yes);AC_DEFINE(TOON_DEPRECATED_GCC,1 )], AC_MSG_RESULT(no))


TEST_AND_SET_CXXFLAG(-Wall)
TEST_AND_SET_CXXFLAG(-Wextra)
TEST_AND_SET_CXXFLAG(-W)
TEST_AND_SET_CXXFLAG(-O3)
TEST_AND_SET_CXXFLAG(-Werror)

AC_CONFIG_HEADERS(internal/config.hh)
AC_OUTPUT(Makefile)

