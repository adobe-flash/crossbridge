LEVEL = ../../../..

PROG     = perimeter
CPPFLAGS = -DTORONTO
LDFLAGS  = 
ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = 11
else
ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS = 9
else
RUN_OPTIONS = 10
endif
endif

include $(LEVEL)/MultiSource/Makefile.multisrc

