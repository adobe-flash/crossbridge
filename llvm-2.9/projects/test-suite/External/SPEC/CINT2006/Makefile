##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building SPEC2006 integer tests.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
PARALLEL_DIRS := \
        400.perlbench  \
        401.bzip2      \
        429.mcf        \
        445.gobmk      \
        456.hmmer      \
        462.libquantum \
        464.h264ref    \
        471.omnetpp    \
        483.xalancbmk

ifndef SMALL_PROBLEM_SIZE
# Temporary. 
PARALLEL_DIRS +=  \
        403.gcc        \
        458.sjeng      \
        473.astar
endif

ifdef DISABLE_CXX
PARALLEL_DIRS := $(filter-out 471.omnetpp, $(PARALLEL_DIRS))
PARALLEL_DIRS := $(filter-out 483.xalancbmk, $(PARALLEL_DIRS))
endif

# Get the $(ARCH) setting
include $(LEVEL)/Makefile.config

include $(LEVEL)/Makefile.programs
