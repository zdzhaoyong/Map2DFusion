TOPDIR          ?= ../..
LIB_MAKE_TYPE   = shared
#LIB_MAKE_TYPE   ?=static
MAKE_TYPE       = $(LIB_MAKE_TYPE)
LIB_PREFIX      = lib
BUILD_PATH      = $(TOPDIR)/build/Thirdparty/$(HereFolderName)

INCLUDE_PATH    += ./src
CPP_FILES       += DBoW2/BowVector.cpp \
                   DBoW2/FORB.cpp \
                   DBoW2/FCNN.cpp \
                   DBoW2/FeatureVector.cpp \
                   DBoW2/ScoringObject.cpp \
                   DUtils/Random.cpp \
                   DUtils/Timestamp.cpp

MODULES         += OPENCV

include $(TOPDIR)/scripts/make.conf

clean_tmp:
	@rm $(BUILD_PATH) -r
