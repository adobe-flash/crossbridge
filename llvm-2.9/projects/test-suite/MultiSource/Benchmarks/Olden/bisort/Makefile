LEVEL = ../../../..

PROG     = bisort
CPPFLAGS = -DTORONTO
LDFLAGS  = -lm

ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = 3000000
else
RUN_OPTIONS = 700000
endif
include $(LEVEL)/MultiSource/Makefile.multisrc

