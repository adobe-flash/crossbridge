LEVEL = ../../../..

PROG     = enc-md5
ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = 50
else
ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS = 1
else
RUN_OPTIONS = 10
endif
endif
include $(LEVEL)/MultiSource/Makefile.multisrc

