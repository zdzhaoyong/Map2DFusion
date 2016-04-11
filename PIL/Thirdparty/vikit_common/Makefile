TOPDIR          ?= ../..
LIB_MAKE_TYPE   = shared
#LIB_MAKE_TYPE   ?=static
MAKE_TYPE       = $(LIB_MAKE_TYPE)
LIB_PREFIX      = lib
BUILD_PATH      = $(TOPDIR)/build/Thirdparty/$(HereFolderName)

INCLUDE_PATH    += ./include
SRC_PATH         = ./src

MODULES         += OPENCV SOPHUS EIGEN3

COMPILEFLAGS    += -Wall -D_LINUX -D_REENTRANT -march=native -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unknown-pragmas
COMPILEFLAGS    += -mmmx -msse -msse -msse2 -msse3 -mssse3
COMPILEFLAGS    += -O3 -fsee -fomit-frame-pointer -fno-signed-zeros -fno-math-errno -funroll-loops
COMPILEFLAGS    += -std=c++11


include $(TOPDIR)/scripts/make.conf

clean_tmp:
	@rm $(BUILD_PATH) -r
