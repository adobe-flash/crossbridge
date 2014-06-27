LEVEL = ../../../..

Source = lucas_distrib_spec.f90

#FP_ABSTOLERANCE=1e-7
#FP_TOLERANCE=0.04

STDIN_FILENAME  = lucas2.in
STDOUT_FILENAME = lucas2.out
#NAGFORTRAN_FLAGS = -dusty -maxcontin=69

include ../../Makefile.spec2000
include $(PROJ_SRC_ROOT)/Makefile.FORTRAN
CPPFLAGS += -DINT64='long long'

