LEVEL = ../../../..

PROG     = enc-3des
ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = 200
else
ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS = 10
else
RUN_OPTIONS = 100
endif
endif
include $(LEVEL)/MultiSource/Makefile.multisrc

