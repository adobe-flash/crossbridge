LEVEL = ../../../..

PROG     = netbench-url
ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = $(PROJ_SRC_DIR)/medium_inputs 1400
else
ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS = $(PROJ_SRC_DIR)/medium_inputs 100
else
RUN_OPTIONS = $(PROJ_SRC_DIR)/medium_inputs 900
endif
endif
include $(LEVEL)/MultiSource/Makefile.multisrc

