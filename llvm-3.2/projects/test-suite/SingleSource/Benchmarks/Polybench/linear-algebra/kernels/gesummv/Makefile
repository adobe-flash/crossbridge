LEVEL = ../../../../../..

POLYBENCH_UTILS = SingleSource/Benchmarks/Polybench/utilities 
PROG          = gesummv
CFLAGS        += -I $(PROJ_SRC_ROOT)/$(POLYBENCH_UTILS) -DPOLYBENCH_DUMP_ARRAYS

ifdef SMALL_PROBLEM_SIZE
CFLAGS        += -DSMALL_DATASET
endif

ifndef SMALL_PROBLEM_SIZE
HASH_PROGRAM_OUTPUT = 1
endif

include $(LEVEL)/SingleSource/Makefile.singlesrc

