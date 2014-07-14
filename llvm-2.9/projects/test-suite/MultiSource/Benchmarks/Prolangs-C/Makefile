LEVEL = ../../..
PARALLEL_DIRS  := TimberWolfMC agrep allroots archie-client assembler bison \
      cdecl compiler fixoutput football gnugo loader \
      plot2fig simulator unix-smail unix-tbl

# Get the $(ARCH) setting
include $(LEVEL)/Makefile.config

# Disable bison due to insane running times of bison.native
ifeq ($(ARCH), Sparc)
PARALLEL_DIRS := $(filter-out bison, $(PARALLEL_DIRS))
endif

# Disable until disk space eating bug can be fixed
ifeq ($(ARCH), Alpha)
PARALLEL_DIRS := $(filter-out bison, $(PARALLEL_DIRS))
endif


include $(LEVEL)/Makefile.programs
