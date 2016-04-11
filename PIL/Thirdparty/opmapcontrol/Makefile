
ifeq "$(MAKE_OS)" "mingw"
DO_QMAKE         = qmake
else
DO_QMAKE         = qmake-qt4
endif


all : 
	$(DO_QMAKE) opmapcontrol.pro -o make.qmake
	make -f make.qmake

