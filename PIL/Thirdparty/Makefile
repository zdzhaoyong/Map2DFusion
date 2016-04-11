################################################################################
#Thirdparty Top Makefile.
################################################################################
TOPDIR    ?=
#BUILD_PATH = $(

target = alglib lua opmapcontrol vikit_common fast pba svo DBoW2 g2o sophus

.PHONY: $(target)
all : $(target)


alglib:
	make -C $@

lua:
	make -C lua-5.1.5

opmapcontrol:
	make -C $@ 

fast: 
	make -C $@

pba:
	make -C $@

svo : fast vikit_common sophus
	make -C $@

vikit_common: sophus
	make -C $@

DBoW2:
	make -C $@

g2o:
	make -C $@

sophus:
	make -C $@


