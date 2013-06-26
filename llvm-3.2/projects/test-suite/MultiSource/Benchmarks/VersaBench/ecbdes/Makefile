LEVEL = ../../../..

PROG     = ecbdes
ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = 34
else
ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS = 4
else
RUN_OPTIONS = 16
endif
endif
include $(LEVEL)/MultiSource/Makefile.multisrc

