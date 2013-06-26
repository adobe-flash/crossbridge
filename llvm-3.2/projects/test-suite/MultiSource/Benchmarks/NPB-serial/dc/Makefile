LEVEL = ../../../..

PROG     = dc
LDFLAGS  = 

Source = adc.c dc.c jobcntl.c c_print_results.c extbuild.c rbt.c

RUN_OPTIONS = 10000000 ADC.par

ifdef SMALL_PROBLEM_SIZE
CPPFLAGS = -DSMALL_PROBLEM_SIZE
endif

include $(LEVEL)/MultiSource/Makefile.multisrc

