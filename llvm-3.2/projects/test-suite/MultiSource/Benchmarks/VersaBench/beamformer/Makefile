LEVEL = ../../../..
LDFLAGS += -lm

PROG     = beamformer
FP_TOLERANCE = 0.000001
ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = -i 400
else 
ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS = -i 25
else
RUN_OPTIONS = -i 140
endif
endif
HASH_PROGRAM_OUTPUT = 1

include $(LEVEL)/MultiSource/Makefile.multisrc

