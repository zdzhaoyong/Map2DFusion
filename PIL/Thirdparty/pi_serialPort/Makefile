
TOPDIR          ?= ../..
LIB_MAKE_TYPE   = shared
#LIB_MAKE_TYPE   ?=static
MAKE_TYPE       = $(LIB_MAKE_TYPE)
LIB_PREFIX      = lib
BUILD_PATH      = $(TOPDIR)/build/Thirdparty/$(HereFolderName)

INCLUDE_PATH    += 
CPP_FILES       += PI_SerialPort.cpp 

#MODULES         += OPENCV

include $(TOPDIR)/scripts/make.conf

clean_tmp:
	@rm $(BUILD_PATH) -r
