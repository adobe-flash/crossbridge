##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 gromacs test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

FP_TOLERANCE = 0.0125

include ../../Makefile.spec2006
include $(PROJ_SRC_ROOT)/Makefile.FORTRAN

CPPFLAGS += -I$(SPEC_BENCH_DIR)/src/ -DHAVE_CONFIG_H
NAGFORTRAN_FLAGS = -maxcontin=50

RUN_OPTIONS     := -silent -deffnm gromacs -nice 0
STDOUT_FILENAME := gromacs.err
