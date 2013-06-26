LEVEL = ../../../..

PROG     = enc-pc1
ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = 20000000
else
ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS = 1000000
else
RUN_OPTIONS = 5000000
endif
endif
include $(LEVEL)/MultiSource/Makefile.multisrc

