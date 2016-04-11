TOPDIR          ?= ../..
LIB_MAKE_TYPE   = shared
#LIB_MAKE_TYPE   ?=static
MAKE_TYPE       = $(LIB_MAKE_TYPE)
LIB_PREFIX      = lib
BUILD_PATH      = $(TOPDIR)/build/Thirdparty/$(HereFolderName)

INCLUDE_PATH    += ./include
CPP_FILES       +=  src/fast_10.cpp \
                    src/fast_10_score.cpp \
                    src/faster_corner_10_sse.cpp \
                    src/nonmax_3x3.cpp

COMPILEFLAGS    += -Wall -Werror -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unknown-pragmas
COMPILEFLAGS    += -O3 -mmmx -msse -msse -msse2 -msse3 -mssse3 -fomit-frame-pointer

include $(TOPDIR)/scripts/make.conf

clean_tmp:
	@rm $(BUILD_PATH) -r
