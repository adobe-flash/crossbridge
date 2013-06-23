LEVEL = ../../../..

PROG     = enc-rc4
ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = 1000000
else
ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS = 2000
else
RUN_OPTIONS = 200000
endif
endif
include $(LEVEL)/MultiSource/Makefile.multisrc

