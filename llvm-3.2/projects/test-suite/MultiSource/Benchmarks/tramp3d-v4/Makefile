LEVEL = ../../..

PROG     = tramp3d-v4
CPPFLAGS = -fno-exceptions
LDFLAGS  = -lstdc++
ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS =  --cartvis 1.0 0.0 --rhomin 1e-8 -n 4 --domain 32 32 32
else
RUN_OPTIONS =  --cartvis 1.0 0.0 --rhomin 1e-8 -n 10 --domain 32 32 32
endif
FP_ABSTOLERANCE :=  0.0000001

include	../../Makefile.multisrc
