##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 sjeng test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

include ../../Makefile.spec2006

ifeq ($(RUN_TYPE),test)
  RUN_OPTIONS      = test.txt
  STDOUT_FILENAME := test.out
else
  RUN_OPTIONS      = train.txt
  STDOUT_FILENAME := train.out
endif
