################################################################################
#Library Top Makefile.
################################################################################
subdirs = base lua gui hardware network cv 

#  
all : $(subdirs)
	@for dir in $(subdirs);do \
	make -C $$dir; done

clean :$(subdirs)
	@for dir in $(subdirs);do \
	make clean -C $$dir; done

clean_tmp:$(subdirs)
	@for dir in $(subdirs);do \
	make clean_tmp -C $$dir; done

