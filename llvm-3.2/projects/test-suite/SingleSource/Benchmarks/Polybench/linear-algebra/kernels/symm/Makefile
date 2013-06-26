LEVEL = ../../../../../..

POLYBENCH_UTILS = SingleSource/Benchmarks/Polybench/utilities 
PROG          = symm
CFLAGS        += -I $(PROJ_SRC_ROOT)/$(POLYBENCH_UTILS) -DPOLYBENCH_DUMP_ARRAYS

ifeq ($(ARCH),Mips)
RUNTIMELIMIT := 2000
endif

HASH_PROGRAM_OUTPUT = 1

include $(LEVEL)/SingleSource/Makefile.singlesrc

