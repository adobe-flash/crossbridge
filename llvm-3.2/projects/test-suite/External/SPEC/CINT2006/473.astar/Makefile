##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 astar test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

FP_TOLERANCE = 0.001

include ../../Makefile.spec2006

ifeq ($(ENDIAN),big)
  CPPFLAGS += -DSPEC_CPU_BIG_ENDIAN
else
  CPPFLAGS += -DSPEC_CPU_LITTLE_ENDIAN
endif

ifeq ($(RUN_TYPE),test)
  RUN_OPTIONS      = lake.cfg
  STDOUT_FILENAME := lake.out
else
  RUN_OPTIONS      = rivers1.cfg
  STDOUT_FILENAME := rivers1.out
endif

LDFLAGS = -lstdc++ -lm
LIBS    = -lm
