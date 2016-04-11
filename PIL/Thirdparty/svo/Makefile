TOPDIR          ?= ../..
LIB_MAKE_TYPE   = shared
#LIB_MAKE_TYPE   ?=static
MAKE_TYPE       = $(LIB_MAKE_TYPE)
LIB_PREFIX      = lib
BUILD_PATH      = $(TOPDIR)/build/Thirdparty/$(HereFolderName)

INCLUDE_PATH    += ./include ./src 
CPP_FILES       +=  src/config.cpp \
                    src/depth_filter.cpp \
                    src/feature_alignment.cpp \
                    src/feature_detection.cpp \
                    src/frame.cpp \
                    src/frame_handler_base.cpp \
                    src/frame_handler_mono.cpp \
                    src/initialization.cpp \
                    src/map.cpp \
                    src/matcher.cpp \
                    src/point.cpp \
                    src/pose_optimizer.cpp \
                    src/reprojector.cpp \
                    src/sparse_img_align.cpp

MODULES         += OPENCV G2O EIGEN3 SOPHUS FAST VIKIT_COMMON

COMPILEFLAGS    += -Wall -D_LINUX -D_REENTRANT -march=native -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unknown-pragmas
COMPILEFLAGS    += -mmmx -msse -msse -msse2 -msse3 -mssse3
COMPILEFLAGS    += -O3 -fsee -fomit-frame-pointer -fno-signed-zeros -fno-math-errno -funroll-loops
COMPILEFLAGS    += -std=c++11


include $(TOPDIR)/scripts/make.conf

clean_tmp:
	@rm $(BUILD_PATH) -r
