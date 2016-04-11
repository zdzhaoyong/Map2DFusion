################################################################################
#Map2DFusion Top Makefile.
################################################################################
export MAKE             = make -j8
export MAKE_OS          = linux # win32 mac

export TOPDIR           = $(shell pwd)

#Where to put object files *.o
export BUILD_PATH       = $(TOPDIR)/build

#Where to put final applications
export BIN_PATH         = $(TOPDIR)

#Where to put libraries
export LIBS_PATH        = $(TOPDIR)/libs
export LIB_MAKE_TYPE    = shared # static #

export LIB_PI_TOP	= $(TOPDIR)/PIL

.PHONY: all apps libs lua opmapcontrol pi_base pi_lua pi_network pi_hardware pi_gui clean_tmp clean
all :libs apps

libs:lua opmapcontrol pi_base pi_lua pi_hardware pi_gui 

apps: libs
	$(MAKE) -C src

lua:
	$(MAKE) -C PIL/Thirdparty/lua-5.1.5

opmapcontrol:
	$(MAKE) -C PIL/Thirdparty/opmapcontrol
	mv PIL/libs/* $(LIBS_PATH)

pi_base:
	$(MAKE) -C PIL/src/base

pi_lua:pi_base
	$(MAKE) -C PIL/src/lua

pi_network:pi_base
	$(MAKE) -C PIL/src/network

pi_hardware:pi_base
	$(MAKE) -C PIL/src/hardware

pi_gui:pi_base pi_lua
	$(MAKE) -C PIL/src/gui

clean:clean_tmp
	rm Map2DFusion -f

clean_tmp:
	rm -r $(BUILD_PATH)/*


