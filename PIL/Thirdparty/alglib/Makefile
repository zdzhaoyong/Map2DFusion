TOPDIR          ?= ../..
LIB_MAKE_TYPE   = shared
#LIB_MAKE_TYPE   ?=static
MAKE_TYPE       = $(LIB_MAKE_TYPE)
LIB_PREFIX      = lib
BUILD_PATH      = $(TOPDIR)/build/Thirdparty/$(HereFolderName)

INCLUDE_PATH    += ./src
CPP_FILES       += src/alglibinternal.cpp \
                   src/alglibmisc.cpp \
                   src/ap.cpp \
                   src/dataanalysis.cpp \
                   src/diffequations.cpp \
                   src/fasttransforms.cpp \
                   src/integration.cpp \
                   src/interpolation.cpp \
                   src/linalg.cpp \
                   src/optimization.cpp \
                   src/solvers.cpp \
                   src/specialfunctions.cpp \
                   src/statistics.cpp

include $(TOPDIR)/scripts/make.conf

clean_tmp:
	@rm $(BUILD_PATH) -r
