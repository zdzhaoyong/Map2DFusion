################################################################################
#Applications Top Makefile.
################################################################################
subdirs =CameraTest  Internet_Test  LUA_Test  ScSO3_Test  SIM3_Test  Svar_Test Timer_Test  VideoReaderTest \
format_demo SE3_Test    SO3_Test   # Template_Test TooN_Test IMU_Test   Opengl_Test

#  IMU_Test
all : $(subdirs)
	@for dir in $(subdirs);do \
	make -C $$dir --no-print-directory; done

clean :$(subdirs)
	@for dir in $(subdirs);do \
	make clean -C $$dir; done

