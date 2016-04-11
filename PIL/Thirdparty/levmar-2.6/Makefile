TOPDIR          ?= ../..
LIB_MAKE_TYPE   = shared
LIB_MAKE_TYPE   ?=static
MAKE_TYPE       = $(LIB_MAKE_TYPE)
LIB_PREFIX      = lib
OUTPUT          = levmar
BUILD_PATH      = $(TOPDIR)/build/Thirdparty/levmar

INCLUDE_PATH   += ./src
C_FILES        +=  src/lm.c \
                   src/Axb.c src/misc.c src/lmlec.c src/lmbc.c src/lmblec.c src/lmbleic.c 


COMPILEFLAGS   += -DLM_DBL_PREC

ifeq "$(MAKE_OS)" "mingw"
LINKFLAGS      += -lopenblas
else
LINKFLAGS      += -llapack -lblas 
endif


#-lf2c

include $(TOPDIR)/scripts/make.conf

clean_tmp:
	@rm $(BUILD_PATH) -r
