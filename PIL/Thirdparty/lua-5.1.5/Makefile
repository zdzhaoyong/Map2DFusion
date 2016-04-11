TOPDIR          ?= ../..
LIB_MAKE_TYPE   = shared
LIB_MAKE_TYPE   ?=static
MAKE_TYPE       = $(LIB_MAKE_TYPE)
LIB_PREFIX      = lib
OUTPUT          = lua
BUILD_PATH      = $(TOPDIR)/build/Thirdparty/lua

INCLUDE_PATH    += ./src
C_FILES         +=  src/lapi.c \
                    src/lauxlib.c \
                    src/lbaselib.c \
                    src/lcode.c \
                    src/ldblib.c \
                    src/ldebug.c \
                    src/ldo.c \
                    src/ldump.c \
                    src/lfunc.c \
                    src/lgc.c \
                    src/linit.c \
                    src/liolib.c \
                    src/llex.c \
                    src/lmathlib.c \
                    src/lmem.c \
                    src/loadlib.c \
                    src/lobject.c \
                    src/lopcodes.c \
                    src/loslib.c \
                    src/lparser.c \
                    src/lstate.c \
                    src/lstring.c \
                    src/lstrlib.c \
                    src/ltable.c \
                    src/ltablib.c \
                    src/ltm.c \
                    src/lundump.c \
                    src/lvm.c \
                    src/lzio.c \
                    src/print.c 


COMPILEFLAGS   += -O2 -Wall

ifeq "$(MAKE_OS)" "mingw"
COMPILEFLAGS   += -D_WIN32
else
COMPILEFLAGS   += -DLUA_USE_LINUX
endif

include $(TOPDIR)/scripts/make.conf

clean_tmp:
	@rm $(BUILD_PATH) -r
