LEVEL = ../../..
PROG     = sgefa
LDFLAGS  = -lm

FP_TOLERANCE = 0.0001
ifdef LARGE_PROBLEM_SIZE
CPPFLAGS = -DSCALE=50
else
CPPFLAGS = -DSCALE=40
endif

include	../../Makefile.multisrc
