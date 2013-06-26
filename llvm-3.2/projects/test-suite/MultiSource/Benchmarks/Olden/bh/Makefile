LEVEL = ../../../..

PROG     = bh
CPPFLAGS = -DTORONTO
LDFLAGS  = -lm
ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = 40000 30
else
ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS = 2000 5
else
RUN_OPTIONS = 20000 20
endif
endif
include $(LEVEL)/MultiSource/Makefile.multisrc

