LEVEL = ../../../..

PROG     = bmm
ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = 1024 1024
else
ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS = 128 32
else
RUN_OPTIONS = 1024 64
endif
endif
include $(LEVEL)/MultiSource/Makefile.multisrc

