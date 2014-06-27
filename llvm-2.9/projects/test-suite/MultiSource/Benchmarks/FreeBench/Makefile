# MultiSource/Olden Makefile:  Build all subdirectories automatically

LEVEL = ../../..
PARALLEL_DIRS = distray mason pcompress2 analyzer fourinarow neural pifft

# SparcV8 can't handle analyzer, but we can't separate V8 from V9 now
ifeq ($(ARCH),Sparc)
PARALLEL_DIRS := $(filter-out analyzer, $(PARALLEL_DIRS))
endif

include $(LEVEL)/Makefile.programs
