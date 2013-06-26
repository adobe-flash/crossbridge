LEVEL = ../../../..

PROG     = neural
CPPFLAGS = -DVERSION='"1.00"' -DCOMPDATE="\"today\"" -DCFLAGS='""' -DHOSTNAME="\"thishost\"" 
LDFLAGS  = -lm
ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = $(PROJ_SRC_DIR)/ref.in
else
RUN_OPTIONS = $(PROJ_SRC_DIR)/test.in
endif
include $(LEVEL)/MultiSource/Makefile.multisrc

