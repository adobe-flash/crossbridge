##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 povray test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

FP_ABSTOLERANCE = 0

STDOUT_FILENAME := SPEC-benchmark.log

LDFLAGS = -lstdc++ -lm
LIBS    = -lstdc++ -lm

include ../../Makefile.spec2006

ifeq ($(RUN_TYPE),test)
  FP_TOLERANCE     = 0.0002
  RUN_OPTIONS      = SPEC-benchmark-test
else
  FP_TOLERANCE     = 0.00005
  RUN_OPTIONS      = SPEC-benchmark-train
endif
