##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 milc test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

FP_TOLERANCE    = 0.0001
FP_ABSTOLERANCE = 0.0000002

# This test is over the 500s default limit on ARM.
RUNTIMELIMIT = 1500

include ../../Makefile.spec2006

CPPFLAGS += -DFN -DFAST -DCONGRAD_TMP_VECTORS -DDSLASH_TMP_LINKS

STDIN_FILENAME  := su3imp.in
STDOUT_FILENAME := su3imp.out
