##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 namd test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

FP_ABSTOLERANCE = 0.00001

# This test is over the 500s default limit on ARM. Thumb1 takes a *long* time.
RUNTIMELIMIT = 3000

RUN_OPTIONS      = --input namd.input --iterations 1 --output -
STDOUT_FILENAME := namd.out

LDFLAGS = -lstdc++ -lm
LIBS    = -lm

include ../../Makefile.spec2006
