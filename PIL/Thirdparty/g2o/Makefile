TOPDIR          ?= ../..
MAKE_TYPE       ?= shared
LIB_PREFIX       = lib
BUILD_PATH       = $(TOPDIR)/build/Thirdparty/g2o
LIBS_PATH       ?= $(TOPDIR)/libs
CURRNET_PATH     = $(shell pwd)

all:
	mkdir -p $(BUILD_PATH);cd $(BUILD_PATH)&&cmake $(CURRNET_PATH)&&make;cd $(CURRNET_PATH);mv $(CURRNET_PATH)/lib/* $(LIBS_PATH)/;

clean:
	rm $(BUILD_PATH) -r;rm lib -r;

clean_tmp:
	rm $(BUILD_PATH) -r;
