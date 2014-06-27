LEVEL = ../../../..

Source = cfftb.f90 cfftf.f90 cffti.f90 FaceRec.f90 fft2d.f90 \
         imageRoutines.f90 fft2d.f90 graphRoutines.f90 \
         gaborRoutines.f90 parameterRoutines.f90 FaceRecTypes.f90

FP_ABSTOLERANCE=2.0e-7
FP_TOLERANCE=0.001

STDIN_FILENAME  = $(RUN_TYPE).in
STDOUT_FILENAME = $(RUN_TYPE).out
NAGFORTRAN_FLAGS = -kind=byte -dusty -I $(SPEC_BENCH_DIR)/src


include ../../Makefile.spec2000
include $(PROJ_SRC_ROOT)/Makefile.FORTRAN
CPPFLAGS += -DINT64='long long'

FaceRec.c: FaceRecTypes.c gaborRoutines.c graphRoutines.c imageRoutines.c parameterRoutines.c
gaborRoutines.c: FaceRecTypes.c fft2d.c imageRoutines.c 
graphRoutines.c: FaceRecTypes.c gaborRoutines.c imageRoutines.c FaceRecTypes.c 
imageRoutines.c: FaceRecTypes.c fft2d.c
parameterRoutines.c: FaceRecTypes.c
