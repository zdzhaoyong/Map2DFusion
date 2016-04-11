################################################################################
#Lib_ZY Top Makefile.
################################################################################
export MAKE             = make -j2
export MAKE_OS          = linux # win32 mac

export TOPDIR           = $(shell pwd)

#Where to put object files *.o
export BUILD_PATH       = $(TOPDIR)/build

#Where to put final applications
export BIN_PATH         = $(TOPDIR)/bin

#Where to put libraries
export LIBS_PATH        = $(TOPDIR)/libs
export LIB_MAKE_TYPE    = shared # static #


.PHONY: all apps libs Thirdparty sophus g2o lua pba
all :libs apps

libs:Thirdparty
	@echo "Compiling librarys of PIL"
	$(MAKE) -C src

apps: libs
	@echo "Compiling apps of PIL"
	$(MAKE) -C apps

Thirdparty:sophus lua
	@echo "Compiling thirdpartys of PIL"

sophus:
	@echo "Compiling thirdparty sophus of PIL"
	$(MAKE) -C Thirdparty/$@

g2o:
	@echo "Compiling thirdparty g2o of PIL"
	$(MAKE) -C Thirdparty/$@

lua:
	@echo "Compiling thirdparty lua of PIL"
	$(MAKE) -C Thirdparty/lua-5.1.5

pba:
	@echo "Compiling thirdparty pba of PIL"
	$(MAKE) -C Thirdparty/$@

clean :
	rm -r $(BUILD_PATH)/* $(BIN_PATH)/* 

clean_tmp:
	rm -r $(BUILD_PATH)/*


