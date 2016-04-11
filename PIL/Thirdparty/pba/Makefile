TOPDIR          ?= ../..
MAKE_TYPE       ?= shared
LIB_PREFIX       = lib
BUILD_PATH       = $(TOPDIR)/build/Thirdparty/$(HereFolderName)
#BIN_PATH         = bin
#LIBS_PATH        ?=bin

COMPILEFLAGS = -O2 -Wno-deprecated -pthread  -march=native -mfpmath=sse -fpermissive 

SRC_FILES = ./pba/pba.cpp ./pba/ConfigBA.cpp ./pba/SparseBundleCPU.cpp ./pba/pba_util.cpp 

#CUDA_PATH=/data/zhaoyong/Linux/Program/Libs/cuda-6.5
CUDA_PATH=
ifneq "$(CUDA_PATH)" ""

    CUDA_LIB_PATH = $(CUDA_PATH)/lib64
    ifdef CUDA_BIN_PATH
	    NVCC = $(CUDA_BIN_PATH)/nvcc -ccbin=g++-4.4
    else
	    NVCC = $(CUDA_PATH)/bin/nvcc -ccbin=g++-4.4
    endif

    ifndef CUDA_INC_PATH
	    CUDA_INC_PATH = $(CUDA_PATH)/include
    endif

    SRC_FILES += pba/CuTexImage.cpp  pba/SparseBundleCU.cpp
    OBJ_FILES += ./pba/ProgramCU.o

    NVCC_FLAGS = -I$(CUDA_INC_PATH) -O2 -Xcompiler -fPIC

    COMPILEFLAGS += -I$(CUDA_INC_PATH)
    LINKFLAGS    +=-L$(CUDA_LIB_PATH) -Wl,-rpath,$(CUDA_LIB_PATH) -lcudart
else
    COMPILEFLAGS += -DPBA_NO_GPU
endif

include $(TOPDIR)/scripts/make.conf

CXX		 = g++-4.4

#build rule for CUDA 
$(BUILD_PATH)/pba/ProgramCU.o: ./pba/ProgramCU.cu ./pba/ProgramCU.h
	$(NVCC) $(NVCC_FLAGS) -o $@ $< -c

driver: $(LIBS_PATH)/lib$(HereFolderName).so
	ar rcs $(LIBS_PATH)/libpba.a $(OBJ_FILES_BUILD)
	$(CC) -o $(BIN_PATH)/driver ./demo/driver.cpp $(COMPILEFLAGS) -L$(LIBS_PATH) -lpba -Wl,-rpath,$(LIBS_PATH) $(LINKFLAGS)


cleanDriver:
	rm $(BIN_PATH)/driver
