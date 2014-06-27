##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building SPEC2006 floating point tests.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..

## C Programs
PARALLEL_DIRS :=  \
    433.milc      \
    444.namd      \
    447.dealII    \
    450.soplex

ifndef SMALL_PROBLEM_SIZE
# Skip tests that take too much memory to run.
PARALLEL_DIRS +=  \
    470.lbm
endif

ifdef USE_F95
PARALLEL_DIRS +=  \
    410.bwaves    \
    416.gamess    \
    434.zeusmp    \
    435.gromacs   \
    436.cactusADM \
    437.leslie3d  \
    454.calculix  \
    459.GemsFDTD  \
    481.wrf
else
ifdef USE_F2C
PARALLEL_DIRS +=  \
    410.bwaves    \
    416.gamess    \
    434.zeusmp    \
    435.gromacs   \
    436.cactusADM \
    437.leslie3d  \
    454.calculix  \
    459.GemsFDTD  \
    481.wrf
endif
endif   # End USE_F95

# These compile but need other things to run:
#   453.povray    - c++ exceptions
#   465.tonto     - infinite loop, works at -O0, not at -O2
#   482.sphinx3   - generates control file at run time

ifdef DISABLE_CXX
PARALLEL_DIRS := $(filter-out 444.namd, $(PARALLEL_DIRS))
PARALLEL_DIRS := $(filter-out 447.dealII, $(PARALLEL_DIRS))
PARALLEL_DIRS := $(filter-out 450.soplex, $(PARALLEL_DIRS))
endif

# Get the $(ARCH) setting
include $(LEVEL)/Makefile.config

include $(LEVEL)/Makefile.programs
