# levmar CMake file; see http://www.cmake.org and 
#                        http://www.insightsoftwareconsortium.org/wiki/index.php/CMake_Tutorial

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(LEVMAR)

SET(HAVE_LAPACK 1 CACHE BOOL "Do we have LAPACK/BLAS?")
# the directory where the lapack/blas/f2c libraries reside
SET(LAPACKBLAS_DIR "/usr/lib" CACHE PATH "Path to lapack/blas libraries")
SET(NEED_F2C 1 CACHE BOOL "Do we need either f2c or F77/I77?")
SET(HAVE_PLASMA 0 CACHE BOOL "Do we have PLASMA parallel linear algebra library?")
IF(HAVE_PLASMA)
 SET(PLASMA_DIR "/usr/local/PLASMA" CACHE PATH "Path to PLASMA root")
ENDIF(HAVE_PLASMA)
SET(LINSOLVERS_RETAIN_MEMORY 1 CACHE BOOL "Should linear solvers retain working memory between calls? (non-reentrant!)")
SET(LM_DBL_PREC 1 CACHE BOOL "Build double precision routines?")
SET(LM_SNGL_PREC 1 CACHE BOOL "Build single precision routines?")
OPTION(BUILD_DEMO "Build demo program?" TRUE)

# actual names for the lapack/blas/f2c libraries
SET(LAPACKBLAS_LIB_NAMES "lapack;blas" CACHE STRING "The name of the lapack & blas libraries")
#SET(LAPACKBLAS_LIB_NAMES "mkl_solver_sequential;mkl_intel_c;mkl_sequential;mkl_core" CACHE STRING "The name of the lapack library") # MKL
IF(NEED_F2C)
  SET(F2C_LIB_NAME f2c CACHE STRING "The name of the f2c or F77/I77 library")
  # f2c is sometimes equivalent to libF77 & libI77
  #SET(F2C_LIB_NAME "libF77;libI77" CACHE STRING "The name of the f2c or F77/I77 library")
ELSE(NEED_F2C)
  SET(F2C_LIB_NAME "" CACHE STRING "The name of the f2c or F77/I77 library")
ENDIF(NEED_F2C)

# actual names for the PLASMA libraries
IF(HAVE_PLASMA)
  SET(PLASMA_LIB_NAMES "plasma;coreblas;quark;lapacke" CACHE STRING "The names of the PLASMA libraries")
ENDIF(HAVE_PLASMA)


IF(0)
# treat paths relative to the source dir.
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0015 NEW)
ENDIF(COMMAND CMAKE_POLICY)
ENDIF(0)

# compiler flags
#ADD_DEFINITIONS(-DLINSOLVERS_RETAIN_MEMORY) # do not free memory between linear solvers calls
#REMOVE_DEFINITIONS(-DLINSOLVERS_RETAIN_MEMORY) # free memory between calls

########################## NO CHANGES BEYOND THIS POINT ##########################

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/levmar.h.in ${CMAKE_CURRENT_SOURCE_DIR}/levmar.h)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
IF(HAVE_PLASMA)
 INCLUDE_DIRECTORIES(${PLASMA_DIR}/include ${PLASMA_DIR}/quark)
ENDIF(HAVE_PLASMA)

# PLASMA headers in Axb.c should be compiled as C++
IF(HAVE_PLASMA)
  SET_SOURCE_FILES_PROPERTIES(Axb.c PROPERTIES LANGUAGE CXX)
ENDIF(HAVE_PLASMA)

# levmar library source files
ADD_LIBRARY(levmar STATIC
  lm.c Axb.c misc.c lmlec.c lmbc.c lmblec.c lmbleic.c
  levmar.h misc.h compiler.h
)

# demo program
IF(BUILD_DEMO)
  SET(LIBS levmar)

  LINK_DIRECTORIES(${CMAKE_BINARY_DIR}) # location of the levmar library
  LINK_DIRECTORIES(${LAPACKBLAS_DIR})

# libraries the demo depends on
  IF(HAVE_PLASMA)
    LINK_DIRECTORIES(${PLASMA_DIR}/lib)
    SET(LIBS ${LIBS} ${PLASMA_LIB_NAMES})
  ENDIF(HAVE_PLASMA)

  IF(HAVE_LAPACK)
    IF(NEED_F2C)
      SET(LIBS ${LIBS} ${LAPACKBLAS_LIB_NAMES} ${F2C_LIB_NAME})
    ELSE(NEED_F2C)
      SET(LIBS ${LIBS} ${LAPACKBLAS_LIB_NAMES})
    ENDIF(NEED_F2C)
  ENDIF(HAVE_LAPACK)

  ADD_EXECUTABLE(lmdemo lmdemo.c levmar.h)
  TARGET_LINK_LIBRARIES(lmdemo ${LIBS})
  MESSAGE(STATUS "lmdemo will be linked against ${LIBS}")

# make sure that the library is built before the demo
  ADD_DEPENDENCIES(lmdemo levmar)
ENDIF(BUILD_DEMO)

#SUBDIRS(matlab)
