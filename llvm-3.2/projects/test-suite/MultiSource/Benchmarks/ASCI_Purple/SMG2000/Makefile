LEVEL = ../../../..

PROG     = smg2000
 
CPPFLAGS += -D_POSIX_SOURCE -DHYPRE_TIMING -DHYPRE_SEQUENTIAL 
CPPFLAGS += -I. 

LIBS += -lm
LDFLAGS += -lm

ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS ="-n 30 15 30 -c 0.1 1.0 10.0"
else
RUN_OPTIONS ="-n 100 40 100 -c 0.1 1.0 10.0"
endif

include	../../../Makefile.multisrc
