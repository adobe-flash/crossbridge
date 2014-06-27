##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 gamess test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

FP_TOLERANCE    = 0.0001
FP_ABSTOLERANCE = 0.00001

include ../../Makefile.spec2006
include $(PROJ_SRC_ROOT)/Makefile.FORTRAN

ifeq ($(RUN_TYPE),test)
  STDIN_FILENAME  := exam29.config
  STDOUT_FILENAME := exam29.out
else
  STDIN_FILENAME  := h2ocu2+.energy.config
  STDOUT_FILENAME := h2ocu2+.energy.out
endif

NAGFORTRAN_FLAGS += -dusty -maxcontin=100 -dcfuns
FPPFLAGS += -DSPEC_CPU_NO_HOLLERITH
